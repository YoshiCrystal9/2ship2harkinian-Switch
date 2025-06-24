#include "Spoiler.h"
#include "Rando/Rando.h"

namespace Rando {

namespace Spoiler {

void ApplyToSaveContext(nlohmann::json spoiler) {
    gSaveContext.save.shipSaveInfo.rando.finalSeed = spoiler["finalSeed"].get<uint32_t>();

    for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
        RANDO_SAVE_OPTIONS[randoOptionId] = spoiler["options"][randoStaticOption.name].get<uint32_t>();
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] != 3) {
        gSaveContext.save.saveInfo.playerData.healthCapacity = gSaveContext.save.saveInfo.playerData.health =
            RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] * 0x10;
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_CONSUMABLES]) {
        GiveItem(RI_DEKU_STICK);
        GiveItem(RI_DEKU_NUT);
        AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
        AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
    }

    std::vector<RandoItemId> startingItems = {};

    for (size_t i = 0; i < Rando::StaticData::StartingItemsMap.size(); i++) {
        RandoItemId itemId = Rando::StaticData::StartingItemsMap[i];
        RandoOptionId optionId;
        if (i < 32) {
            optionId = RO_STARTING_ITEMS_1;
        } else if (i < 64) {
            optionId = RO_STARTING_ITEMS_2;
        } else {
            optionId = RO_STARTING_ITEMS_3;
        }
        uint32_t startingItemsBits = RANDO_SAVE_OPTIONS[optionId];
        if ((startingItemsBits & (1 << (i % 32))) != 0) {
            startingItems.push_back(itemId);
        }
    }

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

    for (RandoItemId startingItem : startingItems) {
        GiveItem(ConvertItem(startingItem));
    }

    if (RANDO_SAVE_OPTIONS[RO_STARTING_RUPEES]) {
        gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
    }

    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            continue;
        }

        if (!spoiler["checks"].contains(randoStaticCheck.name)) {
            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
            RANDO_SAVE_CHECKS[randoCheckId].shuffled = false;
            continue;
        }

        // Check if it's an object or a string
        if (spoiler["checks"][randoStaticCheck.name].is_object()) {
            std::string itemName = spoiler["checks"][randoStaticCheck.name]["randoItemId"].get<std::string>();
            RandoItemId randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());

            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;
            RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;

            // If it has a price, set it
            if (spoiler["checks"][randoStaticCheck.name].contains("price")) {
                RANDO_SAVE_CHECKS[randoCheckId].price =
                    spoiler["checks"][randoStaticCheck.name]["price"].get<uint16_t>();
            }
        } else {
            std::string itemName = spoiler["checks"][randoStaticCheck.name].get<std::string>();
            RandoItemId randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());

            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;
            RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
        }
    }
}

} // namespace Spoiler

} // namespace Rando
