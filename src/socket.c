#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

#include "ping.h"

#define SOCKET_TIMEOUT_SEC 1
#define SOCKET_TIMEOUT_USEC 0

void set_ttl(int32_t socket_fd, uint8_t ttl);

int32_t init_socket() {
	int32_t socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd < 0) {
		error(EXIT_FAILURE, errno, "socket()");
	}
	return socket_fd;
}

void set_ttl(int32_t socket_fd, uint8_t ttl) {
	if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
		error(EXIT_FAILURE, errno, "setsockopt()");
}

void dns_lookup(char* hostname, struct sockaddr_in *address) {
	int32_t status;
	struct addrinfo hints = {0};
	struct addrinfo *res;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	status = getaddrinfo(hostname, NULL, &hints, &res);
	if (status != 0 || res == NULL) {
		error(EXIT_FAILURE, 0, "unknown host");
	}
	*address = *(struct sockaddr_in *)res->ai_addr;
	freeaddrinfo(res);
}

