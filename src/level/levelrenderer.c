#include <err.h>
#include <math.h>
#include <stdlib.h>

#include <OpenGL/gl.h>

#include "chunk.h"
#include "frustum.h"
#include "gccvec.h"
#include "levelrenderer.h"
#include "level.h"
#include <phys/AABB.h>
#include "tesselator.h"
#include "tile.h"

#define CHUNK_SIZE 16

static unsigned xchunks;
static unsigned ychunks;
static unsigned zchunks;

static struct UBLC_chunk *chunks;

void UBLC_levelrenderer_init(void) {
	xchunks = UBLC_level_width / CHUNK_SIZE;
	ychunks = UBLC_level_depth / CHUNK_SIZE;
	zchunks = UBLC_level_height / CHUNK_SIZE;

	size_t totalchunks = (size_t)xchunks * ychunks * zchunks;
	chunks = malloc(totalchunks * sizeof(*chunks));
	if (chunks == NULL)
		err(2, "malloc");

	for (unsigned x = 0; x < xchunks; ++x) {
		for (unsigned y = 0; y < ychunks; ++y) {
			for (unsigned z = 0; z < zchunks; ++z) {
				unsigned xlo = x * 16;
				unsigned ylo = y * 16;
				unsigned zlo = z * 16;
				unsigned xhi = (x + 1) * 16;
				unsigned yhi = (y + 1) * 16;
				unsigned zhi = (z + 1) * 16;

				if (xhi > UBLC_level_width)
					xhi = UBLC_level_width;
				if (yhi > UBLC_level_depth)
					yhi = UBLC_level_depth;
				if (zhi > UBLC_level_height)
					zhi = UBLC_level_height;

				UBLC_chunk_init(&(chunks[(x + y * xchunks) *
						zchunks + z]), xlo, ylo, zlo,
						xhi, yhi, zhi);
			}
		}
	}
}

void UBLC_levelrenderer_delete(void) {
	for (size_t i = 0; i < ((size_t)xchunks * ychunks * zchunks); ++i)
		UBLC_chunk_delete(chunks + i);

	free(chunks);
	chunks = NULL;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void UBLC_levelrenderer_render(struct UBLC_player *player, int layer) {
	UBLC_chunk_rebuilt_this_frame = 0;

	gvec(float,4) frustum[6];
	UBLC_frustum_get(frustum);

	size_t totalchunks = (size_t)xchunks * ychunks * zchunks;
	for (size_t i = 0; i < totalchunks; ++i) {
		struct UBLC_AABB aabb = chunks[i].aabb;
		if (UBLC_frustum_hascube(frustum, aabb.x_lo, aabb.y_lo,
					aabb.z_lo, aabb.x_hi, aabb.y_hi,
					aabb.z_hi))
			UBLC_chunk_render(chunks + i, layer);
	}

	float xlo, ylo, zlo, xhi, yhi, zhi;

	gvec(float,4) offsetp;
	gvec(float,4) offsets;

	pthread_rwlock_rdlock(&(player->lock));
	gvec(float,4) offset = {player->xb, player->yb, player->zb, 0.0f};
	offsetp = offset * player->place;
	gvec(float,4) pos = {player->x, player->y, player->z, 0.0f};
	offsetp += pos;
	offsets = offset * player->smash;
	offsets += pos;
	pthread_rwlock_unlock(&(player->lock));

	//warnx("%i %i %i %g %g %g", x, y, z, player->x, player->y, player->z);
	glPointSize(5.0f);
	glLineWidth(40.0f);

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.5f);
	glVertex3f(floor(offsetp[0]), floor(offsetp[1]), floor(offsetp[2]));
	glColor3f(0.5f, 0.0f, 1.0f);
	glVertex3f(ceil(offsetp[0]), ceil(offsetp[1]), ceil(offsetp[2]));
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.5f);
	glVertex3f(floor(offsets[0]), floor(offsets[1]), floor(offsets[2]));
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex3f(ceil(offsets[0]), ceil(offsets[1]), ceil(offsets[2]));
	glEnd();
	glFlush();
}

void UBLC_levelrenderer_pick(struct UBLC_player *player) {
	float r = 1.0f;
	struct UBLC_AABB box = player->aabb;
	UBLC_AABB_grow(&box, r, r, r);
	int xlo = (int)box.x_lo;
	int xhi = (int)(box.x_hi + 1.0f);
	int ylo = (int)box.y_lo;
	int yhi = (int)(box.y_hi + 1.0f);
	int zlo = (int)box.z_lo;
	int zhi = (int)(box.z_hi + 1.0f);
	glInitNames();

	//warnx("%i %i %i %i %i %i", xlo, xhi, ylo, yhi, zlo, zhi);

	for (int x = xlo; x < xhi; ++x) {
		glPushName(x);

		for (int y = ylo; y < yhi; ++y) {
			glPushName(y);

			for (int z = zlo; z < zhi; ++z) {
				glPushName(z);

				if (!UBLC_level_issolid(x, y, z)) {
					glPopName();
					continue;
				}

				glPushName(0);

				for (int i = 0; i < 6; ++i) {
					glPushName(i);

					UBLC_tesselator_init();
					UBLC_tile_renderface(x, y, z, i);
					UBLC_tesselator_flush();

					glPopName();
				}

				glPopName();
			}

			glPopName();
		}

		glPopName();
	}
}

#pragma GCC diagnostic pop
