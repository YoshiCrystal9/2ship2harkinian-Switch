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
    std::vector<RandoItemId> computedStartingItems = Rando::GetComputedStartingItems(saveInfo);
    startingItems.insert(startingItems.end(), computedStartingItems.begin(), computedStartingItems.end());

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

            if (randoStaticCheck.randoCheckType == RCTYPE_TREE &&
                saveInfo.randoSaveOptions[RO_SHUFFLE_TREE_DROPS] == RO_GENERIC_NO) {
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

    // Songs
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_SONG_SUN] == RO_GENERIC_YES) {
        itemPool.push_back(RI_SONG_SUN);
    }
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_SONG_DOUBLE_TIME] == RO_GENERIC_YES) {
        itemPool.push_back(RI_SONG_DOUBLE_TIME);
    }
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_SONG_INVERTED_TIME] == RO_GENERIC_YES) {
        itemPool.push_back(RI_SONG_INVERTED_TIME);
    }
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_SONG_SARIA] == RO_GENERIC_YES) {
        itemPool.push_back(RI_SONG_SARIA);
    }

    // Shuffle Triforce Pieces into the Pool
    if (saveInfo.randoSaveOptions[RO_SHUFFLE_TRIFORCE_PIECES] == RO_GENERIC_YES) {
        int piecesToShuffle = saveInfo.randoSaveOptions[RO_TRIFORCE_PIECES_MAX];
        while (piecesToShuffle) {
            itemPool.push_back(RI_TRIFORCE_PIECE);
            piecesToShuffle--;
        }
    }

    // Remove extra stray fairies/gold skulltulas from the pool
    std::map<RandoItemId, int> removeAbleItemsInPool = {
        { RI_STONE_TOWER_STRAY_FAIRY, 0 }, { RI_GREAT_BAY_STRAY_FAIRY, 0 }, { RI_SNOWHEAD_STRAY_FAIRY, 0 },
        { RI_WOODFALL_STRAY_FAIRY, 0 },    { RI_GS_TOKEN_SWAMP, 0 },        { RI_GS_TOKEN_OCEAN, 0 },
    };
    for (RandoItemId itemId : itemPool) {
        if (removeAbleItemsInPool.find(itemId) != removeAbleItemsInPool.end()) {
            removeAbleItemsInPool[itemId]++;
        }
    }
    for (auto& [itemId, count] : removeAbleItemsInPool) {
        int max = 0;
        switch (itemId) {
            case RI_STONE_TOWER_STRAY_FAIRY:
            case RI_GREAT_BAY_STRAY_FAIRY:
            case RI_SNOWHEAD_STRAY_FAIRY:
            case RI_WOODFALL_STRAY_FAIRY:
                max = saveInfo.randoSaveOptions[RO_STRAY_FAIRIES_MAX];
                break;
            case RI_GS_TOKEN_SWAMP:
            case RI_GS_TOKEN_OCEAN:
                max = saveInfo.randoSaveOptions[RO_SKULLTULA_TOKENS_MAX];
                break;
            default:
                break;
        }

        while (count > max) {
            auto it = std::find(itemPool.begin(), itemPool.end(), itemId);
            if (it != itemPool.end()) {
                itemPool.erase(it);
                count--;
            } else {
                break;
            }
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
        std::vector<RandoItemId> plentifulItems;
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
                    if (Rando::StaticData::Items[itemPool[i]].itemId != ITEM_TINGLE_MAP &&
                        Rando::StaticData::Items[itemPool[i]].itemId != ITEM_DUNGEON_MAP &&
                        Rando::StaticData::Items[itemPool[i]].itemId != ITEM_COMPASS) {
                        plentifulItems.push_back(itemPool[i]);
                    }
                    break;
                case RITYPE_HEALTH:
                case RITYPE_JUNK:
                default:
                    break;
            }
        }

        for (RandoItemId plentifulItem : plentifulItems) {
            itemPool.push_back(plentifulItem);
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
