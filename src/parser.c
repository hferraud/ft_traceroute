#include "parser.h"

#include <error.h>

#include "traceroute.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#define TRACEROUTE_MIN_ARG	2
#define TRACEROUTE_MAX_ARG	2

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

static error_t parse_opt(int key, char* arg, struct argp_state *state);
static size_t parse_number(const char *optarg, size_t maxval, int allow_zero);

void parse(int argc, char** argv, command_args_t* cmd_args) {
	const struct argp_option argp_options[] = {
		{"first-hop", 'f', "NUM", 0, "set initial hop distance, i.e., time-to-live", 0},
		{"max-hop", 'm', "NUM", 0, "set maximal hop count (default: 64)", 0},
		{"port", 'p', "PORT", 0, "use destination PORT port (default: 33434)", 0},
		{"tries", 'q', "NUM", 0, "send NUM probe packets per hop (default: 3)", 0},
		{"wait", 'w', "NUM", 0, "wait NUM seconds for response (default: 3)", 0},
		{0},
	};
	const char args_doc[] = "HOST ...";
	const char doc[] = "Send ICMP ECHO_REQUEST packets to network hosts."
					   "\vOptions marked with (root only) are available only to "
					   "superuser.";
	struct argp argp = {
			argp_options,
			parse_opt,
			args_doc,
			doc,
			NULL,
			NULL,
			NULL,
	};
	cmd_args->first_hop = FIRST_HOP_DEFAULT;
	cmd_args->max_hop = MAX_HOP_DEFAULT;
	cmd_args->port = PORT_DEFAULT;
	cmd_args->tries = TRIES_DEFAULT;
	cmd_args->wait = WAIT_DEFAULT;
	argp_parse(&argp, argc, argv, 0, NULL, cmd_args);
}

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
	command_args_t *cmd_args = state->input;
	switch (key) {
		case 'f':
			cmd_args->first_hop = parse_number(arg, FIRST_HOP_MAX, 0);
			break;
		case 'm':
			cmd_args->max_hop = parse_number(arg, MAX_HOP_MAX, 0);
			break;
		case 'p':
			cmd_args->port = parse_number(arg, PORT_MAX, 0);
			break;
		case 'q':
			cmd_args->tries = parse_number(arg, TRIES_MAX, 0);
			break;
		case 'w':
			cmd_args->wait = parse_number(arg, WAIT_MAX, 0);
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num > TRACEROUTE_MAX_ARG) {
				argp_error(state, "too many host operands");
			}
			cmd_args->host = arg;
			break;
		case ARGP_NO_ARGS:
			argp_error(state, "missing host operand");
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static size_t parse_number(const char *optarg, size_t maxval, int allow_zero) {
	char *p;
	unsigned long int n;

	n = strtoul(optarg, &p, 0);
	if (*p)
		error(EXIT_FAILURE, 0, "invalid value (`%s' near `%s')", optarg, p);

	if (n == 0 && !allow_zero)
		error(EXIT_FAILURE, 0, "option value too small: %s", optarg);

	if (maxval && n > maxval)
		error(EXIT_FAILURE, 0, "option value too big: %s", optarg);

	return n;
}
