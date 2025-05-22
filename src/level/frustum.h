#ifndef UBLC_LEVEL_FRUSTUM
#define UBLC_LEVEL_FRUSTUM 1

#include <gccvec.h>
#include <phys/AABB.h>

#define FRUSTUM_RIGHT 0
#define FRUSTUM_LEFT 1
#define FRUSTUM_BOTTOM 2
#define FRUSTUM_TOP 3
#define FRUSTUM_BACK 4
#define FRUSTUM_FRONT 5

gvec(float,4) *UBLC_frustum_get(gvec(float,4) frustum[6]);

int UBLC_frustum_pointinfrustum(float x, float y, float z);
int UBLC_frustum_sphereinfrustum(float x, float y, float z, float radius);
int UBLC_frustum_cubefullyinfrustum(float xlo, float ylo, float zlo, float xhi,
		float yhi, float zhi);
int UBLC_frustum_cubeinfrustum(struct UBLC_AABB *);

#endif /* UBLC_LEVEL_FRUSTUM */
