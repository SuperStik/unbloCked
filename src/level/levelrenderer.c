#include <err.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

#include <SDL3/SDL_opengl.h>

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

void UBLC_levelrenderer_render(struct UBLC_player *player, int layer) {
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

	if (layer)
		return;

	int xlo, ylo, zlo, xhi, yhi, zhi;
	char selected, facing;
	pthread_rwlock_rdlock(&(player->lock));
	xlo = player->xb;
	ylo = player->yb;
	zlo = player->zb;
	selected = player->hasselect;
	facing = player->placeface;
	pthread_rwlock_unlock(&(player->lock));

	if (!selected)
		return;

	xhi = xlo + 1;
	yhi = ylo + 1;
	zhi = zlo + 1;

	int place[3] = {xlo, ylo, zlo};

	switch (facing) {
		case 0:
			--(place[1]);
			break;
		case 1:
			++(place[1]);
			break;
		case 2:
			--(place[2]);
			break;
		case 3:
			++(place[2]);
			break;
		case 4:
			--(place[0]);
			break;
		case 5:
			++(place[0]);
			break;
	}

	glLineWidth(40.0f);

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.5f);
	glVertex3i(xlo, ylo, zlo);
	glColor3f(0.5f, 0.0f, 1.0f);
	glVertex3i(xhi, yhi, zhi);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.5f);
	glVertex3iv(place);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex3i(place[0] + 1, place[1] + 1, place[2] + 1);
	glEnd();

	glFlush();
}

void UBLC_levelrenderer_setdirtyrange(unsigned xlo, unsigned ylo, unsigned zlo,
		unsigned xhi, unsigned yhi, unsigned zhi) {
	if (xlo > xhi) {
		unsigned temp = xhi;
		xhi = xlo;
		xlo = temp;
	}

	if (ylo > yhi) {
		unsigned temp = yhi;
		yhi = ylo;
		ylo = temp;
	}

	if (zlo > zhi) {
		unsigned temp = zhi;
		zhi = zlo;
		zlo = temp;
	}

	xlo /= 16u;
	ylo /= 16u;
	zlo /= 16u;
	xhi /= 16u;
	yhi /= 16u;
	zhi /= 16u;

	if (xhi >= xchunks)
		xhi = xchunks - 1;

	if (yhi >= ychunks)
		yhi = ychunks - 1;

	if (zhi >= zchunks)
		zhi = zchunks - 1;

	if (chunks == NULL)
		errx(2, "no chunks yet");

	for (unsigned x = xlo; x <= xhi; ++x) {
		for (unsigned y = ylo; y <= yhi; ++y) {
			for (unsigned z = zlo; z <= zhi; ++z) {
				UBLC_chunk_setdirty(&(chunks[(x + y * xchunks) *
							zchunks + z]));
			}
		}
	}
}

void UBLC_levelrenderer_setdirty(unsigned x, unsigned y, unsigned z) {
	unsigned xlo, ylo, zlo, xhi, yhi, zhi;

	if (x != 0)
		xlo = x - 1;
	else
		xlo = 0;

	if (y != 0)
		ylo = y - 1;
	else
		ylo = 0;

	if (z != 0)
		zlo = z - 1;
	else
		zlo = 0;

	xhi = x + 1;
	yhi = y + 1;
	zhi = z + 1;

	UBLC_levelrenderer_setdirtyrange(xlo, ylo, zlo, xhi, yhi, zhi);
}
