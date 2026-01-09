#ifndef PING_PRINT_H
#define PING_PRINT_H

#include "traceroute.h"

void print_response(traceroute_info_t *info, traceroute_response_t *response);
void print_timeout();
void print_hop(traceroute_hop_t *hop);

#endif
