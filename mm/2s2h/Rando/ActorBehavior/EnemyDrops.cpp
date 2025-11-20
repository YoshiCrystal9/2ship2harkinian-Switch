#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Slime/z_en_slime.h"
#include "overlays/actors/ovl_En_Sw/z_en_sw.h"
}

std::unordered_map<int16_t, RandoCheckId> actorIdToRandoCheckIdMap = {
    { ACTOR_EN_AM, RC_ENEMY_DROP_ARMOS },
    { ACTOR_EN_VM, RC_ENEMY_DROP_BEAMOS },
    { ACTOR_BOSS_05, RC_ENEMY_DROP_BIO_DEKU_BABA },
    { ACTOR_EN_BB, RC_ENEMY_DROP_BLUE_BUBBLE },
    { ACTOR_EN_RAT, RC_ENEMY_DROP_BOMBCHU },
    { ACTOR_EN_SLIME, RC_ENEMY_DROP_CHU },
    { ACTOR_EN_FAMOS, RC_ENEMY_DROP_DEATH_ARMOS },
    { ACTOR_EN_DEKUBABA, RC_ENEMY_DROP_DEKU_BABA },
    { ACTOR_EN_DODONGO, RC_ENEMY_DROP_DODONGO },
    { ACTOR_EN_GRASSHOPPER, RC_ENEMY_DROP_DRAGONFLY },
    { ACTOR_EN_SNOWMAN, RC_ENEMY_DROP_EENO },
    { ACTOR_EN_TUBO_TRAP, RC_ENEMY_DROP_FLYING_POT },
    { ACTOR_EN_FLOORMAS, RC_ENEMY_DROP_FLOORMASTER },
    { ACTOR_EN_FZ, RC_ENEMY_DROP_FREEZARD },
    { ACTOR_EN_CROW, RC_ENEMY_DROP_GUAY },
    { ACTOR_EN_PP, RC_ENEMY_DROP_HIPLOOP },
    { ACTOR_EN_IK, RC_ENEMY_DROP_IRON_KNUCKLE },
    { ACTOR_EN_FIREFLY, RC_ENEMY_DROP_KEESE },
    { ACTOR_EN_NEO_REEBA, RC_ENEMY_DROP_LEEVER },
    { ACTOR_EN_RR, RC_ENEMY_DROP_LIKE_LIKE },
    { ACTOR_EN_DEKUNUTS, RC_ENEMY_DROP_MAD_SCRUB },
    { ACTOR_EN_KAREBABA, RC_ENEMY_DROP_MINI_BABA },
    { ACTOR_EN_BAGUO, RC_ENEMY_DROP_NEJIRON },
    { ACTOR_EN_OKUTA, RC_ENEMY_DROP_OCTOROK },
    { ACTOR_EN_PEEHAT, RC_ENEMY_DROP_PEAHAT },
    { ACTOR_EN_BBFALL, RC_ENEMY_DROP_RED_BUBBLE },
    { ACTOR_EN_RD, RC_ENEMY_DROP_REDEAD },
    { ACTOR_EN_SB, RC_ENEMY_DROP_SHELLBLADE },
    { ACTOR_EN_PR2, RC_ENEMY_DROP_SKULLFISH },
    { ACTOR_EN_ST, RC_ENEMY_DROP_SKULLTULA },
    { ACTOR_EN_SW, RC_ENEMY_DROP_SKULLWALLTULA },
    { ACTOR_EN_KAME, RC_ENEMY_DROP_SNAPPER },
    { ACTOR_EN_SKB, RC_ENEMY_DROP_STALCHILD },
    { ACTOR_EN_TITE, RC_ENEMY_DROP_TEKTITE },
    { ACTOR_EN_WF, RC_ENEMY_DROP_WOLFOS },
    { ACTOR_EN_WALLMAS, RC_ENEMY_DROP_WALLMASTER },
};

RandoCheckId GetRandoCheckByActorId(int16_t actorId) {
    for (auto& map : actorIdToRandoCheckIdMap) {
        if (map.first == actorId) {
            return map.second;
        }
    }

    return RC_UNKNOWN;
}

Actor* FindActor(Vec3f position, ActorType actorType) {
    ActorListEntry actorList = gPlayState->actorCtx.actorLists[actorType];

    Actor* currentActor = actorList.first;
    for (size_t i = 0; i < actorList.length; i++) {
        if (currentActor->world.pos.x == position.x && currentActor->world.pos.y == position.y &&
            currentActor->world.pos.z == position.z) {
            return currentActor;
        } else {
            currentActor = currentActor->next;
        }
    }
    return nullptr;
}

void SpawnEnemyDrop(Vec3f position, Actor* actor, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
        position.x, position.y, position.z, 0,
        CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG, randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM), actor);
        });
}

void Rando::ActorBehavior::InitEnemyDropBehavior() {
    COND_VB_SHOULD(VB_ENEMY_DROP_COLLECTIBLE, IS_RANDO, {
        Vec3f position = va_arg(args, Vec3f);

        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_DROPS] == RO_GENERIC_OFF) {
            return;
        }

        Actor* foundActor = FindActor(position, ACTORCAT_ENEMY);
        if (foundActor == nullptr || foundActor->category != ACTORCAT_ENEMY) {
            return;
        }

        int16_t actorId = foundActor->id;

        // Skullwalltulas need special handling because most of them drop tokens
        if (actorId == ACTOR_EN_SW) {
            return;
        }

        RandoCheckId randoCheckId = GetRandoCheckByActorId(actorId);
        if (randoCheckId == RC_UNKNOWN || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        SpawnEnemyDrop(position, foundActor, randoCheckId);
        *should = false;
    });

    COND_ID_HOOK(OnActorKill, ACTOR_EN_SW, IS_RANDO, [](Actor* actor) {
        if (!ENSW_GET_3(actor)) {
            RandoCheckId randoCheckId = GetRandoCheckByActorId(ACTOR_EN_SW);
            if (RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
                return;
            }

            SpawnEnemyDrop(actor->world.pos, actor, randoCheckId);
        }
    });

    COND_VB_SHOULD(VB_DRAW_SLIME_RANDO_ITEM, IS_RANDO, {
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_DROPS] == RO_GENERIC_OFF) {
            return;
        }

        EnSlime* slime = va_arg(args, EnSlime*);
        RandoCheckId randoCheckId = GetRandoCheckByActorId(ACTOR_EN_SLIME);
        if (RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }
        RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId);

        Matrix_RotateYS(slime->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_Scale(0.25f, 0.25f, 0.25f, MTXMODE_APPLY);
        Rando::DrawItem(randoItemId);

        *should = false;
    });
}
