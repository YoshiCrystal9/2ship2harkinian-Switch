#include "Rando.h"
#include "2s2h/Rando/StaticData/StaticData.h"
#include "2s2h/ShipUtils.h"
#include <libultraship/libultraship.h>
#include <libultraship/bridge/consolevariablebridge.h>

// Starting items is a dynamically sized list of strings, so we can't store it with the other options because it can't
// fit in a CVar. We have to store it in various places
// - In the spoiler file, under startingItems as an array of strings ["RI_OCARINA"...]
// - In the save file, in randoSaveInfo.randoStartingItems as an array of u16 [RI_OCARINA...]
// - In the config file, under CVars.gRando.StartingItems as an array of strings ["RI_OCARINA"...]

namespace Rando {

void GrantStartingItems() {
    std::vector<RandoItemId> startingItems = Rando::GetStartingItemsFromSave(gSaveContext.save.shipSaveInfo.rando);

    if (RANDO_SAVE_OPTIONS[RO_STARTING_MAPS_AND_COMPASSES]) {
        std::vector<RandoItemId> MapsAndCompasses = {
            RI_GREAT_BAY_COMPASS,       RI_GREAT_BAY_MAP,       RI_SNOWHEAD_COMPASS,       RI_SNOWHEAD_MAP,
            RI_STONE_TOWER_COMPASS,     RI_STONE_TOWER_MAP,     RI_TINGLE_MAP_CLOCK_TOWN,  RI_TINGLE_MAP_GREAT_BAY,
            RI_TINGLE_MAP_ROMANI_RANCH, RI_TINGLE_MAP_SNOWHEAD, RI_TINGLE_MAP_STONE_TOWER, RI_TINGLE_MAP_WOODFALL,
            RI_WOODFALL_COMPASS,        RI_WOODFALL_MAP,
        };

        for (RandoItemId itemId : MapsAndCompasses) {
            startingItems.push_back(itemId);
        }
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SWIM] != RO_GENERIC_YES) {
        startingItems.push_back(RI_ABILITY_SWIM);
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENEMY_SOULS] != RO_GENERIC_YES) {
        for (int i = RI_SOUL_ENEMY_ALIEN; i <= RI_SOUL_ENEMY_WOLFOS; i++) {
            startingItems.push_back((RandoItemId)i);
        }
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_OCARINA_BUTTONS] != RO_GENERIC_YES) {
        for (int i = RI_OCARINA_BUTTON_A; i <= RI_OCARINA_BUTTON_C_UP; i++) {
            startingItems.push_back((RandoItemId)i);
        }
    }

    // When shuffling time, if the player did not choose any starting time items, we need to give them at least one.
    if (RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE] == RO_GENERIC_YES) {
        bool hasTimeItem = false;
        for (RandoItemId randoItemId : startingItems) {
            if (randoItemId >= RI_TIME_DAY_1 && randoItemId <= RI_TIME_PROGRESSIVE) {
                hasTimeItem = true;
                break;
            }
        }
        if (!hasTimeItem) {
            if (RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE_PROGRESSIVE] == RO_CLOCK_SHUFFLE_RANDOM) {
                Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed);
                startingItems.push_back((RandoItemId)(RI_TIME_DAY_1 + Ship_Random(0, 5)));
            } else {
                startingItems.push_back(RI_TIME_PROGRESSIVE);
            }
        }
    }

    for (RandoItemId startingItem : startingItems) {
        Rando::GiveItem(Rando::ConvertItem(startingItem));
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] != 3) {
        gSaveContext.save.saveInfo.playerData.healthCapacity = gSaveContext.save.saveInfo.playerData.health =
            RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] * 0x10;
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_CONSUMABLES]) {
        Rando::GiveItem(RI_DEKU_STICK);
        Rando::GiveItem(RI_DEKU_NUT);
        AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
        AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_RUPEES]) {
        gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
    }
}

std::vector<RandoItemId> GetStartingItemsFromSpoiler(nlohmann::json& spoiler) {
    auto startingItemsStrings = spoiler["startingItems"].get<std::vector<std::string>>();
    std::vector<RandoItemId> startingItems;

    for (auto& itemName : startingItemsStrings) {
        auto randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            startingItems.push_back(randoItemId);
        }
    }

    return startingItems;
}

void SetStartingItemsInSpoiler(nlohmann::json& spoiler, std::vector<RandoItemId>& startingItems) {
    std::vector<std::string> startingItemsJson;
    for (auto& randoItemId : startingItems) {
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            startingItemsJson.push_back(Rando::StaticData::Items[randoItemId].spoilerName);
        }
    }
    spoiler["startingItems"] = startingItemsJson;
}

std::vector<RandoItemId> GetStartingItemsFromSave(RandoSaveInfo& randoSaveInfo) {
    std::vector<RandoItemId> startingItems;

    for (int i = 0; i < ARRAY_COUNT(randoSaveInfo.randoStartingItems); i++) {
        if (randoSaveInfo.randoStartingItems[i] > RI_UNKNOWN && randoSaveInfo.randoStartingItems[i] < RI_MAX) {
            startingItems.push_back((RandoItemId)randoSaveInfo.randoStartingItems[i]);
        }
    }

    return startingItems;
}

void SetStartingItemsInSave(RandoSaveInfo& randoSaveInfo, std::vector<RandoItemId>& startingItems) {
    memset(&randoSaveInfo.randoStartingItems, 0, sizeof(randoSaveInfo.randoStartingItems));

    size_t index = 0;
    for (auto& randoItemId : startingItems) {
        if (index >= ARRAY_COUNT(randoSaveInfo.randoStartingItems)) {
            break;
        }
        randoSaveInfo.randoStartingItems[index++] = randoItemId;
    }
}

std::vector<RandoItemId> GetStartingItemsFromConfig() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    std::vector<RandoItemId> startingItems = { RI_PROGRESSIVE_SWORD, RI_SHIELD_HERO, RI_OCARINA, RI_SONG_TIME };

    // Verify that the config has CVars.gRando.StartingItems and its an array
    if (allConfig.find("CVars") != allConfig.end() && allConfig["CVars"].is_object() &&
        allConfig["CVars"].find("gRando") != allConfig["CVars"].end() && allConfig["CVars"]["gRando"].is_object() &&
        allConfig["CVars"]["gRando"].find("StartingItems") != allConfig["CVars"]["gRando"].end() &&
        allConfig["CVars"]["gRando"]["StartingItems"].is_array()) {
        startingItems.clear();

        auto startingItemsStrings = allConfig["CVars"]["gRando"]["StartingItems"].get<std::vector<std::string>>();
        for (auto& itemName : startingItemsStrings) {
            auto randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());
            if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
                startingItems.push_back(randoItemId);
            }
        }
    }

    return startingItems;
}

void SetStartingItemsInConfig(std::vector<RandoItemId>& startingItems) {
    auto startingItemsJson = nlohmann::json::array();
    for (auto& randoItemId : startingItems) {
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            startingItemsJson.push_back(Rando::StaticData::Items[randoItemId].spoilerName);
        }
    }
    Ship::Context::GetInstance()->GetConfig()->SetBlock("CVars.gRando.StartingItems", startingItemsJson);
    Ship::Context::GetInstance()->GetConfig()->Save();
}

} // namespace Rando
