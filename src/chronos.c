#include <err.h>

#include "chronos.h"

#define NS_PER_SECOND 1000000000.0f

static float time2float(const struct timespec *t);
static struct timespec diff_timespec(const struct timespec *start, const struct
		timespec *end);

int UBLC_chronos_gettime(struct timespec *start) {
	return clock_gettime(CLOCK_UPTIME_RAW, start);
}

int UBLC_chronos_sleeprate(const struct timespec *start, unsigned rate,
		float *sleptfor) {
	if (rate == 0)
		return -1;

	struct timespec end;
	if (UBLC_chronos_gettime(&end))
		return -1;

	struct timespec delta = diff_timespec(start, &end);
	struct timespec ideal = {.tv_sec = 0, .tv_nsec = 999999999 / rate};

	struct timespec sleeptime = diff_timespec(&delta, &ideal);

	int ret = nanosleep(&sleeptime, NULL);

	*sleptfor = time2float(&sleeptime);

	return ret;
}

float UBLC_chronos_getdelta(const struct timespec *tickstart, const struct timespec *renderstart, float tickdelta) {
	struct timespec diff = diff_timespec(tickstart, renderstart);

	return time2float(&diff) / tickdelta;
}

static float time2float(const struct timespec *t) {
	return (float)t->tv_sec + ((float)t->tv_nsec / (NS_PER_SECOND * 10.0f));
}

static struct timespec diff_timespec(const struct timespec *start, const struct
		timespec *end) {
	struct timespec diff = {
		.tv_sec = end->tv_sec - start->tv_sec,
		.tv_nsec = end->tv_nsec - start->tv_nsec
	};

	if (diff.tv_nsec < 0) {
		diff.tv_nsec += NS_PER_SECOND;
		--diff.tv_sec;
	}

	return diff;
}
