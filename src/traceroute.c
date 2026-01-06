#include "traceroute.h"

#include "socket.h"
#include "icmp.h"
#include "udp.h"

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SELECT_SUCCESS 1
#define SELECT_TIMEOUT 0
#define SELECT_ERROR 0


static void traceroute_send_probes(traceroute_info_t *info);
static int32_t traceroute_select(traceroute_info_t *info);
static void traceroute_recv(traceroute_info_t *info, traceroute_response_t *response);
static void traceroute_process_response(traceroute_info_t *info, traceroute_response_t *response);

void init_traceroute(traceroute_info_t *info) {
    info->hop = 0;
    info->udp_socket = init_udp_socket();
    info->icmp_socket = init_icmp_socket();
    dns_lookup(info->cmd_args.host, &info->address);
    info->address.sin_port = htons(info->cmd_args.port);
    printf("traceroute to %s (%s), 30 hops max, 60 byte packets\n", info->cmd_args.host, inet_ntoa(info->address.sin_addr));
}

void traceroute(traceroute_info_t *info) {
    traceroute_response_t response;

    while (true) {
        traceroute_send_probes(info);
        if (traceroute_select(info) == SELECT_SUCCESS) {
            traceroute_recv(info, &response);
            traceroute_process_response(info, &response);
        } else {
            printf("*\n");
            info->hop += 1;
        }
    }
}

static void traceroute_send_probes(traceroute_info_t *info) {
    while (info->probes_out != info->cmd_args.sim_queries) {
        udp_hop_probing(info);
    }
}

static int32_t traceroute_select(traceroute_info_t *info) {
    fd_set rfds;
    int32_t status;
    struct timeval timeout;

    FD_ZERO(&rfds);
    FD_SET(info->icmp_socket, &rfds);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    status = select(info->icmp_socket + 1, &rfds, NULL, NULL, &timeout);
    if (status < 0) {
        printf("select returned with status %d\n", status);
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
    if (status < 0) {
        error(EXIT_FAILURE, errno, "recvfrom");
    }
    response->size = status;
}

static void traceroute_process_response(traceroute_info_t *info, traceroute_response_t *response) {
    icmp_process_response(response);
        char hostname[255];
    if (response->icmp_header->type == ICMP_TIME_EXCEEDED) {
        reverse_dns_lookup(&response->address, hostname, 255);
        printf(" %d %s (%s)\n", info->hop, hostname, inet_ntoa(response->address.sin_addr));
        info->hop += 1;
    }
    else if (response->icmp_header->type == ICMP_UNREACH && response->icmp_header->code == ICMP_PORT_UNREACH) {
        reverse_dns_lookup(&response->address, hostname, 255);
        printf(" %d %s (%s)\n", info->hop, hostname, inet_ntoa(response->address.sin_addr));
        info->hop += 1;
        exit(0);
    } else {
        printf("Dropping ICMP packet type=%d, code=%d\n", response->icmp_header->type, response->icmp_header->code);
    }
}
