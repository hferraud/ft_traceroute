#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <stdint.h>
#include <netinet/in.h>

typedef struct traceroute_info_s {
    char*               hostname;
    struct sockaddr_in	address;
    uint16_t            port;
    uint8_t             ttl;
    int                 socket;
} traceroute_info_t;

void init_traceroute(traceroute_info_t *info);
void traceroute(traceroute_info_t *info);

#endif //TRACEROUTE_H
