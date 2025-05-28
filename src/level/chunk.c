#include <err.h>
#include <SDL3/SDL_opengl.h>

#include "chunk.h"
#include "level.h"
#include "tesselator.h"
#include "../textures.h"
#include "tile.h"

static long texture = -1;
unsigned UBLC_chunk_updates = 0;

static void rebuild(struct UBLC_chunk *, int layer);

void UBLC_chunk_initstatic(void) {
	texture = UBLC_textures_loadtexture("textures/terrain.png", GL_NEAREST);
}

void UBLC_chunk_render(struct UBLC_chunk *chunk, int layer) {
	pthread_mutex_lock(&(chunk->lock));

	if (chunk->_dirty) {
		__atomic_add_fetch(&UBLC_chunk_updates, 1ul, __ATOMIC_RELAXED);
		rebuild(chunk, 0);
		rebuild(chunk, 1);
		chunk->_dirty = 0;
	}

	pthread_mutex_unlock(&(chunk->lock));

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
	pthread_mutex_init(&(chunk->lock), NULL);

	return chunk;
}

void UBLC_chunk_delete(struct UBLC_chunk *chunk) {
	glDeleteLists(chunk->_lists, 2);
	pthread_mutex_destroy(&(chunk->lock));
}

static void rebuild(struct UBLC_chunk *chunk, int layer) {
	glNewList(chunk->_lists + layer, GL_COMPILE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	UBLC_tesselator_init();

	UBLC_level_rdlock();

	for (unsigned x = chunk->x_lo; x < chunk->x_hi; ++x) {
		for (unsigned y = chunk->y_lo; y < chunk->y_hi; ++y) {
			for (unsigned z = chunk->z_lo; z < chunk->z_hi; ++z) {
				if (!UBLC_level_istile_unsafe(x, y, z))
					continue;

				int tex = (y != ((UBLC_level_depth * 2) / 3));
				UBLC_tile_render(tex, layer, x, y, z);
			}
		}
	}

	UBLC_level_unlock();

	UBLC_tesselator_flush();
	glDisable(GL_TEXTURE_2D);
	glEndList();
}

void UBLC_chunk_setdirty(struct UBLC_chunk *chunk) {
	pthread_mutex_lock(&(chunk->lock));

	chunk->_dirty = 1;

	pthread_mutex_unlock(&(chunk->lock));
}
