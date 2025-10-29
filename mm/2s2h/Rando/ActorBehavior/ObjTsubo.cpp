#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "objects/object_tsubo/object_tsubo.h"

void ObjTsubo_Draw(Actor* actor, PlayState* play);
}

std::map<SceneId, std::map<std::pair<float, float>, RandoCheckId>> potMap = {
    // Deku Shrine //
    { SCENE_DANPEI,
      {
          { { 1881.0f, 980.0f }, RC_DEKU_SHRINE_POT_01 },
          { { 2373.0f, 0.0f }, RC_DEKU_SHRINE_POT_02 },
      } },
    // Stone Tower //
    { SCENE_F40,
      {
          { { 1204.0f, 1871.0f }, RC_STONE_TOWER_HIGHER_SCARECROW_POT_02 },
          { { 1364.0f, 2391.0f }, RC_STONE_TOWER_HIGHER_SCARECROW_POT_08 },
          { { -1116.0f, 2231.0f }, RC_STONE_TOWER_LOWER_SCARECROW_POT_02 },
          { { -1116.0f, 2031.0f }, RC_STONE_TOWER_LOWER_SCARECROW_POT_04 },
          { { -1116.0f, 1511.0f }, RC_STONE_TOWER_LOWER_SCARECROW_POT_11 },
      } },
    // Beneath the Graveyard //
    { SCENE_HAKASHITA,
      {
          { { 1270.0f, 380.0f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_1_BATS_POT_03 },
          { { -1121.0f, -879.0f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_01 },
          { { -1200.0f, -881.0f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_02 },
          { { -1200.0f, -1400.0f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_03 },
          { { -1120.0f, -1400.0f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_04 },
      } },
    // Snowhead Temple - Goht's Lair //
    { SCENE_HAKUGIN_BS,
      {
          { { -1250.0f, -1250.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_01 },
          { { -1400.0f, -60.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_02 },
          { { -1600.0f, -60.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_03 },
          { { -1250.0f, 1250.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_04 },
          { { 61.0f, 1440.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_05 },
          { { 61.0f, 1740.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_06 },
          { { 1250.0f, 1250.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_07 },
          { { 1460.0f, -60.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_08 },
          { { 1660.0f, -60.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_09 },
          { { 1250.0f, -1250.0f }, RC_SNOWHEAD_TEMPLE_BOSS_POT_10 },
          { { 150.0f, -1170.0f }, RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_01 },
          { { -150.0f, -1170.0f }, RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_02 },
          { { 0.0f, -1400.0f }, RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_03 },
          { { 0.0f, -1600.0f }, RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_04 },
      } },
    // Snowhead Temple //
    { SCENE_HAKUGIN,
      {
          { { -1052.0f, 338.0f }, RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_POT_01 },
          { { -1091.0f, 327.0f }, RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_POT_02 },
          { { 930.0f, 1065.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_01 },
          { { 810.0f, 1065.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_02 },
          { { 870.0f, 1005.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_03 },
          { { 870.0f, 1125.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_04 },
          { { 870.0f, 1065.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_05 },
          { { 795.0f, -60.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_AFTER_POT_01 },
          { { 825.0f, -75.0f }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_AFTER_POT_02 },
          { { -585.0f, -645.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_POT_01 },
          { { -645.0f, -585.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_POT_02 },
          { { 570.0f, -90.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_POT_01 },
          { { 510.0f, -90.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_POT_02 },
          { { 435.0f, -570.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_KEY_POT_01 },
          { { 410.0f, -526.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_KEY_POT_02 },
          { { 473.0f, 458.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SCARECROW_POT_01 },
          { { 519.0f, 535.0f }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SCARECROW_POT_02 },
          { { -1543.0f, 703.0f }, RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_01 },
          { { -1507.0f, 655.0f }, RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_02 },
          { { -351.0f, 1153.0f }, RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_03 },
          { { -378.0f, 1169.0f }, RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_04 },
          { { -1525.0f, 679.0f }, RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_05 },
          { { -727.0f, 993.0f }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_POT_01 },
          { { -675.0f, 920.0f }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_POT_02 },
          { { 225.0f, 975.0f }, RC_SNOWHEAD_TEMPLE_ENTRANCE_POT_01 },
          { { 225.0f, 1065.0f }, RC_SNOWHEAD_TEMPLE_ENTRANCE_POT_02 },
          { { -452.0f, -995.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_01 },
          { { -473.0f, -958.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_02 },
          { { -493.0f, -921.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_03 },
          { { -420.0f, -930.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_04 },
          { { -1275.0f, -600.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_05 },
          { { -1305.0f, -623.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_06 },
          { { -1335.0f, -645.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_07 },
          { { -870.0f, -105.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_01 },
          { { -915.0f, -105.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_02 },
          { { 135.0f, -1050.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_03 },
          { { 150.0f, -990.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_04 },
          { { -1184.0f, -1316.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_05 },
          { { -1116.0f, -1374.0f }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_06 },
          { { 1320.0f, -690.0f }, RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_01 },
          { { 1440.0f, -690.0f }, RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_02 },
          { { 1380.0f, -690.0f }, RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_03 },
          { { 1440.0f, -630.0f }, RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_04 },
          { { 1440.0f, -570.0f }, RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_05 },
      } },
    // The Moon - Majora's Lair //
    { SCENE_LAST_BS,
      {
          { { 80.0f, 800.0f }, RC_MOON_MAJORA_POT_01 },
          { { -80.0f, 800.0f }, RC_MOON_MAJORA_POT_02 },
      } },
    // The Moon - Goron Trial //
    { SCENE_LAST_GORON,
      {
          { { -5670.0f, -2280.0f }, RC_MOON_TRIAL_GORON_POT_01 },
          { { -5610.0f, -2280.0f }, RC_MOON_TRIAL_GORON_POT_02 },
          { { -2700.0f, -2490.0f }, RC_MOON_TRIAL_GORON_POT_03 },
          { { -2640.0f, -2490.0f }, RC_MOON_TRIAL_GORON_POT_04 },
          { { -2580.0f, -2490.0f }, RC_MOON_TRIAL_GORON_POT_05 },
          { { -7200.0f, -1290.0f }, RC_MOON_TRIAL_GORON_POT_06 },
          { { -7260.0f, -1290.0f }, RC_MOON_TRIAL_GORON_POT_07 },
          { { -7200.0f, -1410.0f }, RC_MOON_TRIAL_GORON_POT_08 },
          { { -7260.0f, -1410.0f }, RC_MOON_TRIAL_GORON_POT_09 },
          { { -4890.0f, -1380.0f }, RC_MOON_TRIAL_GORON_POT_10 },
          { { -4890.0f, -1320.0f }, RC_MOON_TRIAL_GORON_POT_11 },
          { { -420.0f, -1425.0f }, RC_MOON_TRIAL_GORON_EARLY_POT_01 },
          { { -480.0f, -1425.0f }, RC_MOON_TRIAL_GORON_EARLY_POT_02 },
          { { -420.0f, -1275.0f }, RC_MOON_TRIAL_GORON_EARLY_POT_03 },
          { { -480.0f, -1275.0f }, RC_MOON_TRIAL_GORON_EARLY_POT_04 },
      } },
    // The Moon - Link Trial //
    { SCENE_LAST_LINK,
      {
          { { -270.0f, -90.0f }, RC_MOON_TRIAL_LINK_POT_01 },
          { { -270.0f, -150.0f }, RC_MOON_TRIAL_LINK_POT_02 },
          { { -210.0f, -90.0f }, RC_MOON_TRIAL_LINK_POT_03 },
          { { -210.0f, -150.0f }, RC_MOON_TRIAL_LINK_POT_04 },
          { { 210.0f, -90.0f }, RC_MOON_TRIAL_LINK_POT_05 },
          { { 210.0f, -150.0f }, RC_MOON_TRIAL_LINK_POT_06 },
          { { 270.0f, -90.0f }, RC_MOON_TRIAL_LINK_POT_07 },
          { { 270.0f, -150.0f }, RC_MOON_TRIAL_LINK_POT_08 },
      } },
    // Pirate's Fortress //
    { SCENE_PIRATE,
      {
          { { 4020.0f, -1860.0f }, RC_PIRATE_FORTRESS_INTERIOR_GUARDED_POT_01 },
          { { 2040.0f, -1380.0f }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_POT_02 },
          { { 1920.0f, -640.0f }, RC_PIRATE_FORTRESS_SEWERS_WATERWAY_POT_01 },
      } },
    // Beneath the Well //
    { SCENE_REDEAD,
      {
          { { 1620.0f, -960.0f }, RC_BENEATH_THE_WELL_BIG_POE_POT_01 },
          { { 1620.0f, -1560.0f }, RC_BENEATH_THE_WELL_BIG_POE_POT_02 },
          { { 2280.0f, -1560.0f }, RC_BENEATH_THE_WELL_BIG_POE_POT_03 },
          { { 2280.0f, -960.0f }, RC_BENEATH_THE_WELL_BIG_POE_POT_04 },
          { { 1185.0f, -915.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_01 },
          { { 1185.0f, -945.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_02 },
          { { 1185.0f, -975.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_03 },
          { { 1185.0f, -1005.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_04 },
          { { 1185.0f, -1035.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_05 },
          { { 1185.0f, -1065.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_06 },
          { { 1185.0f, -1095.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_07 },
          { { 1185.0f, -1125.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_08 },
          { { 1185.0f, -1155.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_09 },
          { { 1185.0f, -1185.0f }, RC_BENEATH_THE_WELL_MIDDLE_POT_10 },
      } },
    // Woodfall Temple //
    { SCENE_MITURIN,
      {
          { { 120.0f, 2100.0f }, RC_WOODFALL_TEMPLE_ENTRANCE_POT },
          { { 660.0f, -120.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_01 },
          { { 660.0f, 120.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_02 },
          { { 630.0f, 510.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_03 },
          { { 570.0f, 570.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_04 },
          { { 510.0f, 630.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_05 },
          { { 510.0f, 510.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_06 },
          { { 675.0f, 255.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_UPPER_POT_01 },
          { { 645.0f, 255.0f }, RC_WOODFALL_TEMPLE_MAIN_ROOM_UPPER_POT_02 },
          { { -780.0f, -165.0f }, RC_WOODFALL_TEMPLE_MAZE_POT_01 },
          { { -780.0f, -105.0f }, RC_WOODFALL_TEMPLE_MAZE_POT_02 },
          { { -120.0f, -2220.0f }, RC_WOODFALL_TEMPLE_PRE_BOSS_POT_01 },
          { { 120.0f, -2220.0f }, RC_WOODFALL_TEMPLE_PRE_BOSS_POT_02 },
          { { 1335.0f, 585.0f }, RC_WOODFALL_TEMPLE_WATER_ROOM_POT_01 },
          { { 1335.0f, 615.0f }, RC_WOODFALL_TEMPLE_WATER_ROOM_POT_02 },
          { { 1335.0f, 645.0f }, RC_WOODFALL_TEMPLE_WATER_ROOM_POT_03 },
          { { 1335.0f, 675.0f }, RC_WOODFALL_TEMPLE_WATER_ROOM_POT_04 },
      } },
    // Swamp Spider House //
    { SCENE_KINSTA1,
      {
          { { -30.0f, -1650.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_LOWER_POT_01 },
          { { 30.0f, -1650.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_LOWER_POT_02 },
          { { 330.0f, -1651.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_01 },
          { { 330.0f, -1590.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_02 },
          { { 330.0f, -1531.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_03 },
          { { 330.0f, -1470.0f }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_04 },
          { { 544.0f, -569.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_01 },
          { { 483.0f, -569.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_02 },
          { { 575.0f, -538.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_03 },
          { { 514.0f, -539.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_04 },
          { { 454.0f, -539.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_05 },
          { { 451.0f, -1232.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_06 },
          { { 451.0f, -1290.0f }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_07 },
          { { 90.0f, -209.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_01 },
          { { 89.0f, -269.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_02 },
          { { -90.0f, -270.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_03 },
          { { 330.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_LEFT_POT_01 },
          { { 270.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_LEFT_POT_02 },
          { { -270.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_RIGHT_POT_01 },
          { { -330.0f, -210.0f }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_RIGHT_POT_02 },
          { { -871.0f, -872.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_POT_01 },
          { { -809.0f, -870.0f }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_POT_02 },
      } },
    // Ocean Spider House //
    { SCENE_KINDAN2,
      {
          { { 64.0f, -1650.0f }, RC_OCEAN_SPIDER_HOUSE_MAIN_ROOM_POT_01 },
          { { 2.0f, -1722.0f }, RC_OCEAN_SPIDER_HOUSE_MAIN_ROOM_WEB_POT },
          { { -569.0f, -1200.0f }, RC_OCEAN_SPIDER_HOUSE_STORAGE_POT_02 },
          { { -630.0f, -1200.0f }, RC_OCEAN_SPIDER_HOUSE_STORAGE_POT_03 },
      } },
    // Great Bay Temple - Gyorg's Lair //
    { SCENE_SEA_BS,
      {
          { { 150.0f, 150.0f }, RC_GREAT_BAY_TEMPLE_BOSS_POT_01 },
          { { -150.0f, 150.0f }, RC_GREAT_BAY_TEMPLE_BOSS_POT_02 },
          { { -150.0f, -150.0f }, RC_GREAT_BAY_TEMPLE_BOSS_POT_03 },
          { { 150.0f, -150.0f }, RC_GREAT_BAY_TEMPLE_BOSS_POT_04 },
          { { 1200.0f, -1200.0f }, RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_01 },
          { { 1200.0f, 1200.0f }, RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_02 },
          { { -1200.0f, 1200.0f }, RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_03 },
          { { -1200.0f, -1200.0f }, RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_04 },
      } },
    // Great Bay Temple //
    { SCENE_SEA,
      {
          { { -1905.0f, -1200.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_03 },
          { { -1755.0f, -1200.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_06 },
          { { -1755.0f, -1680.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_07 },
          { { -1755.0f, -1725.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_08 },
          { { -1755.0f, -1770.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_09 },
          { { -1905.0f, -1680.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_10 },
          { { -1905.0f, -1725.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_11 },
          { { -1905.0f, -1770.0f }, RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_12 },
          { { 600.0f, -30.0f }, RC_GREAT_BAY_TEMPLE_CENTRAL_ROOM_POT_01 },
          { { 600.0f, 30.0f }, RC_GREAT_BAY_TEMPLE_CENTRAL_ROOM_POT_02 },
          { { 1265.0f, -785.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_01 },
          { { 1235.0f, -815.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_02 },
          { { 1575.0f, -1155.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_03 },
          { { 1605.0f, -1125.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_04 },
          { { 1170.0f, -1078.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_01 },
          { { 1310.0f, -1078.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_02 },
          { { 1310.0f, -1218.0f }, RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_03 },
          { { 540.0f, -930.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_01 },
          { { -540.0f, -930.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_02 },
          { { -540.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_03 },
          { { 540.0f, -2070.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_04 },
          { { -479.0f, 2141.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_01 },
          { { -524.0f, 2141.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_02 },
          { { -391.0f, 1959.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_03 },
          { { -436.0f, 1959.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_04 },
          { { -391.0f, 1810.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_05 },
          { { -436.0f, 1810.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_06 },
          { { -524.0f, 1628.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_07 },
          { { -479.0f, 1628.0f }, RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_08 },
          { { 2970.0f, 1020.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_UPPER_POT_01 },
          { { 2970.0f, 960.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_UPPER_POT_02 },
          { { 1500.0f, 540.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_01 },
          { { 1590.0f, 540.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_02 },
          { { 1712.0f, 1215.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_03 },
          { { 1500.0f, 1030.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_03 },
          { { 1590.0f, 1250.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_04 },
          { { 1635.0f, 1620.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_05 },
          { { 1470.0f, 1665.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_07 },
          { { 1455.0f, 1620.0f }, RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_08 },
          { { 2835.0f, -555.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_01 },
          { { 2835.0f, -495.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_02 },
          { { 2745.0f, -495.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_03 },
          { { 2745.0f, -555.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_04 },
          { { 2730.0f, -975.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_05 },
          { { 2775.0f, -975.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_06 },
          { { 3555.0f, -720.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_07 },
          { { 3555.0f, -675.0f }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_08 },
          { { -1530.0f, 60.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_01 },
          { { -1560.0f, 30.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_02 },
          { { -2130.0f, 60.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_03 },
          { { -2100.0f, 30.0f }, RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_04 },
          { { -2250.0f, -1890.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_01 },
          { { -2280.0f, -1920.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_02 },
          { { -2280.0f, -2760.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_03 },
          { { -2250.0f, -2790.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_04 },
          { { -1410.0f, -2790.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_05 },
          { { -1380.0f, -2760.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_06 },
          { { -1380.0f, -1920.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_07 },
          { { -1410.0f, -1890.0f }, RC_GREAT_BAY_TEMPLE_WART_POT_08 },
      } },
    // Stone Tower Temple //
    { SCENE_INISIE_N,
      {
          { { -30.0f, -1995.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_01 },
          { { -30.0f, -1965.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_02 },
          { { -30.0f, -1935.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_03 },
          { { -30.0f, -1905.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_04 },
          { { 30.0f, -1995.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_05 },
          { { 30.0f, -1965.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_06 },
          { { 30.0f, -1935.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_07 },
          { { 30.0f, -1905.0f }, RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_08 },
          { { 45.0f, -690.0f }, RC_STONE_TOWER_TEMPLE_ENTRANCE_POT_01 },
          { { -45.0f, -690.0f }, RC_STONE_TOWER_TEMPLE_ENTRANCE_POT_02 },
          { { -1275.0f, -1395.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_01 },
          { { -1275.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_02 },
          { { -1425.0f, -1395.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_03 },
          { { -1425.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_04 },
          { { -1275.0f, -705.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_01 },
          { { -1275.0f, -615.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_02 },
          { { -1425.0f, -705.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_03 },
          { { -1425.0f, -615.0f }, RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_04 },
          { { 1350.0f, -1770.0f }, RC_STONE_TOWER_TEMPLE_MIRROR_ROOM_POT_01 },
          { { 1230.0f, -1770.0f }, RC_STONE_TOWER_TEMPLE_MIRROR_ROOM_POT_02 },
          { { 810.0f, -1260.0f }, RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_01 },
          { { 750.0f, -1260.0f }, RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_02 },
          { { 810.0f, -1320.0f }, RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_03 },
          { { 1800.0f, -900.0f }, RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_UPPER_POT_01 },
          { { 1800.0f, -840.0f }, RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_UPPER_POT_02 },
          { { 615.0f, -3375.0f }, RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_01 },
          { { 585.0f, -3375.0f }, RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_02 },
          { { 615.0f, -3465.0f }, RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_03 },
          { { 585.0f, -3465.0f }, RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_04 },
      } },
    // Stone Tower Temple - Inverted //
    { SCENE_INISIE_R,
      {
          { { 2070.0f, -1620.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_01 },
          { { 2790.0f, -1620.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_02 },
          { { 2790.0f, -900.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_03 },
          { { 2070.0f, -900.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_04 },
          { { -735.0f, -1215.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_MAZE_SIDE_POT_01 },
          { { -735.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_MAZE_SIDE_POT_02 },
          { { -1605.0f, -1395.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_WIZZROBE_SIDE_POT_01 },
          { { -1605.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_WIZZROBE_SIDE_POT_02 },
          { { -30.0f, -1995.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_01 },
          { { -30.0f, -1965.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_02 },
          { { -30.0f, -1935.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_03 },
          { { -30.0f, -1905.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_04 },
          { { 30.0f, -1995.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_05 },
          { { 30.0f, -1965.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_06 },
          { { 30.0f, -1935.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_07 },
          { { 30.0f, -1905.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_08 },
          { { 1350.0f, -630.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_BRIDGE_POT_01 },
          { { 1230.0f, -630.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_BRIDGE_POT_02 },
          { { 735.0f, -1215.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_LEDGE_POT_02 },
          { { 735.0f, -1305.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_LEDGE_POT_03 },
          { { -1290.0f, -510.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_01 },
          { { -1290.0f, -450.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_02 },
          { { -1410.0f, -450.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_03 },
          { { -1410.0f, -510.0f }, RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_04 },
      } },
};

#define OBJTSUBO_RC (actor->home.rot.x)

RandoCheckId IdentifyPot(Actor* actor) {
    auto randoStaticCheck =
        Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, OBJ_TSUBO_PFE00(actor), gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId != RC_UNKNOWN && randoStaticCheck.randoCheckType == RCTYPE_POT) {
        return randoStaticCheck.randoCheckId;
    }
    RandoCheckId randoCheckId = RC_UNKNOWN;

    auto it = potMap.find((SceneId)gPlayState->sceneId);
    if (it == potMap.end()) {
        return randoCheckId;
    }

    auto innerIt = it->second.find({ actor->home.pos.x, actor->home.pos.z });
    if (innerIt == it->second.end()) {
        return randoCheckId;
    }
    return innerIt->second;
}

void ObjTsubo_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gPotStandardDL);
        return;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[OBJTSUBO_RC].randoItemId, (RandoCheckId)OBJTSUBO_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotBossKeyDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gPotHeartDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMinorDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMajorDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMaskDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gPotTokenDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotSmallKeyDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotFairyDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gPotStandardDL);
            break;
    }
}

void Rando::ActorBehavior::InitObjTsuboBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TSUBO, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = IdentifyPot(actor);
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        OBJTSUBO_RC = randoCheckId;
    });

    COND_VB_SHOULD(VB_POT_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        if (OBJTSUBO_RC != RC_UNKNOWN) {
            *should = false;
            actor->draw = ObjTsubo_RandoDraw;
        }
    });

    COND_VB_SHOULD(VB_POT_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = (RandoCheckId)OBJTSUBO_RC;
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
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
        *should = false;

        // Clear the stored Check ID for pots that are on a timed respawn
        OBJTSUBO_RC = RC_UNKNOWN;
        actor->draw = ObjTsubo_Draw;
    });
}
