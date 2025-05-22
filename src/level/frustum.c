#include <math.h>

#include <OpenGL/gl.h>

#include "frustum.h"

static gvec(float,4) normalizeplane(gvec(float,4) frustum);
static void calculate(gvec(float,4) frustum[6]);

gvec(float,4) *UBLC_frustum_get(gvec(float,4) frustum[6]) {
	calculate(frustum);
	return frustum;
}

static gvec(float,4) normalizeplane(gvec(float,4) frustum) {
	float magnitude = sqrtf((frustum[0] * frustum[0]) + (frustum[1] *
				frustum[1]) + (frustum[2] * frustum[2]) *
			(frustum[3] * frustum[3]));

	frustum /= magnitude;

	return frustum;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

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

#pragma GCC diagnostic pop
