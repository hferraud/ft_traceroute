#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>

#include "time.h"

static void normalize_timeval(struct timeval *tv);

struct timeval elapsed_time(struct timeval start, struct timeval end) {
	struct timeval elapsed;

	elapsed.tv_sec = end.tv_sec - start.tv_sec;
	elapsed.tv_usec = end.tv_usec - start.tv_usec;
	normalize_timeval(&elapsed);
	return elapsed;
}

struct timeval get_wait_time(struct timeval *tv, uint8_t wait) {
	struct timeval now;
	struct timeval wait_tv;

	gettimeofday(&now, NULL);
	wait_tv.tv_sec = tv->tv_sec + wait - now.tv_sec;
	wait_tv.tv_usec = tv->tv_usec - now.tv_usec;
	normalize_timeval(&wait_tv);
	return wait_tv;
}

float tv_to_ms(struct timeval tv) {
	return tv.tv_sec * 1000. + tv.tv_usec / 1000.;
}

struct timeval sec_to_tv(const double seconds) {
	struct timeval tv;

	tv.tv_sec = (long)seconds;
	tv.tv_usec = (long)((seconds - tv.tv_sec) * 1000000);
	return tv;
}

static void normalize_timeval(struct timeval *tv) {
	while (tv->tv_usec < 0) {
	    tv->tv_usec += 1000000;
	    tv->tv_sec--;
	}
	while (tv->tv_usec >= 1000000) {
	    tv->tv_usec -= 1000000;
	    tv->tv_sec++;
	}
	if (tv->tv_sec < 0) {
		tv->tv_sec = 0;
		tv->tv_usec = 0;
	}
}