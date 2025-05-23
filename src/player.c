#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "level/level.h"
#include "player.h"

static void resetpos(struct UBLC_player *ply);
static void setpos(struct UBLC_player *ply, float x, float y, float z);

struct UBLC_player *UBLC_player_init(struct UBLC_player *ply) {
	ply->onground = 0;
	ply->pitch = 0.0f;
	ply->yaw = 0.0f;

	resetpos(ply);
	return ply;
}

void UBLC_player_turn(struct UBLC_player *ply, float xo, float yo) {
	ply->yaw = (ply->yaw + xo * 0.15f);
	float newpitch = (ply->pitch + yo * 0.15f);

	if (fabsf(newpitch) > 90.0f)
		ply->pitch = copysignf(90.0f, newpitch);
	else
		ply->pitch = newpitch;
}

void UBLC_player_tick(struct UBLC_player *ply) {
	ply->xo = ply->x;
	ply->yo = ply->y;
	ply->zo = ply->z;
	float xa = 0.0f;
	float ya = 0.0f;

	UBLC_player_moverelative(ply, xa, ya, ply->onground ? 0.02f : 0.005f);
	//ply->yd = (ply->yd - 0.05f);
	UBLC_player_move(ply, ply->xd, ply->yd, ply->zd);
	ply->xd *= 0.91f;
	ply->yd *= 0.98f;
	ply->zd *= 0.91f;
	
	if (ply->onground) {
		ply->xd *= 0.8f;
		ply->zd *= 0.8f;
	}
}

void UBLC_player_move(struct UBLC_player *ply, float xa, float ya, float za) {
	float xaOrg = xa;
	float yaOrg = ya;
	float zaOrg = za;
	size_t count;
	struct UBLC_AABB expanded;

	memcpy(&expanded, &(ply->aabb), sizeof(expanded));
	UBLC_AABB_expand(&expanded, xa, ya, za);

	const struct UBLC_AABB *aabbs = UBLC_level_getcubes(&expanded, &count);

	for (size_t i = 0; i < count; ++i)
		ya = UBLC_AABB_clipYcollide(&aabbs[i], &(ply->aabb), ya);
	UBLC_AABB_move(&(ply->aabb), 0, ya, 0);

	for (size_t i = 0; i < count; ++i)
		xa = UBLC_AABB_clipXcollide(&aabbs[i], &(ply->aabb), xa);
	UBLC_AABB_move(&(ply->aabb), xa, 0, 0);

	for (size_t i = 0; i < count; ++i)
		za = UBLC_AABB_clipZcollide(&aabbs[i], &(ply->aabb), za);
	UBLC_AABB_move(&(ply->aabb), 0, 0, za);

	ply->onground = yaOrg != ya && yaOrg < 0.0f;

	if (xaOrg != xa)
		ply->xd = 0.0f;

	if (yaOrg != ya)
		ply->yd = 0.0f;

	if (zaOrg != za)
		ply->zd = 0.0f;

	ply->x = (ply->aabb.x_lo + ply->aabb.x_hi) / 2.0f;
	ply->y = ply->aabb.y_lo + 1.62f;
	ply->z = (ply->aabb.z_lo + ply->aabb.z_hi) / 2.0f;
}

void UBLC_player_moverelative(struct UBLC_player *ply, float xa, float za, float
		speed) {
	float dist = (xa * xa) + (za * za);
	
	if (dist < 0.01f)
		return;

	dist = speed / sqrtf(dist);
	xa *= dist;
	za *= dist;
	
	float sinval, cosval;
	__sincospif(ply->yaw / 180.0f, &sinval, &cosval);
	ply->xd += xa * cosval - za * sinval;
	ply->zd += za * cosval + xa * sinval;
}

static void resetpos(struct UBLC_player *ply) {
	float x = (float)drand48() * (float)UBLC_level_width;
	float y = (float)(UBLC_level_depth + 40);
	float z = (float)drand48() * (float)UBLC_level_height;
	setpos(ply, x, y, z);
}

static void setpos(struct UBLC_player *ply, float x, float y, float z) {
	ply->x = x;
	ply->y = y;
	ply->z = z;
	float w = 0.3f;
	float h = 0.9f;
	UBLC_AABB_INIT(&(ply->aabb), x - w, y - h, z - w, x + w, y + h, z + w);
}
