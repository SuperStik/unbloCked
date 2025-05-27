#ifndef GUTL_H
#define GUTL_H 1

#include <SDL3/SDL_opengl.h>

float *GUTL_perspectivef(float matrix[16], float fovy, float aspect, float
		zNear, float zFar);
double *GUTL_perspectived(double matrix[16], double fovy, double aspect, double
		zNear, double zFar);

const char *GUTL_errorstr(GLenum);

#endif /* GUTL_H */
