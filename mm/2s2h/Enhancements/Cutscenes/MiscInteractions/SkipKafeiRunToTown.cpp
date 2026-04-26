#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Skips the cutscene where Kafei runs back to town after completing Sakon's Hideout
static void RegisterSkipKafeiRunToTown() {
    COND_VB_SHOULD(VB_KAFEI_RUN_TO_TOWN, CVAR, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterSkipKafeiRunToTown, { CVAR_NAME });
