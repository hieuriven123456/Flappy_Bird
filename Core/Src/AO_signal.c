#include "AO_signal.h"
#include "signal_handle.h"

void SignalAO_Handle(EventMsg *e)
{
    switch(e->type)
    {
        case EVT_GAME_UPDATE:
            Signal_Update();
            break;
        default:
            break;
    }
}
