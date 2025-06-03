#ifndef CHARACTER_CUBE_H
#define CHARACTER_CUBE_H 1

#include <level/vbuffer.h>

struct UBLC_cube {
	float x;
	float y;
	float z;

	float pitch;
	float yaw;
	float roll;
};

void UBLC_cube_genbox(struct UBLC_vbuffer[24], float x, float y, float z, float
		w, float h, float d, float texUoffs, float texVoffs);

void UBLC_cube_render(const struct UBLC_cube *, unsigned buf);

#endif /* CHARACTER_CUBE_H */
