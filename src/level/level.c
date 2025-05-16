#include <stdlib.h>

#include "level.h"

static unsigned width;
static unsigned height;
static unsigned depth;

unsigned char *blocks;
unsigned *light_depths;

int UBLC_level_new(unsigned w, unsigned h, unsigned d) {
	width = w;
	height = h;
	depth = d;

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
}
