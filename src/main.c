#include <signal.h>
#include <stdlib.h>
#include <error.h>

#include "parser.h"
#include "traceroute.h"

int main(int argc, char** argv) {
	traceroute_info_t info = {0};

	parse(argc, argv, &info.cmd_args);
	init_traceroute(&info);
	traceroute(&info);
	return EXIT_SUCCESS;
}
