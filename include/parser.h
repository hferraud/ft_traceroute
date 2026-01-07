#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdint.h>

typedef struct {
    char        *host;
    uint32_t    sim_queries;
    uint16_t    port;
    uint8_t     queries;
    uint8_t     max_hops;
} command_args_t;

void parse(int argc, char** argv, command_args_t* cmd_args);

#endif
