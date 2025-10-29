#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_Boss_Hakugin/z_boss_hakugin.h"

void BossHakugin_DrawIce(BossHakugin*, PlayState*);
}

bool shouldMajoraRegister() {
    bool registerStatus = false;
    if (IS_RANDO) {
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES ||
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_TRIFORCE_PIECES] == RO_GENERIC_YES) {
            registerStatus = true;
        }
    }
    return registerStatus;
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

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_07, shouldMajoraRegister(),
                 [](Actor* actor, bool* should) { ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_MAJORA); });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_01, shouldRegister,
                 [](Actor* actor, bool* should) { ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_ODOLWA); });

    COND_ID_HOOK(ShouldActorDraw, ACTOR_BOSS_02, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorDraw(actor, should, RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_03, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_GYORG);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_07, shouldMajoraRegister(), [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_MAJORA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_01, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_ODOLWA);
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BOSS_02, shouldRegister, [](Actor* actor, bool* should) {
        ShouldActorUpdate(actor, should, RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD);
    });

    /*
     * Giant's Mask functionality is handled by two pieces. The scene (Twinmold's Lair) determines whether the mask can
     * be used, while the Twinmold actor itself handles the transformation. Boss Souls prevent Twinmold from updating
     * unless its soul has been obtained, which results in a softlock. In this case, disable the item.
     */
    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, shouldRegister, {
        ItemId itemId = *va_arg(args, ItemId*);
        if (itemId == ITEM_MASK_GIANT && gPlayState->sceneId == SCENE_INISIE_BS &&
            !Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_TWINMOLD)) {
            *should = true;
        }
    });
}
