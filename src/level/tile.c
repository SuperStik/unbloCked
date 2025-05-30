#include "level.h"
#include "tesselator.h"
#include "tile.h"

void UBLC_tile_render(struct UBLC_vbuffer *buffer, size_t count, int tex, int layer, int x, int y, int z) {
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

	/* my fingers hurt */
	if (!UBLC_level_issolid_unsafe(x, y - 1, z)) {
		br = UBLC_level_getbrightness_unsafe(x, y - 1, z) * side;
		if ((br == side) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xlo, ylo, zhi, uhi, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xlo, ylo, zlo, uhi, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xhi, ylo, zlo, ulo, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xhi, ylo, zhi, ulo, vhi, br, br,
					br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y + 1, z)) {
		br = UBLC_level_getbrightness_unsafe(x, y + 1, z) * side;
		if ((br == side) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xhi, yhi, zhi, uhi, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xhi, yhi, zlo, uhi, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xlo, yhi, zlo, ulo, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xlo, yhi, zhi, ulo, vhi, br, br,
					br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y, z - 1)) {
		br = UBLC_level_getbrightness_unsafe(x, y, z - 1) * frnt;
		if ((br == frnt) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xlo, yhi, zlo, uhi, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xhi, yhi, zlo, ulo, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xhi, ylo, zlo, ulo, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xlo, ylo, zlo, uhi, vhi, br, br,
					br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x, y, z + 1)) {
		br = UBLC_level_getbrightness_unsafe(x, y, z + 1) * frnt;
		if ((br == frnt) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xlo, yhi, zhi, ulo, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xlo, ylo, zhi, ulo, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xhi, ylo, zhi, uhi, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xhi, yhi, zhi, uhi, vlo, br, br,
					br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x - 1, y, z)) {
		br = UBLC_level_getbrightness_unsafe(x - 1, y, z) * botm;
		if ((br == botm) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xlo, yhi, zhi, uhi, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xlo, yhi, zlo, ulo, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xlo, ylo, zlo, ulo, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xlo, ylo, zhi, uhi, vhi, br, br,
					br);
		}
	}

	if (!UBLC_level_issolid_unsafe(x + 1, y, z)) {
		br = UBLC_level_getbrightness_unsafe(x + 1, y, z) * botm;
		if ((br == botm) ^ (layer == 1)) {
			UBLC_tesselator_setcolor(1);
			UBLC_tesselator_settexture(1);

			UBLC_tesselator_vertex(xhi, ylo, zhi, ulo, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xhi, ylo, zlo, uhi, vhi, br, br,
					br);
			UBLC_tesselator_vertex(xhi, yhi, zlo, uhi, vlo, br, br,
					br);
			UBLC_tesselator_vertex(xhi, yhi, zhi, ulo, vlo, br, br,
					br);
		}
	}

}

void UBLC_tile_renderface(int x, int y, int z, enum UBLC_tile_face face) {
	float xlo = (float)x + 0.0f;
	float xhi = (float)x + 1.0f;
	float ylo = (float)y + 0.0f;
	float yhi = (float)y + 1.0f;
	float zlo = (float)z + 0.0f;
	float zhi = (float)z + 1.0f;

	switch (face) {
		case UBLC_FACE_DOWN:
			UBLC_tesselator_vertex(xlo, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
		case UBLC_FACE_UP:
			UBLC_tesselator_vertex(xhi, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
		case UBLC_FACE_FRONT:
			UBLC_tesselator_vertex(xlo, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
		case UBLC_FACE_BACK:
			UBLC_tesselator_vertex(xlo, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
		case UBLC_FACE_LEFT:
			UBLC_tesselator_vertex(xlo, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xlo, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
		case UBLC_FACE_RIGHT:
			UBLC_tesselator_vertex(xhi, ylo, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, ylo, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, yhi, zlo, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			UBLC_tesselator_vertex(xhi, yhi, zhi, 0.0f, 0.0f, 1.0f,
					1.0f, 1.0f);
			break;
	}
}
