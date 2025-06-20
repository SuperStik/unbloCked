#ifndef UBLC_ENTITY_H
#define UBLC_ENTITY_H 1

#include <pthread.h>

#include <phys/AABB.h>

struct UBLC_entity_pos {
	float x;
	float y;
	float z;

	float xo;
	float yo;
	float zo;
};

struct UBLC_entity {
	pthread_rwlock_t lock;

	struct UBLC_entity_pos pos;

	float xd;
	float yd;
	float zd;

	float pitch;
	float yaw;
	struct UBLC_AABB aabb;

	unsigned char onground:1;
};

struct UBLC_entity *UBLC_entity_init(struct UBLC_entity *);
void UBLC_entity_delete(struct UBLC_entity *ply);

void UBLC_entity_tick(struct UBLC_entity *ent);

void UBLC_entity_turn(struct UBLC_entity *ent, float xo, float yo);

void UBLC_entity_getangles(struct UBLC_entity *, float *pitch, float *yaw);
void UBLC_entity_getpos(struct UBLC_entity *, float *x, float *y, float *z);
void UBLC_entity_getrenderpos(struct UBLC_entity *, struct UBLC_entity_pos *);

void UBLC_entity_move(struct UBLC_entity *ent, float xa, float ya, float za);
void UBLC_entity_moverelative(struct UBLC_entity *ent, float xa, float za, float
		speed);

void UBLC_entity_resetpos(struct UBLC_entity *ent);

#endif /* UBLC_ENTITY_H */
