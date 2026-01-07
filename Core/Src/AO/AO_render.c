#include "AO_render.h"
#include "game.h"

void RenderAO_Handle(EventMsg *e)
{
    if(e->type == EVT_DRAW_FRAME)
    {
        Game_Draw();
    }
}
