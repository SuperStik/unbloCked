#include <err.h>
#include <math.h>

#include "gccvec.h"
#include "level/level.h"
#include "player.h"

static void resetpos(struct UBLC_player *);
static void setpos(struct UBLC_player *, float x, float y, float z);

struct UBLC_player *UBLC_player_init(struct UBLC_player *ply) {
	UBLC_entity_init(&(ply->ent));

	ply->flying = 0;
	ply->hasselect = 0;

	return ply;
}

void UBLC_player_delete(struct UBLC_player *ply) {
	UBLC_entity_delete(&(ply->ent));
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

void UBLC_player_tick(struct UBLC_player *ply) {
	struct UBLC_hitresult hit;

	float psin, pcos;
	float ysin, ycos;

	__sincospif(ply->ent.pitch / -180.0f, &psin, &pcos);
	__sincospif(ply->ent.yaw / 180.0f, &ysin, &ycos);

	gvec(float,4) offset = {pcos * ysin, psin, -ycos * pcos, 0.0f};
	offset *= 3.0f;
	offset += (gvec(float,4)){ply->ent.x, ply->ent.y, ply->ent.z, 0.0f};

	float start[3] = {ply->ent.x, ply->ent.y, ply->ent.z};
	float end[3] = {offset[0], offset[1], offset[2]};

	UBLC_level_clip(&hit, start, end);

	pthread_rwlock_wrlock(&(ply->ent.lock));

	if (hit.hit) {
		ply->xb = hit.x;
		ply->yb = hit.y;
		ply->zb = hit.z;
		ply->hasselect = 1;
		ply->placeface = hit.f;
	} else
		ply->hasselect = 0;

	pthread_rwlock_unlock(&(ply->ent.lock));

	UBLC_entity_tick(&(ply->ent));

	float xa = 0.0f;
	float ya = 0.0f;
	float za = 0.0f;

	int keys = UBLC_player_getkeys(ply);

	int oneshotkeys = 0;

	if (keys & UBLC_KF_R) {
		UBLC_entity_resetpos(&(ply->ent));
		oneshotkeys |= UBLC_KF_R;
	}

	if (keys & (UBLC_KF_UP | UBLC_KF_W))
		za -= 1.0f;

	if (keys & (UBLC_KF_DOWN | UBLC_KF_S))
		za += 1.0f;

	if (keys & (UBLC_KF_LEFT | UBLC_KF_A))
		xa -= 1.0f;

	if (keys & (UBLC_KF_RIGHT | UBLC_KF_D))
		xa += 1.0f;

	if (keys & UBLC_KF_V) {
		ply->flying = !ply->flying;
		oneshotkeys |= UBLC_KF_V;
	}

	int grounded = ply->ent.onground;
	int flying = ply->flying;

	if (keys & UBLC_KF_SPACE) {
		if (grounded)
			ply->ent.yd = 0.5;

		if (flying)
			ya += 0.1;
	}

	if ((keys & (UBLC_KF_LSHIFT | UBLC_KF_RSHIFT)) && flying)
		ya -= 0.1;

	ply->ent.yd += ya;

	UBLC_entity_moverelative(&(ply->ent), xa, za, (grounded || flying) ?
			0.1f : 0.02f);

	if (!flying)
		ply->ent.yd = (ply->ent.yd - 0.08f);

	UBLC_entity_move(&(ply->ent), ply->ent.xd, ply->ent.yd, ply->ent.zd);
	ply->ent.xd *= 0.91f;
	ply->ent.yd *= 0.98f;
	ply->ent.zd *= 0.91f;
	
	if (grounded) {
		ply->ent.xd *= 0.7f;
		ply->ent.zd *= 0.7f;
	}

	if (flying) {
		ply->ent.yd *= 0.7f;
	}

	if (oneshotkeys)
		UBLC_player_unsetkeys(ply, oneshotkeys);
}
