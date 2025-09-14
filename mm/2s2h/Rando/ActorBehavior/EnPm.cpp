#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "variables.h"
void func_80837B60(PlayState* play, Player* player);
s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
}

void Rando::ActorBehavior::InitEnPmBehavior() {
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_PM) { // Postman
            MsgScript* script = va_arg(args, MsgScript*);
            Player* player = GET_PLAYER(gPlayState);
            switch (cmdId) {
                case MSCRIPT_CMD_ID_OFFER_ITEM:
                    // Lock the player into conversation because a notebook message might appear
                    func_80832558(gPlayState, player, func_80837B60);
                    *should = false;
                    break;
                case MSCRIPT_CMD_ID_CHECK_ITEM: {
                    /*
                     * The Postman's Hat check does a branch if the player already has the Postman's Hat in their
                     * inventory. In rando, it is possible to already have the Postman's Hat before ever giving him the
                     * Express Mail, so we're just going to always act as if the player does not have the Postman's Hat.
                     * This enables repeat rewards as well, which does slightly differ from vanilla.
                     */
                    MsgScriptCmdCheckItem* cmd = (MsgScriptCmdCheckItem*)*script;
                    ItemId itemId = (ItemId)SCRIPT_PACK_16(cmd->itemH, cmd->itemL);
                    if (itemId == ITEM_MASK_POSTMAN) {
                        *should = false;
                    }
                } break;
                case MSCRIPT_CMD_ID_DONE: // MSCRIPT_DONE
                    // Prevent softlocks in case a notebook message did not appear
                    Message_CloseTextbox(gPlayState);
                    break;
                default:
                    break;
            }
        }
    });
}
