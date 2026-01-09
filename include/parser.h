#ifndef PING_PARSER_H
#define PING_PARSER_H

#include <stdint.h>

#define FIRST_HOP_DEFAULT	1
#define FIRST_HOP_MAX		255
#define MAX_HOP_DEFAULT		64
#define MAX_HOP_MAX			255
#define PORT_DEFAULT		33434
#define PORT_MAX			UINT16_MAX
#define TRIES_DEFAULT		3
#define TRIES_MAX			10
#define WAIT_DEFAULT		3
#define WAIT_MAX			60

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
