#include <err.h>
#include <pthread.h>

#include "chronos.h"

#define NS_PER_SECOND 1000000000.0f

static float time2float(const struct timespec *t);
static struct timespec diff_timespec(const struct timespec *start, const struct
		timespec *end);

static pthread_rwlock_t deltalock = PTHREAD_RWLOCK_INITIALIZER;
static struct timespec starttime = {.tv_sec = 0, .tv_nsec = 0};
static float delta;

int UBLC_chronos_gettime(struct timespec *curtime) {
	return clock_gettime(CLOCK_UPTIME_RAW, curtime);
}

struct timespec UBLC_chronos_getstart(void) {
	pthread_rwlock_rdlock(&deltalock);

	struct timespec start = starttime;

	pthread_rwlock_unlock(&deltalock);

	return start;
}

int UBLC_chronos_setstart(void) {
	pthread_rwlock_wrlock(&deltalock);

	int ret = UBLC_chronos_gettime(&starttime);

	pthread_rwlock_unlock(&deltalock);

	return ret;
}

int UBLC_chronos_sleeprate(unsigned rate) {
	if (rate == 0)
		return -1;

	struct timespec start, end;
	if (UBLC_chronos_gettime(&end))
		return -1;

	start = UBLC_chronos_getstart();

	struct timespec deltasleep = diff_timespec(&start, &end);
	struct timespec ideal = {.tv_sec = 0, .tv_nsec = 999999999 / rate};

	struct timespec sleeptime = diff_timespec(&deltasleep, &ideal);

	int ret = nanosleep(&sleeptime, NULL);

	pthread_rwlock_wrlock(&deltalock);

	delta = time2float(&sleeptime);

	pthread_rwlock_unlock(&deltalock);

	return ret;
}

float UBLC_chronos_getdelta(void) {
	struct timespec lerp;
	UBLC_chronos_gettime(&lerp);

	pthread_rwlock_rdlock(&deltalock);

	struct timespec diff = diff_timespec(&starttime, &lerp);

	float tickdelta = delta;

	pthread_rwlock_unlock(&deltalock);

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
