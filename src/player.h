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
};

struct UBLC_player *UBLC_player_init(struct UBLC_player *);

#endif /* PLAYER_H */
