#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdint.h>

typedef struct {
    char        *host;
    uint16_t    port;
    uint8_t     first_hop;
    uint8_t     max_hop;
    uint8_t     tries;
    uint8_t     wait;
} command_args_t;

void parse(int argc, char** argv, command_args_t* cmd_args);

#endif
