#ifndef GUTL_H
#define GUTL_H 1

float *GUTL_perspectivef(float matrix[16], float fovy, float aspect, float
		zNear, float zFar);
double *GUTL_perspectived(double matrix[16], double fovy, double aspect, double
		zNear, double zFar);

unsigned GUTL_loadshaderfd(long shadertype, int fd);

int GUTL_linkandcheckprog(unsigned program);

const char *GUTL_errorstr(long error);

#endif /* GUTL_H */
