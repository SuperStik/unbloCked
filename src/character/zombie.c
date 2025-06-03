#define GL_GLEXT_PROTOTYPES 1
#include <err.h>

#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>

#include "cube.h"
#include "../textures.h"
#include "zombie.h"

#define HEAD 0
#define BODY 1
#define ARML 2
#define ARMR 3
#define LEGL 4
#define LEGR 5

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
