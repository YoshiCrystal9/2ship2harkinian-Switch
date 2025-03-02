#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_GORON_GRAVEYARD] = RandoRegion{ .sceneId = SCENE_GORON_HAKA,
        .checks = {
            CHECK(RC_GORON_GRAVEYARD_DARMANI, CAN_PLAY_SONG(HEALING) && HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH))
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3),      ENTRANCE(GORON_GRAVERYARD, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_HOT_SPRING_WATER, CAN_BE_GORON),
            EVENT(RE_ACCESS_SPRING_WATER, CAN_BE_GORON),
        },
    };
    Regions[RR_GORON_SHRINE] = RandoRegion{ .sceneId = SCENE_16GORON_HOUSE,
        .checks = {
            CHECK(RC_GORON_SHRINE_FULL_LULLABY, CAN_BE_GORON && (CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(LULLABY_INTRO))),
            CHECK(RC_GORON_SHRINE_POT_01, true),
            CHECK(RC_GORON_SHRINE_POT_02, true),
            CHECK(RC_GORON_SHRINE_POT_03, true),
            CHECK(RC_GORON_SHRINE_POT_04, true),
            CHECK(RC_GORON_SHRINE_POT_05, true),
            CHECK(RC_GORON_SHRINE_POT_06, true),
            CHECK(RC_GORON_SHRINE_POT_07, true),
            CHECK(RC_GORON_SHRINE_POT_08, true),
            CHECK(RC_GORON_SHRINE_POT_09, true),
            CHECK(RC_GORON_SHRINE_POT_10, true),
            CHECK(RC_GORON_SHRINE_POT_11, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GORON_VILLAGE_WINTER, 2),      ENTRANCE(GORON_SHRINE, 0), true),
            EXIT(ENTRANCE(GORON_SHOP, 0),                ENTRANCE(GORON_SHRINE, 1), true)
        },
        .events = {
            // Can either play the song for the Goron child to light torches then use sticks, or shoot the chandelier with fire arrows to skip needing the song(or I guess the small torches themselves, same result)
            EVENT(RE_SPAWN_SIRLOIN, ((CAN_PLAY_SONG(LULLABY) && HAS_ITEM(ITEM_DEKU_STICK) || CAN_USE_MAGIC_ARROW(FIRE))) && CAN_BE_GORON && HAS_MAGIC),
        },
    };
    Regions[RR_GORON_RACETRACK] = RandoRegion{ .sceneId = SCENE_GORONRACE,
        .checks = {
            CHECK(RC_GORON_RACETRACK_GOLD_DUST, CAN_BE_GORON && HAS_MAGIC && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_GORON_RACETRACK_POT_01, true),
            CHECK(RC_GORON_RACETRACK_POT_02, true),
            CHECK(RC_GORON_RACETRACK_POT_03, true),
            CHECK(RC_GORON_RACETRACK_POT_04, true),
            CHECK(RC_GORON_RACETRACK_POT_05, true),
            CHECK(RC_GORON_RACETRACK_POT_06, true),
            CHECK(RC_GORON_RACETRACK_POT_07, true),
            CHECK(RC_GORON_RACETRACK_POT_08, true),
            CHECK(RC_GORON_RACETRACK_POT_09, true),
            CHECK(RC_GORON_RACETRACK_POT_10, true),
            CHECK(RC_GORON_RACETRACK_POT_11, true),
            CHECK(RC_GORON_RACETRACK_POT_12, true),
            CHECK(RC_GORON_RACETRACK_POT_13, true),
            CHECK(RC_GORON_RACETRACK_POT_14, true),
            CHECK(RC_GORON_RACETRACK_POT_15, true),
            CHECK(RC_GORON_RACETRACK_POT_16, true),
            CHECK(RC_GORON_RACETRACK_POT_17, true),
            CHECK(RC_GORON_RACETRACK_POT_18, true),
            CHECK(RC_GORON_RACETRACK_POT_19, true),
            CHECK(RC_GORON_RACETRACK_POT_20, true),
            CHECK(RC_GORON_RACETRACK_POT_21, true),
            CHECK(RC_GORON_RACETRACK_POT_22, true),
            CHECK(RC_GORON_RACETRACK_POT_23, true),
            CHECK(RC_GORON_RACETRACK_POT_24, true),
            CHECK(RC_GORON_RACETRACK_POT_25, true),
            CHECK(RC_GORON_RACETRACK_POT_26, true),
            CHECK(RC_GORON_RACETRACK_POT_27, true),
            CHECK(RC_GORON_RACETRACK_POT_28, true),
            CHECK(RC_GORON_RACETRACK_POT_29, true),
            CHECK(RC_GORON_RACETRACK_POT_30, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 2),     ENTRANCE(GORON_RACETRACK, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_GOLD_DUST, CAN_BE_GORON && HAS_MAGIC && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
        }
    };
    Regions[RR_GORON_SHOP] = RandoRegion{ .sceneId = SCENE_GORONSHOP,
        .checks = {
            CHECK(RC_GORON_SHOP_ITEM_01, CAN_AFFORD(RC_GORON_SHOP_ITEM_01)),
            CHECK(RC_GORON_SHOP_ITEM_02, CAN_AFFORD(RC_GORON_SHOP_ITEM_02)),
            CHECK(RC_GORON_SHOP_ITEM_03, CAN_AFFORD(RC_GORON_SHOP_ITEM_03)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GORON_SHRINE, 1),              ENTRANCE(GORON_SHOP, 0), true)
        },
    };
    Regions[RR_GORON_VILLAGE] = RandoRegion{ .sceneId = SCENE_11GORONNOSATO,
        .checks = {
            CHECK(RC_GORON_VILLAGE_PIECE_OF_HEART, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) && CAN_BE_DEKU),
            CHECK(RC_GORON_VILLAGE_LARGE_CRATE, CAN_USE_MAGIC_ARROW(FIRE) || RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_GORON_VILLAGE_MEDIGORON, HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE) && HAS_MAGIC && CAN_BE_GORON),
            CHECK(RC_GORON_VILLAGE_SCRUB_BOMB_BAG, (GET_CUR_UPG_VALUE(UPG_WALLET) >= 1) && CAN_BE_GORON),
            CHECK(RC_GORON_VILLAGE_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) && CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            // During First Day a NPC Goron can open the door to the the Shrine
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 1),     ENTRANCE(GORON_VILLAGE_WINTER, 0), true),
            EXIT(ENTRANCE(GORON_SHRINE, 0),                     ENTRANCE(GORON_VILLAGE_WINTER, 2), true),
        },
        .connections = {
            CONNECTION(RR_LONE_PEAK_SHRINE_ENTRANCE, true)
        }
    };
    Regions[RR_LONE_PEAK_SHRINE_ENTRANCE] = RandoRegion{ .name = "Lone Peak Entrance", .sceneId = SCENE_11GORONNOSATO,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GROTTOS, 16),               ENTRANCE(GORON_VILLAGE_WINTER, 3), true)
        },
        .connections = {
            CONNECTION(RR_GORON_VILLAGE, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH))
        }
    };
    Regions[RR_LONE_PEAK_SHRINE] = RandoRegion{ .name = "Lone Peak", .sceneId = SCENE_KAKUSIANA,
        // Lone Peak Shrine behaves differently from Grottos despite sharing a map. Seems okay to add here.
        .checks = {
            CHECK(RC_LONE_PEAK_SHRINE_BOULDER_CHEST, CAN_USE_EXPLOSIVE),
            CHECK(RC_LONE_PEAK_SHRINE_INVISIBLE_CHEST, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            CHECK(RC_LONE_PEAK_SHRINE_LENS_CHEST, true)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GORON_VILLAGE_WINTER, 3),         ENTRANCE(GROTTOS, 16), true)
        },
    };
    Regions[RR_MOUNTAIN_SMITHY] = RandoRegion{ .sceneId = SCENE_KAJIYA,
        .checks = {
            CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD, (RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE] || CAN_USE_MAGIC_ARROW(FIRE) || (HAS_BOTTLE && CAN_ACCESS(HOT_SPRING_WATER))) && GET_CUR_UPG_VALUE(UPG_WALLET) >= 1),
            CHECK(RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD, (RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE] || CAN_USE_MAGIC_ARROW(FIRE) || (HAS_BOTTLE && CAN_ACCESS(HOT_SPRING_WATER))) && HAS_BOTTLE && CAN_ACCESS(GOLD_DUST) && (GET_CUR_UPG_VALUE(UPG_WALLET) >= 1)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1),      ENTRANCE(MOUNTAIN_SMITHY, 0), true),
        },
    };
    Regions[RR_MOUNTAIN_VILLAGE_TUNNEL_GROTTO] = RandoRegion{ .name = "Mountain Village Tunnel Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_MOUNTAIN_VILLAGE_TUNNEL_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_MOUNTAIN_VILLAGE, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_MOUNTAIN_VILLAGE] = RandoRegion{ .sceneId = SCENE_10YUKIYAMANOMURA,
        .checks = {
            CHECK(RC_KEATON_QUIZ,                                   HAS_ITEM(ITEM_MASK_KEATON)),
            CHECK(RC_MOUNTAIN_VILLAGE_DON_GERO_MASK,                RANDO_EVENTS[RE_SPAWN_SIRLOIN]), // TODO: For entrance rando we need to find a way to ensure the Rock Sirloin can be "walked" here
            CHECK(RC_MOUNTAIN_VILLAGE_OWL_STATUE,                   CAN_USE_SWORD),
            CHECK(RC_MOUNTAIN_VILLAGE_WATERFALL_CHEST,              RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE] && HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            CHECK(RC_MOUNTAIN_VILLAGE_WINTER_POT,                   CAN_HOOK_SCARECROW),
            CHECK(RC_MOUNTAIN_VILLAGE_SPRING_POT,                   CAN_HOOK_SCARECROW && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_MOUNTAIN_VILLAGE_SPRING_FREESTANDING_RUPEE_01, CAN_BE_GORON && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_SMITHY, 0),              ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 1), true),
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 2), true),
            // TODO: When it's spring you need goron mask or zora mask instead?
            EXIT(ENTRANCE(GORON_GRAVERYARD, 0),             ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 3), HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 0),             ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4), true),
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1),     ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6), true),
        },
        .connections = {
            CONNECTION(RR_MOUNTAIN_VILLAGE_TUNNEL_GROTTO, RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]), // TODO: Grotto mapping
        },
        .oneWayEntrances = {
            ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8), // From Song of Soaring
        }
    };
    Regions[RR_PATH_TO_GORON_VILLAGE_HOT_SPRING_GROTTO] = RandoRegion{ .name = "Path to Goron Village Hot Spring Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_TWIN_ISLANDS_FROZEN_GROTTO_CHEST, CAN_USE_EXPLOSIVE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), ENTRANCE(GROTTOS, 5), true), // TODO: Grotto mapping
        },
        .events = {
            EVENT(RE_ACCESS_HOT_SPRING_WATER, true),
        },
    };
    Regions[RR_PATH_TO_GORON_VILLAGE_RAMP_GROTTO] = RandoRegion{ .name = "Path to Goron Village Ramp Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_TWIN_ISLANDS_RAMP_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_GORON_VILLAGE, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_PATH_TO_GORON_VILLAGE] = RandoRegion{ .sceneId = SCENE_17SETUGEN,
        .checks = {
            // TODO : Add Spring only checks.
            CHECK(RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO, CAN_BE_GORON && (CAN_USE_MAGIC_ARROW(FIRE) || CAN_ACCESS(HOT_SPRING_WATER))),
            CHECK(RC_TWIN_ISLANDS_TINGLE_MAP_01,          CAN_USE_PROJECTILE && CAN_AFFORD(RC_TWIN_ISLANDS_TINGLE_MAP_01)),
            CHECK(RC_TWIN_ISLANDS_TINGLE_MAP_02,          CAN_USE_PROJECTILE && CAN_AFFORD(RC_TWIN_ISLANDS_TINGLE_MAP_02)),
            CHECK(RC_TWIN_ISLANDS_UNDERWATER_CHEST_01,    CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_TWIN_ISLANDS_UNDERWATER_CHEST_02,    CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_TWIN_ISLANDS_FREESTANDING_RUPEE_01, CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_TWIN_ISLANDS_FREESTANDING_RUPEE_02, CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_TWIN_ISLANDS_FREESTANDING_RUPEE_03, CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
            CHECK(RC_TWIN_ISLANDS_FREESTANDING_RUPEE_04, CAN_BE_ZORA && RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE]),
        },
        .exits = { //     TO                                     FROM
            EXIT(ENTRANCE(GROTTOS, 5),                  ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), RANDO_EVENTS[RE_CLEARED_SNOWHEAD_TEMPLE] || CAN_USE_MAGIC_ARROW(FIRE)), // TODO: Grotto mapping Hot spring
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 2),  ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), true),
            EXIT(ENTRANCE(GORON_VILLAGE_WINTER, 0),     ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 1), true),
            // This could also be opened by completing Medigoron's test without actually getting the Powder Keg as a item. Not sure what the flag for that is however.
            EXIT(ENTRANCE(GORON_RACETRACK, 0),          ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 2), HAS_ITEM(ITEM_POWDER_KEG) && CAN_BE_GORON),
        },
        .connections = {
            // TODO: This can be reached in entrance rando if coming from Goron Racetrack
            CONNECTION(RR_PATH_TO_GORON_VILLAGE_RAMP_GROTTO, CAN_USE_EXPLOSIVE && (CAN_BE_GORON || CAN_HOOK_SCARECROW)), // TODO: Grotto mapping
        },
    };
    Regions[RR_PATH_TO_MOUNTAIN_VILLAGE_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_13HUBUKINOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 6),      ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_MOUNTAIN_VILLAGE_UPPER, CAN_BE_GORON || CAN_USE_EXPLOSIVE || CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_PATH_TO_MOUNTAIN_VILLAGE_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_13HUBUKINOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 3),                ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_MOUNTAIN_VILLAGE_LOWER, CAN_BE_GORON || CAN_USE_EXPLOSIVE || CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_PATH_TO_SNOWHEAD_GROTTO] = RandoRegion{ .name = "Path To Snowhead Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_PATH_TO_SNOWHEAD_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_UPPER, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_PATH_TO_SNOWHEAD_LOWER] = RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 4),      ENTRANCE(PATH_TO_SNOWHEAD, 0), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_MIDDLE, CAN_BE_GORON && HAS_MAGIC),
        },
    };
    Regions[RR_PATH_TO_SNOWHEAD_MIDDLE] = RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .checks = {
            CHECK(RC_PATH_TO_SNOWHEAD_PIECE_OF_HEART, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC && CAN_HOOK_SCARECROW)
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_LOWER, CAN_BE_GORON && HAS_MAGIC),
            CONNECTION(RR_PATH_TO_SNOWHEAD_UPPER, CAN_BE_GORON && HAS_MAGIC),
        },
    };
    Regions[RR_PATH_TO_SNOWHEAD_UPPER] = RandoRegion{ .sceneId = SCENE_14YUKIDAMANOMITI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD, 0),                     ENTRANCE(PATH_TO_SNOWHEAD, 1), true),
        },
        .connections = {
            CONNECTION(RR_PATH_TO_SNOWHEAD_MIDDLE, CAN_BE_GORON && HAS_MAGIC),
            CONNECTION(RR_PATH_TO_SNOWHEAD_GROTTO, CAN_USE_EXPLOSIVE), // TODO: Grotto mapping
        },
    };
    Regions[RR_SNOWHEAD_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_SNOWHEAD_GREAT_FAIRY, HAS_ALL_STRAY_FAIRIES(DUNGEON_INDEX_SNOWHEAD_TEMPLE)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD, 2),                     ENTRANCE(FAIRY_FOUNTAIN, 2), true),
        },
    };
    Regions[RR_SNOWHEAD_NEAR_PATH] = RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .checks = {
            CHECK(RC_SNOWHEAD_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_SNOWHEAD, 1),             ENTRANCE(SNOWHEAD, 0), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_TEMPLE, CanAccessDungeon(DUNGEON_INDEX_SNOWHEAD_TEMPLE)
),
        },
        .oneWayEntrances = {
            ENTRANCE(SNOWHEAD, 3), // From Song of Soaring
        }
    };
    Regions[RR_SNOWHEAD_NEAR_TEMPLE] = RandoRegion{ .sceneId = SCENE_12HAKUGINMAE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SNOWHEAD_TEMPLE, 0),              ENTRANCE(SNOWHEAD, 1), CAN_BE_GORON),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 2),               ENTRANCE(SNOWHEAD, 2), true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_NEAR_PATH, true),
        },
    };
}, {});
// clang-format on
