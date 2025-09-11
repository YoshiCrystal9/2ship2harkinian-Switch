#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Rz/z_en_rz.h"
void func_80BFC270(EnRz* enRz, PlayState* play);
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipRosaSistersDance() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
            // West Clock Town
            if (gPlayState->sceneId == SCENE_ICHIBA) {
                if (*csId == 11) { // Link teaches the dance
                    Actor* actor = va_arg(args, Actor*);
                    EnRz* enRz = (EnRz*)actor;
                    // The function for yielding the Heart Piece and changing other state information
                    enRz->actionFunc = func_80BFC270;
                    // Queue the item check, as Actor_OfferGetItem won't work normally
                    // WEEKEVENTREG_75_80 is set once player completes this notebook entry.
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_75_80) && !IS_RANDO) {
                        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                            .showGetItemCutscene = true,
                            .param = GID_HEART_PIECE,
                            .giveItem = [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You received a Piece of Heart!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You received a Piece of Heart!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                            } });
                    }
                    Player* player = GET_PLAYER(gPlayState);
                    actor->parent = &player->actor;
                    player->talkActor = actor;
                    player->talkActorDistance = actor->xzDistToPlayer;
                    player->exchangeItemAction = PLAYER_IA_MINUS1;
                    Player_TalkWithPlayer(gPlayState, actor);
                    *should = false;
                } else if (*csId == 12) { // The sisters applaud Link
                    Actor* actor = va_arg(args, Actor*);
                    /*
                     * The randomizer actor behavior unsets this flag to prevent an extra textbox from appearing when
                     * this cutscene plays. If we're skipping the cutscene, we have to set it again to prevent the
                     * dialog from hanging. This does not affect vanilla.
                     */
                    actor->flags |= ACTOR_FLAG_TALK;
                    *should = false;
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipRosaSistersDance, { CVAR_NAME });
