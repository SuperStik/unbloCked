#ifndef PLAYER_H
#define PLAYER_H 1

#include "phys/AABB.h"

struct UBLC_player {
	float xo;
	float yo;
	float zo;
	float x;
	float y;
	float z;
	float xd;
	float yd;
	float zd;
	float pitch;
	float yaw;
	struct UBLC_AABB aabb;
	unsigned char onground:1;
	unsigned char gravity:1;
};

struct UBLC_player *UBLC_player_init(struct UBLC_player *);

void UBLC_player_turn(struct UBLC_player *, float xo, float yo);
void UBLC_player_tick(struct UBLC_player *ply);
void UBLC_player_move(struct UBLC_player *, float xa, float ya, float za);
void UBLC_player_moverelative(struct UBLC_player *, float xa, float za, float
		speed);

#endif /* PLAYER_H */
