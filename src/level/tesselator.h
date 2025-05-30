#ifndef UBLC_LEVEL_TESSELATOR_H
#define UBLC_LEVEL_TESSELATOR_H 1

#include <level/vbuffer.h>

void UBLC_tesselator_vertex(struct UBLC_vbuffer *buf, float x, float y, float z,
		float u, float v, float r, float g, float b);

#endif /* UBLC_LEVEL_TESSELATOR_H */
