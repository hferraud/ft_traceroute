#ifndef PING_TIME_H
#define PING_TIME_H

struct	        timeval elapsed_time(struct timeval start, struct timeval end);
struct timeval  get_wait_time(struct timeval *tv, uint8_t wait);
float	        tv_to_ms(struct timeval tv);
struct timeval  sec_to_tv(double seconds);

#endif
