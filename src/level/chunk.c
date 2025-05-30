#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>

#include "chunk.h"
#include "gutl.h"
#include "level.h"
#include "tesselator.h"
#include "../textures.h"
#include "tile.h"
#include "vbuffer.h"

static void rebuild(struct UBLC_chunk *, int layer);

#define BUFFER_COUNT (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

unsigned UBLC_chunk_updates = 0;
static struct UBLC_vbuffer cpuvbo[BUFFER_COUNT];
static long texture = -1;

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

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glEnable(GL_TEXTURE_2D);

	glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, chunk->_buffers[layer]);
	GLenum glerr = glGetError();
	if (glerr)
		warnx("glBindBuffer: %s", GUTL_errorstr(glerr));

	/* TODO: fix this! */
	glVertexPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, x));
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, u));
	glColorPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, r));

	glDrawArrays(GL_QUADS, 0, chunk->indices[layer]);

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

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
	glGenBuffers(2, chunk->_buffers);
	chunk->_dirty = 1;
	pthread_mutex_init(&(chunk->lock), NULL);

	return chunk;
}

void UBLC_chunk_delete(struct UBLC_chunk *chunk) {
	glDeleteBuffers(2, chunk->_buffers);
	pthread_mutex_destroy(&(chunk->lock));
}

static void rebuild(struct UBLC_chunk *chunk, int layer) {
	/*glNewList(chunk->_lists + layer, GL_COMPILE);*/

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	UBLC_level_rdlock();

	size_t bufcount = 0;
	for (unsigned x = chunk->x_lo; x < chunk->x_hi; ++x) {
		for (unsigned y = chunk->y_lo; y < chunk->y_hi; ++y) {
			for (unsigned z = chunk->z_lo; z < chunk->z_hi; ++z) {
				if (!UBLC_level_istile_unsafe(x, y, z))
					continue;

				int tex = (y != ((UBLC_level_depth * 2) / 3));
				size_t c = UBLC_tile_render(&(cpuvbo[bufcount]),
						tex, layer, x, y, z);

				bufcount += c;
			}
		}
	}

	UBLC_level_unlock();

	glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, chunk->_buffers[layer]);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * bufcount,
			cpuvbo, GL_STATIC_DRAW);
	chunk->indices[layer] = bufcount;
	glDisable(GL_TEXTURE_2D);
}

void UBLC_chunk_setdirty(struct UBLC_chunk *chunk) {
	pthread_mutex_lock(&(chunk->lock));

	chunk->_dirty = 1;

	pthread_mutex_unlock(&(chunk->lock));
}
