#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
#include <math.h>

#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_timer.h>

#include "cube.h"
#include "../textures.h"
#include "zombie.h"

#define HEAD 0
#define BODY 1
#define ARML 2
#define ARMR 3
#define LEGL 4
#define LEGR 5

#define RAD2DEG (90.0 * M_2_PI)

static unsigned skin;
static unsigned model[6];

void UBLC_zombie_initstatic(void) {
	long tex = UBLC_textures_loadtexture("textures/char.png", GL_NEAREST);
	if (tex < 0)
		errx(2, "Cannot allocate texture");

	skin = tex;

	glGenBuffers(6, model);

	struct UBLC_vbuffer buf[24];

	UBLC_cube_genbox(buf, -4.0f, -8.0f, -4.0f, 8.0f, 8.0f, 8.0f, 0.0f,
			0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[HEAD]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -4.0f, 0.0f, -2.0f, 8.0f, 12.0f, 4.0f, 16.0f,
			16.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[BODY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -3.0f, -2.0f, -2.0f, 4.0f, 12.0f, 4.0f, 40.0f,
			16.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[ARML]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -1.0f, -2.0f, -2.0f, 4.0f, 12.0f, 4.0f, 40.0f,
			16.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[ARMR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -2.0f, 0.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.0f,
			16.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[LEGL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);
	
	UBLC_cube_genbox(buf, -2.0f, 0.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.0f,
			16.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[LEGR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

}

void UBLC_zombie_destroystatic(void) {
	glDeleteTextures(1, &skin);

	glDeleteBuffers(6, model);
}

struct UBLC_zombie *UBLC_zombie_init(struct UBLC_zombie *zom, float x, float y,
		float z) {
	UBLC_entity_init(&(zom->ent));
	zom->ent.x = x;
	zom->ent.y = y;
	zom->ent.z = z;

	return zom;
}

void UBLC_zombie_delete(struct UBLC_zombie *zom) {
	UBLC_entity_delete(&(zom->ent));
}

void UBLC_zombie_render(struct UBLC_zombie *zom, float a) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skin);

	glPushMatrix();
	float curtime = (float)(SDL_GetTicks() % 36000) / 100.0f * zom->speed +
		zom->timeoffs;
	float size= 7.0f/120.0f;
	/* TODO: don't use magic numbers */
	float sintbl[5] = {
		sinf(curtime),
		sinf(curtime * 0.6662f),
		sinf(curtime * 0.83f),
		sinf(curtime * 0.2312f),
		sinf(curtime * 0.2812f)
	};
	float yy = -fabsf(sintbl[1]) * 5.0f - 23.0f;

	glTranslatef(
			zom->ent.xo + (zom->ent.x - zom->ent.xo) * a,
			zom->ent.yo + (zom->ent.y - zom->ent.yo) * a,
			zom->ent.zo + (zom->ent.z - zom->ent.zo) * a
		    );
	glScalef(1.0f, -1.0f, 1.0f);
	glScalef(size, size, size);
	glTranslatef(0.0f, yy, 0.0f);
	glRotatef(zom->rot * RAD2DEG + 180.0f, 0.0f, 1.0f, 0.0f);

	struct UBLC_cube cubes[6] = {
		{.x = 0.0f, .y = 0.0f, .z = 0.0f, .roll = 0.0f},
		{.x = 0.0f, .y = 0.0f, .z = 0.0f, .pitch = 0.0f, .yaw = 0.0f,
			.roll = 0.0f},
		{.x = -5.0f, .y = 2.0f, .z = 0.0f, .yaw = 0.0f},
		{.x = 5.0f, .y = 2.0f, .z = 0.0f, .yaw = 0.0f},
		{.x = -2.0f, .y = 12.0f, .z = 0.0f, .yaw = 0.0f, .roll = 0.0f},
		{.x = 2.0f, .y = 12.0f, .z = 0.0f, .yaw = 0.0f, .roll = 0.0f}
	};

	cubes[HEAD].yaw = sintbl[2];
	cubes[HEAD].pitch = sintbl[0] * 0.8f;
	cubes[ARML].pitch = -sintbl[1] * 2.0f;
	cubes[ARML].roll = sintbl[3] + 1.0f;
	cubes[ARMR].pitch = sintbl[1] * 2.0f;
	cubes[ARMR].roll = sintbl[4] - 1.0f;
	cubes[LEGL].pitch = sintbl[1] * 1.4f;
	cubes[LEGR].pitch = -sintbl[1] * 1.4f;

	for (int i = 0; i < 6; ++i)
		UBLC_cube_render(cubes + i, model[i]);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
