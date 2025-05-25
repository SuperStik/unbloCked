#ifndef PHYS_AABB_H
#define PHYS_AABB_H 1

#define UBLC_AABB_EPSILON 0.0f

struct UBLC_AABB {
	float x_lo;
	float y_lo;
	float z_lo;
	float x_hi;
	float y_hi;
	float z_hi;
};

#define UBLC_AABB_INIT(aabb, xlo, ylo, zlo, xhi, yhi, zhi) do { \
	struct UBLC_AABB *__ublcaabb__ = (aabb); \
	__ublcaabb__->x_lo = xlo; \
	__ublcaabb__->y_lo = ylo; \
	__ublcaabb__->z_lo = zlo; \
	__ublcaabb__->x_hi = xhi; \
	__ublcaabb__->y_hi = yhi; \
	__ublcaabb__->z_hi = zhi; \
} while(0)

struct UBLC_AABB *UBLC_AABB_expand(struct UBLC_AABB *, float xa, float ya,
		float za);

struct UBLC_AABB *UBLC_AABB_grow(struct UBLC_AABB *, float xa, float ya,
		float za);

float UBLC_AABB_clipXcollide(const struct UBLC_AABB *restrict, const struct
		UBLC_AABB *restrict c, float xa);

float UBLC_AABB_clipYcollide(const struct UBLC_AABB *restrict, const struct
		UBLC_AABB *restrict c, float ya);

float UBLC_AABB_clipZcollide(const struct UBLC_AABB *restrict, const struct
		UBLC_AABB *restrict c, float za);

int UBLC_AABB_intersects(const struct UBLC_AABB *restrict, const struct
		UBLC_AABB *restrict c);

void UBLC_AABB_move(struct UBLC_AABB *, float xa, float ya, float za);

#endif /* PHYS_AABB_H */
