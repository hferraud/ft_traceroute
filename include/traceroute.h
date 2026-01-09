#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>

#include "parser.h"

#define RESPONSE_MAX_SIZE (sizeof(struct iphdr) * 2 + sizeof(struct icmphdr) * 2 + 100)


typedef struct {
    uint8_t             index;
    uint8_t             probe_index;
    bool                address_found;
    bool                last_hop;
    struct sockaddr_in	address;
    float               rtt[TRIES_MAX];
} traceroute_hop_t;

typedef struct {
    struct sockaddr_in  address;
    int                 udp_socket;
    int                 icmp_socket;
    command_args_t      cmd_args;
    size_t              probe_sent;
    struct timeval      probe_send_time;
    traceroute_hop_t    current_hop;
} traceroute_info_t;


typedef struct {
    uint8_t             buffer[RESPONSE_MAX_SIZE];
    size_t              size;
    struct sockaddr_in	address;
    struct iphdr        *ip_header;
    struct icmphdr      *icmp_header;
    struct timeval      recv_time;
} traceroute_response_t;

void traceroute_init(traceroute_info_t *info);
void traceroute(traceroute_info_t *info);

#endif //TRACEROUTE_H
