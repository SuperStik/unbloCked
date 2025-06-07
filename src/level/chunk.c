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

void UBLC_chunk_render(struct UBLC_chunk *chunk, int layer) {
	if (__atomic_load_n(&(chunk->_dirty), __ATOMIC_ACQUIRE)) {
		__atomic_add_fetch(&UBLC_chunk_updates, 1ul, __ATOMIC_RELAXED);

		rebuild(chunk, 0);
		rebuild(chunk, 1);

		__atomic_store_n(&(chunk->_dirty), 0, __ATOMIC_RELEASE);
	}

	glBindBuffer(GL_ARRAY_BUFFER, chunk->_buffers[layer]);
	GLenum glerr = glGetError();
	if (glerr)
		warnx("glBindBuffer: %s", GUTL_errorstr(glerr));

	glVertexPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, x));
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, u));
	glColorPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, r));

	glDrawArrays(GL_QUADS, 0, chunk->indices[layer]);
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

	return chunk;
}

void UBLC_chunk_delete(struct UBLC_chunk *chunk) {
	glDeleteBuffers(2, chunk->_buffers);
}

static void rebuild(struct UBLC_chunk *chunk, int layer) {
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

	glBindBuffer(GL_ARRAY_BUFFER, chunk->_buffers[layer]);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * bufcount,
			cpuvbo, GL_STATIC_DRAW);
	chunk->indices[layer] = bufcount;
}

void UBLC_chunk_setdirty(struct UBLC_chunk *chunk) {
	__atomic_store_n(&(chunk->_dirty), 1, __ATOMIC_RELEASE);
}
