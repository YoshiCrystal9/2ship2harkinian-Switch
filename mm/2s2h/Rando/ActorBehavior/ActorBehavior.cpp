#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/ActorExtension/ActorExtension.h"

extern "C" {
#include "variables.h"
}

// This is kind of a catch-all for things that are simple enough to not need their own file.
void MiscVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list optionalArg) {
    switch (id) {
        case VB_MADAME_AROMA_ASK_FOR_HELP:
            *should = !CHECK_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK);
            break;
        case VB_GIVE_ITEM_FROM_ROMANI:
            *should = CHECK_QUEST_ITEM(QUEST_SONG_EPONA);
            break;
        // TODO: This should be configurable
        case VB_DOOR_HEALTH_CHECK_FAIL:
            *should = false;
            break;
        case VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI:
        case VB_MSG_SCRIPT_DEL_ITEM:
        case VB_GIVE_NEW_WAVE_BOSSA_NOVA:
        case VB_TOILET_HAND_TAKE_ITEM:
            *should = false;
            break;
    }
}

ActorExtensionId randoCheckIdActorExt = 0;

RandoCheckId Rando::ActorBehavior::GetActorRandoCheckId(Actor* actor) {
    RandoCheckId* actorRandoCheckId = (RandoCheckId*)ActorExtension_Get(actor, randoCheckIdActorExt);
    if (actorRandoCheckId == NULL) {
        return RC_UNKNOWN;
    }

    return *actorRandoCheckId;
}

void Rando::ActorBehavior::SetActorRandoCheckId(Actor* actor, RandoCheckId rc) {
    RandoCheckId* actorRandoCheckId = (RandoCheckId*)ActorExtension_Get(actor, randoCheckIdActorExt);

    if (actorRandoCheckId == NULL) {
        assert(false);
    } else {
        *actorRandoCheckId = rc;
    }
}

// Entry point for the module, run once on game boot
void Rando::ActorBehavior::Init() {
    if (randoCheckIdActorExt == 0) {
        randoCheckIdActorExt = ActorExtension_CreateForAll(sizeof(RandoCheckId));
    }
}

void Rando::ActorBehavior::OnFileLoad() {
    Rando::ActorBehavior::InitDmChar01Behavior();
    Rando::ActorBehavior::InitDmChar05Behavior();
    Rando::ActorBehavior::InitDmChar08Behavior();
    Rando::ActorBehavior::InitDmHinaBehavior();
    Rando::ActorBehavior::InitDmStkBehavior();
    Rando::ActorBehavior::InitDoorWarp1VBehavior();
    Rando::ActorBehavior::InitEnAkindonutsBehavior();
    Rando::ActorBehavior::InitEnAlBehavior();
    Rando::ActorBehavior::InitEnAnBehavior();
    Rando::ActorBehavior::InitEnAob01Behavior();
    Rando::ActorBehavior::InitEnAzBehavior();
    Rando::ActorBehavior::InitEnBabaBehavior();
    Rando::ActorBehavior::InitEnBalBehavior();
    Rando::ActorBehavior::InitEnBjtBehavior();
    Rando::ActorBehavior::InitEnBomBowlManBehavior();
    Rando::ActorBehavior::InitEnBoxBehavior();
    Rando::ActorBehavior::InitEnCowBehavior();
    Rando::ActorBehavior::InitEnDaiBehavior();
    Rando::ActorBehavior::InitEnDnhBehavior();
    Rando::ActorBehavior::InitEnElfgrpBehavior();
    Rando::ActorBehavior::InitEnElforgBehavior();
    Rando::ActorBehavior::InitEnFish2Behavior();
    Rando::ActorBehavior::InitEnFsnBehavior();
    Rando::ActorBehavior::InitEnFuBehavior();
    Rando::ActorBehavior::InitEnGamelupyBehavior();
    Rando::ActorBehavior::InitEnGb2Behavior();
    Rando::ActorBehavior::InitEnGegBehavior();
    Rando::ActorBehavior::InitEnGgBehavior();
    Rando::ActorBehavior::InitEnGinkoBehavior();
    Rando::ActorBehavior::InitEnGirlABehavior();
    Rando::ActorBehavior::InitEnGKBehavior();
    Rando::ActorBehavior::InitEnGoBehavior();
    Rando::ActorBehavior::InitEnGsBehavior();
    Rando::ActorBehavior::InitEnHgBehavior();
    Rando::ActorBehavior::InitEnInBehavior();
    Rando::ActorBehavior::InitEnItem00Behavior();
    Rando::ActorBehavior::InitEnJgameTsnBehavior();
    Rando::ActorBehavior::InitEnJgBehavior();
    Rando::ActorBehavior::InitEnJsBehavior();
    Rando::ActorBehavior::InitEnKgyBehavior();
    Rando::ActorBehavior::InitEnKitanBehavior();
    Rando::ActorBehavior::InitEnKnightBehavior();
    Rando::ActorBehavior::InitEnKujiyaBehavior();
    Rando::ActorBehavior::InitEnLiftNutsBehavior();
    Rando::ActorBehavior::InitEnMa4Behavior();
    Rando::ActorBehavior::InitEnMaYtoBehavior();
    Rando::ActorBehavior::InitEnMnkBehavior();
    Rando::ActorBehavior::InitEnNbBehavior();
    Rando::ActorBehavior::InitEnOsnBehavior();
    Rando::ActorBehavior::InitEnOtBehavior();
    Rando::ActorBehavior::InitEnOwlBehavior();
    Rando::ActorBehavior::InitEnPmBehavior();
    Rando::ActorBehavior::InitEnRuppecrowBehavior();
    Rando::ActorBehavior::InitEnRzBehavior();
    Rando::ActorBehavior::InitEnScopenutsBehavior();
    Rando::ActorBehavior::InitEnSellnutsBehavior();
    Rando::ActorBehavior::InitEnShnBehavior();
    Rando::ActorBehavior::InitEnSiBehavior();
    Rando::ActorBehavior::InitEnSob1Behavior();
    Rando::ActorBehavior::InitEnSshBehavior();
    Rando::ActorBehavior::InitEnStoneheishiBehavior();
    Rando::ActorBehavior::InitEnSyatekiManBehavior();
    Rando::ActorBehavior::InitEnTabBehavior();
    Rando::ActorBehavior::InitEnTalkBehavior();
    Rando::ActorBehavior::InitEnTotoBehavior();
    Rando::ActorBehavior::InitEnTrtBehavior();
    Rando::ActorBehavior::InitEnYbBehavior();
    Rando::ActorBehavior::InitEnZogBehavior();
    Rando::ActorBehavior::InitEnZotBehavior();
    Rando::ActorBehavior::InitEnZowBehavior();
    Rando::ActorBehavior::InitItemBHeartBehavior();
    Rando::ActorBehavior::InitObjKibakoBehavior();
    Rando::ActorBehavior::InitObjMoonStoneBehavior();
    Rando::ActorBehavior::InitObjSnowballBehavior();
    Rando::ActorBehavior::InitObjTaruBehavior();
    Rando::ActorBehavior::InitObjTsuboBehavior();
    Rando::ActorBehavior::InitObjWarpstoneBehavior();
    Rando::ActorBehavior::InitSoulsBehavior();

    COND_HOOK(ShouldVanillaBehavior, IS_RANDO, MiscVanillaBehaviorHandler);
}
