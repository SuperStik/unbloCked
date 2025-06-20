#ifndef UBLC_LEVEL_FRUSTUM
#define UBLC_LEVEL_FRUSTUM 1

#include <gccvec.h>

#define FRUSTUM_RIGHT 0
#define FRUSTUM_LEFT 1
#define FRUSTUM_BOTTOM 2
#define FRUSTUM_TOP 3
#define FRUSTUM_BACK 4
#define FRUSTUM_FRONT 5

gvec(float,4) *UBLC_frustum_get(gvec(float,4) frustum[6]);

int UBLC_frustum_haspoint(const gvec(float,4) frustum[6], float x, float y,
		float z);
int UBLC_frustum_hassphere(const gvec(float,4) frustum[6], float x, float y,
		float z, float radius);
int UBLC_frustum_hascube(const gvec(float,4) frustum[6], float xlo, float ylo, float zlo, float xhi, float yhi, float zhi);

#endif /* UBLC_LEVEL_FRUSTUM */
