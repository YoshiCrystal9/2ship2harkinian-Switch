#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "Logic.h"

namespace Rando {

namespace Logic {

std::map<RandoRegionId, RandoRegion> Regions = {};

RandoRegionId GetRegionIdFromEntrance(s32 entrance) {
    static std::map<s32, RandoRegionId> entranceToRegionId;
    if (entranceToRegionId.empty()) {
        for (auto& [randoRegionId, randoRegion] : Regions) {
            for (auto& [_, regionExit] : randoRegion.exits) {
                if (regionExit.returnEntrance == ONE_WAY_EXIT) {
                    continue;
                }
                entranceToRegionId[regionExit.returnEntrance] = randoRegionId;
            }
            for (auto& entrance : randoRegion.oneWayEntrances) {
                entranceToRegionId[entrance] = randoRegionId;
            }
        }
    }

    if (entranceToRegionId.contains(entrance)) {
        return entranceToRegionId[entrance];
    }

    return RR_MAX;
}

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions) {
    auto& randoRegion = Rando::Logic::Regions[currentRegion];

    for (auto& [connectedRegionId, condition] : randoRegion.connections) {
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && condition.first()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }

    for (auto& [exitId, regionExit] : randoRegion.exits) {
        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && regionExit.condition()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }
}

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_MAX] = RandoRegion{ .sceneId = SCENE_MAX,
        .checks = {
            CHECK(RC_STARTING_ITEM_DEKU_MASK, true),
            CHECK(RC_STARTING_ITEM_SONG_OF_HEALING, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),                      ONE_WAY_EXIT, true), // Save warp
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 9),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_CLOCK_TOWN)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 10),              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP)),
            EXIT(ENTRANCE(WOODFALL, 4),                              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_WOODFALL)),
            EXIT(ENTRANCE(MILK_ROAD, 4),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MILK_ROAD)),
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),               ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE)),
            EXIT(ENTRANCE(SNOWHEAD, 3),                              ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_SNOWHEAD)),
            EXIT(ENTRANCE(GREAT_BAY_COAST, 11),                      ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_GREAT_BAY_COAST)),
            EXIT(ENTRANCE(ZORA_CAPE, 6),                             ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_ZORA_CAPE)),
            EXIT(ENTRANCE(IKANA_CANYON, 4),                          ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_IKANA_CANYON)),
            EXIT(ENTRANCE(STONE_TOWER, 3),                           ONE_WAY_EXIT, CAN_PLAY_SONG(SOARING) && CAN_OWL_WARP(OWL_WARP_STONE_TOWER)),
            EXIT(ENTRANCE(CUTSCENE, 0),                              ONE_WAY_EXIT, true), // Enemy Drop Region
        },
        .connections = {
            CONNECTION(RR_MISCELLANEOUS, true), // Enemy Drop Region
        },
    };
}, {});
// clang-format on

} // namespace Logic

} // namespace Rando
