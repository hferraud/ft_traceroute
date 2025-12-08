#include "traceroute.h"

#include <error.h>
#include <errno.h>

#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

static void send_traceroute_probe(traceroute_info_t *info);

void init_traceroute(traceroute_info_t *info) {
    info->ttl = 0;
    info->socket = init_socket(info->ttl);
    dns_lookup(info->hostname, &info->address);
    info->address.sin_port = htons(info->port);
    printf("DNS resolved %s\n", inet_ntoa(info->address.sin_addr));
}

void traceroute(traceroute_info_t *info) {
    send_traceroute_probe(info);
}

static void send_traceroute_probe(traceroute_info_t *info) {
    set_ttl(info->socket, info->ttl);
    ssize_t res = sendto(
        info->socket,
        NULL,
        0,
        0,
        (struct sockaddr *)&info->address,
        sizeof(info->address));
    if (res == -1) {
        error(EXIT_FAILURE, errno, "sendto");
    }
    info->ttl++;
}