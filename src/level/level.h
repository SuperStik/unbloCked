#ifndef LEVEL_H
#define LEVEL_H 1

int UBLC_level_new(unsigned w, unsigned h, unsigned d);
void UBLC_level_delete(void); 

int UBLC_level_istile(unsigned x, unsigned y, unsigned z);
int UBLC_level_issolid(unsigned x, unsigned y, unsigned z);
int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z);

#endif /* LEVEL_H */
