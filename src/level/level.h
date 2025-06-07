#ifndef LEVEL_LEVEL_H
#define LEVEL_LEVEL_H 1

#include <stddef.h>

#include <hitresult.h>
#include <phys/AABB.h>

int UBLC_level_new(unsigned w, unsigned h, unsigned d);
void UBLC_level_delete(void); 

void UBLC_level_save(void);
void UBLC_level_load(void);

void UBLC_level_calclightdepths(unsigned xlo, unsigned zlo, unsigned xhi,
		unsigned zhi);

int UBLC_level_istile(unsigned x, unsigned y, unsigned z);
int UBLC_level_issolid(unsigned x, unsigned y, unsigned z);
int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z);

int UBLC_level_istile_unsafe(unsigned x, unsigned y, unsigned z);
int UBLC_level_issolid_unsafe(unsigned x, unsigned y, unsigned z);
int UBLC_level_islightblocker_unsafe(unsigned x, unsigned y, unsigned z);

const struct UBLC_AABB *UBLC_level_getcubes(struct UBLC_AABB *, size_t *count);

float UBLC_level_getbrightness(unsigned x, unsigned y, unsigned z);
float UBLC_level_getbrightness_unsafe(unsigned x, unsigned y, unsigned z);

int UBLC_level_islit(unsigned x, unsigned y, unsigned z);
int UBLC_level_islit_unsafe(unsigned x, unsigned y, unsigned z);

void UBLC_level_settile(unsigned x, unsigned y, unsigned z, unsigned type);

struct UBLC_hitresult *UBLC_level_clip(struct UBLC_hitresult *, float vstart[3],
		float vend[3]);

int UBLC_level_rdlock(void);
int UBLC_level_wrlock(void);
int UBLC_level_unlock(void);
extern unsigned UBLC_level_width;
extern unsigned UBLC_level_height;
extern unsigned UBLC_level_depth;

#endif /* LEVEL_LEVEL_H */
