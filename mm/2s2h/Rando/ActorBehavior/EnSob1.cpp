#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Sob1/z_en_sob1.h"
}

void EnSob1_DrawCustomItem(Actor* thisx, PlayState* play) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_01];

    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    Matrix_Translate(23.0f, 0.0f, -43.0f, MTXMODE_APPLY);

    s16 rotX = (s16)((-90.0f / 180.0f) * 32768.0f);
    Matrix_RotateZYX(rotX, 0, 0, MTXMODE_APPLY);

    Rando::DrawItem(randoSaveCheck.randoItemId);
}

void Rando::ActorBehavior::InitEnSob1Behavior() {
    COND_VB_SHOULD(VB_DRAW_ITEM_FROM_SOB1, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        if (RANDO_SAVE_CHECKS[RC_BOMB_SHOP_ITEM_01].shuffled) {
            EnSob1_DrawCustomItem(actor, gPlayState);
            *should = false;
        }
    });
}