#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Az/z_en_az.h"

void func_80A979DC(EnAz* thisx, PlayState* play);
void func_80A97F9C(EnAz* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.BeaverRaceRingsCollected"
#define CVAR CVarGetInteger(CVAR_NAME, 20)

static bool minigameScoreSet = false; // Flag to track if the score has been set

void RegisterBeaverRace() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_AZ, CVAR < 20, [](Actor* actor, bool* should) {
        EnAz* enAz = (EnAz*)actor;
        Player* player = GET_PLAYER(gPlayState);

        if (!minigameScoreSet) {
            gSaveContext.minigameScore = CVAR;
            minigameScoreSet = true; // Set the flag after assignment
        }

        if (enAz->actionFunc != func_80A97F9C) {
            minigameScoreSet = false;
            return;
        }

        // Check if minigameScore has already been set

        bool allRingsCollected = false;

        // Check WEEKEVENTREG_24_01 and set condition based on the score.
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
            if (gSaveContext.minigameScore <= 0) {
                allRingsCollected = true;
            }
        } else {
            if (gSaveContext.minigameScore <= 0) {
                allRingsCollected = true;
            }
        }

        // If the condition is met, execute the shared code
        if (allRingsCollected) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_01);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            enAz->unk_374 &= ~0x10;
            gPlayState->nextEntrance = Entrance_CreateFromSpawn(2);
            gSaveContext.nextCutsceneIndex = 0;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            enAz->actor.speed = 0.0f;
            func_80A979DC(enAz, gPlayState);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBeaverRace, { CVAR_NAME });
