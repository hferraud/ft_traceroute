#include "traceroute.h"

#include "socket.h"
#include "icmp.h"
#include "print.h"

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/udp.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#include "time.h"

#define TRACEROUTE_TIMEOUT 5

#define SELECT_SUCCESS 1
#define SELECT_TIMEOUT 0
#define SELECT_ERROR 0

static void traceroute_send_probe(traceroute_info_t *info);
static int32_t traceroute_select(traceroute_info_t *info);
static void traceroute_recv(traceroute_info_t *info, traceroute_response_t *response);
static void traceroute_process_response(traceroute_info_t *info, traceroute_response_t *response);
static void traceroute_update(traceroute_info_t *info);

void traceroute_init(traceroute_info_t *info) {
    info->udp_socket = init_udp_socket();
    info->icmp_socket = init_icmp_socket();
    dns_lookup(info->cmd_args.host, &info->address);
    info->address.sin_port = htons(info->cmd_args.port);
    info->current_hop.index = info->cmd_args.first_hop;
    for (size_t i = 0; i < TRIES_MAX; i++) {
        info->current_hop.rtt[i] = -1;
    }
}

void traceroute_cleanup(traceroute_info_t *info) {
    close(info->udp_socket);
    close(info->icmp_socket);
}

void traceroute(traceroute_info_t *info) {
    traceroute_response_t response;

    print_traceroute(info);
    print_hop(info);
    while (!(info->current_hop.last_hop && info->current_hop.probe_index == info->cmd_args.tries)) {
        traceroute_send_probe(info);
        if (traceroute_select(info) == SELECT_SUCCESS) {
            traceroute_recv(info, &response);
            traceroute_process_response(info, &response);
        } else {
            print_timeout();
        }
        traceroute_update(info);
    }
}

static void traceroute_send_probe(traceroute_info_t *info) {
    set_ttl(info->udp_socket, info->current_hop.index);
    info->address.sin_port = htons(info->cmd_args.port + info->probe_sent);
    ssize_t res = sendto(
        info->udp_socket,
        TRACEROUTE_DATA,
        TRACEROUTE_DATA_LEN,
        0,
        (struct sockaddr *)&info->address,
        sizeof(info->address));
    gettimeofday(&info->probe_send_time, NULL);
    if (res == -1 || res != TRACEROUTE_DATA_LEN) {
        error(EXIT_FAILURE, errno, "sendto");
    }
    info->probe_sent++;
}

static int32_t traceroute_select(traceroute_info_t *info) {
    fd_set rfds;
    int32_t status;
    struct timeval timeout;

    FD_ZERO(&rfds);
    FD_SET(info->icmp_socket, &rfds);
    timeout = get_wait_time(&info->probe_send_time, info->cmd_args.wait);
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
        TRACEROUTE_RESPONSE_MAX_SIZE,
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
    struct udphdr *udp_header;

    icmp_process_response(response);
    if (response->icmp_header->type != ICMP_TIME_EXCEEDED &&
        (response->icmp_header->type != ICMP_UNREACH && response->icmp_header->code != ICMP_PORT_UNREACH)) {
        printf("Dropping ICMP packet type=%d, code=%d\n", response->icmp_header->type, response->icmp_header->code);
        return;
    }
    udp_header = (struct udphdr *) (response->buffer + sizeof(struct iphdr) * 2 + sizeof(struct icmphdr));
    if (ntohs(udp_header->dest) != info->cmd_args.port + info->probe_sent - 1) {
        // The udp port of the response don't correspond to the probe we sent
        // TODO: change this, just drop the packet
        error(EXIT_FAILURE, 0, "udp dest mismatch in response sent=%lu received=%u", info->cmd_args.port + info->probe_sent - 1, ntohs(udp_header->dest));
    }
    if (response->icmp_header->type == ICMP_UNREACH) {
        info->current_hop.last_hop = true;
        info->current_hop.address = response->address;
        info->current_hop.address_found = true;
    }
    if (!info->current_hop.address_found) {
        info->current_hop.address_found = true;
        info->current_hop.address = response->address;
    }
    size_t i;
    for (i = 0; i < TRIES_MAX; i++) {
        if (info->current_hop.rtt[i] == -1) {
            break;
        }
    }
    info->current_hop.rtt[i] = tv_to_ms(elapsed_time(info->probe_send_time, response->recv_time));
}

static void traceroute_update(traceroute_info_t *info) {
    info->current_hop.probe_index++;
    if (info->current_hop.probe_index == info->cmd_args.tries) {
        // We jump to next hop
        print_hop_probes(&info->current_hop);
        if (info->current_hop.last_hop) {
            return;
        }
        info->current_hop.probe_index = 0;
        info->current_hop.index++;
        info->current_hop.address_found = false;
        print_hop(info);
        if (info->current_hop.index == info->cmd_args.max_hop) {
            info->current_hop.last_hop = true;
        }
    }
}