#include "Logic.h"
#include "Rando/MiscBehavior/ClockShuffle.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <sstream>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void GeneratePools(RandoSaveInfo& saveInfo, std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    std::vector<RandoItemId> startingItems = Rando::GetStartingItemsFromSave(saveInfo);

    if (saveInfo.randoSaveOptions[RO_STARTING_MAPS_AND_COMPASSES]) {
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

    std::vector<RandoCheckId> excludedChecks;
    std::string excludedChecksList = CVarGetString("gRando.ExcludedChecks", "");
    std::string word;
    std::istringstream stream(excludedChecksList);
    while (std::getline(stream, word, ',')) {
        excludedChecks.push_back((RandoCheckId)std::stoi(word));
    }

    // First loop through all regions and add checks/items to the pool
    for (auto& [randoRegionId, randoRegion] : Rando::Logic::Regions) {
        for (auto& [randoCheckId, _] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

            // Initialize the check with it's vanilla item
            if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                saveInfo.randoSaveChecks[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
            }

            // Skip checks that are already in the pool
            if (std::find(checkPool.begin(), checkPool.end(), randoCheckId) != checkPool.end()) {
                continue;
            }

            // TODO: We may never shuffle these 2 pots, leaving this decision for later
            if (randoStaticCheck.sceneId == SCENE_LAST_BS) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_OWL &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_OWL_STATUES] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_POT &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_POT_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_CRATE &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_CRATE_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_BARREL &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_BARREL_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_GRASS &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_GRASS_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_FREESTANDING &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_FREESTANDING_ITEMS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_SNOWBALL &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_SNOWBALL_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_FROG &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_FROGS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_REMAINS &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_BOSS_REMAINS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_COW &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_COWS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_ENEMY_DROP &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_ENEMY_DROPS] == RO_GENERIC_NO) {
                continue;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_TINGLE_SHOP &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_TINGLE_SHOPS] == RO_GENERIC_NO) {
                continue;
            } else {
                int price = Ship_Random(0, 200);
                saveInfo.randoSaveChecks[randoCheckId].price = price;
            }

            if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                // We always want shuffle RC_CURIOSITY_SHOP_SPECIAL_ITEM &
                // RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM
                if (saveInfo.randoSaveOptions[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO &&
                    randoCheckId != RC_CURIOSITY_SHOP_SPECIAL_ITEM &&
                    randoCheckId != RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) {
                    continue;
                } else {
                    // We may come up with a better solution for this in the future, but for now we choose a
                    // random price ahead of time, logic will account for whatever price we choose
                    int price = Ship_Random(0, 200);
                    saveInfo.randoSaveChecks[randoCheckId].price = price;
                }
            }

            // When a check is skipped, we still want to add it's vanilla item to the pool, but we don't add the check.
            // Mark it as skipped and set it to junk. These leaves an inbalance in the pools that will get sorted
            // automatically if there is enough space.
            if (saveInfo.randoSaveOptions[RO_LOGIC] != RO_LOGIC_VANILLA) {
                auto it = std::find(excludedChecks.begin(), excludedChecks.end(), randoCheckId);
                if (it != excludedChecks.end()) {
                    itemPool.push_back(randoStaticCheck.randoItemId);

                    saveInfo.randoSaveChecks[randoCheckId].shuffled = true;
                    saveInfo.randoSaveChecks[randoCheckId].randoItemId = RI_JUNK;
                    saveInfo.randoSaveChecks[randoCheckId].skipped = true;
                    continue;
                }
            }

            checkPool.emplace_back(randoCheckId);
            itemPool.push_back(randoStaticCheck.randoItemId);
        }
    }

    // Add sword and shield to the pool because they don't have a vanilla location, if you are starting with
    // them they will be removed from the pool in the next step
    itemPool.push_back(RI_PROGRESSIVE_SWORD);
    itemPool.push_back(RI_SHIELD_HERO);

    // Add other items that don't have a vanilla location like Sun's Song or Song of Double Time

    // Boss Souls
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES) {
        for (int i = RI_SOUL_BOSS_GOHT; i <= RI_SOUL_BOSS_TWINMOLD; i++) {
            if (i == RI_SOUL_BOSS_MAJORA && saveInfo.randoSaveOptions[RO_SHUFFLE_TRIFORCE_PIECES] == RO_GENERIC_YES) {
                continue;
            }
            itemPool.push_back((RandoItemId)i);
        }
    }

    // Enemy Souls
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_ENEMY_SOULS] == RO_GENERIC_YES) {
        for (int i = RI_SOUL_ENEMY_ALIEN; i <= RI_SOUL_ENEMY_WOLFOS; i++) {
            itemPool.push_back((RandoItemId)i);
        }
    }

    // Shuffle Time
    if (saveInfo.randoSaveOptions[RO_CLOCK_SHUFFLE] == RO_GENERIC_YES) {
        auto clockShuffleMode = saveInfo.randoSaveOptions[RO_CLOCK_SHUFFLE_PROGRESSIVE];

        if (clockShuffleMode == RO_CLOCK_SHUFFLE_RANDOM) {
            itemPool.push_back(RI_TIME_DAY_1);
            itemPool.push_back(RI_TIME_NIGHT_1);
            itemPool.push_back(RI_TIME_DAY_2);
            itemPool.push_back(RI_TIME_NIGHT_2);
            itemPool.push_back(RI_TIME_DAY_3);
            itemPool.push_back(RI_TIME_NIGHT_3);
        } else {
            for (int i = 0; i < ClockItems::HALF_COUNT; ++i) {
                itemPool.push_back(RI_TIME_PROGRESSIVE);
            }
        }
    }

    // Abilities
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_SWIM] == RO_GENERIC_YES) {
        itemPool.push_back(RI_ABILITY_SWIM);
    }

    // Ocarina Buttons
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_OCARINA_BUTTONS] == RO_GENERIC_YES) {
        for (int i = RI_OCARINA_BUTTON_A; i <= RI_OCARINA_BUTTON_C_UP; i++) {
            itemPool.push_back((RandoItemId)i);
        }
    }

    // Shuffle Triforce Pieces into the Pool
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_TRIFORCE_PIECES] == RO_GENERIC_YES) {
        int piecesToShuffle = saveInfo.randoSaveOptions[RO_TRIFORCE_PIECES_MAX];
        while (piecesToShuffle) {
            itemPool.push_back(RI_TRIFORCE_PIECE);
            piecesToShuffle--;
        }
    }

    // Remove starting items from the pool (but only one per entry in startingItems)
    for (RandoItemId startingItem : startingItems) {
        auto it = std::find(itemPool.begin(), itemPool.end(), startingItem);
        if (it != itemPool.end()) {
            itemPool.erase(it);
        }
    }

    // Plentiful
    if (saveInfo.randoSaveOptions[RO_PLENTIFUL_ITEMS] == RO_GENERIC_YES) {
        int replaceableItems = 0;
        std::vector<RandoItemId> plentifulItems;
        std::vector<RandoItemId> potentialPlentifulItems;
        for (size_t i = 0; i < itemPool.size(); i++) {
            // The user can specify exactly how many pieces they want to shuffle, so skip those
            if (itemPool[i] == RI_TRIFORCE_PIECE) {
                continue;
            }

            switch (Rando::StaticData::Items[itemPool[i]].randoItemType) {
                case RITYPE_BOSS_KEY:
                case RITYPE_SMALL_KEY:
                case RITYPE_MASK:
                case RITYPE_MAJOR:
                    plentifulItems.push_back(itemPool[i]);
                    break;
                case RITYPE_LESSER:
                case RITYPE_SKULLTULA_TOKEN:
                case RITYPE_STRAY_FAIRY:
                    if (Ship_Random(0, 2) == 1) {
                        potentialPlentifulItems.push_back(itemPool[i]);
                    }
                    break;
                case RITYPE_HEALTH:
                case RITYPE_JUNK:
                default:
                    replaceableItems++;
                    break;
            }
        }

        if (replaceableItems > plentifulItems.size()) {
            for (RandoItemId plentifulItem : plentifulItems) {
                itemPool.push_back(plentifulItem);
            }
        }

        // Only add potentialPlentifulItems if we think we have enough room (this might not be perfect)
        if ((replaceableItems - plentifulItems.size() - 10) > potentialPlentifulItems.size()) {
            for (RandoItemId plentifulItem : potentialPlentifulItems) {
                itemPool.push_back(plentifulItem);
            }
        }
    }

    // Traps
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_TRAPS] == RO_GENERIC_YES) {
        int trapsToShuffle = saveInfo.randoSaveOptions[RO_TRAP_AMOUNT];
        while (trapsToShuffle) {
            itemPool.push_back(RI_TRAP);
            trapsToShuffle--;
        }
    }
}

} // namespace Logic

} // namespace Rando
