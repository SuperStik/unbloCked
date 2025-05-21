#include <stddef.h>

#include "tesselator.h"

#define MAX_VERTICES 100000

static size_t vertices = 0;

static float vertexbuffer[MAX_VERTICES * 3];
static float texcoordbuffer[MAX_VERTICES * 2];
static float colorbuffer[MAX_VERTICES * 3];

static struct {
	unsigned char color:1;
	unsigned char texture:1;
} flags;

void UBLC_tesselator_flush(void) {
	vertices = 0;
}

int UBLC_tesselator_setcolor(int enabled) {
	int old = flags.color;
	flags.color = !!enabled;
	return old;
}

int UBLC_tesselator_settexture(int enabled) {
	int old = flags.texture;
	flags.texture = !!enabled;
	return old;
}

int UBLC_tesselator_getcolor(void) {
	return flags.color;
}

int UBLC_tesselator_gettexture(void) {
	return flags.texture;
}

void UBLC_tesselator_vertex(float x, float y, float z, float u, float v, float
		r, float g, float b) {
	vertexbuffer[vertices * 3 + 0] = x;
	vertexbuffer[vertices * 3 + 1] = y;
	vertexbuffer[vertices * 3 + 2] = z;

	texcoordbuffer[vertices * 2 + 0] = u;
	texcoordbuffer[vertices * 2 + 1] = v;

	colorbuffer[vertices * 3 + 0] = r;
	colorbuffer[vertices * 3 + 1] = g;
	colorbuffer[vertices * 3 + 2] = b;

	if (++vertices >= MAX_VERTICES)
		UBLC_tesselator_flush();
}
