#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "include/z64player.h"
extern s32 Player_SetAction(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
extern void Player_Action_1(Player* player, PlayState* play);
}

void RespawnOnWaterTouch(Player* player) {
    // This is Honey & Darlings Shop, touching the water ends the minigame as its vanilla behavior.
    // No reason to handle it a second time here.
    if (gPlayState->sceneId == SCENE_BOWLING) {
        return;
    }

    if (player->stateFlags1 & PLAYER_STATE1_8000000) {
        // Mimic Deku Hop failure behavior
        Player_SetAction(gPlayState, player, Player_Action_1, 0);
        player->stateFlags1 |= PLAYER_STATE1_20000000;
    }
}

void Rando::ActorBehavior::InitPlayerBehavior() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SWIM], [](Actor* actor) {
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SWIM)) {
            RespawnOnWaterTouch(GET_PLAYER(gPlayState));
        }
    });
}
