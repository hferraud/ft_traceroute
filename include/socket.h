#ifndef PING_SOCKET_H
#define PING_SOCKET_H

#include <netinet/in.h>

int32_t init_udp_socket();
int32_t init_icmp_socket();
void set_ttl(int32_t socket_fd, int32_t ttl);
void dns_lookup(char* hostname, struct sockaddr_in *address);

#endif
