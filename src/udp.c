#include "udp.h"

#include <stdlib.h>
#include <netinet/udp.h>
#include <error.h>
#include <errno.h>
#include <sys/time.h>

#include "traceroute.h"

#define PACKET_LEN 60

static void udp_init_probe(uint8_t *buffer, uint16_t port);

void udp_send_probe(traceroute_info_t *info, traceroute_probe_info_t *probe_info) {
    static uint8_t buffer[PACKET_LEN];
    udp_init_probe(buffer, info->cmd_args.port + info->probes_sent);
    ssize_t res = sendto(
        info->udp_socket,
        buffer,
        PACKET_LEN,
        0,
        (struct sockaddr *)&info->address,
        sizeof(info->address));
    gettimeofday(&probe_info->send_time, NULL);
    if (res == -1 || res != PACKET_LEN) {
        error(EXIT_FAILURE, errno, "sendto");
    }
}

static void udp_init_probe(uint8_t *buffer, uint16_t port) {
    size_t packet_len = PACKET_LEN - sizeof(struct iphdr);

    struct udphdr *udp_header = (struct udphdr *)buffer;
    udp_header->dest = htonl(port);
    udp_header->len = htons(packet_len);
}


