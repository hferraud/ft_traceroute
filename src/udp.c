#include "udp.h"

#include <stdlib.h>
#include <netinet/udp.h>
#include <error.h>
#include <errno.h>
#include <sys/time.h>

#include "traceroute.h"

#define DATA "SUPERMAN"
#define DATA_LEN 9

void udp_send_probe(traceroute_info_t *info) {
    info->address.sin_port = htons(info->cmd_args.port + info->probe_sent);
    ssize_t res = sendto(
        info->udp_socket,
        DATA,
        DATA_LEN,
        0,
        (struct sockaddr *)&info->address,
        sizeof(info->address));
    gettimeofday(&info->probe_send_time, NULL);
    if (res == -1 || res != DATA_LEN) {
        error(EXIT_FAILURE, errno, "sendto");
    }
}


