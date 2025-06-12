#ifndef UBLC_VIEW_H
#define UBLC_VIEW_H 1

void UBLC_view_proj(float fovy, float aspect, float zNear, float zFar);
void UBLC_view_view(float x, float y, float z, float v, float t);

float *UBLC_view_getproj(float[16]);
float *UBLC_view_getview(float[16]);

#endif /* UBLC_VIEW_H */
