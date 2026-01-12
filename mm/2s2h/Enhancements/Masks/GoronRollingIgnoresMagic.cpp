#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Masks.GoronRollingIgnoresMagic"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
extern Input* sPlayerControlInput;
}

void RegisterGoronRollingIgnoresMagic() {
    // Disable magic consumption
    COND_VB_SHOULD(VB_GORON_ROLL_CONSUME_MAGIC, CVAR, { *should = false; });

    // Disable check for if the player has magic to increase spike level
    COND_VB_SHOULD(VB_GORON_ROLL_INCREASE_SPIKE_LEVEL, CVAR, { *should = true; });

    // Mimicking the vanilla condition minus the magic check
    COND_VB_SHOULD(VB_GORON_ROLL_DISABLE_SPIKE_MODE, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        bool disableSpikes = (player->stateFlags3 & PLAYER_STATE3_80000) &&
                             (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) ||
                              ((player->av1.actionVar1 == 4) && (player->unk_B08 < 12.0f)));
        if (!disableSpikes) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronRollingIgnoresMagic, { CVAR_NAME });
