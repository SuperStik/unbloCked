#ifndef PLAYER_H
#define PLAYER_H 1

#include <pthread.h>

#include "phys/AABB.h"

#define UBLC_KF_UP (1 << 0)
#define UBLC_KF_DOWN (1 << 1)
#define UBLC_KF_LEFT (1 << 2)
#define UBLC_KF_RIGHT (1 << 3)
#define UBLC_KF_W (1 << 4)
#define UBLC_KF_S (1 << 5)
#define UBLC_KF_A (1 << 6)
#define UBLC_KF_D (1 << 7)
#define UBLC_KF_SPACE (1 << 8)
#define UBLC_KF_R (1 << 9)
#define UBLC_KF_LSHIFT (1 << 10)
#define UBLC_KF_RSHIFT (1 << 11)

#define UBLC_FPLY_ONGROUND (1 << 0)
#define UBLC_FPLY_FLYING (1 << 1)
#define UBLC_FPLY_HASRESET (1 << 2)

struct UBLC_player {
	pthread_rwlock_t lock;
	float xo;
	float yo;
	float zo;

	float x;
	float y;
	float z;

	float xd;
	float yd;
	float zd;

	float xb;
	float yb;
	float zb;
	float place;
	float smash;

	float pitch;
	float yaw;

	struct UBLC_AABB aabb;

	int keyflags;

	char flags;
};

struct UBLC_player *UBLC_player_init(struct UBLC_player *);
void UBLC_player_delete(struct UBLC_player *);

int UBLC_player_setkeys(struct UBLC_player *ply, int keys);
int UBLC_player_unsetkeys(struct UBLC_player *ply, int keys);
int UBLC_player_getkeys(const struct UBLC_player *ply);

int UBLC_player_setflying(struct UBLC_player *ply, int flying);
int UBLC_player_toggleflying(struct UBLC_player *ply);
int UBLC_player_getflying(const struct UBLC_player *ply);

void UBLC_player_turn(struct UBLC_player *, float xo, float yo);
void UBLC_player_getangles(struct UBLC_player *ply, float *pitch, float *yaw);

void UBLC_player_tick(struct UBLC_player *ply);
void UBLC_player_move(struct UBLC_player *, float xa, float ya, float za);
void UBLC_player_moverelative(struct UBLC_player *, float xa, float za, float
		speed);

#endif /* PLAYER_H */
