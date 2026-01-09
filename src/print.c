#include "print.h"

#include <stdio.h>
#include <arpa/inet.h>

#include "time.h"
#include "traceroute.h"

void print_response(traceroute_info_t *info, traceroute_response_t *response) {
    struct timeval elapsed_tv;
    if (info->current_hop.address_found) {
        printf(" %s", inet_ntoa(response->address.sin_addr));
    }
    elapsed_tv = elapsed_time(info->probe_send_time, response->recv_time);
    printf(" %f", tv_to_ms(elapsed_tv));
}

void print_timeout() {
    printf(" *");
}

void print_hop(traceroute_hop_t *hop) {
    printf("\n %d", hop->index);
}