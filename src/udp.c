#include "udp.h"

#include <stdlib.h>
#include <netinet/udp.h>
#include <error.h>
#include <errno.h>

#include "traceroute.h"

#define PACKET_LEN 60
#define PROBE_NB 3

static void udp_init_probe(uint8_t *buffer, uint16_t port);

void udp_hop_probing(traceroute_info_t *info) {
    uint8_t* buffer = malloc(PACKET_LEN); // TODO: call this in traceroute_init once
    for (int i = 0; i < PROBE_NB; i++) {
        udp_init_probe(buffer, info->cmd_args.port + info->hop * PROBE_NB + i);
        ssize_t res = sendto(
            info->udp_socket,
            buffer,
            PACKET_LEN,
            0,
            (struct sockaddr *)&info->address,
            sizeof(info->address));
        if (res == -1 || res != PACKET_LEN) {
            free(buffer);
            error(EXIT_FAILURE, errno, "sendto");
            return;
        }
    }
    free(buffer);
}

static void udp_init_probe(uint8_t *buffer, uint16_t port) {
    size_t packet_len = PACKET_LEN - sizeof(struct iphdr);

    struct udphdr *udp_header = (struct udphdr *)buffer;
    udp_header->dest = htonl(port);
    udp_header->len = htons(packet_len);
}


