#ifndef CHARACTER_ZOMBIE_H
#define CHARACTER_ZOMBIE_H 1

#include <character/cube.h>
#include <entity.h>

struct UBLC_zombie {
	struct UBLC_entity ent;

	struct UBLC_cube head;
	struct UBLC_cube body;
	struct UBLC_cube arml;
	struct UBLC_cube armr;
	struct UBLC_cube legl;
	struct UBLC_cube legr;

	float rot;
	float timeoffs;
	float speed;
	float rot_a;
};

void UBLC_zombie_initstatic(void);
void UBLC_zombie_destroystatic(void);

struct UBLC_zombie *UBLC_zombie_init(struct UBLC_zombie *, float x, float y,
		float z);
void UBLC_zombie_delete(struct UBLC_zombie *);

void UBLC_zombie_tick(struct UBLC_zombie *);

void UBLC_zombie_render(struct UBLC_zombie *, float a);

#endif /* CHARACTER_ZOMBIE_H */
