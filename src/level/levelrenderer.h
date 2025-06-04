#ifndef UBLC_LEVEL_LEVELRENDERER
#define UBLC_LEVEL_LEVELRENDERER 1

#include <player.h>

void UBLC_levelrenderer_initstatic(void);
void UBLC_levelrenderer_destroystatic(void);

void UBLC_levelrenderer_init(void);
void UBLC_levelrenderer_delete(void);

void UBLC_levelrenderer_render(struct UBLC_player *, int layer);

void UBLC_levelrenderer_setdirtyrange(unsigned xlo, unsigned ylo, unsigned zlo,
		unsigned xhi, unsigned yhi, unsigned zhi);
void UBLC_levelrenderer_setdirty(unsigned x, unsigned y, unsigned z);

#endif /* UBLC_LEVEL_LEVELRENDERER */
