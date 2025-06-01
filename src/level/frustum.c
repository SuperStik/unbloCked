#include <math.h>

#include <SDL3/SDL_opengl.h>

#include "frustum.h"

static gvec(float,4) normalizeplane(gvec(float,4) frustum);
static void calculate(gvec(float,4) frustum[6]);

gvec(float,4) *UBLC_frustum_get(gvec(float,4) frustum[6]) {
	calculate(frustum);
	return frustum;
}

int UBLC_frustum_haspoint(const gvec(float,4) frustum[6], float x, float y,
		float z) {
	for (int i = 0; i < 6; ++i) {
		if ((frustum[i][0] * x + frustum[i][1] * y + frustum[i][2] * z)
				<= 0.0f)
			return 0;
	}

	return 1;
}

int UBLC_frustum_hassphere(const gvec(float,4) frustum[6], float x, float y,
		float z, float radius) {
	for (int i = 0; i < 6; ++i) {
		if ((frustum[i][0] * x + frustum[i][1] * y + frustum[i][2] * z)
				<= -radius)
			return 0;
	}

	return 1;
}

int UBLC_frustum_hascube(const gvec(float,4) frustum[6], float xlo, float
		ylo, float zlo, float xhi, float yhi, float zhi) {
	float x[2] = {xlo, xhi};
	float y[2] = {ylo, yhi};
	float z[2] = {zlo, zhi};

	/* egyptian pyramids */
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				for (int l = 0; l < 2; ++l) {
					if ((frustum[i][0] * x[j] +
								frustum[i][1] *
								y[k] +
								frustum[i][2] *
								z[l]) <= 0)
						return 0;
				}
			}
		}
	}

	return 1;
}

static gvec(float,4) normalizeplane(gvec(float,4) frustum) {
	float magnitude = sqrtf((frustum[0] * frustum[0]) + (frustum[1] *
				frustum[1]) + (frustum[2] * frustum[2]) +
			(frustum[3] * frustum[3]));

	frustum /= magnitude;

	return frustum;
}

static void calculate(gvec(float,4) frustum[6]) {
	union {
		float array;
		gvec(float,4) mat[4];
	} proj, modl, clip;

	glGetFloatv(GL_PROJECTION_MATRIX, &proj.array);
	glGetFloatv(GL_MODELVIEW_MATRIX, &proj.array);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			clip.mat[i][j] = modl.mat[i][0] * proj.mat[0][j] +
				modl.mat[i][1] * proj.mat[1][j] + modl.mat[i][2]
				* proj.mat[2][j] + modl.mat[i][3] *
				proj.mat[3][j];
		}
	}

	for (int i = 0; i < 6; i += 2) {
		float inv = ((i / 2) & 1) ? -1.0f : 1.0f;

		for (int j = 0; j < 4; ++j) {
			frustum[i][j] = clip.mat[j][3] - clip.mat[0][i / 2] *
				inv;
			frustum[i+1][j] = clip.mat[j][3] + clip.mat[0][i / 2] *
				inv;
		}

		frustum[i] = normalizeplane(frustum[i]);
		frustum[i+1] = normalizeplane(frustum[i+1]);
	}
}
