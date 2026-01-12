#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipOnePointCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipOnePointCutscenes() {
    COND_VB_SHOULD(VB_CAMERA_SET_FOCAL_ACTOR, CVAR, {
        Actor* actor = va_arg(args, Actor*);

        switch (actor->id) {
            case ACTOR_EN_BAL:
                *should = false;
                break;
            default:
                break;
        }
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId == -1 || actor == NULL) {
            return;
        }

        switch (actor->id) {
            case ACTOR_OBJ_SYOKUDAI: { // Torch
                ObjSyokudai* torch = (ObjSyokudai*)actor;
                s32 switchFlag = OBJ_SYOKUDAI_GET_SWITCH_FLAG(actor);

                // Set the flag if needed
                if (torch->pendingAction >= OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH) {
                    Flags_SetSwitch(gPlayState, switchFlag);
                }

                torch->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_NONE;
                torch->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                *should = false;
                break;
            }
            case ACTOR_OBJ_COMB:
                if (gPlayState->sceneId != SCENE_PIRATE) {
                    actor->csId = -1;
                    *should = false;
                }
                break;
            case ACTOR_OBJ_BEAN:        // Bean Patch
            case ACTOR_OBJ_MAKEKINSUTA: // Bean Patch
            case ACTOR_OBJ_SPIDERTENT:
                actor->csId = -1;
                *should = false;
                break;
            case ACTOR_EN_BOX: // Chest
                // Currently this breaks the treasure chest minigame, so we're not skipping there
                if (gPlayState->sceneId != SCENE_TAKARAYA) {
                    *should = false;
                }
                break;
            case ACTOR_BG_SPDWEB: // Spider Web
            case ACTOR_DOOR_SHUTTER:
            case ACTOR_BG_NUMA_HANA: // Big wooden flower in Woodfall Temple
            case ACTOR_BG_LADDER:
            case ACTOR_OBJ_RAILLIFT: // Moving Platform
            case ACTOR_OBJ_SWITCH:
            case ACTOR_OBJ_FIRESHIELD:
            case ACTOR_OBJ_ICE_POLY:
            case ACTOR_BG_DBLUE_MOVEBG: // Moveable block in Great Bay Temple
            case ACTOR_OBJ_HUNSUI:      // Geyser in Great Bay Temple
            case ACTOR_BG_KIN2_BOMBWALL:
            case ACTOR_BG_ASTR_BOMBWALL:
            case ACTOR_BG_KIN2_PICTURE:
            case ACTOR_BG_HAKUGIN_ELVPOLE:
            case ACTOR_BG_HAKUGIN_SWITCH:
            case ACTOR_BG_HAKUGIN_POST:
            case ACTOR_OBJ_Y2SHUTTER:
            case ACTOR_OBJ_LIGHTBLOCK:
            case ACTOR_OBJ_LIGHTSWITCH:
            case ACTOR_BG_IKANA_BOMBWALL:
            case ACTOR_BG_IKANA_BLOCK:
            case ACTOR_BG_HAKUGIN_BOMBWALL:
            case ACTOR_EN_SW:
            case ACTOR_OBJ_CHAN:
            case ACTOR_EN_MM:
            case ACTOR_BG_F40_BLOCK:
            case ACTOR_EN_BAL:
            case ACTOR_BG_TOBIRA01:
            case ACTOR_OBJ_BIGICICLE:
            case ACTOR_OBJ_HAKAISI:
            case ACTOR_BG_HAKA_BOMBWALL:
            case ACTOR_EN_DRAGON:
            case ACTOR_BG_DBLUE_BALANCE:
            case ACTOR_BG_DBLUE_WATERFALL:
            case ACTOR_OBJ_ICEBLOCK:
            case ACTOR_BG_IKNIN_SUSCEIL:
            case ACTOR_BG_IKANA_DHARMA:
            case ACTOR_OBJ_HUGEBOMBIWA:
                *should = false;
                break;
            default:
                // SPDLOG_INFO("Unhandled actor id: {}", actor->id);
                break;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipOnePointCutscenes, { CVAR_NAME });
