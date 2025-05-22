#include <OpenGL/gl.h>

#include "chunk.h"
#include "level.h"
#include "tesselator.h"
#include "../textures.h"
#include "tile.h"

static long texture = -1;
int UBLC_chunk_rebuilt_this_frame = 0;
unsigned UBLC_chunk_updates = 0;

static void rebuild(struct UBLC_chunk *, int layer);

void UBLC_chunk_initstatic(void) {
	texture = UBLC_textures_loadtexture("textures/terrain.png", GL_NEAREST);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void UBLC_chunk_render(struct UBLC_chunk *chunk, int layer) {
	if (chunk->_dirty) {
		rebuild(chunk, 0);
		rebuild(chunk, 1);
	}

	glCallList(chunk->_lists + layer);
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

void UBLC_chunk_delete(struct UBLC_chunk *chunk) {
	glDeleteLists(chunk->_lists, 2);
}

static void rebuild(struct UBLC_chunk *chunk, int layer) {
	if (UBLC_chunk_rebuilt_this_frame == 2)
		return;

	chunk->_dirty = 0;
	++UBLC_chunk_updates;
	++UBLC_chunk_rebuilt_this_frame;

	glNewList(chunk->_lists + layer, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	UBLC_tesselator_init();

	for (int x = chunk->x_lo; x < chunk->x_hi; ++x) {
		for (int y = chunk->y_lo; y < chunk->y_hi; ++y) {
			for (int z = chunk->z_lo; z < chunk->z_hi; ++z) {
				if (!UBLC_level_istile(x, y, z))
					continue;

				int tex = (y != ((UBLC_level_depth * 2) / 3));
				UBLC_tile_render(tex, layer, x, y, z);
			}
		}
	}

	UBLC_tesselator_flush();
	glDisable(GL_TEXTURE_2D);
	glEndList();
}

#pragma GCC diagnostic pop

void UBLC_chunk_setdirty(struct UBLC_chunk *chunk) {
	chunk->_dirty = 1;
}
