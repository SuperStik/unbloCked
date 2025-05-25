#include <err.h>
#include <stdio.h>
#include "AABB.h"

struct UBLC_AABB *UBLC_AABB_expand(struct UBLC_AABB *aabb, float xa, float ya,
		float za) {
	if (xa < 0.0f)
		aabb->x_lo += xa;
	else if (xa > 0.0f)
		aabb->x_hi += xa;

	if (ya < 0.0f)
		aabb->y_lo += ya;
	else if (ya > 0.0f)
		aabb->y_hi += ya;

	if (za < 0.0f)
		aabb->z_lo += za;
	else if (za > 0.0f)
		aabb->z_hi += za;

	return aabb;
}

struct UBLC_AABB *UBLC_AABB_grow(struct UBLC_AABB *aabb, float xa, float ya,
		float za) {
	aabb->x_lo -= xa;
	aabb->y_lo -= ya;
	aabb->z_lo -= za;
	aabb->x_hi += xa;
	aabb->y_hi += ya;
	aabb->z_hi += za;
	return aabb;
}

float UBLC_AABB_clipXcollide(const struct UBLC_AABB *restrict aabb, const struct
		UBLC_AABB *restrict c, float xa) {
	if (!(c->y_hi > aabb->y_lo && c->y_lo < aabb->y_hi))
		goto fastreturnx;
	if (!(c->z_hi > aabb->z_lo && c->z_lo < aabb->z_hi))
		goto fastreturnx;

	float max = aabb->x_lo - c->x_hi - UBLC_AABB_EPSILON;
	if (xa > 0.0f && c->x_hi <= aabb->x_lo && max < xa)
		xa = max;

	max = aabb->x_hi - c->x_lo + UBLC_AABB_EPSILON;
	if (xa < 0.0f && c->x_lo >= aabb->x_hi && max > xa)
		xa = max;

fastreturnx:
	return xa;
}

float UBLC_AABB_clipYcollide(const struct UBLC_AABB *restrict aabb, const struct
		UBLC_AABB *restrict c, float ya) {
	if (!(c->x_hi > aabb->x_lo && c->x_lo < aabb->x_hi))
		goto fastreturny;
	if (!(c->z_hi > aabb->z_lo && c->z_lo < aabb->z_hi))
		goto fastreturny;

	float max = aabb->y_lo - c->y_hi - UBLC_AABB_EPSILON;
	if (ya > 0.0f && c->y_hi <= aabb->y_lo && max < ya)
		ya = max;

	max = aabb->y_hi - c->y_lo + UBLC_AABB_EPSILON;
	if (ya < 0.0f && c->y_lo >= aabb->y_hi && max > ya)
		ya = max;

fastreturny:
	return ya;
}

float UBLC_AABB_clipZcollide(const struct UBLC_AABB *restrict aabb, const struct
		UBLC_AABB *restrict c, float za) {
	if (!(c->x_hi > aabb->x_lo && c->x_lo < aabb->x_hi))
		goto fastreturnz;
	if (!(c->y_hi > aabb->y_lo && c->y_lo < aabb->y_hi))
		goto fastreturnz;

	float max = aabb->z_lo - c->z_hi - UBLC_AABB_EPSILON;
	if (za > 0.0f && c->z_hi <= aabb->z_lo && max < za)
		za = max;

	max = aabb->z_hi - c->z_lo + UBLC_AABB_EPSILON;
	if (za < 0.0f && c->z_lo >= aabb->z_hi && max > za)
		za = max;

fastreturnz:
	return za;
}

int UBLC_AABB_intersects(const struct UBLC_AABB *restrict aabb, const struct
		UBLC_AABB *restrict c) {
	int intersects = c->x_hi > aabb->x_lo;
	intersects = intersects && c->x_lo < aabb->x_hi;

	intersects = intersects && c->y_hi > aabb->y_lo;
	intersects = intersects && c->y_lo < aabb->y_hi;

	intersects = intersects && c->z_hi > aabb->z_lo;
	intersects = intersects && c->z_lo < aabb->z_hi;

	return intersects;
}

void UBLC_AABB_move(struct UBLC_AABB *aabb, float xa, float ya, float za) {
	aabb->x_lo += xa;
	aabb->y_lo += ya;
	aabb->z_lo += za;
	aabb->x_hi += xa;
	aabb->y_hi += ya;
	aabb->z_hi += za;
}
