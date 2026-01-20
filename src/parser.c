#include "parser.h"

#include <error.h>

#include "traceroute.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>

#define TRACEROUTE_MIN_ARG	1
#define TRACEROUTE_MAX_ARG	1

static error_t parse_opt(int key, char* arg, struct argp_state *state);
static int32_t parse_number(const char *optarg, size_t maxval, int allow_zero, unsigned long *result);

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
			if (parse_number(arg, FIRST_HOP_MAX, 0, (unsigned long *)&cmd_args->first_hop) == -1) {
				dprintf(STDERR_FILENO, "%s: impossible distance '%s'\n", state->argv[0], arg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'm':
			if (parse_number(arg, MAX_HOP_MAX, 0, (unsigned long *)&cmd_args->max_hop) == -1) {
				dprintf(STDERR_FILENO, "%s: invalid hops value '%s'\n", state->argv[0], arg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'p':
			if (parse_number(arg, PORT_MAX, 0, (unsigned long *)&cmd_args->port) == -1) {
				dprintf(STDERR_FILENO, "%s: invalid port number '%s'\n", state->argv[0], arg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'q':
			if (parse_number(arg, TRIES_MAX, 0, (unsigned long *)&cmd_args->tries) == -1) {
				dprintf(STDERR_FILENO, "%s: number of tries should be between 1 and 10\n", state->argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
		case 'w':
			if (parse_number(arg, WAIT_MAX, 0, (unsigned long *)&cmd_args->wait) == -1) {
				dprintf(STDERR_FILENO, "%s: ridiculous waiting time '%s'\n", state->argv[0], arg);
				exit(EXIT_FAILURE);
			}
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

static int32_t parse_number(const char *optarg, size_t maxval, int allow_zero, unsigned long *result) {
	char *p;

	*result = strtoul(optarg, &p, 0);
	if (*p) {
		return -1;
	}
	if (*result == 0 && !allow_zero) {
		return -1;
	}
	if (maxval && *result > maxval) {
		return -1;
	}
	return 0;
}
