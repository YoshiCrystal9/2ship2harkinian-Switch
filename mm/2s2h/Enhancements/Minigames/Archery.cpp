#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"

void EnSyatekiMan_Swamp_RunGame(EnSyatekiMan* enSyatekiMan, PlayState* play);
void EnSyatekiMan_Town_RunGame(EnSyatekiMan* enSyatekiMan, PlayState* play);
}

#define SWAMP_CVAR_NAME "gEnhancements.Minigames.SwampArcheryScore"
#define SWAMP_CVAR CVarGetInteger(SWAMP_CVAR_NAME, 2180)
#define TOWN_CVAR_NAME "gEnhancements.Minigames.TownArcheryScore"
#define TOWN_CVAR CVarGetInteger(TOWN_CVAR_NAME, 50)

void RegisterArchery() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SYATEKI_MAN, SWAMP_CVAR != 2180, [](Actor* actor, bool* should) {
        EnSyatekiMan* enSyatekiMan = (EnSyatekiMan*)actor;

        if (enSyatekiMan->actionFunc == EnSyatekiMan_Swamp_RunGame) {
            // This checks if their current score plus the amount of bonus points they would get from the timer is
            // greater than or equal to the score required to win
            if (enSyatekiMan->score != 0 &&
                (enSyatekiMan->score + (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] / 10)) >= SWAMP_CVAR) {
                enSyatekiMan->score = 2120;
                enSyatekiMan->currentWave = 4;
                enSyatekiMan->wolfosFlags = 0;
                enSyatekiMan->bonusDekuScrubHitCounter = 2;
            }
        }
    });

    COND_VB_SHOULD(VB_ARCHERY_ADD_BONUS_POINTS, SWAMP_CVAR != 2180, {
        Actor* actor = va_arg(args, Actor*);
        s32* sBonusTimer = va_arg(args, s32*);

        *sBonusTimer = 11;
        *should = true;
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SYATEKI_MAN, TOWN_CVAR != 50, [](Actor* actor, bool* should) {
        EnSyatekiMan* enSyatekiMan = (EnSyatekiMan*)actor;

        if (enSyatekiMan->actionFunc == EnSyatekiMan_Town_RunGame) {
            if (enSyatekiMan->score >= TOWN_CVAR) {
                enSyatekiMan->score = 50;
                gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] = 0;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterArchery, { SWAMP_CVAR_NAME, TOWN_CVAR_NAME });
