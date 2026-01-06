#ifndef PING_ICMP_H
#define PING_ICMP_H

#include "traceroute.h"

void icmp_process_response(traceroute_response_t *response);
uint16_t icmp_checksum(void* packet, size_t len);

#endif
