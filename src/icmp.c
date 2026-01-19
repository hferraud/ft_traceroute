#include <string.h>
#include <error.h>
#include <arpa/inet.h>

#include "icmp.h"
#include "traceroute.h"

/**
 * @return 0 if the icmp packet was correctly processed, -1 otherwise
 */
int32_t icmp_process_response(traceroute_response_t *response) {
	uint16_t		checksum;

	response->ip_header = (struct iphdr *)response->buffer;
	response->icmp_header = (struct icmphdr *)(response->buffer + sizeof(struct iphdr));
	checksum = response->icmp_header->checksum;
	response->icmp_header->checksum = 0;
	if (checksum != icmp_checksum(response->icmp_header, response->size - sizeof(struct iphdr))) {
		return -1;
	}
	return 0;
}

uint16_t icmp_checksum(void* packet, size_t len) {
	u_int16_t* buffer;
	u_int32_t sum;

	sum = 0;
	buffer = (uint16_t*)packet;
	while (len > 1) {
		sum += *buffer;
		if (sum > 0xFFFF) {
			sum = (sum >> 16) + (sum & 0xFFFF);
		}
		++buffer;
		len -= 2;
	}
	if (len == 1) {
		sum += *(u_int8_t*) buffer;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	return (u_int16_t) ~sum;
}