#include <OpenGL/gl.h>

#include "chunk.h"
#include "../textures.h"

static long texture = -1;
int UBLC_chunk_rebuilt_this_frame = 0;
unsigned UBLC_chunk_updates = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void UBLC_chunk_initstatic(void) {
	texture = UBLC_textures_loadtexture("textures/terrain.png", GL_NEAREST);
}

struct UBLC_chunk *UBLC_chunk_init(struct UBLC_chunk *chunk, int x_lo, int y_lo,
		int z_lo, int x_hi, int y_hi, int z_hi) {
	UBLC_AABB_INIT(&(chunk->aabb), (float)x_lo, (float)y_lo, (float)z_lo,
			(float)x_hi, (float)y_hi, (float)z_hi);
	chunk->x_lo = x_lo;
	chunk->y_lo = y_lo;
	chunk->z_lo = z_lo;
	chunk->x_hi = x_hi;
	chunk->y_hi = y_hi;
	chunk->z_hi = z_hi;
	chunk->_lists = glGenLists(2);
	chunk->_dirty = 1;
	return chunk;
}

void UBLC_chunk_destroy(struct UBLC_chunk *chunk) {
	glDeleteLists(chunk->_lists, 2);
}

#pragma GCC diagnostic pop

void UBLC_chunk_setdirty(struct UBLC_chunk *chunk) {
	chunk->_dirty = 1;
}
