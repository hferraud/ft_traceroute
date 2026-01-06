#include "parser.h"

#include <error.h>

#include "traceroute.h"
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#define TRACEROUTE_MIN_ARG	2
#define TRACEROUTE_MAX_ARG	2

#define PORT_DEFAULT		33434
#define PORT_MAX			UINT16_MAX
#define SIM_QUERIES_DEFAULT	16
#define SIM_QUERIES_MAX		UINT32_MAX
#define	QUERIES_DEFAULT	3
#define	QUERIES_MAX		10


static error_t parse_opt(int key, char* arg, struct argp_state *state);
static size_t parse_number(const char *optarg, size_t maxval, int allow_zero);

void parse(int argc, char** argv, command_args_t* cmd_args) {
	const struct argp_option argp_options[] = {
		{"port", 'p', "port", 0, "Set the destination port to use.", 0},
		{"sim-queries", 'N', "squeries", 0, "Set the number of probes to be tried simultaneously (default is 16)", 0},
		{"queries", 'q', "nqueries", 0, "Set the number of probes per each hop (default is 3)", 0},
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
	cmd_args->port = PORT_DEFAULT;
	cmd_args->queries = QUERIES_DEFAULT;
	cmd_args->sim_queries = SIM_QUERIES_DEFAULT;
	argp_parse(&argp, argc, argv, 0, NULL, cmd_args);
}

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
	command_args_t *cmd_args = state->input;
	switch (key) {
		case 'p':
			cmd_args->port = parse_number(arg, PORT_MAX, 0);
			break;
		case 'q':
			cmd_args->queries = parse_number(arg, QUERIES_MAX, 0);
			break;
		case 'N':
			cmd_args->sim_queries = parse_number(arg, PORT_MAX, 0);
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
