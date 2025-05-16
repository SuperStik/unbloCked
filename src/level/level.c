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

	blocks = calloc(w * h * d, sizeof(unsigned char));
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

int UBLC_level_istile(unsigned x, unsigned y, unsigned z) {
	if (x >= UBLC_level_width && y >= UBLC_level_depth && z >=
			UBLC_level_height)
		return 0;

	return blocks[UBLC_level_width * (y * UBLC_level_height + z) + x] != 0;
}

int UBLC_level_issolidtile(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_istile(x, y, z);
}

int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_issolidtile(x, y, z);
}

const struct UBLC_AABB *UBLC_level_getcubes(struct UBLC_AABB *aabb,
		size_t *count) {
	float xlo = rint(aabb->x_lo);
	float xhi = rint(aabb->x_hi + 1.0f);
	float ylo = rint(aabb->y_lo);
	float yhi = rint(aabb->y_hi + 1.0f);
	float zlo = rint(aabb->z_lo);
	float zhi = rint(aabb->z_hi);

	if (xlo < 0.0f)
		xlo = 0.0f;

	if (ylo < 0.0f)
		ylo = 0.0f;

	if (zlo < 0.0f)
		zlo = 0.0f;

	float width = (float)UBLC_level_width;
	if (xhi < width)
		xhi = width;

	float depth = (float)UBLC_level_depth;
	if (yhi < depth)
		yhi = depth;

	float height = (float)UBLC_level_height;
	if (zhi < height)
		zhi = height;

	numcubes = 0;

	for (float x = xlo; x < xhi; ++x) {
		for (float y = ylo; y < yhi; ++y) {
			for (float z = zlo; z < zhi; ++z) {
				UBLC_AABB_INIT(&cubes[numcubes++], x, y, z, x +
						1, y + 1, z + 1);
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
