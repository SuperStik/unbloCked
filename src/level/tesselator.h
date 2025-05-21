#ifndef UBLC_LEVEL_TESSELATOR_H
#define UBLC_LEVEL_TESSELATOR_H 1

struct UBLC_vertex {
	float x;
	float y;
	float z;
	float u;
	float v;
	float r;
	float g;
	float b;
};

void UBLC_tesselator_flush(void);
void UBLC_tesselator_vertex(struct UBLC_vertex *);

#endif /* UBLC_LEVEL_TESSELATOR_H */
