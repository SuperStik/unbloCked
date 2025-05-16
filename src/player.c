#include <stdlib.h>

#include "level/level.h"
#include "player.h"

static void resetpos(struct UBLC_player *ply);
static void setpos(struct UBLC_player *ply, float x, float y, float z);

struct UBLC_player *UBLC_player_init(struct UBLC_player *ply) {
	ply->onground = 0;
	resetpos(ply);
	return ply;
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
	float w = 0.3f;
	float h = 0.9f;
	UBLC_AABB_INIT(&(ply->aabb), x - w, y - h, z - w, x + w, y + h, z + w);
}
