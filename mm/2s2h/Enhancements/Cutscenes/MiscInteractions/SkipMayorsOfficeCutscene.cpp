#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Dt/z_en_dt.h"
void func_80BEA394(EnDt* enDt, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// These skips still allow the first textboxes to display, but they do ignore most of the scenes
void RegisterSkipMayorsOfficeCutscene() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        if (gPlayState->sceneId == SCENE_SONCHONOIE) { // At Mayor's Residence
            s16* csId = va_arg(args, s16*);
            /*
             * The Mayor starts these cutscenes but passes the target actor, which always seems to be Viscen for
             * the start. The Mayor also handles the logic for progressing and ending these cutscenes. We need to
             * alter Dotour's state but cannot rely on the cutscene start actor for that. Because of this, we use
             * Actor_FindNearby to find the EnDt in this scene.
             */
            EnDt* enDt = (EnDt*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_DT, ACTORCAT_NPC,
                                                 99999.9f);
            if (enDt != nullptr) {
                if (*csId == 17) { // Argument scenes without Couples Mask
                    enDt->actionFunc = func_80BEA394;
                    enDt->unk_26E = 26;
                    enDt->unk_270 = 2;
                    enDt->unk_256 = 8;
                    *should = false;
                } else if (*csId == 21) { // Couples Mask scene
                    // Set flags to trigger scene transition and reward
                    enDt->actionFunc = func_80BEA394;
                    enDt->unk_244 = 0;
                    enDt->unk_256 = 20;
                    enDt->unk_270 = 0;
                    *should = false;
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipMayorsOfficeCutscene, { CVAR_NAME });
