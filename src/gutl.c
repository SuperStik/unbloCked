#include <math.h>

#include "gutl.h"

static void frustumf(float matrix[16], float left, float right, float bottom,
		float top, float znear, float zfar);

static void frustumd(double matrix[16], double left, double right, double
		bottom, double top, double znear, double zfar);

float *GUTL_perspectivef(float matrix[16], float fovy, float aspect, float
		zNear, float zFar) {
	float ymax, xmax;
	ymax = zNear * __tanpif(fovy / 360.0f);
	xmax = ymax * aspect;

	frustumf(matrix, -xmax, xmax, -ymax, ymax, zNear, zFar);
	return matrix;
}

double *GUTL_perspectived(double matrix[16], double fovy, double aspect, double
		zNear, double zFar) {
	double ymax, xmax;
	ymax = zNear * __tanpi(fovy / 360.0);
	xmax = ymax * aspect;

	frustumd(matrix, -xmax, xmax, -ymax, ymax, zNear, zFar);
	return matrix;
}

const char *GUTL_errorstr(GLenum glerr) {
	const char *str;
	switch (glerr) {
		case GL_INVALID_ENUM:
			str = "invalid enumerator";
			break;
		case GL_INVALID_VALUE:
			str = "invalid value";
			break;
		case GL_INVALID_OPERATION:
			str = "invalid operation";
			break;
		case GL_STACK_OVERFLOW:
			str = "stack overflow";
			break;
		case GL_STACK_UNDERFLOW:
			str = "stack underflow";
			break;
		case GL_OUT_OF_MEMORY:
			str = "out of memory";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "table too large";
			break;
		default:
			str = "no error";
	}

	return str;
}

static void frustumf(float matrix[16], float left, float right, float bottom,
		float top, float znear, float zfar) {
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;
	matrix[4] = 0.0f;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0f;
	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0f;
}

static void frustumd(double matrix[16], double left, double right, double
		bottom, double top, double znear, double zfar) {
	double temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}


