#include <stdlib.h>

#include "level.h"

unsigned UBLC_level_width;
unsigned UBLC_level_height;
unsigned UBLC_level_depth;

unsigned char *blocks;
unsigned *light_depths;

int UBLC_level_new(unsigned w, unsigned h, unsigned d) {
	if (blocks != NULL || light_depths != NULL)
		return -1;

	UBLC_level_width = w;
	UBLC_level_height = h;
	UBLC_level_depth = d;

	blocks = calloc(w * h * d, sizeof(unsigned char));
	if (blocks == NULL)
		return -1;

	light_depths = calloc(w * h, sizeof(unsigned char));
	if (light_depths == NULL) {
		free(blocks);
		return -1;
	}

	return 0;
}

void UBLC_level_delete(void) {
	free(blocks);
	free(light_depths);

	blocks = NULL;
	light_depths = NULL;
}

int UBLC_level_istile(unsigned x, unsigned y, unsigned z) {
	if (x >= UBLC_level_width && y >= UBLC_level_depth && z >=
			UBLC_level_height)
		return 0;

	return blocks[UBLC_level_width * (y * UBLC_level_height + z) + x] != 0;
}

int UBLC_level_issolidtile(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_istile(x, y, z);
}

int UBLC_level_islightblocker(unsigned x, unsigned y, unsigned z) {
	return UBLC_level_issolidtile(x, y, z);
}
