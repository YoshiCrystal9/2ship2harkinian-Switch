#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterSkipHideoutDoorOpening() {
    COND_VB_SHOULD(VB_HIDEOUT_DOOR_OPEN, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterSkipHideoutDoorOpening, { CVAR_NAME });
