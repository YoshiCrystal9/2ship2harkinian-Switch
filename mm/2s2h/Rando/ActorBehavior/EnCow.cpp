#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Cow/z_en_cow.h"
}

#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

RandoCheckId IdentifyCow(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_UNK_3].data;
    switch (gPlayState->sceneId) {
        case SCENE_REDEAD:
            randoCheckId = RC_BENEATH_THE_WELL_COW;
            break;
        case SCENE_F01:
            if (IS_AT(1947.0f, 1152.0f)) {
                randoCheckId = RC_ROMANI_RANCH_FIELD_COW_ENTRANCE;
            } else if (IS_AT(1126.0f, -1527.0f)) {
                randoCheckId = RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_BACK;
            } else if (IS_AT(1086.0f, -1290.0f)) {
                randoCheckId = RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_FRONT;
            }
            break;
        case SCENE_OMOYA:
            if (IS_AT(-311.0f, -97.0f)) {
                randoCheckId = RC_ROMANI_RANCH_BARN_COW_MIDDLE;
            } else if (IS_AT(-82.0f, -127.0f)) {
                randoCheckId = RC_ROMANI_RANCH_BARN_COW_LEFT;
            } else if (IS_AT(-83.0f, -32.0f)) {
                randoCheckId = RC_ROMANI_RANCH_BARN_COW_RIGHT;
            }
            break;
        case SCENE_KAKUSIANA:
            if (respawnData == -1) {
                if (IS_AT(2394.0f, 907.0f)) {
                    randoCheckId = RC_GREAT_BAY_COAST_COW_BACK;
                }
                if (IS_AT(2466.0f, 952.0f)) {
                    randoCheckId = RC_GREAT_BAY_COAST_COW_FRONT;
                }
            }
            if (respawnData == 31) {
                if (IS_AT(2394.0f, 907.0f)) {
                    randoCheckId = RC_TERMINA_FIELD_COW_BACK;
                }
                if (IS_AT(2466.0f, 952.0f)) {
                    randoCheckId = RC_TERMINA_FIELD_COW_FRONT;
                }
            }
            break;
        default:
            break;
    }
    return randoCheckId;
}

void Rando::ActorBehavior::InitEnCowBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_COW, IS_RANDO, {
        // Original Should is the Range check, if it fails just Return.
        Actor* actor = va_arg(args, Actor*);
        if (!((actor->xzDistToPlayer < 90.0f) &&
              ABS_ALT(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y)) < 25000)) {
            *should = false;
            return;
        }
        RandoCheckId randoCheckId = IdentifyCow(actor);

        if (randoCheckId == RC_UNKNOWN) {
            *should = true;
            return;
        }
        ((EnCow*)actor)->flags |= EN_COW_FLAG_WONT_GIVE_MILK;

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
            *should = true;
            return;
        }
        randoSaveCheck.eligible = true;
        *should = false;
    });

    COND_VB_SHOULD(VB_COW_CONSIDER_EPONAS_SONG_PLAYED, IS_RANDO, {
        EnCow* actor = va_arg(args, EnCow*);
        if (actor->flags & EN_COW_FLAG_WONT_GIVE_MILK) {
            gPlayState->msgCtx.songPlayed = 0;
        }
        *should = gPlayState->msgCtx.songPlayed == OCARINA_SONG_EPONAS;
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_COW, IS_RANDO, [](Actor* refActor, bool* should) {
        RandoCheckId randoCheckId = IdentifyCow(refActor);
        switch (randoCheckId) {
            case RC_ROMANI_RANCH_BARN_COW_LEFT:
                refActor->world.pos.x = -470.0f;
                refActor->world.pos.y = 1.0f;
                refActor->world.pos.z = 53.0f;
                refActor->shape.rot.y = 7116.0f;
                break;
            case RC_ROMANI_RANCH_BARN_COW_RIGHT:
                refActor->world.pos.x = -208.0f;
                refActor->world.pos.y = 0.0f;
                refActor->world.pos.z = 87.0f;
                refActor->shape.rot.y = -32768.0f;
                break;
            case RC_GREAT_BAY_COAST_COW_FRONT:
            case RC_TERMINA_FIELD_COW_FRONT:
                refActor->world.pos.x = 2503.0f;
                refActor->world.pos.y = 0.0f;
                refActor->world.pos.z = 907.0f;
                refActor->shape.rot.y = -5085.0f;
                break;
            case RC_GREAT_BAY_COAST_COW_BACK:
            case RC_TERMINA_FIELD_COW_BACK:
                refActor->world.pos.x = 2269.0f;
                refActor->world.pos.y = 0.0f;
                refActor->world.pos.z = 907.0f;
                refActor->shape.rot.y = 2950.0f;
                break;
            default:
                break;
        }
    });
}
