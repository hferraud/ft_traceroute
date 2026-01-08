#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>

#include "parser.h"

#define RESPONSE_MAX_SIZE (sizeof(struct iphdr) * 2 + sizeof(struct icmphdr) * 2)

typedef struct traceroute_probe_info_s traceroute_probe_info_t;

typedef struct {
    struct sockaddr_in	        address;
    int                         udp_socket;
    int                         icmp_socket;
    command_args_t              cmd_args;
    uint32_t                    probes_sent;
    uint32_t                    probes_out;
    size_t                      packet_size;
    traceroute_probe_info_t**   probes_saved;
    uint8_t                     current_hop;
    uint8_t                     current_index;
    bool                        last_hop;
    bool                        last_probe;
} traceroute_info_t;

typedef struct {
    uint8_t             buffer[RESPONSE_MAX_SIZE];
    size_t              size;
    struct sockaddr_in	address;
    struct iphdr        *ip_header;
    struct icmphdr      *icmp_header;
    struct timeval      recv_time;
} traceroute_response_t;

typedef enum {
    IDLE = 0,
    SENT,
    RECVD,
    TIMEOUT,
} probe_state_t;

struct traceroute_probe_info_s {
    struct timeval      send_time;
    struct timeval      round_trip_time;
    struct sockaddr_in	address;
    char                hostname[255];
    uint8_t             hop;
    uint8_t             index;
    probe_state_t       state;
};

void traceroute_init(traceroute_info_t *info);
void traceroute(traceroute_info_t *info);

#endif //TRACEROUTE_H
