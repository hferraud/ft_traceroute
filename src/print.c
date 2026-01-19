#include "print.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "traceroute.h"

void print_unreach(int8_t code);

void print_traceroute(traceroute_info_t *info) {
    printf("traceroute to %s (%s), %d hops max\n", info->cmd_args.host, inet_ntoa(info->address.sin_addr), info->cmd_args.max_hop);
}

void print_hop_probes(traceroute_hop_t *hop) {
    if (hop->address_found) {
        printf(" %s ", inet_ntoa(hop->address.sin_addr));
    }
    for (size_t i = 0; i < TRIES_MAX; i++) {
        if (hop->rtt[i] != -1) {
            printf(" %.3fms ", hop->rtt[i]);
            hop->rtt[i] = -1;
        }
        if (hop->unreach_code[i] != -1) {
            print_unreach(hop->unreach_code[i]);
            hop->unreach_code[i] = -1;
        }
    }
    printf("\n");
}

void print_unreach(int8_t code) {
    switch (code) {
        case 0: printf("!N"); break;
        case 1: printf("!H"); break;
        case 2: printf("!P"); break;
        case 4: printf("!F"); break;
        case 5: printf("!S"); break;
        case 13: printf("!X"); break;
        case 14: printf("!V"); break;
        case 15: printf("!C"); break;
        default:
            printf("!%d", code);
    }
}

void print_timeout() {
    printf(" * ");
    fflush(STDIN_FILENO);
}

void print_hop(traceroute_info_t *info) {
    printf("%3d  ", info->current_hop.index - info->cmd_args.first_hop + 1);
}
