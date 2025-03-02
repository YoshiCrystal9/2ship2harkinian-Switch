#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_CUCCO_SHACK] = RandoRegion{ .sceneId = SCENE_F01C,
        .checks = {
            CHECK(RC_ROMANI_RANCH_GROG, HAS_ITEM(ITEM_MASK_BREMEN)),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_01, true),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_02, true),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 4),                 ENTRANCE(CUCCO_SHACK, 0), true),
        },
    };
    Regions[RR_DEKU_KINGS_CHAMBER_HOLDING_CELL] = RandoRegion{ .name = "Holding Cell", .sceneId = SCENE_DEKU_KING,
        .checks = {
            CHECK(RC_DEKU_KINGS_CHAMBER_MONKEY, CAN_BE_DEKU && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 3),                  ENTRANCE(DEKU_KINGS_CHAMBER, 1), true),
        },
    };
    Regions[RR_DEKU_KINGS_CHAMBER] = RandoRegion{ .sceneId = SCENE_DEKU_KING,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 2),                  ENTRANCE(DEKU_KINGS_CHAMBER, 0), true),
        },
        .events = {
            EVENT(RE_RETURN_DEKU_PRINCESS, HAS_BOTTLE && CAN_ACCESS(DEKU_PRINCESS)),
        }
    };
    Regions[RR_DEKU_PALACE_BEAN_SALESMAN_GROTTO] = RandoRegion{ .name = "Deku Palace Bean Salesman Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_DEKU_PALACE_GROTTO_CHEST, CAN_GROW_BEAN_PLANT || HAS_ITEM(ITEM_HOOKSHOT)),
            // TODO: Bean salesman check
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 9),                  ENTRANCE(GROTTOS, 12), true), // TODO: Grotto mapping
        },
    };
    Regions[RR_DEKU_PALACE_INSIDE] = RandoRegion{ .name = "Inside", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_DEKU_PALACE_PIECE_OF_HEART,    true),
            CHECK(RC_DEKU_PALACE_POT_01, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_POT_02, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_01, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_02, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_03, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_04, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_05, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_06, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_07, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_08, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_09, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_10, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_11, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_12, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_13, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_14, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_15, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_16, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_17, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_18, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_19, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_20, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_21, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_22, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_23, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_24, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 0),           ENTRANCE(DEKU_PALACE, 2), true),
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 1),           ENTRANCE(DEKU_PALACE, 3), CAN_BE_DEKU), // Cell TODO: Is there something to do with beans here?
            EXIT(ENTRANCE(GROTTOS, 12),                     ENTRANCE(DEKU_PALACE, 9), true), // TODO: Grotto mapping
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_OUTSIDE, true),
        },
    };
    Regions[RR_DEKU_PALACE_OUTSIDE] = RandoRegion{ .name = "Outside", .sceneId = SCENE_22DEKUCITY,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 3),      ENTRANCE(DEKU_PALACE, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 4),      ENTRANCE(DEKU_PALACE, 5), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(DEKU_SHRINE, 0),                  ENTRANCE(DEKU_PALACE, 4), RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE, CAN_BE_DEKU),
        },
    };
    Regions[RR_DEKU_SHRINE_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_DANPEI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 4),                  ENTRANCE(DEKU_SHRINE, 0), true),
        },
        .connections = {
            CONNECTION(RR_DEKU_SHRINE, RANDO_EVENTS[RE_RETURN_DEKU_PRINCESS]),
        },
    };
    Regions[RR_DEKU_SHRINE] = RandoRegion{ .sceneId = SCENE_DANPEI,
        .checks = {
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_01,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_02,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_03,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_04,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_05,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_06,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_07,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_08,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_09,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_10,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_11,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_12,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_13,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_14,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_15,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_16,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_17,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_18,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_19,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_20,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_21,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_22,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_23,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_24,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_25,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_26,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_27,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_28,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_29,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_30,             true),
            CHECK(RC_DEKU_SHRINE_POT_01,                            true),
            CHECK(RC_DEKU_SHRINE_POT_02,                            true),
            CHECK(RC_DEKU_SHRINE_MASK_OF_SCENTS,                    true),
        },
        .connections = {
            CONNECTION(RR_DEKU_SHRINE_ENTRANCE, RANDO_EVENTS[RE_RETURN_DEKU_PRINCESS]),
        },
    };
    Regions[RR_DOGGY_RACETRACK] = RandoRegion{ .sceneId = SCENE_F01_B,
        .checks = {
            // TODO: Trick: Jumpslash to clip through (similar to Clock Town Straw).
            // Zora can just climb up, adding it to logic for now but if someone wants to make it a trick later feel free.
            CHECK(RC_DOGGY_RACETRACK_CHEST, HAS_ITEM(ITEM_HOOKSHOT) || CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_DOGGY_RACETRACK_PIECE_OF_HEART,    HAS_ITEM(ITEM_MASK_TRUTH)),
            CHECK(RC_DOGGY_RACETRACK_POT_01, true),
            CHECK(RC_DOGGY_RACETRACK_POT_02, true),
            CHECK(RC_DOGGY_RACETRACK_POT_03, true),
            CHECK(RC_DOGGY_RACETRACK_POT_04, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 5),                 ENTRANCE(DOGGY_RACETRACK, 0), true),
        },
    };
    Regions[RR_GORMAN_TRACK] = RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .checks = {
            CHECK(RC_GORMAN_MILK_PURCHASE, CAN_AFFORD(RC_GORMAN_MILK_PURCHASE)),
            // TODO : Also apparently can be obtained using a trick with Goron mask and Bombs. Add trick later here
            CHECK(RC_GORMAN_TRACK_LARGE_CRATE, RANDO_EVENTS[RE_COWS_FROM_ALIENS]), // Night 2 only, after defending cows from aliens.
            CHECK(RC_GORMAN_TRACK_GARO_MASK, CAN_PLAY_SONG(EPONA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 2),                    ENTRANCE(GORMAN_TRACK, 3), CAN_PLAY_SONG(EPONA)),
            EXIT(ENTRANCE(MILK_ROAD, 3),                    ENTRANCE(GORMAN_TRACK, 0), true),
        },
    };
    Regions[RR_MAGIC_HAGS_POTION_SHOP] = RandoRegion{ .sceneId = SCENE_WITCH_SHOP,
        .checks = {
            CHECK(RC_HAGS_POTION_SHOP_FREESTANDING_RUPEE, true),
            // TODO: Add CAN_ACCESS(MUSHROOM) once that is shuffled.
            CHECK(RC_HAGS_POTION_SHOP_ITEM_01, CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_01) && HAS_ITEM(ITEM_MASK_SCENTS) && HAS_BOTTLE),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_02, CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_02)),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_03, CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_03)),
            CHECK(RC_HAGS_POTION_SHOP_KOTAKE, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 5),      ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_RED_POTION_REFILL, true),
        },
    };
    Regions[RR_MILK_ROAD] = RandoRegion{ .sceneId = SCENE_ROMANYMAE,
        .checks = {
            CHECK(RC_KEATON_QUIZ, HAS_ITEM(ITEM_MASK_KEATON)),
            CHECK(RC_MILK_ROAD_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_MILK_ROAD_TINGLE_MAP_01)),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_MILK_ROAD_TINGLE_MAP_02)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 5),                ENTRANCE(MILK_ROAD, 0), true),
            EXIT(ENTRANCE(ROMANI_RANCH, 0),                 ENTRANCE(MILK_ROAD, 1), true),
            EXIT(ENTRANCE(GORMAN_TRACK, 3),                 ENTRANCE(MILK_ROAD, 2), CAN_PLAY_SONG(EPONA)),
            EXIT(ENTRANCE(GORMAN_TRACK, 0),                 ENTRANCE(MILK_ROAD, 3), true),
        },
        .oneWayEntrances = {
            ENTRANCE(MILK_ROAD, 4), // From Song of Soaring
        }
    };
    Regions[RR_RANCH_BARN] = RandoRegion{ .sceneId = SCENE_OMOYA,
        .checks = {
            CHECK(RC_ROMANI_RANCH_BARN_COW_LEFT, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_BARN_COW_MIDDLE, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_BARN_COW_RIGHT, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG))
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 2),                 ENTRANCE(RANCH_HOUSE, 0), true),
        },
    };
    Regions[RR_RANCH_HOUSE] = RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 3),                 ENTRANCE(RANCH_HOUSE, 1), true),
        },
    };
    Regions[RR_ROAD_TO_SOUTHERN_SWAMP_GROTTO] = RandoRegion{ .name = "Road to Southern Swamp Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_SOUTHERN_SWAMP, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_ROAD_TO_SOUTHERN_SWAMP] = RandoRegion{ .sceneId = SCENE_24KEMONOMITI,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_PIECE_OF_HEART, CAN_USE_PROJECTILE || HAS_ITEM(ITEM_BOMBCHU)),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01)),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 1),                ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 0),      ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1), true),
            EXIT(ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),       ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_SOUTHERN_SWAMP_GROTTO, true), // TODO: Grotto mapping
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
        },
    };
    Regions[RR_ROMANI_RANCH] = RandoRegion{ .sceneId = SCENE_F01,
        .checks = {
            CHECK(RC_ROMANI_RANCH_ALIENS, HAS_ITEM(ITEM_BOW) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_EPONAS_SONG, CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_ENTRANCE, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_BACK, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_FRONT, CAN_PLAY_SONG(EPONA) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_FIELD_LARGE_CRATE, true),
            CHECK(RC_CREMIA_ESCORT, HAS_ITEM(ITEM_BOW) && RANDO_EVENTS[RE_COWS_FROM_ALIENS]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 1),                    ENTRANCE(ROMANI_RANCH, 0), true),
            EXIT(ENTRANCE(RANCH_HOUSE, 0),                  ENTRANCE(ROMANI_RANCH, 2), true), // Barn
            EXIT(ENTRANCE(RANCH_HOUSE, 1),                  ENTRANCE(ROMANI_RANCH, 3), true), // House
            EXIT(ENTRANCE(CUCCO_SHACK, 0),                  ENTRANCE(ROMANI_RANCH, 4), true),
            EXIT(ENTRANCE(DOGGY_RACETRACK, 0),              ENTRANCE(ROMANI_RANCH, 5), true),
        },
        .events = {
            EVENT(RE_COWS_FROM_ALIENS, CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG) && HAS_ITEM(ITEM_BOW)),
        },
    };
    Regions[RR_SOUTHERN_SWAMP_GROTTO] = RandoRegion{ .name = "Southern Swamp Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_SOUTHERN_SWAMP_NORTH] = RandoRegion{ .name = "North Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_PIECE_OF_HEART, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_BEANS, CAN_BE_DEKU),
            CHECK(RC_SOUTHERN_SWAMP_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_01, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_02, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_03, true),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_01, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_02, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_03, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_FREESTANDING_RUPEE_01, CAN_BE_DEKU || CAN_BE_ZORA),
            CHECK(RC_SOUTHERN_SWAMP_FREESTANDING_RUPEE_02, CAN_BE_DEKU || CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), true),
            EXIT(ENTRANCE(TOURIST_INFORMATION, 0),          ENTRANCE(SOUTHERN_SWAMP_POISONED, 1), true),
            EXIT(ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 5), true),
            EXIT(ENTRANCE(WOODS_OF_MYSTERY, 0),             ENTRANCE(SOUTHERN_SWAMP_POISONED, 7), true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, (RANDO_EVENTS[RE_SOUTHERN_SWAMP_KILL_OCTOROK] || RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE])),
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_ACCESS_BEANS_REFILL, CAN_BE_DEKU && HAS_ITEM(ITEM_MAGIC_BEANS)),
            EVENT(RE_SOUTHERN_SWAMP_KILL_OCTOROK, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 9), // From river in Ikana
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // From Song of Soaring
        }
    };
    Regions[RR_SOUTHERN_SWAMP_SOUTH] = RandoRegion{ .name = "South Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_SONG_OF_SOARING, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 0),                     ENTRANCE(SOUTHERN_SWAMP_POISONED, 2), CAN_BE_DEKU || RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            EXIT(ENTRANCE(DEKU_PALACE, 0),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 3), true),
            EXIT(ENTRANCE(DEKU_PALACE, 5),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 4), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(SWAMP_SPIDER_HOUSE, 0),           ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), CAN_LIGHT_TORCH_NEAR_ANOTHER && (CAN_BE_DEKU || RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE])),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_NORTH, (RANDO_EVENTS[RE_SOUTHERN_SWAMP_KILL_OCTOROK] || RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE])),
            CONNECTION(RR_SOUTHERN_SWAMP_GROTTO, CAN_BE_DEKU), // TODO: Grotto mapping
        },
    };
    Regions[RR_SWAMP_SHOOTING_GALLERY] = RandoRegion{ .sceneId = SCENE_SYATEKI_MORI,
        .checks = {
            CHECK(RC_SWAMP_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_SWAMP_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2),       ENTRANCE(SWAMP_SHOOTING_GALLERY, 0), true),
        },
    };
    Regions[RR_TOURIST_INFORMATION] = RandoRegion{ .sceneId = SCENE_MAP_SHOP,
        .checks = {
            // Also requires poison to not be cleared
            CHECK(RC_TOURIST_INFORMATION_ARCHERY, RANDO_EVENTS[RE_SAVED_KOUME] && RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_TOURIST_INFORMATION_PICTOBOX, RANDO_EVENTS[RE_SAVED_KOUME]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 1),      ENTRANCE(TOURIST_INFORMATION, 0), true),
        },
        .events = {
            EVENT(RE_SOUTHERN_SWAMP_KILL_OCTOROK, RANDO_EVENTS[RE_SAVED_KOUME]),
        },
    };
    Regions[RR_WOODFALL_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Woodfall", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_WOODFALL_GREAT_FAIRY, HAS_ALL_STRAY_FAIRIES(DUNGEON_INDEX_WOODFALL_TEMPLE)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 2),                     ENTRANCE(FAIRY_FOUNTAIN, 1), true),
        },
    };
    Regions[RR_WOODFALL] = RandoRegion{ .sceneId = SCENE_21MITURINMAE,
        .checks = {
            CHECK(RC_WOODFALL_ENTRANCE_CHEST, CAN_BE_DEKU || RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_WOODFALL_PIECE_OF_HEART_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_WOODFALL_NEAR_OWL_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_POT_01, CAN_BE_DEKU || CAN_OWL_WARP(OWL_WARP_WOODFALL)),
            CHECK(RC_WOODFALL_POT_02, CAN_BE_DEKU || CAN_OWL_WARP(OWL_WARP_WOODFALL)),
            CHECK(RC_WOODFALL_POT_03, CAN_BE_DEKU || CAN_OWL_WARP(OWL_WARP_WOODFALL)),
            CHECK(RC_WOODFALL_FREESTANDING_RUPEE, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 2),      ENTRANCE(WOODFALL, 0), true),
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 0),              ENTRANCE(WOODFALL, 1), CanAccessDungeon(DUNGEON_INDEX_WOODFALL_TEMPLE)),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 1),               ENTRANCE(WOODFALL, 2), CAN_BE_DEKU),
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 2),              ENTRANCE(WOODFALL, 3), RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
        },
    };
    Regions[RR_WOODS_OF_MYSTERY_GROTTO] = RandoRegion{ .name = "Woods of Mystery Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_CHEST, true),
        },
        .connections = {
            CONNECTION(RR_WOODS_OF_MYSTERY, true), // TODO: Grotto mapping
        },
    };
    Regions[RR_WOODS_OF_MYSTERY] = RandoRegion{ .sceneId = SCENE_26SARUNOMORI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 7),      ENTRANCE(WOODS_OF_MYSTERY, 0), true),
        },
        .connections = {
            CONNECTION(RR_WOODS_OF_MYSTERY_GROTTO, true), // TODO: Grotto mapping
        },
        .events = {
            EVENT(RE_SAVED_KOUME, HAS_BOTTLE && (CAN_ACCESS(RED_POTION_REFILL) || CAN_ACCESS(BLUE_POTION_REFILL))),
        },
    };
}, {});
// clang-format on
