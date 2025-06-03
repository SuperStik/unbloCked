#ifndef PLAYER_H
#define PLAYER_H 1

#include <entity.h>

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
#define UBLC_KF_V (1 << 12)

struct UBLC_player {
	struct UBLC_entity ent;

	unsigned xb;
	unsigned yb;
	unsigned zb;

	float place;
	float smash;

	int keyflags;

	unsigned char flying:1;

	unsigned char hasselect:1;
	unsigned char placeface:3;
};

struct UBLC_player *UBLC_player_init(struct UBLC_player *);
void UBLC_player_delete(struct UBLC_player *);

int UBLC_player_setkeys(struct UBLC_player *ply, int keys);
int UBLC_player_unsetkeys(struct UBLC_player *ply, int keys);
int UBLC_player_getkeys(const struct UBLC_player *ply);

void UBLC_player_tick(struct UBLC_player *ply);

#endif /* PLAYER_H */
