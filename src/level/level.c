#include <err.h>
#include <math.h>
#include <stdlib.h>

#include "level.h"

unsigned UBLC_level_width;
unsigned UBLC_level_height;
unsigned UBLC_level_depth;

static unsigned char *blocks;
static unsigned *light_depths;
static struct UBLC_AABB *cubes;
static size_t numcubes;

int UBLC_level_new(unsigned w, unsigned h, unsigned d) {
	if (blocks != NULL || light_depths != NULL)
		return -1;

	UBLC_level_width = w;
	UBLC_level_height = h;
	UBLC_level_depth = d;

	blocks = malloc((size_t)w * h * d * sizeof(unsigned char));
	if (blocks == NULL)
		return -1;

	light_depths = calloc(w * h, sizeof(unsigned char));
	if (light_depths == NULL) {
		free(blocks);
		return -1;
	}

	cubes = malloc(w * h * d * sizeof(struct UBLC_AABB));
	if (cubes == NULL) {
		free(blocks);
		free(light_depths);
		return -1;
	}

	for (unsigned x = 0; x < w; ++x) {
		for (unsigned y = 0; y < d; ++y) {
			for (unsigned z = 0; z < h; ++z) {
				size_t i = (y * h + z) * w + x;
				blocks[i] = (unsigned char)(y <= ((d * 2) / 3));
			}
		}
	}

	return 0;
}

void UBLC_level_delete(void) {
	free(blocks);
	free(light_depths);
	free(cubes);

	blocks = NULL;
	light_depths = NULL;
	cubes = NULL;
}

void UBLC_level_calclightdepths(unsigned xlo, unsigned zlo, unsigned xhi,
		unsigned zhi) {
	for (unsigned x = xlo; x < xlo + xhi; ++x) {
		for (unsigned z = zlo; z < zlo + zhi; ++z) {
			unsigned olddepth = light_depths[x + z *
				UBLC_level_width];

			unsigned y;
			for (y = UBLC_level_depth - 1; y > 0 &&
					!UBLC_level_islightblocker(x, y, z);
					--y);

			light_depths[x + z * UBLC_level_width] = y;
			if (olddepth == y)
				continue;

			unsigned yl_lo = (olddepth < y) ? olddepth : y;
			unsigned yl_hi = (olddepth > y) ? olddepth : y;
		}
	}
}

int UBLC_level_istile(unsigned x, unsigned y, unsigned z) {
	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return 0;

	return blocks[(y * UBLC_level_height + z) * UBLC_level_width + x] != 0;
}

int UBLC_level_issolid(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_istile(x, y, z);
}

int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_issolid(x, y, z);
}

const struct UBLC_AABB *UBLC_level_getcubes(struct UBLC_AABB *aabb,
		size_t *count) {
	float xlo = floorf(aabb->x_lo);
	float xhi = floorf(aabb->x_hi + 1.0f);
	float ylo = floorf(aabb->y_lo);
	float yhi = floorf(aabb->y_hi + 1.0f);
	float zlo = floorf(aabb->z_lo);
	float zhi = floorf(aabb->z_hi + 1.0f);

	if (xlo < 0.0f)
		xlo = 0.0f;

	if (ylo < 0.0f)
		ylo = 0.0f;

	if (zlo < 0.0f)
		zlo = 0.0f;

	float width = (float)UBLC_level_width;
	if (xhi > width)
		xhi = width;

	float depth = (float)UBLC_level_depth;
	if (yhi > depth)
		yhi = depth;

	float height = (float)UBLC_level_height;
	if (zhi > height)
		zhi = height;

	numcubes = 0;

	for (float x = xlo; x < xhi; x += 1.0f) {
		for (float y = ylo; y < yhi; y += 1.0f) {
			for (float z = zlo; z < zhi; z += 1.0f) {
				if (!UBLC_level_issolid(x,y,z))
					continue;

				UBLC_AABB_INIT(&cubes[numcubes], x, y, z, x +
						1, y + 1, z + 1);
				++numcubes;
			}
		}
	}

	*count = numcubes;
	return cubes;
}

float UBLC_level_getbrightness(unsigned x, unsigned y, unsigned z) {
	float dark = 0.8f;
	float light = 1.0f;

	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return light;

	return y < light_depths[x + z * UBLC_level_width] ? dark : light;

}

void UBLC_level_settile(unsigned x, unsigned y, unsigned z, unsigned type) {
	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return;

	blocks[(y * UBLC_level_height + z) * UBLC_level_width + x] = type;
}
