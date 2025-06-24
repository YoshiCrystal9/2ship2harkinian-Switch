#include "ActorBehavior.h"

void Rando::ActorBehavior::InitDmHinaBehavior() {
    COND_VB_SHOULD(VB_DRAW_BOSS_REMAINS, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId checkId;
        switch (actor->params) {
            case 0: // Odolwa's Remains
                checkId = RC_WOODFALL_TEMPLE_BOSS_WARP;
                break;
            case 1: // Goht's Remains
                checkId = RC_SNOWHEAD_TEMPLE_BOSS_WARP;
                break;
            case 2: // Gyorg's Remains
                checkId = RC_GREAT_BAY_TEMPLE_BOSS_WARP;
                break;
            case 3: // Twinmold's Remains
                checkId = RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP;
                break;
            default:
                return;
        }
        *should = false;

        auto randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
        // Do not display if already obtained (i.e. for repeat visits)
        if (!randoSaveCheck.obtained) {
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, checkId), actor);
        }
    });
}
