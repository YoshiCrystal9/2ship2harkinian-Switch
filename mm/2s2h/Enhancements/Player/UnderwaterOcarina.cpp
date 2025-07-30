#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64item.h"
}

#define CVAR_NAME "gEnhancements.Player.UnderwaterOcarina"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterUnderwaterOcarina() {
    COND_VB_SHOULD(VB_DISABLE_ITEM_UNDERWATER_FLOOR, CVAR, {
        const auto item = va_arg(args, s32);
        if (item == ITEM_OCARINA_OF_TIME) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterUnderwaterOcarina, { CVAR_NAME });
