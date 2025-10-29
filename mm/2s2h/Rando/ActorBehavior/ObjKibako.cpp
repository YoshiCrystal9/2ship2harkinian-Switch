#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
}

#define OBJKIBAKO_RC (actor->home.rot.x)

std::map<std::pair<float, float>, RandoCheckId> crateMap = {
    // Clock Town //
    { { -1668.0f, 615.0f }, RC_CLOCK_TOWN_LAUNDRY_SMALL_CRATE },
    { { 759.0f, -458.0f }, RC_CLOCK_TOWN_EAST_SMALL_CRATE_01 },
    { { 1100.0f, -460.0f }, RC_CLOCK_TOWN_EAST_SMALL_CRATE_02 },

    // Great Bay Temple //
    { { 1725.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_01 },
    { { 1725.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_02 },
    { { 1755.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_03 },
    { { 1755.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_04 },
    { { 960.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_05 },
    { { 930.0f, -2100.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_06 },
    { { 960.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_07 },
    { { 930.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_08 },
    { { 960.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_09 },
    { { 960.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_10 },
    { { 930.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_11 },
    { { 930.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_12 },
    { { 1725.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_13 },
    { { 1755.0f, -2880.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_14 },
    { { 1725.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_15 },
    { { 1755.0f, -2910.0f }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_16 },
    { { 2415.0f, 240.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_01 },
    { { 2415.0f, 192.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_02 },
    { { 2415.0f, 144.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_03 },
    { { 2409.0f, 990.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_04 },
    { { 2457.0f, 990.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_05 },
    { { 2457.0f, 1050.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_06 },
    { { 2409.0f, 1050.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_07 },
    { { 1845.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_01 },
    { { 1845.0f, 1992.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_02 },
    { { 1845.0f, 2040.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_03 },
    { { 1305.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_04 },
    { { 1245.0f, 1944.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_05 },

    // Pirates Fortress //
    { { 797.0f, -915.0f }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_01 },
    { { 1151.0f, 180.0f }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_02 },
    { { 889.0f, 180.0f }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_03 },

    // Snowhead Temple //
    { { 1247.0f, 49.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_LARGE_CRATE },
    { { 263.0f, 1301.0f }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM_LARGE_CRATE_01 },
    { { 282.0f, 1257.0f }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM_LARGE_CRATE_02 },
    { { 1296.0f, 330.0f }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_01 },
    { { 1248.0f, 330.0f }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_02 },
    { { 1200.0f, 330.0f }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_03 },
    { { 1152.0f, 330.0f }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_04 },
    { { 1104.0f, 330.0f }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_05 },

    // Stone Tower Temple //
    { { 435.0f, -705.0f }, RC_STONE_TOWER_TEMPLE_ENTRANCE_SMALL_CRATE_01 },
    { { 465.0f, -705.0f }, RC_STONE_TOWER_TEMPLE_ENTRANCE_SMALL_CRATE_02 },
    { { 1050.0f, -2910.0f }, RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_LARGE_CRATE_01 },
    { { 1530.0f, -2910.0f }, RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_LARGE_CRATE_02 },
    { { 495.0f, -885.0f }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_01 },
    { { 495.0f, -855.0f }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_02 },
    { { 495.0f, -825.0f }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_03 },
    { { -1410.0f, -330.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_01 },
    { { -1410.0f, -390.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_02 },
    { { -1410.0f, -450.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_03 },
    { { -1410.0f, -510.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_04 },
    { { -1410.0f, -570.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_05 },
    { { -1485.0f, -390.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_SMALL_CRATE_01 },
    { { -1485.0f, -510.0f }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_SMALL_CRATE_02 },
    { { 495.0f, -705.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_01 },
    { { 495.0f, -735.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_02 },
    { { -525.0f, -165.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_03 },
    { { -555.0f, -165.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_04 },
    { { -585.0f, -165.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_05 },
    { { 1815.0f, -1215.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_01 },
    { { 1845.0f, -1215.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_02 },
    { { 1875.0f, -1215.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_03 },
    { { 1815.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_04 },
    { { 1845.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_05 },
    { { 1875.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_06 },

    // Termina Field //
    { { 1750.0f, 830.0f }, RC_TERMINA_FIELD_SCRUB_LARGE_CRATE },

    // Gorman Track //
    { { -1624.0f, -891.0f }, RC_GORMAN_TRACK_LARGE_CRATE },

    // Romani Ranch //
    { { -576.0f, -1193.0f }, RC_ROMANI_RANCH_FIELD_LARGE_CRATE },

    // Cucco Shack //
    { { -3439.0f, -583.0f }, RC_CUCCO_SHACK_LARGE_CRATE_01 },
    { { -3382.0f, -565.0f }, RC_CUCCO_SHACK_LARGE_CRATE_02 },
    { { -3325.0f, -546.0f }, RC_CUCCO_SHACK_LARGE_CRATE_03 },

    // Goron Village //
    { { -1184.0f, -3051.0f }, RC_GORON_VILLAGE_LARGE_CRATE },

    // Swamp Spider House //
    { { -828.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_01 },
    { { -876.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_02 },
    { { -876.0f, -509.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_03 },
    { { -870.0f, -876.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_04 },
    { { -810.0f, -876.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_05 },
    { { -690.0f, -876.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_06 },
    { { -301.0f, -1299.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_LARGE_CRATE_01 },
    { { -241.0f, -1299.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_LARGE_CRATE_02 },
};

void ObjKibako_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gSmallJunkCrateDL);
        return;
    }

    RandoItemId randoItemId =
        Rando::ConvertItem(RANDO_SAVE_CHECKS[OBJKIBAKO_RC].randoItemId, (RandoCheckId)OBJKIBAKO_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallBossKeyCrateDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallHeartCrateDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMinorCrateDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMajorCrateDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMaskCrateDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallTokenCrateDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallSmallKeyCrateDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallFairyCrateDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallJunkCrateDL);
            break;
    }
}

void ObjKibako2_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
        return;
    }

    RandoItemId randoItemId =
        Rando::ConvertItem(RANDO_SAVE_CHECKS[OBJKIBAKO_RC].randoItemId, (RandoCheckId)OBJKIBAKO_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeBossKeyCrateDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeHeartCrateDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMinorCrateDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMajorCrateDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMaskCrateDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeTokenCrateDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeSmallKeyCrateDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeFairyCrateDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
            break;
    }
}

void Rando::ActorBehavior::InitObjKibakoBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;

        auto it = crateMap.find({ actor->home.pos.x, actor->home.pos.z });
        if (it == crateMap.end()) {
            return;
        }

        randoCheckId = it->second;

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        OBJKIBAKO_RC = randoCheckId;
        actor->draw = ObjKibako_RandoDraw;
    });

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO2, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;

        auto it = crateMap.find({ actor->home.pos.x, actor->home.pos.z });
        if (it == crateMap.end()) {
            return;
        }

        randoCheckId = it->second;

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        OBJKIBAKO_RC = randoCheckId;
        actor->draw = ObjKibako2_RandoDraw;
    });

    COND_VB_SHOULD(VB_CRATE_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        if (OBJKIBAKO_RC != RC_UNKNOWN) {
            *should = false;
            actor->draw = ObjKibako_RandoDraw;
        }
    });

    COND_VB_SHOULD(VB_BARREL_OR_CRATE_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);

        if (actor->id != ACTOR_OBJ_KIBAKO && actor->id != ACTOR_OBJ_KIBAKO2) {
            return;
        }

        if (OBJKIBAKO_RC == RC_UNKNOWN) {
            return;
        }

        *should = false;

        EnItem00* spawn = CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, OBJKIBAKO_RC,
            [](Actor* actor, PlayState* play) {
                auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
                switch (randoStaticCheck.flagType) {
                    case FLAG_NONE:
                        if (RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].shuffled) {
                            RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true;
                        }
                        break;
                    case FLAG_CYCL_SCENE_COLLECTIBLE:
                        Flags_SetCollectible(play, randoStaticCheck.flag);
                        break;
                    default:
                        break;
                }
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM), actor);
            });
    });
}
