#include "print.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "time.h"
#include "traceroute.h"

void print_traceroute(traceroute_info_t *info) {
    printf("traceroute to %s (%s), %d hops max\n", info->cmd_args.host, inet_ntoa(info->address.sin_addr), info->cmd_args.max_hop);
}

void print_hop_probes(traceroute_hop_t *hop) {
    size_t i = 0;

    if (hop->address_found) {
        printf(" %s ", inet_ntoa(hop->address.sin_addr));
    }
    while (hop->rtt[i] != -1) {
        printf(" %.3fms ", hop->rtt[i]);
        hop->rtt[i] = -1;
        i++;
    }
    printf("\n");
}

void print_timeout() {
    printf(" * ");
    fflush(STDIN_FILENO);
}

void print_hop(traceroute_hop_t *hop) {
    printf("%3d  ", hop->index);
}