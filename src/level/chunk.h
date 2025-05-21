#ifndef UBLC_LEVEL_CHUNK_H
#define UBLC_LEVEL_CHUNK_H 1

#include <phys/AABB.h>

struct UBLC_chunk {
	struct UBLC_AABB aabb;
	int x_lo;
	int y_lo;
	int z_lo;
	int x_hi;
	int y_hi;
	int z_hi;
	unsigned _lists;
	unsigned char _dirty:1;
};

extern int UBLC_chunk_rebuilt_this_frame;
extern unsigned UBLC_chunk_updates;

void UBLC_chunk_initstatic(void);

struct UBLC_chunk *UBLC_chunk_init(struct UBLC_chunk *, int x_lo, int y_lo, int
		z_lo, int x_hi, int y_hi, int z_hi);

void UBLC_chunk_setdirty(struct UBLC_chunk *);

#endif /* UBLC_LEVEL_CHUNK_H */
