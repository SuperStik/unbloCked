#include "AABB.h"

struct UBLC_AABB *UBLC_aabb_expand(struct UBLC_AABB *aabb, float xa, float ya,
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
