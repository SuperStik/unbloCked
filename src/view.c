#include <math.h>
#include <pthread.h>
#include <string.h>

#include "gutl.h"
#include "gccvec.h"
#include "view.h"

static pthread_mutex_t viewlock = PTHREAD_MUTEX_INITIALIZER;
static float proj[16] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

static float view[16] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

static int view_lock(void);
static int view_unlock(void);

void UBLC_view_proj(float fovy, float aspect, float zNear, float zFar) {
	view_lock();

	GUTL_perspectivef(proj, fovy, aspect, zNear, zFar);

	view_unlock();
}

void UBLC_view_view(float x, float y, float z, float t, float v) {
	float sint, cost;
	float sinv, cosv;

	__sincospif(t / 360.0f, &sint, &cost);
	__sincospif(v / 360.0f, &sinv, &cosv);

	gvec(float,4) q = {
		cost * cosv,
		-sint * sinv,
		sint * cosv,
		cost * sint
	};

	gvec(float,4) qsqr = q * q;

	view_lock();

	view[0] = qsqr[0] + qsqr[1] - qsqr[2] - qsqr[3];
	view[1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	view[2] = 2.0f * (q[1] * q[3] + q[0] * q[2]);

	view[4] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	view[5] = qsqr[0] - qsqr[1] + qsqr[2] - qsqr[3];
	view[6] = 2.0f * (q[0] * q[1] + q[2] * q[3]);

	view[8] = 2.0f * (q[0] * q[2] + q[1] * q[3]);
	view[9] = 2.0f * (q[2] * q[3] - q[0] * q[1]);
	view[10] = qsqr[0] - qsqr[1] - qsqr[2] + qsqr[3];

	view_unlock();
}

float *UBLC_view_getproj(float *copyproj) {
	view_lock();

	memcpy(copyproj, proj, sizeof(proj));

	view_unlock();

	return copyproj;
}

float *UBLC_view_getview(float *copyview) {
	view_lock();

	memcpy(copyview, view, sizeof(view));

	view_unlock();

	return copyview;
}

static int view_lock(void) {
	return pthread_mutex_lock(&viewlock);
}

static int view_unlock(void) {
	return pthread_mutex_unlock(&viewlock);
}
