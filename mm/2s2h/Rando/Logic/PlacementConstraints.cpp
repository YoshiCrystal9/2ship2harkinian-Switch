#include "Logic.h"
#include "Rando/StaticData/StaticData.h"

namespace Rando {

namespace Logic {

int SceneIdToDungeon(SceneId sceneId) {
    switch (sceneId) {
        case SCENE_MITURIN:
        case SCENE_MITURIN_BS:
            return DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE;
        case SCENE_HAKUGIN:
        case SCENE_HAKUGIN_BS:
            return DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE;
        case SCENE_SEA:
        case SCENE_SEA_BS:
            return DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE;
        case SCENE_INISIE_N:
        case SCENE_INISIE_R:
        case SCENE_INISIE_BS:
            return DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE;
        default:
            return -1;
    }
}

int DungeonItemToDungeon(RandoItemId itemId) {
    int dungeon = -1;
    switch (itemId) {
        case RI_WOODFALL_SMALL_KEY:
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_STRAY_FAIRY:
            dungeon = DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE;
            break;
        case RI_SNOWHEAD_SMALL_KEY:
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_STRAY_FAIRY:
            dungeon = DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE;
            break;
        case RI_GREAT_BAY_SMALL_KEY:
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_STRAY_FAIRY:
            dungeon = DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE;
            break;
        case RI_STONE_TOWER_SMALL_KEY:
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_STRAY_FAIRY:
            dungeon = DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE;
            break;
        default:
            return -1;
    }
    return dungeon;
}

RandoOptionId DungeonItemPlacementOption(RandoItemId itemId) {
    if (DungeonItemToDungeon(itemId) < 0 && itemId != RI_CLOCK_TOWN_STRAY_FAIRY) {
        return RO_MAX;
    }
    switch (Rando::StaticData::Items[itemId].randoItemType) {
        case RITYPE_SMALL_KEY:
            return RO_PLACEMENT_SMALL_KEYS;
        case RITYPE_BOSS_KEY:
            return RO_PLACEMENT_BOSS_KEYS;
        case RITYPE_STRAY_FAIRY:
            return RO_PLACEMENT_STRAY_FAIRIES;
        default:
            return RO_MAX;
    }
}

bool StaysAtVanillaCheck(RandoItemId itemId, const RandoSaveInfo& saveInfo) {
    RandoOptionId placementOption = DungeonItemPlacementOption(itemId);
    if (placementOption == RO_MAX) {
        return false;
    }
    switch (saveInfo.randoSaveOptions[placementOption]) {
        case RO_DUNGEON_ITEM_VANILLA:
            return true;
        case RO_DUNGEON_ITEM_OWN_DUNGEON:
            return DungeonItemToDungeon(itemId) < 0;
        default:
            return false;
    }
}

int RandoItemIdToDungeon(RandoItemId itemId) {
    RandoOptionId placementOption = DungeonItemPlacementOption(itemId);
    if (placementOption == RO_MAX || RANDO_SAVE_OPTIONS[placementOption] != RO_DUNGEON_ITEM_OWN_DUNGEON) {
        return -1;
    }
    return DungeonItemToDungeon(itemId);
}

bool IsItemAllowedAtCheck(RandoItemId itemId, RandoCheckId checkId) {
    int confinedDungeon = RandoItemIdToDungeon(itemId);
    return confinedDungeon < 0 || confinedDungeon == SceneIdToDungeon(Rando::StaticData::Checks[checkId].sceneId);
}

size_t SelectItemForCheck(const std::vector<RandoItemId>& itemPool, const std::vector<RandoCheckId>& checkPool,
                          RandoCheckId checkId) {
    int dungeon = SceneIdToDungeon(Rando::StaticData::Checks[checkId].sceneId);
    if (dungeon >= 0) {
        int confinedItems = 0;
        for (RandoItemId itemId : itemPool) {
            if (RandoItemIdToDungeon(itemId) == dungeon) {
                confinedItems++;
            }
        }
        if (confinedItems > 0) {
            int dungeonChecks = 1; // this check was already removed from checkPool
            for (RandoCheckId other : checkPool) {
                if (SceneIdToDungeon(Rando::StaticData::Checks[other].sceneId) == dungeon) {
                    dungeonChecks++;
                }
            }
            if (confinedItems >= dungeonChecks) {
                for (size_t i = itemPool.size(); i-- > 0;) {
                    if (RandoItemIdToDungeon(itemPool[i]) == dungeon) {
                        return i;
                    }
                }
            }
        }
    }

    for (size_t i = itemPool.size(); i-- > 0;) {
        if (IsItemAllowedAtCheck(itemPool[i], checkId)) {
            return i;
        }
    }
    return itemPool.size();
}

void PreplaceConfinedItems(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    std::map<int, std::vector<RandoCheckId>> dungeonChecks;
    for (RandoCheckId checkId : checkPool) {
        int dungeon = SceneIdToDungeon(Rando::StaticData::Checks[checkId].sceneId);
        if (dungeon >= 0) {
            dungeonChecks[dungeon].push_back(checkId);
        }
    }

    std::set<RandoCheckId> placedChecks;
    std::set<size_t> placedItems;
    for (size_t i = 0; i < itemPool.size(); i++) {
        int dungeon = RandoItemIdToDungeon(itemPool[i]);
        if (dungeon < 0) {
            continue; // not confined to a dungeon
        }
        auto& checks = dungeonChecks[dungeon];
        if (checks.empty()) {
            continue; // dungeon out of room: leave it for the general pool
        }
        size_t pick = Ship_Random(0, checks.size());
        RandoCheckId checkId = checks[pick];
        checks.erase(checks.begin() + pick);

        RANDO_SAVE_CHECKS[checkId].shuffled = true;
        RANDO_SAVE_CHECKS[checkId].randoItemId = itemPool[i];
        placedChecks.insert(checkId);
        placedItems.insert(i);
    }

    // Remove the placed items and checks from the pools.
    std::vector<RandoItemId> remainingItems;
    for (size_t i = 0; i < itemPool.size(); i++) {
        if (placedItems.find(i) == placedItems.end()) {
            remainingItems.push_back(itemPool[i]);
        }
    }
    itemPool = std::move(remainingItems);

    std::vector<RandoCheckId> remainingChecks;
    for (RandoCheckId checkId : checkPool) {
        if (placedChecks.find(checkId) == placedChecks.end()) {
            remainingChecks.push_back(checkId);
        }
    }
    checkPool = std::move(remainingChecks);
}

} // namespace Logic

} // namespace Rando
