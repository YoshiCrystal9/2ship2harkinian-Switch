#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/CustomItem/CustomItem.h"
#include "assets/2s2h_assets.h"
#include "2s2h/ActorExtension/ActorListIndex.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Snowball/z_obj_snowball.h"
#include "src/overlays/actors/ovl_Obj_Snowball2/z_obj_snowball2.h"

void func_80B02D58(ObjSnowball* snowballActor, PlayState* play);
}

// clang-format off
std::map<std::tuple<s16, s16, s16>, RandoCheckId> snowballActorIdMap = {
    { { SCENE_11GORONNOSATO,    0,  29 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_01 },
    { { SCENE_11GORONNOSATO,    0,  30 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_02 },
    { { SCENE_11GORONNOSATO,    0,  31 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_03 },
    { { SCENE_11GORONNOSATO,    0,  32 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_04 },
    { { SCENE_11GORONNOSATO,    0,  33 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_05 },
    { { SCENE_11GORONNOSATO,    0,  34 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_06 },
    { { SCENE_11GORONNOSATO,    0,  35 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_07 },
    { { SCENE_11GORONNOSATO,    0,  36 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_08 },
    { { SCENE_11GORONNOSATO,    0,  37 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_09 },
    { { SCENE_11GORONNOSATO,    0,  38 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_10 },
    { { SCENE_11GORONNOSATO,    0,  39 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_11 },
    { { SCENE_11GORONNOSATO,    0,  40 }, RC_GORON_VILLAGE_SMALL_SNOWBALL_12 },
    { { SCENE_11GORONNOSATO,    0,  50 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_01 },
    { { SCENE_11GORONNOSATO,    0,  51 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_02 },
    { { SCENE_11GORONNOSATO,    0,  52 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_03 },
    { { SCENE_11GORONNOSATO,    0,  53 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_04 },
    { { SCENE_11GORONNOSATO,    0,  54 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_05 },
    { { SCENE_11GORONNOSATO,    0,  55 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_06 },
    { { SCENE_11GORONNOSATO,    0,  56 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_07 },
    { { SCENE_11GORONNOSATO,    0,  57 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_08 },
    { { SCENE_11GORONNOSATO,    0,  58 }, RC_GORON_VILLAGE_LARGE_SNOWBALL_09 },
    { { SCENE_10YUKIYAMANOMURA, 0,  39 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_01 },
    { { SCENE_10YUKIYAMANOMURA, 0,  40 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_02 },
    { { SCENE_10YUKIYAMANOMURA, 0,  41 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_03 },
    { { SCENE_10YUKIYAMANOMURA, 0,  42 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_04 },
    { { SCENE_10YUKIYAMANOMURA, 0,  43 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_05 },
    { { SCENE_10YUKIYAMANOMURA, 0,  49 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_10 },
    { { SCENE_10YUKIYAMANOMURA, 0,  50 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_11 },
    { { SCENE_10YUKIYAMANOMURA, 0,  53 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_08 },
    { { SCENE_10YUKIYAMANOMURA, 0,  54 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_09 },
    { { SCENE_10YUKIYAMANOMURA, 0,  55 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_06 },
    { { SCENE_10YUKIYAMANOMURA, 0,  56 }, RC_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_07 },
    { { SCENE_13HUBUKINOMITI,   0,  33 }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_08 },
    { { SCENE_13HUBUKINOMITI,   0,  34 }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_05 },
    { { SCENE_13HUBUKINOMITI,   0,  35 }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_06 },
    { { SCENE_13HUBUKINOMITI,   0,  36 }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_07 },
    { { SCENE_13HUBUKINOMITI,   0,  37 }, RC_PATH_TO_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_01 },
    { { SCENE_13HUBUKINOMITI,   0,  38 }, RC_PATH_TO_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_02 },
    { { SCENE_13HUBUKINOMITI,   0,  39 }, RC_PATH_TO_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_03 },
    { { SCENE_13HUBUKINOMITI,   0,  40 }, RC_PATH_TO_MOUNTAIN_VILLAGE_SMALL_SNOWBALL_04 },
    { { SCENE_14YUKIDAMANOMITI, 0,  9 },  RC_PATH_TO_SNOWHEAD_SMALL_SNOWBALL_01 },
    { { SCENE_14YUKIDAMANOMITI, 0,  10 }, RC_PATH_TO_SNOWHEAD_SMALL_SNOWBALL_02 },
    { { SCENE_14YUKIDAMANOMITI, 0,  11 }, RC_PATH_TO_SNOWHEAD_SMALL_SNOWBALL_03 },
    { { SCENE_14YUKIDAMANOMITI, 0,  28 }, RC_PATH_TO_SNOWHEAD_LARGE_SNOWBALL_03 },
    { { SCENE_14YUKIDAMANOMITI, 0,  29 }, RC_PATH_TO_SNOWHEAD_LARGE_SNOWBALL_04 },
    { { SCENE_14YUKIDAMANOMITI, 0,  30 }, RC_PATH_TO_SNOWHEAD_LARGE_SNOWBALL_01 },
    { { SCENE_14YUKIDAMANOMITI, 0,  31 }, RC_PATH_TO_SNOWHEAD_LARGE_SNOWBALL_02 },
    { { SCENE_12HAKUGINMAE,     0,  42 }, RC_SNOWHEAD_SMALL_SNOWBALL_01 },
    { { SCENE_12HAKUGINMAE,     0,  43 }, RC_SNOWHEAD_SMALL_SNOWBALL_02 },
    { { SCENE_12HAKUGINMAE,     0,  44 }, RC_SNOWHEAD_SMALL_SNOWBALL_03 },
    { { SCENE_12HAKUGINMAE,     0,  45 }, RC_SNOWHEAD_SMALL_SNOWBALL_04 },
    { { SCENE_12HAKUGINMAE,     0,  46 }, RC_SNOWHEAD_LARGE_SNOWBALL_01 },
    { { SCENE_12HAKUGINMAE,     0,  47 }, RC_SNOWHEAD_LARGE_SNOWBALL_02 },
    { { SCENE_12HAKUGINMAE,     0,  48 }, RC_SNOWHEAD_LARGE_SNOWBALL_03 },
    { { SCENE_12HAKUGINMAE,     0,  49 }, RC_SNOWHEAD_LARGE_SNOWBALL_04 },
    { { SCENE_12HAKUGINMAE,     0,  50 }, RC_SNOWHEAD_LARGE_SNOWBALL_05 },
    { { SCENE_12HAKUGINMAE,     0,  51 }, RC_SNOWHEAD_LARGE_SNOWBALL_06 },
    { { SCENE_HAKUGIN,          2,  3 },  RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_SMALL_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          2,  4 },  RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_SMALL_SNOWBALL_02 },
    { { SCENE_HAKUGIN,          4,  33 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_06 },
    { { SCENE_HAKUGIN,          4,  34 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_04 },
    { { SCENE_HAKUGIN,          4,  35 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_03 },
    { { SCENE_HAKUGIN,          4,  36 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_02 },
    { { SCENE_HAKUGIN,          4,  37 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          4,  38 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_SMALL_SNOWBALL_05 },
    { { SCENE_HAKUGIN,          4,  75 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_3_LARGE_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          4,  76 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_3_LARGE_SNOWBALL_02 },
    { { SCENE_HAKUGIN,          4,  77 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_3_LARGE_SNOWBALL_03 },
    { { SCENE_HAKUGIN,          4,  78 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_3_LARGE_SNOWBALL_04 },
    { { SCENE_HAKUGIN,          7,  8 },  RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_SMALL_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          7,  9 },  RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_SMALL_SNOWBALL_02 },
    { { SCENE_HAKUGIN,          7,  10 }, RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_SMALL_SNOWBALL_03 },
    { { SCENE_HAKUGIN,          7,  11 }, RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_SMALL_SNOWBALL_04 },
    { { SCENE_HAKUGIN,          7,  12 }, RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_SMALL_SNOWBALL_05 },
    { { SCENE_HAKUGIN,          7,  26 }, RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_LARGE_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          10, 8 },  RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_01 },
    { { SCENE_HAKUGIN,          10, 9 },  RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_02 },
    { { SCENE_HAKUGIN,          10, 10 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_03 },
    { { SCENE_HAKUGIN,          10, 11 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_04 },
    { { SCENE_HAKUGIN,          10, 12 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_05 },
    { { SCENE_HAKUGIN,          10, 13 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_06 },
    { { SCENE_HAKUGIN,          10, 14 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_07 },
    { { SCENE_HAKUGIN,          10, 15 }, RC_SNOWHEAD_TEMPLE_SNOW_ROOM_SMALL_SNOWBALL_08 },
    { { SCENE_17SETUGEN,        0,  62 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_01 },
    { { SCENE_17SETUGEN,        0,  63 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_02 },
    { { SCENE_17SETUGEN,        0,  64 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_03 },
    { { SCENE_17SETUGEN,        0,  65 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_04 },
    { { SCENE_17SETUGEN,        0,  66 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_05 },
    { { SCENE_17SETUGEN,        0,  67 }, RC_TWIN_ISLANDS_SMALL_SNOWBALL_06 },
};

std::map<std::pair<float, float>, RandoCheckId> snowballCoordsMap = {
    { { -1025.00f,   137.00f }, RC_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_01 },
    { { -1062.00f,   891.00f }, RC_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_02 },
    { {  1863.00f,   705.00f }, RC_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_03 },
    { {  -197.00f,  -172.00f }, RC_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_04 },
    { {  1462.00f,   863.00f }, RC_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_05 },
    { {  1674.00f,  6811.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_01 },
    { {  2061.00f,  6748.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_02 },
    { {   203.00f,  6068.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_03 },
    { {   623.00f,  6237.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_04 },
    { {   987.00f,  4607.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_09 },
    { {   976.00f,  4691.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_10 },
    { {   979.00f,  4517.00f }, RC_PATH_TO_MOUNTAIN_VILLAGE_LARGE_SNOWBALL_11 },
    { {   427.00f,  -613.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_01 },
    { {    65.00f,   345.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_02 },
    { {  -179.00f,  -876.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_03 },
    { {   851.00f,   643.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_04 },
    { {   371.00f,   778.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_05 },
    { {  -563.00f,  -523.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_06 },
    { {  1203.00f,   109.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_07 },
    { { -1029.00f,   -25.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_08 },
    { {  -880.00f,   497.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_09 },
    { {  -395.00f,   677.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_10 },
    { {     5.00f,  -560.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_11 },
    { {    60.00f,   -46.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_12 },
    { {  1001.00f,  -426.00f }, RC_TWIN_ISLANDS_LARGE_SNOWBALL_13 },
};
// clang-format on

// We primarily rely on the ActorListIndex, however in some scenes the ActorListIndex for Large
// snowballs is inconsistent based on the day, so we fallback to the coords which are consistent
void IdentifySnowball(Actor* actor, bool* should) {
    s16 actorListIndex = GetActorListIndex(actor);

    auto it = snowballActorIdMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
    if (it != snowballActorIdMap.end()) {
        Rando::ActorBehavior::SetActorRandoCheckId(actor, it->second);
        return;
    }

    auto it2 = snowballCoordsMap.find({ actor->home.pos.x, actor->home.pos.z });
    if (it2 != snowballCoordsMap.end()) {
        Rando::ActorBehavior::SetActorRandoCheckId(actor, it2->second);
        return;
    }
}

void SpawnSnowballDrop(Vec3f pos, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM), actor);
        });
}

void Rando::ActorBehavior::InitObjSnowballBehavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_OBJ_SNOWBALL, IS_RANDO, IdentifySnowball);
    COND_ID_HOOK(ShouldActorInit, ACTOR_OBJ_SNOWBALL2, IS_RANDO, IdentifySnowball);

    COND_VB_SHOULD(VB_SNOWBALL_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);

        RandoCheckId randoCheckId = GetActorRandoCheckId(actor);

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            SpawnSnowballDrop(actor->world.pos, randoCheckId);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SNOWBALL_SET_FLAG, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        ActorId actorToSpawn = (ActorId)va_arg(args, s32);
        ObjSnowballActionFunc actorFunction = va_arg(args, ObjSnowballActionFunc);

        // Ignore snowballs that drop a collectable, those are handled above with VB_SNOWBALL_DROP_COLLECTIBLE
        if (actorFunction == func_80B02D58) {
            return;
        }

        RandoCheckId randoCheckId = GetActorRandoCheckId(actor);

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            SpawnSnowballDrop(actor->world.pos, randoCheckId);
        }
    });
}
