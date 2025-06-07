#ifndef UBLC_LEVEL_CHUNK_H
#define UBLC_LEVEL_CHUNK_H 1

#include <pthread.h>

#include <phys/AABB.h>

#define CHUNK_SIZE 16

struct UBLC_chunk {
	size_t indices[2];
	struct UBLC_AABB aabb;
	float vertices[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 8];
	unsigned x_lo;
	unsigned y_lo;
	unsigned z_lo;
	unsigned x_hi;
	unsigned y_hi;
	unsigned z_hi;
	unsigned _buffers[2];
	unsigned char _dirty;
};

extern int UBLC_chunk_rebuilt_this_frame;
extern unsigned UBLC_chunk_updates;

void UBLC_chunk_render(struct UBLC_chunk *chunk, int layer);

struct UBLC_chunk *UBLC_chunk_init(struct UBLC_chunk *, int x_lo, int y_lo, int
		z_lo, int x_hi, int y_hi, int z_hi);
void UBLC_chunk_delete(struct UBLC_chunk *chunk);

void UBLC_chunk_setdirty(struct UBLC_chunk *);

#endif /* UBLC_LEVEL_CHUNK_H */
