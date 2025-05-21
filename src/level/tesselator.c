#include <stddef.h>

#include "tesselator.h"

#define MAX_VERTICES 100000

static size_t vertices = 0;

static float vertexbuffer[MAX_VERTICES * 3];
static float texcoordbuffer[MAX_VERTICES * 2];
static float colorbuffer[MAX_VERTICES * 3];

void UBLC_tesselator_flush(void) {
	vertices = 0;
}

void UBLC_tesselator_vertex(struct UBLC_vertex *vertex) {
	vertexbuffer[vertices * 3 + 0] = vertex->x;
	vertexbuffer[vertices * 3 + 1] = vertex->y;
	vertexbuffer[vertices * 3 + 2] = vertex->z;

	texcoordbuffer[vertices * 2 + 0] = vertex->u;
	texcoordbuffer[vertices * 2 + 1] = vertex->v;

	colorbuffer[vertices * 3 + 0] = vertex->r;
	colorbuffer[vertices * 3 + 1] = vertex->g;
	colorbuffer[vertices * 3 + 2] = vertex->b;

	if (++vertices == MAX_VERTICES)
		UBLC_tesselator_flush();
}
