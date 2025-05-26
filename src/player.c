#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "level/level.h"
#include "player.h"

static void resetpos(struct UBLC_player *);
static void setpos(struct UBLC_player *, float x, float y, float z);

static int setonground(struct UBLC_player *, int onground);
static int isonground(struct UBLC_player *);

static int sethasreset(struct UBLC_player *, int reset);
static int gethasreset(struct UBLC_player *);

struct UBLC_player *UBLC_player_init(struct UBLC_player *ply) {
	pthread_rwlock_init(&(ply->lock), NULL);
	pthread_rwlock_wrlock(&(ply->lock));

	ply->flags = UBLC_FPLY_FLYING;
	ply->pitch = 0.0f;
	ply->yaw = 0.0f;

	resetpos(ply);

	pthread_rwlock_unlock(&(ply->lock));

	return ply;
}

void UBLC_player_delete(struct UBLC_player *ply) {
	pthread_rwlock_destroy(&(ply->lock));
}

int UBLC_player_setkeys(struct UBLC_player *ply, int keys) {
	return __atomic_fetch_or(&(ply->keyflags), keys, __ATOMIC_RELAXED);
}

int UBLC_player_unsetkeys(struct UBLC_player *ply, int keys) {
	return __atomic_fetch_and(&(ply->keyflags), ~keys, __ATOMIC_RELAXED);
}

int UBLC_player_getkeys(const struct UBLC_player *ply) {
	return __atomic_load_n(&(ply->keyflags), __ATOMIC_RELAXED);
}

int UBLC_player_setflying(struct UBLC_player *ply, int flying) {
	if (flying) {
		return !!(__atomic_fetch_or(&(ply->flags), UBLC_FPLY_FLYING,
					__ATOMIC_RELAXED) & UBLC_FPLY_FLYING);
	} else {
		return !!(__atomic_fetch_and(&(ply->flags), ~UBLC_FPLY_FLYING,
					__ATOMIC_RELAXED) & UBLC_FPLY_FLYING);
	}
}

int UBLC_player_toggleflying(struct UBLC_player *ply) {
	return !!(__atomic_fetch_xor(&(ply->flags), UBLC_FPLY_FLYING,
				__ATOMIC_RELAXED) & UBLC_FPLY_FLYING);
}

int UBLC_player_getflying(const struct UBLC_player *ply) {
	return !!(__atomic_load_n(&(ply->flags), __ATOMIC_RELAXED) &
			UBLC_FPLY_FLYING);
}

static int setonground(struct UBLC_player *ply, int onground) {
	if (onground) {
		return __atomic_fetch_or(&(ply->flags), UBLC_FPLY_ONGROUND,
				__ATOMIC_RELAXED) & UBLC_FPLY_ONGROUND;
	} else {
		return __atomic_fetch_and(&(ply->flags), ~UBLC_FPLY_ONGROUND,
				__ATOMIC_RELAXED) & UBLC_FPLY_ONGROUND;
	}
}

static int isonground(struct UBLC_player *ply) {
	return __atomic_load_n(&(ply->flags), __ATOMIC_RELAXED) &
		UBLC_FPLY_ONGROUND;
}

static int sethasreset(struct UBLC_player *ply, int reset) {
	if (reset) {
		return __atomic_fetch_or(&(ply->flags), UBLC_FPLY_HASRESET,
				__ATOMIC_RELAXED) & UBLC_FPLY_HASRESET;
	} else {
		return __atomic_fetch_and(&(ply->flags), ~UBLC_FPLY_HASRESET,
				__ATOMIC_RELAXED) & UBLC_FPLY_HASRESET;
	}
}

static int gethasreset(struct UBLC_player *ply) {
	return __atomic_load_n(&(ply->flags), __ATOMIC_RELAXED) &
		UBLC_FPLY_HASRESET;
}

void UBLC_player_turn(struct UBLC_player *ply, float xo, float yo) {
	pthread_rwlock_wrlock(&(ply->lock));

	ply->yaw = (ply->yaw + xo * 0.15f);
	float newpitch = (ply->pitch + yo * 0.15f);

	if (fabsf(newpitch) > 90.0f)
		ply->pitch = copysignf(90.0f, newpitch);
	else
		ply->pitch = newpitch;

	pthread_rwlock_unlock(&(ply->lock));
}

void UBLC_player_getangles(struct UBLC_player *ply, float *pitch, float *yaw) {
	pthread_rwlock_rdlock(&(ply->lock));

	*pitch = ply->pitch;
	*yaw = ply->yaw;

	pthread_rwlock_unlock(&(ply->lock));
}

void UBLC_player_tick(struct UBLC_player *ply) {
	ply->xo = ply->x;
	ply->yo = ply->y;
	ply->zo = ply->z;
	float xa = 0.0f;
	float ya = 0.0f;

	int keys = UBLC_player_getkeys(ply);

	if ((keys & UBLC_KF_R) && !gethasreset(ply)) {
		resetpos(ply);
		sethasreset(ply, 1);
	} else if (!(keys & UBLC_KF_R))
		sethasreset(ply, 0);

	if (keys & (UBLC_KF_UP | UBLC_KF_W))
		ya -= 1.0f;

	if (keys & (UBLC_KF_DOWN | UBLC_KF_S))
		ya += 1.0f;

	if (keys & (UBLC_KF_LEFT | UBLC_KF_A))
		xa -= 1.0f;

	if (keys & (UBLC_KF_RIGHT | UBLC_KF_D))
		xa += 1.0f;

	int grounded = isonground(ply);

	if ((keys & UBLC_KF_SPACE) && grounded)
		ply->yd = 0.12;

	UBLC_player_moverelative(ply, xa, ya, grounded ? 0.02f : 0.005f);

	if (UBLC_player_getflying(ply))
		ply->yd = (ply->yd - 0.005f);

	UBLC_player_move(ply, ply->xd, ply->yd, ply->zd);
	ply->xd *= 0.91f;
	ply->yd *= 0.98f;
	ply->zd *= 0.91f;
	
	if (grounded) {
		ply->xd *= 0.8f;
		ply->zd *= 0.8f;
	}
}

void UBLC_player_move(struct UBLC_player *ply, float xa, float ya, float za) {
	float xaOrg = xa;
	float yaOrg = ya;
	float zaOrg = za;
	size_t count;
	struct UBLC_AABB expanded = ply->aabb;

	UBLC_AABB_expand(&expanded, xa, ya, za);

	const struct UBLC_AABB *aabbs = UBLC_level_getcubes(&expanded, &count);

	for (size_t i = 0; i < count; ++i) {
		ya = UBLC_AABB_clipYcollide(aabbs + i, &(ply->aabb), ya);
		xa = UBLC_AABB_clipXcollide(aabbs + i, &(ply->aabb), xa);
		za = UBLC_AABB_clipZcollide(aabbs + i, &(ply->aabb), za);
	}
	UBLC_AABB_move(&(ply->aabb), xa, ya, za);

	setonground(ply, yaOrg != ya && yaOrg < 0.0f);

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
	float y = (float)(UBLC_level_depth + 10);
	float z = (float)drand48() * (float)UBLC_level_height;
	setpos(ply, x, y, z);
}

static void setpos(struct UBLC_player *ply, float x, float y, float z) {
	ply->x = x;
	ply->y = y;
	ply->z = z;
	ply->xd = 0.0f;
	ply->yd = 0.0f;
	ply->zd = 0.0f;
	float w = 0.3f;
	float h = 0.9f;
	UBLC_AABB_INIT(&(ply->aabb), x - w, y - h, z - w, x + w, y + h, z + w);
}
