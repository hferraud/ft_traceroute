#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "parser.h"

#define RESPONSE_MAX_SIZE (sizeof(struct iphdr) * 2 + sizeof(struct icmphdr) * 2)

typedef struct {
    struct sockaddr_in	address;
    uint8_t             hop;
    int                 udp_socket;
    int                 icmp_socket;
    command_args_t      cmd_args;
    uint32_t            probes_out;
    uint32_t            probes_sent;
    size_t              packet_size;
} traceroute_info_t;

typedef struct {
    uint8_t             buffer[RESPONSE_MAX_SIZE];
    size_t              size;
    struct sockaddr_in	address;
    struct iphdr        *ip_header;
    struct icmphdr      *icmp_header;
} traceroute_response_t;

void init_traceroute(traceroute_info_t *info);
void traceroute(traceroute_info_t *info);

#endif //TRACEROUTE_H
