#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnShnBehavior() {

    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_SHN && cmdId == MSCRIPT_CMD_06) { // Swamp Tourist Center Guide
            MsgScript* script = va_arg(args, MsgScript*);
            GetItemId getItemId = (GetItemId)MSCRIPT_GET_16(script, 1);
            if (getItemId == GI_HEART_PIECE) { // Showed picture of Tingle or the Deku King
                // Do not do any substituted behavior, just skip this single command
                *should = false;
            }
        }
    });
}
