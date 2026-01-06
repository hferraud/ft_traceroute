#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdint.h>

typedef struct {
    uint16_t    port;
    char        *host;
    uint32_t    sim_queries;
    uint8_t     queries;
} command_args_t;

void parse(int argc, char** argv, command_args_t* cmd_args);

#endif
