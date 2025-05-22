#include <err.h>
#include <stdlib.h>

#include "chunk.h"
#include "levelrenderer.h"
#include "level.h"

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
	free(chunks);
	chunks = NULL;
}
