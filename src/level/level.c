#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "level.h"
#include "levelrenderer.h"
#include "resources.h"

unsigned UBLC_level_width;
unsigned UBLC_level_height;
unsigned UBLC_level_depth;

static unsigned char *blocks;
static unsigned *light_depths;
static pthread_rwlock_t levellock = PTHREAD_RWLOCK_INITIALIZER;
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

	light_depths = malloc(w * h * sizeof(unsigned));
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

	UBLC_level_wrlock();

	for (unsigned x = 0; x < w; ++x) {
		for (unsigned y = 0; y < d; ++y) {
			for (unsigned z = 0; z < h; ++z) {
				size_t i = (y * h + z) * w + x;
				blocks[i] = (unsigned char)(y <= ((d * 2) / 3));
			}
		}
	}

	UBLC_level_load();

	UBLC_level_calclightdepths(0, 0, w, h);

	UBLC_level_unlock();

	UBLC_levelrenderer_init();

	return 0;
}

void UBLC_level_delete(void) {
	UBLC_level_wrlock();

	free(blocks);
	free(light_depths);
	free(cubes);

	blocks = NULL;
	light_depths = NULL;
	cubes = NULL;

	UBLC_level_unlock();

	UBLC_levelrenderer_delete();
}

void UBLC_level_save() {
	int save = openat(UBLC_fs.pref, "level.dat", O_WRONLY | O_CREAT |
			O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (save < 0)
		err(2, "openat: %i %s", UBLC_fs.pref, "level.dat");

	UBLC_level_rdlock();

	size_t worldsize = (size_t)UBLC_level_width * UBLC_level_height *
		UBLC_level_depth;

	if (write(save, blocks, worldsize) < 0)
		err(2, "write: %i", save);

	UBLC_level_unlock();

	if (close(save))
		warn("close: %i", save);
}

/* TODO: thread safety */
void UBLC_level_load(void) {
	int save = openat(UBLC_fs.pref, "level.dat", O_RDONLY);

	if (save < 0 && errno != ENOENT)
		err(2, "openat: %i %s", UBLC_fs.pref, "level.dat");
	else if (save < 0)
		return;

	size_t worldsize = (size_t)UBLC_level_width * UBLC_level_height *
		UBLC_level_depth;

	void *level = mmap(NULL, worldsize, PROT_READ, MAP_PRIVATE, save, 0);
	if (level == MAP_FAILED)
		err(2, "mmap: %i", save);

	memcpy(blocks, level, worldsize);

	if (munmap(level, worldsize))
		warn("munmap: %i", save);

	if (close(save))
		warn("close: %i", save);
}

void UBLC_level_calclightdepths(unsigned xlo, unsigned zlo, unsigned xhi,
		unsigned zhi) {
	if (xlo >= UBLC_level_width || zlo >= UBLC_level_height) {
		warnx("bad light coordinates: <%u, %u>", xlo, zlo);
		return;
	}

	if ((zlo + xhi) >= UBLC_level_width)
		xhi = (UBLC_level_width - 1) - xlo;

	if ((zlo + zhi) >= UBLC_level_height)
		zhi = (UBLC_level_height - 1) - zlo;

	unsigned ylo = UBLC_level_depth;
	for (unsigned x = xlo; x <= xlo + xhi; ++x) {
		for (unsigned z = zlo; z <= zlo + zhi; ++z) {
			unsigned y;
			for (y = UBLC_level_depth - 1; y > 0 &&
					!UBLC_level_islightblocker_unsafe(x, y,
						z);
					--y);

			light_depths[x + z * UBLC_level_width] = y;
		}
	}
}

int UBLC_level_istile(unsigned x, unsigned y, unsigned z) {
	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return 0;

	int istile;
	UBLC_level_rdlock();

	istile = blocks[(y * UBLC_level_height + z) * UBLC_level_width + x] !=
		0;
	UBLC_level_unlock();

	return istile;
}

int UBLC_level_istile_unsafe(unsigned x, unsigned y, unsigned z) {
	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return 0;

	return blocks[(y * UBLC_level_height + z) * UBLC_level_width + x] != 0;
}

int UBLC_level_issolid(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_istile(x, y, z);
}

int UBLC_level_issolid_unsafe(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_istile_unsafe(x, y, z);
}

int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_issolid(x, y, z);
}

int UBLC_level_islightblocker_unsafe(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_issolid_unsafe(x, y, z);
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

	/* TODO: accept local array to make reentrant */
	UBLC_level_wrlock();

	for (float x = xlo; x < xhi; x += 1.0f) {
		for (float y = ylo; y < yhi; y += 1.0f) {
			for (float z = zlo; z < zhi; z += 1.0f) {
				if (!UBLC_level_issolid_unsafe(x,y,z))
					continue;

				UBLC_AABB_INIT(&cubes[numcubes], x, y, z, x +
						1, y + 1, z + 1);
				++numcubes;
			}
		}
	}

	*count = numcubes;

	UBLC_level_unlock();

	return cubes;
}

float UBLC_level_getbrightness(unsigned x, unsigned y, unsigned z) {
	float dark = 0.8f;
	float light = 1.0f;

	if (x >= UBLC_level_width || y >= UBLC_level_depth || z >=
			UBLC_level_height)
		return light;

	float brightness;

	UBLC_level_rdlock();

	brightness = y < light_depths[x + z * UBLC_level_width] ? dark : light;

	UBLC_level_unlock();

	return brightness;
}

float UBLC_level_getbrightness_unsafe(unsigned x, unsigned y, unsigned z) {
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

	UBLC_level_wrlock();
	unsigned ylo = y;
	if (ylo > 0)
		--ylo;

	blocks[(y * UBLC_level_height + z) * UBLC_level_width + x] = type;
	UBLC_level_calclightdepths(x, z, 1, 1);

	for (; ylo > 0; --ylo) {
		if (UBLC_level_islightblocker_unsafe(x, ylo, z))
			break;
	}

	UBLC_level_unlock();

	__builtin_prefetch(&(blocks[(y * UBLC_level_height + z) *
				UBLC_level_width + x]), 0, 2);

	unsigned xlo = x;
	unsigned zlo = z;

	if (xlo != 0)
		--xlo;

	if (zlo != 0)
		--zlo;

	UBLC_levelrenderer_setdirtyrange(xlo, ylo, zlo, x + 1, y + 1, z + 1);
}

struct UBLC_hitresult *UBLC_level_clip(struct UBLC_hitresult *hit, float
		vstart[3], float vend[3]) {
	if (isnan(vstart[0]) || isnan(vstart[1]) || isnan(vstart[2])) {
		hit->hit = 0;
		return hit;
	}
	if (isnan(vend[0]) || isnan(vend[1]) || isnan(vend[2])) {
		hit->hit = 0;
		return hit;
	}

	long ilo[3] = {
		(long)vstart[0],
		(long)vstart[1],
		(long)vstart[2]
	};
	long ihi[3] = {
		(long)vend[0],
		(long)vend[1],
		(long)vend[2]
	};

	for (int i = 20; i >= 0; --i) {
		if (!isfinite(vstart[0]) || !isfinite(vstart[1]) ||
				!isfinite(vstart[2])) {
			hit->hit = 0;
			return hit;
		}

		if (ilo[0] == ihi[0] && ilo[1] == ihi[1] && ilo[2] == ihi[2]) {
			hit->hit = 0;
			return hit;
		}

		float a[3] = {INFINITY, INFINITY, INFINITY};

		for (int j = 0; j < 3; ++j) {
			if (ihi[j] > ilo[j])
				a[j] = (float)ilo[j] + 1.0f;
			else if (ihi[j] < ilo[j])
				a[j] = (float)ilo[j];
		}

		float b[3] = {INFINITY, INFINITY, INFINITY};
		float d[3] = {
			vend[0] - vstart[0],
			vend[1] - vstart[1],
			vend[2] - vstart[2]
		};

		for (int j = 0; j < 3; ++j) {
			if (isfinite(a[j]))
				b[j] = (a[j] - vstart[j]) / d[j];
		}

		int c;
		if (b[0] < b[1] && b[0] < b[2]) {
			c = ihi[0] > ilo[0] ? 4 : 5;
			vstart[0] = a[0];
			vstart[1] += d[1] * b[0];
			vstart[2] += d[2] * b[0];
		} else if (b[1] < b[2]) {
			c = ihi[1] > ilo[1] ? 0 : 1;
			vstart[1] = a[1];
			vstart[0] += d[0] * b[1];
			vstart[2] += d[2] * b[1];
		} else {
			c = ihi[2] > ilo[2] ? 2 : 3;
			vstart[2] = a[2];
			vstart[0] += d[0] * b[2];
			vstart[1] += d[1] * b[2];
		}

		for (int j = 0; j < 3; ++j) {
			ilo[j] = (long)vstart[j];
		}

		switch (c) {
			case 5:
				--(ilo[0]);
				break;
			case 1:
				--(ilo[1]);
				break;
			case 3:
				--(ilo[2]);
				break;
		}

		int istile = UBLC_level_istile(ilo[0], ilo[1], ilo[2]);

		if (istile) {
			hit->x = ilo[0];
			hit->y = ilo[1];
			hit->z = ilo[2];
			hit->f = c;
			hit->hit = 1;
			return hit;
		}
	}

	hit->hit = 0;
	return hit;
}

int UBLC_level_rdlock(void) {
	return pthread_rwlock_rdlock(&levellock);
}

int UBLC_level_wrlock(void) {
	return pthread_rwlock_wrlock(&levellock);
}

int UBLC_level_unlock(void) {
	return pthread_rwlock_unlock(&levellock);
}
