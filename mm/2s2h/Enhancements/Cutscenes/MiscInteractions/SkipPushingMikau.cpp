#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipPushingMikau() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ZOG, CVAR, [](Actor* actor, bool* should) {
        Flags_SetWeekEventReg(WEEKEVENTREG_88_10);
        Flags_SetWeekEventReg(WEEKEVENTREG_29_20);
        Flags_SetWeekEventReg(WEEKEVENTREG_91_01);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipPushingMikau, { CVAR_NAME });
