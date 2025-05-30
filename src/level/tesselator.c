#include <err.h>
#include <stddef.h>
#include <string.h>

#include <SDL3/SDL_opengl.h>

#include "../gutl.h"
#include "tesselator.h"
#include "vbuffer.h"

void UBLC_tesselator_vertex(struct UBLC_vbuffer *buf, float x, float y, float z,
		float u, float v, float r, float g, float b) {
	buf->x = x;
	buf->y = y;
	buf->z = z;

	buf->u = u;
	buf->v = v;

	buf->r = r;
	buf->g = g;
	buf->b = b;
}
