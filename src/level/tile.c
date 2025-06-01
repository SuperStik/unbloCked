#include <err.h>
#include <math.h>

#include <SDL3/SDL_timer.h>

#include "level.h"
#include "tesselator.h"
#include "tile.h"

size_t UBLC_tile_render(struct UBLC_vbuffer *buf, int tex, int layer, int x,
		int y, int z) {
	float ulo = (float)tex / 16.0f;
	float uhi = ulo + 1.0f / 16.0f;
	float vlo = 0.0f;
	float vhi = vlo + 1.0f / 16.0f;

	float side = 1.0f;
	float frnt = 0.8f;
	float botm = 0.6f;

	float xlo = (float)x + 0.0f;
	float xhi = (float)x + 1.0f;
	float ylo = (float)y + 0.0f;
	float yhi = (float)y + 1.0f;
	float zlo = (float)z + 0.0f;
	float zhi = (float)z + 1.0f;

	float br;

	struct UBLC_vbuffer *orig = buf;

	/* my fingers hurt */
	if (!UBLC_level_issolid_unsafe(x, y - 1, z)) {
		br = UBLC_level_getbrightness_unsafe(x, y - 1, z) * side;
		if ((br == side) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xlo, ylo, zhi, uhi, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, ylo, zlo, uhi, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, ylo, zlo, ulo, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, ylo, zhi, ulo, vhi,
					br, br, br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y + 1, z)) {
		br = UBLC_level_getbrightness_unsafe(x, y + 1, z) * side;
		if ((br == side) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xhi, yhi, zhi, uhi, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, yhi, zlo, uhi, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, yhi, zlo, ulo, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, yhi, zhi, ulo, vhi,
					br, br, br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y, z - 1)) {
		br = UBLC_level_getbrightness_unsafe(x, y, z - 1) * frnt;
		if ((br == frnt) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xlo, yhi, zlo, uhi, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, yhi, zlo, ulo, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, ylo, zlo, ulo, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, ylo, zlo, uhi, vhi,
					br, br, br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y, z + 1)) {
		br = UBLC_level_getbrightness_unsafe(x, y, z + 1) * frnt;
		if ((br == frnt) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xlo, yhi, zhi, ulo, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, ylo, zhi, ulo, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, ylo, zhi, uhi, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, yhi, zhi, uhi, vlo,
					br, br, br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x - 1, y, z)) {
		br = UBLC_level_getbrightness_unsafe(x - 1, y, z) * botm;
		if ((br == botm) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xlo, yhi, zhi, uhi, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, yhi, zlo, ulo, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, ylo, zlo, ulo, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xlo, ylo, zhi, uhi, vhi,
					br, br, br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x + 1, y, z)) {
		br = UBLC_level_getbrightness_unsafe(x + 1, y, z) * botm;
		if ((br == botm) ^ (layer == 1)) {
			UBLC_tesselator_vertex(buf++, xhi, ylo, zhi, ulo, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, ylo, zlo, uhi, vhi,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, yhi, zlo, uhi, vlo,
					br, br, br);
			UBLC_tesselator_vertex(buf++, xhi, yhi, zhi, ulo, vlo,
					br, br, br);
		}
	}

	return buf - orig;
}

void UBLC_tile_renderface(struct UBLC_vbuffer buf[4], int x, int y, int z, enum UBLC_tile_face face) {
	float xlo = (float)x + 0.0f;
	float xhi = (float)x + 1.0f;
	float ylo = (float)y + 0.0f;
	float yhi = (float)y + 1.0f;
	float zlo = (float)z + 0.0f;
	float zhi = (float)z + 1.0f;

	float ticktime = (float)(SDL_GetTicks() % 36000);
	float br = __sinpif(ticktime / 360.0f) * 0.2f + 0.4f;

	switch (face) {
		case UBLC_FACE_DOWN:
			UBLC_tesselator_vertex(buf + 0, xlo, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xlo, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xhi, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xhi, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			break;
		case UBLC_FACE_UP:
			UBLC_tesselator_vertex(buf + 0, xhi, yhi, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xhi, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xlo, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xlo, yhi, zhi, 0.0f,
					0.0f, br, br, br);
			break;
		case UBLC_FACE_FRONT:
			UBLC_tesselator_vertex(buf + 0, xlo, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xhi, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xhi, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xlo, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			break;
		case UBLC_FACE_BACK:
			UBLC_tesselator_vertex(buf + 0, xlo, yhi, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xlo, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xhi, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xhi, yhi, zhi, 0.0f,
					0.0f, br, br, br);
			break;
		case UBLC_FACE_LEFT:
			UBLC_tesselator_vertex(buf + 0, xlo, yhi, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xlo, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xlo, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xlo, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			break;
		case UBLC_FACE_RIGHT:
			UBLC_tesselator_vertex(buf + 0, xhi, ylo, zhi, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 1, xhi, ylo, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 2, xhi, yhi, zlo, 0.0f,
					0.0f, br, br, br);
			UBLC_tesselator_vertex(buf + 3, xhi, yhi, zhi, 0.0f,
					0.0f, br, br, br);;
			break;
	}
}
