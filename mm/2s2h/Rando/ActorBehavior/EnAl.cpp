#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "variables.h"
void func_80837B60(PlayState* play, Player* player);
s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
void func_80848250(PlayState* play, Player* player);
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

static std::vector<u8> skipCmds = {};

void Rando::ActorBehavior::InitEnAlBehavior() {

    COND_ID_HOOK(OnActorInit, ACTOR_EN_AL, IS_RANDO, [](Actor* actor) { skipCmds.clear(); });

    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_AL) { // Madame Aroma
            Player* player = GET_PLAYER(gPlayState);

            if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
                *should = false;
                MsgScriptCmdOfferItem* cmd = (MsgScriptCmdOfferItem*)*va_arg(args, MsgScript*);
                GetItemId getItemId = (GetItemId)SCRIPT_PACK_16(cmd->itemIdH, cmd->itemIdL);
                skipCmds.clear();
                if (getItemId == GI_MASK_KAFEIS_MASK) { // Mayor's Residence
                    // Prevents the player from moving freely in case a notebook event message pops afterward
                    func_80832558(gPlayState, player, func_80837B60);
                } else { // Express Mail reward
                    /*
                     * We do something a little tricky here. We manually open a textbox with the message that normally
                     * plays after the player receives the reward (0x2B20), then also skip the MsgScript commands to
                     * open that textbox and wait on it. The func_80832558 call above does not work for this scenario,
                     * as it will softlock. More naive attempts at handling this actor case resulted in softlocks, not
                     * appropriately locking textboxes, duplicate textboxes, or Bombers' Notebook messages being eaten.
                     * The method below handles the intended behavior, both with or without notebook messages, even if
                     * it is a little counterintuitive.
                     */
                    Message_StartTextbox(gPlayState, 0x2B20, actor);
                    Player_TalkWithPlayer(gPlayState, actor);
                    func_80848250(gPlayState, player); // End the giveItem animation, or the Express Mail will persist
                    skipCmds.push_back(MSCRIPT_CMD_ID_BEGIN_TEXT); // The scripted text at textId 0x2B20
                    skipCmds.push_back(MSCRIPT_CMD_ID_AWAIT_TEXT);
                }
                return;
            }

            if (skipCmds.empty()) {
                return;
            }

            if (cmdId == skipCmds[0]) {
                skipCmds.erase(skipCmds.begin());
                *should = false;
            }
        }
    });
}
