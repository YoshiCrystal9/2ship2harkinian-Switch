#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// TODO: Handle other bosses like Majora
void RegisterSkipBossCutscenes() {
    // Odolwa intro
    COND_ID_HOOK(ShouldActorInit, ACTOR_BOSS_01, CVAR, [](Actor* actor, bool* should) { SET_EVENTINF(EVENTINF_54); });

    // Goht intro
    COND_ID_HOOK(ShouldActorInit, ACTOR_BOSS_HAKUGIN, CVAR, [](Actor* actor, bool* should) {
        /*
         * EVENTINF_62 gets set after seeing the boss lair intro cutscene.
         * EVENTINF_53 gets set after melting the ice (not skipped here, as that would skip the ice melting
         * requirement).
         */
        SET_EVENTINF(EVENTINF_62);
    });

    // Gyorg intro
    COND_ID_HOOK(ShouldActorInit, ACTOR_BOSS_03, CVAR, [](Actor* actor, bool* should) { SET_EVENTINF(EVENTINF_56); });

    // Twinmold intro
    COND_ID_HOOK(ShouldActorInit, ACTOR_BOSS_02, CVAR, [](Actor* actor, bool* should) { SET_EVENTINF(EVENTINF_55); });

    // Igos du Ikana (and lackeys) intro
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_KNIGHT, CVAR, [](Actor* actor, bool* should) {
        // In the credits, this sceneLayer will 1. Do not set this flag in that case, or things will break.
        if (gSaveContext.sceneLayer == 0) {
            SET_EVENTINF(EVENTINF_57);
        }
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        switch (gPlayState->sceneId) {
            case SCENE_MITURIN_BS: // Odolwa's Lair
                if (*csId == 9) {  // Warping out
                    *should = false;
                }
                break;
            case SCENE_MITURIN:    // Woodfall Temple
                if (*csId == 34) { // Cleared vines to room Deku Princess is in
                    *should = false;
                }
                break;
            case SCENE_HAKUGIN_BS: // Goht's Lair
                if (*csId == 10) { // Warping out
                    *should = false;
                }
                break;
            case SCENE_10YUKIYAMANOMURA2: // Mountain Village (Spring)
                if (*csId == 13) {        // Warping from Goht's Lair
                    *should = false;
                }
                break;
            case SCENE_SEA_BS:    // Gyorg's Lair
                if (*csId == 9) { // Warping out
                    *should = false;
                }
                break;
            case SCENE_31MISAKI:   // Zora Cape
                if (*csId == 18) { // Warping from Gyorg's Lair
                    *should = false;
                }
                break;
            case SCENE_INISIE_BS: // Twinmold's Lair
                if (*csId == 9) { // Warping out
                    *should = false;
                }
                break;
            case SCENE_IKANA:      // Ikana Canyon
                if (*csId == 31) { // Warping from Twinmold's Lair
                    *should = false;
                }
                break;
            default:
                break;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBossCutscenes, { CVAR_NAME });
