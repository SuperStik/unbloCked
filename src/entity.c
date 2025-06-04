#include <err.h>
#include <math.h>
#include <stdlib.h>

#include "entity.h"
#include "level/level.h"

static void setpos(struct UBLC_entity *ent, float x, float y, float z);

struct UBLC_entity *UBLC_entity_init(struct UBLC_entity *ent) {
	pthread_rwlock_init(&(ent->lock), NULL);
	pthread_rwlock_wrlock(&(ent->lock));

	ent->pitch = 0.0f;
	ent->yaw = 0.0f;
	ent->onground = 0;

	UBLC_entity_resetpos(ent);

	pthread_rwlock_unlock(&(ent->lock));

	return ent;
}

void UBLC_entity_delete(struct UBLC_entity *ent) {
	pthread_rwlock_destroy(&(ent->lock));
}

void UBLC_entity_turn(struct UBLC_entity *ent, float xo, float yo) {
	pthread_rwlock_wrlock(&(ent->lock));

	ent->yaw = (ent->yaw + xo * 0.15f);
	float newpitch = (ent->pitch + yo * 0.15f);

	if (fabsf(newpitch) > 90.0f)
		ent->pitch = copysignf(90.0f, newpitch);
	else
		ent->pitch = newpitch;

	pthread_rwlock_unlock(&(ent->lock));
}

void UBLC_entity_getangles(struct UBLC_entity *ent, float *pitch, float *yaw) {
	pthread_rwlock_rdlock(&(ent->lock));

	*pitch = ent->pitch;
	*yaw = ent->yaw;

	pthread_rwlock_unlock(&(ent->lock));
}

void UBLC_entity_move(struct UBLC_entity *ent, float xa, float ya, float za) {
	float xaOrg = xa;
	float yaOrg = ya;
	float zaOrg = za;
	size_t count;
	struct UBLC_AABB expanded = ent->aabb;

	UBLC_AABB_expand(&expanded, xa, ya, za);

	const struct UBLC_AABB *aabbs = UBLC_level_getcubes(&expanded, &count);

	for (size_t i = 0; i < count; ++i)
		xa = UBLC_AABB_clipXcollide(aabbs + i, &(ent->aabb), xa);
	UBLC_AABB_move(&(ent->aabb), xa, 0.0f, 0.0f);

	for (size_t i = 0; i < count; ++i)
		ya = UBLC_AABB_clipYcollide(aabbs + i, &(ent->aabb), ya);
	UBLC_AABB_move(&(ent->aabb), 0.0f, ya, 0.0f);

	for (size_t i = 0; i < count; ++i)
		za = UBLC_AABB_clipZcollide(aabbs + i, &(ent->aabb), za);
	UBLC_AABB_move(&(ent->aabb), 0.0f, 0.0f, za);

	ent->onground = yaOrg != ya && yaOrg < 0.0f;

	if (xaOrg != xa)
		ent->xd = 0.0f;

	if (yaOrg != ya)
		ent->yd = 0.0f;

	if (zaOrg != za)
		ent->zd = 0.0f;

	ent->x = (ent->aabb.x_lo + ent->aabb.x_hi) / 2.0f;
	ent->y = ent->aabb.y_lo + 1.62f;
	ent->z = (ent->aabb.z_lo + ent->aabb.z_hi) / 2.0f;
}

void UBLC_entity_moverelative(struct UBLC_entity *ent, float xa, float za, float
		speed) {
	float dist = (xa * xa) + (za * za);
	
	if (dist < 0.1f)
		return;

	dist = speed / sqrtf(dist);
	xa *= dist;
	za *= dist;
	
	float sinval, cosval;
	__sincospif(ent->yaw / 180.0f, &sinval, &cosval);
	ent->xd += xa * cosval - za * sinval;
	ent->zd += za * cosval + xa * sinval;
}

void UBLC_entity_resetpos(struct UBLC_entity *ent) {
	float x = (float)drand48() * (float)UBLC_level_width;
	float y = (float)(UBLC_level_depth + 10);
	float z = (float)drand48() * (float)UBLC_level_height;
	setpos(ent, x, y, z);
}

void UBLC_entity_tick(struct UBLC_entity *ent) {
	pthread_rwlock_wrlock(&(ent->lock));

	ent->xo = ent->x;
	ent->yo = ent->y;
	ent->zo = ent->z;

	pthread_rwlock_unlock(&(ent->lock));
}

static void setpos(struct UBLC_entity *ent, float x, float y, float z) {
	ent->x = x;
	ent->y = y;
	ent->z = z;
	ent->xd = 0.0f;
	ent->yd = 0.0f;
	ent->zd = 0.0f;
	float w = 0.3f;
	float h = 0.9f;
	UBLC_AABB_INIT(&(ent->aabb), x - w, y - h, z - w, x + w, y + h, z + w);
}
