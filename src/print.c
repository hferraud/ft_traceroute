#include "print.h"

#include <stdio.h>
#include <arpa/inet.h>

#include "traceroute.h"

void print_traceroute_probe_info(traceroute_probe_info_t *probe_info) {
    if (probe_info->state == TIMEOUT) {
        printf(" *");
        return;
    }
    printf(" %s (%s)\n", probe_info->hostname, inet_ntoa(probe_info->address.sin_addr));
}

void print_hop(traceroute_info_t *info) {
    if (info->last_hop) {
        printf(" %d", info->current_hop);
    }
}