#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_Boss_Hakugin/z_boss_hakugin.h"

void BossHakugin_DrawIce(BossHakugin*, PlayState*);
}

void ShouldActorUpdate(Actor* actor, bool* should, RandoInf randoInf) {
    if (!Flags_GetRandoInf(randoInf)) {
        *should = false;
        actor->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else if (!actor->flags & ACTOR_FLAG_ATTENTION_ENABLED) {
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void ShouldActorDraw(Actor* actor, bool* should, RandoInf randoInf) {
    if (!Flags_GetRandoInf(randoInf)) {
        *should = false;
    }
}

void Rando::ActorBehavior::InitSoulsBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES;

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_HAKUGIN, shouldRegister, [](Actor* actor, bool* should) {
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_GOHT)) {
            BossHakugin_DrawIce((BossHakugin*)actor, gPlayState);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GOHT_UNFREEZE, shouldRegister, {
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_GOHT)) {
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_03, shouldRegister,
                 [](Actor* actor, bool* should) { ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_GYORG); });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_07, shouldRegister,
                 [](Actor* actor, bool* should) { ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_MAJORA); });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_01, shouldRegister,
                 [](Actor* actor, bool* should) { ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_ODOLWA); });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_02, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_03, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_GYORG);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_07, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_MAJORA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_01, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_ODOLWA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_02, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD);
    });
}
