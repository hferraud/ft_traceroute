#ifndef PING_PRINT_H
#define PING_PRINT_H

#include "traceroute.h"

void print_traceroute(traceroute_info_t *info);
void print_hop_probes(traceroute_hop_t *hop);
void print_timeout();
void print_hop(traceroute_info_t *info);

#endif
