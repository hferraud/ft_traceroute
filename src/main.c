#include <signal.h>
#include <stdlib.h>
#include <error.h>

#include "parser.h"
#include "traceroute.h"

int main(int argc, char** argv) {
	traceroute_info_t info;

	parse(argc, argv, &info);
	init_traceroute(&info);
	traceroute(&info);
	return EXIT_SUCCESS;
}
