#ifndef LEVEL_LEVEL_H
#define LEVEL_LEVEL_H 1

int UBLC_level_new(unsigned w, unsigned h, unsigned d);
void UBLC_level_delete(void); 

int UBLC_level_istile(unsigned x, unsigned y, unsigned z);
int UBLC_level_issolid(unsigned x, unsigned y, unsigned z);
int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z);

extern unsigned UBLC_level_width;
extern unsigned UBLC_level_height;
extern unsigned UBLC_level_depth;

#endif /* LEVEL_LEVEL_H */
