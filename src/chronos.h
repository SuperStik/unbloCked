#ifndef CHRONOS_H
#define CHRONOS_H 1

#include <time.h>

int UBLC_chronos_initialtime(struct timespec *start);

int UBLC_chronos_sleeprate(const struct timespec *start, unsigned rate,
		float *sleptfor);

#endif /* CHRONOS_H */
