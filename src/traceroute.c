#include "traceroute.h"

#include "socket.h"
#include "icmp.h"
#include "udp.h"
#include "print.h"

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "time.h"

#define TRACEROUTE_TIMEOUT 5

#define SELECT_SUCCESS 1
#define SELECT_TIMEOUT 0
#define SELECT_ERROR 0

static void traceroute_send_probes(traceroute_info_t *info);
static int32_t traceroute_select(traceroute_info_t *info);
static void traceroute_recv(traceroute_info_t *info, traceroute_response_t *response);
static void traceroute_process_response(traceroute_info_t *info, traceroute_response_t *response);
static void traceroute_process_probe_response(traceroute_info_t *info, traceroute_response_t *response, uint8_t hop, uint8_t index);
static void traceroute_process_timeout(traceroute_info_t *info);
static void traceroute_update(traceroute_info_t *info);

void traceroute_init(traceroute_info_t *info) {
    info->udp_socket = init_udp_socket();
    info->icmp_socket = init_icmp_socket();
    info->last_hop = false;
    info->probes_saved = malloc(info->cmd_args.max_hops * sizeof(traceroute_probe_info_t *));
    if (info->probes_saved == NULL) {
        error(EXIT_FAILURE, errno, "traceroute_init: malloc");
    }
    for (int i = 0; i < info->cmd_args.max_hops; i++) {
        info->probes_saved[i] = malloc(info->cmd_args.queries * sizeof(traceroute_probe_info_t));
        if (info->probes_saved[i] == NULL) {
            error(EXIT_FAILURE, errno, "traceroute_init: malloc");
        }
        info->probes_saved[i]->state = IDLE;
    }
    dns_lookup(info->cmd_args.host, &info->address);
    info->address.sin_port = htons(info->cmd_args.port);
    printf("traceroute to %s (%s), 30 hops max, 60 byte packets\n", info->cmd_args.host, inet_ntoa(info->address.sin_addr));
}

void traceroute_cleanup(traceroute_info_t *info) {
    close(info->udp_socket);
    close(info->icmp_socket);
    for (int i = 0; i < info->cmd_args.max_hops; i++) {
        free(info->probes_saved[i]);
    }
    free(info->probes_saved);
}

void traceroute(traceroute_info_t *info) {
    traceroute_response_t response;

    while (true && !info->last_hop) {
        traceroute_send_probes(info);
        if (traceroute_select(info) == SELECT_SUCCESS) {
            traceroute_recv(info, &response);
            traceroute_process_response(info, &response);
        } else {
            traceroute_process_timeout(info);
        }
        traceroute_update(info);
    }
}

static void traceroute_send_probes(traceroute_info_t *info) {
    size_t hop, index;
    while (info->probes_out < info->cmd_args.sim_queries && info->probes_sent < info->cmd_args.max_hops * info->cmd_args.queries) {
        hop = info->probes_sent / info->cmd_args.queries;
        index = info->probes_sent % info->cmd_args.queries;
        set_ttl(info->udp_socket, (int32_t)hop + 1);
        udp_send_probe(info, &info->probes_saved[hop][index]);
        info->probes_sent++;
        info->probes_out++;
    }
}

static int32_t traceroute_select(traceroute_info_t *info) {
    fd_set rfds;
    int32_t status;
    struct timeval timeout;

    FD_ZERO(&rfds);
    FD_SET(info->icmp_socket, &rfds);
    timeout.tv_sec = TRACEROUTE_TIMEOUT;
    timeout.tv_usec = 0;
    status = select(info->icmp_socket + 1, &rfds, NULL, NULL, &timeout);
    if (status < 0) {
        error(EXIT_FAILURE, errno, "select");
    }
    return status;
}

static void traceroute_recv(traceroute_info_t *info, traceroute_response_t *response) {
    ssize_t status;
    socklen_t address_len = sizeof(response->address);

    status = recvfrom(
        info->icmp_socket,
        response->buffer,
        RESPONSE_MAX_SIZE,
        0,
        (struct sockaddr *) &response->address,
        &address_len
    );
    gettimeofday(&response->recv_time, NULL);
    if (status < 0) {
        error(EXIT_FAILURE, errno, "recvfrom");
    }
    response->size = status;
}

static void traceroute_process_response(traceroute_info_t *info, traceroute_response_t *response) {
    size_t hop;
    size_t index;
    struct udphdr *udp_header;

    icmp_process_response(response);
    if (response->icmp_header->type != ICMP_TIME_EXCEEDED &&
        !(response->icmp_header->type == ICMP_UNREACH && response->icmp_header->code == ICMP_PORT_UNREACH)) {
        printf("Dropping ICMP packet type=%d, code=%d\n", response->icmp_header->type, response->icmp_header->code);
        return;
    }
    udp_header = (struct udphdr *) (response->buffer + sizeof(struct iphdr) * 2 + sizeof(struct icmphdr));
    if (udp_header->dest < info->cmd_args.port || udp_header->dest > info->cmd_args.port + info->probes_sent) {
        // The udp port of the response don't correspond to anything we sent
        // TODO: change this, just drop the packet
        error(EXIT_FAILURE, 0, "ICMP response is incorrect");
    }
    hop = (udp_header->dest - info->cmd_args.port) / info->cmd_args.queries;
    index = (udp_header->dest - info->cmd_args.port) % info->cmd_args.queries;
    if (info->probes_saved[hop][index].state != SENT) {
        // The udp port of the response don't correspond to anything we sent
        // TODO: change this, just drop the packet
        error(EXIT_FAILURE, 0, "ICMP response is incorrect");
    }
    if (response->icmp_header->type == ICMP_UNREACH) {
        info->last_hop = true;
    }
    traceroute_process_probe_response(info, response, hop, index);
}

static void traceroute_process_probe_response(traceroute_info_t *info, traceroute_response_t *response, uint8_t hop, uint8_t index) {
    traceroute_probe_info_t probe_info;

    probe_info = info->probes_saved[hop][index];
    if (info->last_hop && index == info->cmd_args.queries - 1) {
        info->last_probe = true;
    }
    probe_info.round_trip_time = elapsed_time(probe_info.send_time, response->recv_time);
    probe_info.address = response->address;
    probe_info.state = RECVD;
    for (int i = 0; i < info->cmd_args.queries; i++) {
        if (i == index) {
            continue;
        }
        // If the address is already resolved there is no need to resolve it again
        if (probe_info.address.sin_addr.s_addr == info->probes_saved[hop][i].address.sin_addr.s_addr) {
            strcpy(probe_info.hostname, info->probes_saved[hop][i].hostname);
            return;
        }
    }
    reverse_dns_lookup(&probe_info.address, probe_info.hostname, 255);
}

static void traceroute_update(traceroute_info_t *info) {
    traceroute_probe_info_t *probe_info;

    probe_info = &info->probes_saved[info->current_hop][info->current_index];
    while ((probe_info->state == RECVD || probe_info->state == TIMEOUT) || info->last_probe) {
        print_traceroute_probe_info(probe_info);
        if (info->current_index + 1 == info->cmd_args.queries) {
            info->current_index = 0;
            info->current_hop++;
            print_hop(info);
        } else {
            info->current_index++;
        }
        probe_info = &info->probes_saved[info->current_hop][info->current_index];
    }
}

static void traceroute_process_timeout(traceroute_info_t *info) {
    info->probes_saved[info->current_hop][info->current_index].state = TIMEOUT;
}
