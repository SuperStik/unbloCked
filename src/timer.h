#ifndef TIMER_H
#define TIMER_H 1

extern unsigned long long UBLC_timer_fps;
extern unsigned UBLC_timer_ticks;
extern float UBLC_timer_a;
extern float UBLC_timer_timescale;
extern float UBLC_timer_passedtime;

void UBLC_timer_init(float tickspersecond);
void UBLC_timer_advancetime(void);

#endif /* TIMER_H */
