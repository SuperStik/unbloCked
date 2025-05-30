#ifndef UBLC_LEVEL_TILE_H
#define UBLC_LEVEL_TILE_H 1

#include <stddef.h>

#include <level/vbuffer.h>

#define UBLC_TILE_ROCK 0
#define UBLC_TILE_GRASS 1

enum UBLC_tile_face {
	UBLC_FACE_DOWN,
	UBLC_FACE_UP,
	UBLC_FACE_FRONT,
	UBLC_FACE_BACK,
	UBLC_FACE_LEFT,
	UBLC_FACE_RIGHT
};

size_t UBLC_tile_render(struct UBLC_vbuffer *buffer, int tex, int layer, int x,
		int y, int z);

void UBLC_tile_renderface(int x, int y, int z, enum UBLC_tile_face);

#endif /* UBLC_LEVEL_TILE_H */
