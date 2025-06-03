#define GL_GLEXT_PROTOTYPES 1
#include <math.h>
#include <stddef.h>

#include <SDL3/SDL_opengl.h>

#include "cube.h"

#define RAD2DEG (90.0 * M_2_PI)

static void polygon(struct UBLC_vbuffer[4], float ulo, float vlo, float uhi,
		float vhi);

void UBLC_cube_genbox(struct UBLC_vbuffer buf[24], float xlo, float ylo, float
		zlo, float w, float h, float d, float texUoffs, float texVoffs)
{
	float xhi = xlo + w;
	float yhi = ylo + h;
	float zhi = zlo + d;

	struct UBLC_vbuffer verts[8] = {
		{xlo, ylo, zlo},
		{xhi, ylo, zlo},
		{xhi, yhi, zlo},
		{xlo, yhi, zlo},
		{xlo, ylo, zhi},
		{xhi, ylo, zhi},
		{xhi, yhi, zhi},
		{xlo, yhi, zhi}
	};

	buf[0] = verts[5];
	buf[1] = verts[1];
	buf[2] = verts[2];
	buf[3] = verts[6];
	polygon(&(buf[0]), texUoffs + d + w, texVoffs + d, texUoffs + d + w + d,
			texVoffs + d + h);

	buf[4] = verts[0];
	buf[5] = verts[4];
	buf[6] = verts[7];
	buf[7] = verts[3];
	polygon(&(buf[4]), texUoffs, texVoffs + d, texUoffs + d + w, texVoffs +
			d);

	buf[8] = verts[5];
	buf[9] = verts[4];
	buf[10] = verts[0];
	buf[11] = verts[1];
	polygon(&(buf[8]), texUoffs + d, texVoffs, texUoffs + d + w, texVoffs +
			d);

	buf[12] = verts[2];
	buf[13] = verts[3];
	buf[14] = verts[7];
	buf[15] = verts[6];
	polygon(&(buf[12]), texUoffs + d + w, texVoffs, texUoffs + d + w,
			texVoffs + d);

	buf[16] = verts[1];
	buf[17] = verts[0];
	buf[18] = verts[3];
	buf[19] = verts[2];
	polygon(&(buf[16]), texUoffs + d, texVoffs + d, texUoffs + d + w,
			texVoffs + d + h);

	buf[20] = verts[4];
	buf[21] = verts[5];
	buf[22] = verts[6];
	buf[23] = verts[7];
	polygon(&(buf[20]), texUoffs + d + w + d, texVoffs + d, texUoffs + d + w
			+ d, texVoffs + d + h);
}

void UBLC_cube_render(const struct UBLC_cube *cube, unsigned buf) {
	glPushMatrix();

	glTranslatef(cube->x, cube->y, cube->z);
	glRotatef(cube->roll * (float)RAD2DEG, 0.0f, 0.0f, 1.0f);
	glRotatef(cube->yaw * (float)RAD2DEG, 0.0f, 1.0f, 0.0f);
	glRotatef(cube->pitch * (float)RAD2DEG, 1.0f, 0.0f, 0.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, buf);

	glVertexPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, x));
	glTexCoordPointer(3, GL_FLOAT, sizeof(struct UBLC_vbuffer),
			(void *)offsetof(struct UBLC_vbuffer, u));

	glDrawArrays(GL_QUADS, 0, 24);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

static void polygon(struct UBLC_vbuffer buf[4], float ulo, float vlo, float uhi,
		float vhi) {
	buf[0].u = uhi;
	buf[0].v = vlo;

	buf[1].u = ulo;
	buf[1].v = vlo;

	buf[2].u = ulo;
	buf[2].v = vhi;

	buf[3].u = uhi;
	buf[3].v = vhi;
}
