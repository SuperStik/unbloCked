#ifndef LEVEL_LEVEL_H
#define LEVEL_LEVEL_H 1

#include <phys/AABB.h>

int UBLC_level_new(unsigned w, unsigned h, unsigned d);
void UBLC_level_delete(void); 

int UBLC_level_istile(unsigned x, unsigned y, unsigned z);
int UBLC_level_issolid(unsigned x, unsigned y, unsigned z);
int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z);
const struct UBLC_AABB *UBLC_level_getcubes(struct UBLC_AABB *, size_t *count);
float UBLC_level_getbrightness(unsigned x, unsigned y, unsigned z);
void UBLC_level_settile(unsigned x, unsigned y, unsigned z, unsigned type);

extern unsigned UBLC_level_width;
extern unsigned UBLC_level_height;
extern unsigned UBLC_level_depth;

#endif /* LEVEL_LEVEL_H */
