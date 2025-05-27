#ifndef UBLC_LEVEL_LEVELRENDERER
#define UBLC_LEVEL_LEVELRENDERER 1

#include <player.h>

void UBLC_levelrenderer_init(void);
void UBLC_levelrenderer_delete(void);

void UBLC_levelrenderer_render(struct UBLC_player *, int layer);
void UBLC_levelrenderer_pick(struct UBLC_player *);

#endif /* UBLC_LEVEL_LEVELRENDERER */
