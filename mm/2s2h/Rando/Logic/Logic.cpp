#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "Logic.h"

namespace Rando {

namespace Logic {

std::map<RandoRegionId, RandoRegion> Regions = {};

// Thread-local storage for current region time during check evaluation
thread_local uint64_t gCurrentRegionTime = 0;

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

// Helper: Convert runtime game time to TimeSlice enum for dynamic time checking
TimeSlice TimeSliceFromGameTime(s32 day, u16 time) {
    // Handle edge cases: day 0 or invalid inputs
    if (day < 1 || day > 3) {
        return TIME_DAY1_AM_06_00; // Default fallback
    }

    // Convert to time slice based on day/time ranges
    // This is approximate - exact mapping would need game time constants
    bool isNight = (time >= GAME_TIME_NIGHT_START || time < GAME_TIME_DAY_START);
    int halfDayOffset = (day - 1) * 2 + (isNight ? 1 : 0);

    // Map to approximate time slice within the half-day
    if (halfDayOffset >= 6)
        return TIME_NIGHT3_AM_05_00;

    const auto& range = HALF_DAY_TIME_RANGES[halfDayOffset];
    return static_cast<TimeSlice>(range.startSlice);
}

// Helper: Returns the initial time state for logic solving (start at Day 1, 6:00 AM)
RegionTimeState InitialTimeState() {
    return { .timeSlices = (TIME_BIT_ONE << TIME_DAY1_AM_06_00), .canStayOverTime = false };
}

// Shared initialization function for region time states
std::unordered_map<RandoRegionId, RegionTimeState> InitializeRegionTimeStates(RandoRegionId startRegion) {
    std::unordered_map<RandoRegionId, RegionTimeState> states;

    // Start with appropriate time based on Clock Shuffle
    if (SettingClocks()) {
        // Clock Shuffle: start with owned time slices only
        states[startRegion] = { .timeSlices = TimeLogic::GetOwnedTimeSlices(), .canStayOverTime = false };
    } else {
        // No Clock Shuffle: start at Day 1 6am
        states[startRegion] = InitialTimeState();
    }

    return states;
}

// Helper to ensure region time state exists
void EnsureRegionTimeState(std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates,
                           RandoRegionId regionId) {
    if (regionTimeStates.find(regionId) == regionTimeStates.end()) {
        auto& region = Regions[regionId];
        regionTimeStates[regionId] = { .timeSlices = TimeLogic::GetOwnedTimeSlices(),
                                       .canStayOverTime = region.canStayOverTime };
    }
}

// Time expansion during region traversal with stay restrictions
// Time expansion semantics: if canStayOverTime, sequentially test each future time slice
// Stop permanently if any timeStayRestrictions check fails
void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions,
                          std::unordered_map<RandoRegionId, RegionTimeState>& regionTimeStates) {
    // Ensure current region has time state
    EnsureRegionTimeState(regionTimeStates, currentRegion);

    auto& sourceRegion = Regions[currentRegion];
    auto& sourceTimeState = regionTimeStates[currentRegion];

    // Expand time if player can wait in this region
    uint64_t currentTime = sourceTimeState.timeSlices;
    if (sourceTimeState.canStayOverTime) {
        currentTime = TimeLogic::ExpandTimeForward(currentTime, sourceRegion);
        sourceTimeState.timeSlices = currentTime;
    }

    // Set global time for check evaluation
    gCurrentRegionTime = currentTime;

    // Explore connections
    for (auto& [connectedRegionId, condition] : sourceRegion.connections) {
        if (reachableRegions.count(connectedRegionId) == 0 && condition.first()) {
            reachableRegions.insert(connectedRegionId);

            auto& targetRegion = Regions[connectedRegionId];
            regionTimeStates[connectedRegionId] = { .timeSlices = currentTime,
                                                    .canStayOverTime = targetRegion.canStayOverTime };

            FindReachableRegions(connectedRegionId, reachableRegions, regionTimeStates);
        }
    }

    // Explore exits
    for (auto& [exitId, regionExit] : sourceRegion.exits) {
        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        if (reachableRegions.count(connectedRegionId) == 0 && regionExit.condition()) {
            reachableRegions.insert(connectedRegionId);

            auto& targetRegion = Regions[connectedRegionId];
            regionTimeStates[connectedRegionId] = { .timeSlices = currentTime,
                                                    .canStayOverTime = targetRegion.canStayOverTime };

            FindReachableRegions(connectedRegionId, reachableRegions, regionTimeStates);
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
        },
    };
}, {});
// clang-format on

} // namespace Logic

} // namespace Rando
