#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
#include <math.h>
#include <simd/math.h>
#include <stdlib.h>

#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_timer.h>

#include "../chronos.h"
#include "cube.h"
#include "../level/level.h"
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
	zom->ent.pos.x = x;
	zom->ent.pos.y = y;
	zom->ent.pos.z = z;

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
	float za;

	zom->rot += zom->rot_a;
	zom->rot_a *= 0.99f;
	zom->rot_a += (float)((drand48() - drand48()) * drand48() * drand48() *
			0.08);

	__sincosf(zom->rot, &xa, &za);

	if (zom->ent.onground && drand48() < 0.08)
		zom->ent.yd = 0.5f;

	UBLC_entity_moverelative(&(zom->ent), xa, za, zom->ent.onground ? 0.1f
			: 0.02f);
	zom->ent.yd -= 0.08f;
	UBLC_entity_move(&(zom->ent), zom->ent.xd, zom->ent.yd, zom->ent.zd);

	zom->ent.xd *= 0.91f;
	zom->ent.yd *= 0.98f;
	zom->ent.zd *= 0.91f;

	if (zom->ent.pos.y > 100.0f || zom->ent.pos.y < 0.0f)
		UBLC_entity_resetpos(&(zom->ent));

	if (zom->ent.onground) {
		zom->ent.xd *= 0.7f;
		zom->ent.zd *= 0.7f;
	}
}

void UBLC_zombie_render(struct UBLC_zombie *zom) {
	struct UBLC_entity_pos pos;
	UBLC_entity_getrenderpos(&(zom->ent), &pos);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skin);

	glPushMatrix();
	float curtime = (float)SDL_GetTicks() / 100.0f * zom->speed +
		zom->timeoffs;
	float size= 1.0f/20.0f;
	/* TODO: don't use magic numbers */
#if SIMD_COMPILER_HAS_REQUIRED_FEATURES
	simd_float4 params = {
		curtime,
		curtime,
		curtime,
		curtime
	};
	simd_float4 parammul = {1.0f, 0.6662f, 0.83f, 0.2312f};
	params *= parammul;

	simd_float4 sins = sin(params);

	float sintbl[5] = {
		sins.x,
		sins.y,
		sins.z,
		sins.w,
		sin(curtime * 0.2312f)
	};
#else
	float sintbl[5] = {
		sinf(curtime),
		sinf(curtime * 0.6662f),
		sinf(curtime * 0.83f),
		sinf(curtime * 0.2312f),
		sinf(curtime * 0.2812f)
	};
#endif /* SIMD_COMPILER_HAS_REQUIRED_FEATURES */

	float yy = fabsf(sintbl[1]) / 4.0f;

	float a = UBLC_chronos_getdelta();

	glTranslatef(
			pos.xo + (pos.x - pos.xo) * a,
			pos.yo + (pos.y - pos.yo) * a + yy,
			pos.zo + (pos.z - pos.zo) * a
		    );
	glScalef(size, -size, size);
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

	float br = UBLC_level_getbrightness(pos.x, pos.y, pos.z);
	br *= br;

	glColor3f(br, br, br);
	for (int i = 0; i < 6; ++i)
		UBLC_cube_render(cubes + i, model[i]);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}
