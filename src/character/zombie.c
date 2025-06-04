#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
#include <math.h>
#include <stdlib.h>

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
	const char *texstr = "textures/char.png";
	long tex = UBLC_textures_loadtexture(texstr, GL_NEAREST);
	if (tex < 0)
		errx(2, "Cannot allocate texture: %s", texstr);

	skin = tex;

	glGenBuffers(6, model);

	struct UBLC_vbuffer buf[24];

	UBLC_cube_genbox(buf, -4.0f, -8.0f, -4.0f, 8.0f, 8.0f, 8.0f, 0.0f,
			0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, model[HEAD]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -4.0f, 0.0f, -2.0f, 8.0f, 12.0f, 4.0f, 0.25f,
			0.5f);
	glBindBuffer(GL_ARRAY_BUFFER, model[BODY]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -3.0f, -2.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.625f,
			0.5f);
	glBindBuffer(GL_ARRAY_BUFFER, model[ARML]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -1.0f, -2.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.625f,
			0.5f);
	glBindBuffer(GL_ARRAY_BUFFER, model[ARMR]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);

	UBLC_cube_genbox(buf, -2.0f, 0.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.0f,
			0.5f);
	glBindBuffer(GL_ARRAY_BUFFER, model[LEGL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct UBLC_vbuffer) * 24, buf,
			GL_STATIC_DRAW);
	
	UBLC_cube_genbox(buf, -2.0f, 0.0f, -2.0f, 4.0f, 12.0f, 4.0f, 0.0f,
			0.5f);
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

	zom->timeoffs = (float)(drand48() * 1239813.0);
	zom->rot = (float)(drand48() * M_PI * 2.0);
	zom->speed = 1.0f;
	zom->rot_a = (float)(drand48() + 1.0) * 0.01f;

	return zom;
}

void UBLC_zombie_delete(struct UBLC_zombie *zom) {
	UBLC_entity_delete(&(zom->ent));
}

void UBLC_zombie_tick(struct UBLC_zombie *zom) {
	UBLC_entity_tick(&(zom->ent));

	float xa;
	float ya;

	zom->rot += zom->rot_a;
	zom->rot_a *= 0.99f;
	zom->rot_a += (float)((drand48() - drand48()) * drand48() * drand48() *
			0.01);

	__sincosf(zom->rot, &xa, &ya);

	if (zom->ent.onground && drand48() < 0.01)
		zom->ent.yd = 0.12f;

	UBLC_entity_moverelative(&(zom->ent), xa, ya, zom->ent.onground ? 0.02f
			: 0.005f);
	zom->ent.yd -= 0.005f;
	UBLC_entity_move(&(zom->ent), zom->ent.xd, zom->ent.yd, zom->ent.zd);

	zom->ent.xd *= 0.91f;
	zom->ent.yd *= 0.98f;
	zom->ent.zd *= 0.91f;

	if (zom->ent.y > 100.0f)
		UBLC_entity_resetpos(&(zom->ent));

	if (zom->ent.onground) {
		zom->ent.xd *= 0.8f;
		zom->ent.yd *= 0.8f;
	}
}

void UBLC_zombie_render(struct UBLC_zombie *zom, float a) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skin);

	glPushMatrix();
	float curtime = (float)(SDL_GetTicks() % 36000) / 100.0f * zom->speed +
		zom->timeoffs;
	float size= 1.0f/20.0f;
	/* TODO: don't use magic numbers */
	float sintbl[5] = {
		sinf(curtime),
		sinf(curtime * 0.6662f),
		sinf(curtime * 0.83f),
		sinf(curtime * 0.2312f),
		sinf(curtime * 0.2812f)
	};
	float yy = fabsf(sintbl[1]) / 4.0f;

	glTranslatef(
			zom->ent.xo + (zom->ent.x - zom->ent.xo) * a,
			zom->ent.yo + (zom->ent.y - zom->ent.yo) * a + yy,
			zom->ent.zo + (zom->ent.z - zom->ent.zo) * a
		    );
	glScalef(1.0f, -1.0f, 1.0f);
	glScalef(size, size, size);
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
