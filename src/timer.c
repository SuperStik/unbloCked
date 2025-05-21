#include <string.h>
#include <time.h>

#include "timer.h"

#define NS_PER_SECOND 1000000000ll
#define MAX_NS_PER_UPDATE 1000000000ll
#define MAX_TICKS_PER_UPDATE 100

static float tickspersecond;
static struct timespec lasttime;

unsigned long long UBLC_timer_fps = 0;
unsigned UBLC_timer_ticks;
float UBLC_timer_a;
float UBLC_timer_timescale = 1.0f;
float UBLC_timer_passedtime = 0.0f;

static struct timespec diff_timespec(const struct timespec *start, const struct
		timespec *end);

void UBLC_timer_init(float tps) {
	tickspersecond = tps;
	clock_gettime(CLOCK_MONOTONIC, &lasttime);
}

void UBLC_timer_advancetime(void) {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	memcpy(&lasttime, &now, sizeof(struct timespec));

	struct timespec passed = diff_timespec(&lasttime, &now);
	if (passed.tv_sec < 0 || passed.tv_nsec < 0) {
		passed.tv_sec = 0;
		passed.tv_nsec = 0;
	}

	float passedns = (float)passed.tv_nsec + ((float)passed.tv_nsec *
			(float)NS_PER_SECOND);

	if (passed.tv_sec > 0) {
		passed.tv_sec = 1;
		passed.tv_nsec = !!passed.tv_nsec;
	}

	unsigned long long fpsns = passed.tv_nsec + (passed.tv_sec *
			NS_PER_SECOND);

	UBLC_timer_fps = NS_PER_SECOND / fpsns;

	UBLC_timer_passedtime = passedns * UBLC_timer_timescale * tickspersecond
		/ (float)NS_PER_SECOND;
	UBLC_timer_ticks = (unsigned)UBLC_timer_passedtime;
	if (UBLC_timer_ticks > 100)
		UBLC_timer_ticks = 100;

	UBLC_timer_passedtime -= (float)UBLC_timer_ticks;
	UBLC_timer_a = UBLC_timer_passedtime;
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
