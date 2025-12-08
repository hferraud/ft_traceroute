#include "parser.h"

#include <error.h>

#include "traceroute.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define TRACEROUTE_MIN_ARG 2
#define TRACEROUTE_MAX_ARG 3


void parse(int argc, char** argv, traceroute_info_t* traceroute_info) {
    if (argc < TRACEROUTE_MIN_ARG || argc > TRACEROUTE_MAX_ARG) {
        error (EXIT_FAILURE, 0, "usage: %s <address> [port]", argv[0]);
    }
    traceroute_info->hostname = argv[1];
    if (argc == TRACEROUTE_MIN_ARG) {
        return;
    }
    char *endptr;
    traceroute_info->port = strtol(argv[2], &endptr, 10);
    if (errno) {
        error (EXIT_FAILURE, errno, "%s", argv[2]);
    }
    if (*endptr != 0) {
        error (EXIT_FAILURE, 0, "%s: invalid port number", argv[2]);
    }
}
