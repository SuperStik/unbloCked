#ifndef UBLC_LEVEL_TESSELATOR_H
#define UBLC_LEVEL_TESSELATOR_H 1

#include <level/vbuffer.h>

void UBLC_tesselator_flush(void);

void UBLC_tesselator_init(void);

int UBLC_tesselator_setcolor(int enabled);
int UBLC_tesselator_settexture(int enabled);
int UBLC_tesselator_getcolor(void);
int UBLC_tesselator_getcolor(void);

void UBLC_tesselator_vertex(struct UBLC_vbuffer *buf, float x, float y, float z,
		float u, float v, float r, float g, float b);

#endif /* UBLC_LEVEL_TESSELATOR_H */
