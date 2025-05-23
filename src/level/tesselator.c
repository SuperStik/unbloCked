#include <err.h>
#include <stddef.h>
#include <string.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "tesselator.h"

#define MAX_VERTICES 100000

static size_t vertices = 0;

/* TODO: Use VBOs */
static float vertexbuffer[MAX_VERTICES * 3];
static float texcoordbuffer[MAX_VERTICES * 2];
static float colorbuffer[MAX_VERTICES * 3];

static struct {
	unsigned char color:1;
	unsigned char texture:1;
} flags;

static void clear(void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void UBLC_tesselator_flush(void) {
	glVertexPointer(3, GL_FLOAT, 0, vertexbuffer);

	if (flags.texture)
		glTexCoordPointer(2, GL_FLOAT, 0, texcoordbuffer);

	if (flags.color)
		glColorPointer(3, GL_FLOAT, 0, colorbuffer);

	glEnableClientState(GL_VERTEX_ARRAY);

	if (flags.texture)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (flags.color)
		glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_QUADS, GL_POINTS, vertices);

	glDisableClientState(GL_VERTEX_ARRAY);

	if (flags.texture)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (flags.color)
		glDisableClientState(GL_COLOR_ARRAY);

	clear();
}

#pragma GCC diagnostic pop

void UBLC_tesselator_init(void) {
	clear();
	memset(&flags, 0, sizeof(flags));
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

static void clear(void) {
	vertices = 0;
	for (size_t i = 0; i < (MAX_VERTICES * 3); ++i) {
		vertexbuffer[i] = 0.0f;
		colorbuffer[i] = 0.0f;
	}

	for (size_t i = 0; i < (MAX_VERTICES * 2); ++i)
		texcoordbuffer[i] = 0.0f;
}
