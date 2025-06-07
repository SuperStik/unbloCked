#ifndef CHRONOS_H
#define CHRONOS_H 1

#include <time.h>

int UBLC_chronos_gettime(struct timespec *start);
struct timespec UBLC_chronos_getstart(void);
int UBLC_chronos_setstart(void);

int UBLC_chronos_sleeprate(unsigned rate);

float UBLC_chronos_getdelta(void);

#endif /* CHRONOS_H */
