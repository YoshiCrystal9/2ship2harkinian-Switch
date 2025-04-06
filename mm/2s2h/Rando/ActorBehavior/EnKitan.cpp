#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "overlays/actors/ovl_En_Kitan/z_en_kitan.h"
void func_80C09518(EnKitan* enKitan, PlayState* play);
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnKitanBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* getItemId = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_KITAN && *getItemId == GI_HEART_PIECE) { // Leave repeat rupee reward as-is for now
            *should = false;
            // The actor sets this flag using direct syntax, which does not trigger rando's FLAG_WEEK_EVENT_REG handling
            SET_WEEKEVENTREG(WEEKEVENTREG_79_80);
            ((EnKitan*)actor)->actionFunc = func_80C09518;
            // This forces the previous BGM to resume, ending the Keaton quiz BGM
            Audio_PlayFanfare(NA_BGM_GET_SMALL_ITEM);
            Player_TalkWithPlayer(gPlayState, actor);
        }
    });
}
