#ifndef CHRONOS_H
#define CHRONOS_H 1

#include <time.h>

int UBLC_chronos_gettime(struct timespec *start);

int UBLC_chronos_sleeprate(const struct timespec *start, unsigned rate,
		float *sleptfor);

float UBLC_chronos_getdelta(const struct timespec *tickstart, const struct timespec *renderstart, float tickdelta);

#endif /* CHRONOS_H */
