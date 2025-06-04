#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
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
#include "textures.h"
#include "tile.h"

static struct UBLC_chunk *chunks;

static unsigned xchunks;
static unsigned ychunks;
static unsigned zchunks;

static unsigned selectbuffer;
static unsigned terrain;

void UBLC_levelrenderer_initstatic(void) {
	const char *texstr = "textures/terrain.png";
	long tex = UBLC_textures_loadtexture(texstr, GL_NEAREST);
	if (tex < 0)
		err(2, "Cannot allocate texture: %s", texstr);

	terrain = tex;
}

void UBLC_levelrenderer_destroystatic(void) {
	glDeleteTextures(1, &terrain);
}

void UBLC_levelrenderer_init(void) {
	xchunks = UBLC_level_width / CHUNK_SIZE;
	ychunks = UBLC_level_depth / CHUNK_SIZE;
	zchunks = UBLC_level_height / CHUNK_SIZE;

	size_t totalchunks = (size_t)xchunks * ychunks * zchunks;
	chunks = malloc(totalchunks * sizeof(*chunks));
	if (chunks == NULL)
		err(2, "malloc");

	glGenBuffers(1, &selectbuffer);

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

	glDeleteBuffers(1, &selectbuffer);

	free(chunks);
	chunks = NULL;
}

void UBLC_levelrenderer_render(struct UBLC_player *player, int layer) {
	gvec(float,4) frustum[6];
	UBLC_frustum_get(frustum);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, terrain);
	size_t totalchunks = (size_t)xchunks * ychunks * zchunks;
	for (size_t i = 0; i < totalchunks; ++i)
		UBLC_chunk_render(&(chunks[i]), layer);
	glDisable(GL_TEXTURE_2D);

	if (layer)
		return;

	int xlo, ylo, zlo, xhi, yhi, zhi;
	char selected, facing;
	pthread_rwlock_rdlock(&(player->ent.lock));
	xlo = player->xb;
	ylo = player->yb;
	zlo = player->zb;
	selected = player->hasselect;
	facing = player->placeface;
	pthread_rwlock_unlock(&(player->ent.lock));

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

	struct UBLC_vbuffer buf[4];

	UBLC_tile_renderface(buf, xlo, ylo, zlo, facing);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, selectbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 4, buf,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, selectbuffer);
	glVertexPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, x));
	glColorPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, r));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_BLEND);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

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

	xlo /= CHUNK_SIZE;
	ylo /= CHUNK_SIZE;
	zlo /= CHUNK_SIZE;
	xhi /= CHUNK_SIZE;
	yhi /= CHUNK_SIZE;
	zhi /= CHUNK_SIZE;

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
