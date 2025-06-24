#include "MiscBehavior.h"
#include "Rando/Spoiler/Spoiler.h"
#include "Rando/Logic/Logic.h"
#include <boost_custom/container_hash/hash_32.hpp>
#include "public/bridge/consolevariablebridge.h"
#include <spdlog/spdlog.h>

extern "C" {
#include "functions.h"
#include "variables.h"
#include "ShipUtils.h"
}

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void Rando::MiscBehavior::OnFileCreate(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;
        // Zero out the rando struct
        memset(&gSaveContext.save.shipSaveInfo.rando, 0, sizeof(gSaveContext.save.shipSaveInfo.rando));
        // Copy whatever the current dungeon keys are, they're initialized as -1 in the save, not 0
        memcpy(&gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys,
               &gSaveContext.save.saveInfo.inventory.dungeonKeys,
               sizeof(gSaveContext.save.saveInfo.inventory.dungeonKeys));

        // Skip the first cycle, in Rando we start as Human at south clock town.
        gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
        gSaveContext.save.cutsceneIndex = 0;
        gSaveContext.save.hasTatl = true;
        gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
        gSaveContext.save.saveInfo.playerData.threeDayResetCount = 1;
        gSaveContext.save.isFirstCycle = true;
        SET_WEEKEVENTREG(WEEKEVENTREG_59_04);                                                  // Tatl
        SET_WEEKEVENTREG(WEEKEVENTREG_31_04);                                                  // Tatl
        gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_INSIDETOWER].switch0 |= (1 << 0); // Happy Mask Salesman

        // Remove Sword & Shield
        SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_NONE;
        SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_NONE);

        try {
            // SpoilerFileIndex == 0 means we're generating a new one
            if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
                bool hadInputSeed = true;
                std::string inputSeed = CVarGetString("gRando.InputSeed", "");
                if (inputSeed.empty()) {
                    inputSeed = std::to_string(Ship_Random(0, 1000000));
                    hadInputSeed = false;
                }

                uint32_t finalSeed = boost::hash_32<std::string>{}(inputSeed);
                Ship_Random_Seed(finalSeed);

                // Persist options to the save
                gSaveContext.save.shipSaveInfo.rando.finalSeed = finalSeed;
                for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                    RANDO_SAVE_OPTIONS[randoOptionId] =
                        (uint32_t)CVarGetInteger(randoStaticOption.cvar, randoStaticOption.defaultValue);
                }

                if (RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] != 3) {
                    gSaveContext.save.saveInfo.playerData.healthCapacity =
                        gSaveContext.save.saveInfo.playerData.health = RANDO_SAVE_OPTIONS[RO_STARTING_HEALTH] * 0x10;
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
                        RI_GREAT_BAY_COMPASS,     RI_GREAT_BAY_MAP,          RI_SNOWHEAD_COMPASS,
                        RI_SNOWHEAD_MAP,          RI_STONE_TOWER_COMPASS,    RI_STONE_TOWER_MAP,
                        RI_TINGLE_MAP_CLOCK_TOWN, RI_TINGLE_MAP_GREAT_BAY,   RI_TINGLE_MAP_ROMANI_RANCH,
                        RI_TINGLE_MAP_SNOWHEAD,   RI_TINGLE_MAP_STONE_TOWER, RI_TINGLE_MAP_WOODFALL,
                        RI_WOODFALL_COMPASS,      RI_WOODFALL_MAP,
                    };

                    for (RandoItemId itemId : MapsAndCompasses) {
                        startingItems.push_back(itemId);
                    }
                }

                std::unordered_map<RandoCheckId, bool> checkPool;
                std::vector<RandoItemId> itemPool;

                // First loop through all regions and add checks/items to the pool
                for (auto& [randoRegionId, randoRegion] : Rando::Logic::Regions) {
                    for (auto& [randoCheckId, _] : randoRegion.checks) {
                        auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

                        // Initialize the check with it's vanilla item
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
                        }

                        // Skip checks that are already in the pool
                        if (checkPool.find(randoCheckId) != checkPool.end()) {
                            continue;
                        }

                        // TODO: We may never shuffle these 2 pots, leaving this decision for later
                        if (randoStaticCheck.sceneId == SCENE_LAST_BS) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_OWL &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_OWL_STATUES] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_POT &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_POT_DROPS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_CRATE &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_CRATE_DROPS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_BARREL &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_BARREL_DROPS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_GRASS &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_GRASS_DROPS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_FREESTANDING &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_FREESTANDING_ITEMS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SNOWBALL &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_SNOWBALL_DROPS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_REMAINS &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_REMAINS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_COW &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_COWS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_TINGLE_SHOP &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE_SHOPS] == RO_GENERIC_NO) {
                            continue;
                        } else {
                            int price = Ship_Random(0, 200);
                            RANDO_SAVE_CHECKS[randoCheckId].price = price;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                            // We always want shuffle RC_CURIOSITY_SHOP_SPECIAL_ITEM &
                            // RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM
                            if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO &&
                                randoCheckId != RC_CURIOSITY_SHOP_SPECIAL_ITEM &&
                                randoCheckId != RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) {
                                continue;
                            } else {
                                // We may come up with a better solution for this in the future, but for now we choose a
                                // random price ahead of time, logic will account for whatever price we choose
                                int price = Ship_Random(0, 200);
                                RANDO_SAVE_CHECKS[randoCheckId].price = price;
                            }
                        }

                        checkPool.insert({ randoCheckId, true });
                        itemPool.push_back(randoStaticCheck.randoItemId);
                    }
                }

                // Add sword and shield to the pool because they don't have a vanilla location, if you are starting with
                // them they will be removed from the pool in the next step
                itemPool.push_back(RI_PROGRESSIVE_SWORD);
                itemPool.push_back(RI_SHIELD_HERO);

                // Add other items that don't have a vanilla location like Sun's Song or Song of Double Time
                if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_SOULS] == RO_GENERIC_YES) {
                    for (int i = RI_SOUL_GOHT; i <= RI_SOUL_TWINMOLD; i++) {
                        itemPool.push_back((RandoItemId)i);
                    }
                }

                // Remove starting items from the pool (but only one per entry in startingItems)
                for (RandoItemId startingItem : startingItems) {
                    auto it = std::find(itemPool.begin(), itemPool.end(), startingItem);
                    if (it != itemPool.end()) {
                        itemPool.erase(it);
                    }
                }

                if (RANDO_SAVE_OPTIONS[RO_PLENTIFUL_ITEMS] == RO_GENERIC_YES) {
                    int replaceableItems = 0;
                    std::vector<RandoItemId> plentifulItems;
                    std::vector<RandoItemId> potentialPlentifulItems;
                    for (size_t i = 0; i < itemPool.size(); i++) {
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

                if (checkPool.empty()) {
                    throw std::runtime_error("No checks in logic");
                }
                if (itemPool.empty()) {
                    throw std::runtime_error("No items in logic");
                }

                int heartPiecesRemoved = 0;
                // Add/Remove junk items to/from the pool to make the item pool size match the check pool size
                while (checkPool.size() != itemPool.size()) {
                    if (checkPool.size() > itemPool.size()) {
                        itemPool.push_back(RI_JUNK);
                    } else {
                        // First, remove junk items if we have any
                        bool removedJunk = false;
                        for (int i = 0; i < itemPool.size(); i++) {
                            if (Rando::StaticData::Items[itemPool[i]].randoItemType == RITYPE_JUNK) {
                                itemPool.erase(itemPool.begin() + i);
                                removedJunk = true;
                                break;
                            }
                        }
                        if (removedJunk) {
                            continue;
                        }

                        // Next replace 4 heart pieces with a heart container
                        bool removedHeartPieces = false;
                        for (int i = 0; i < itemPool.size(); i++) {
                            if (Rando::StaticData::Items[itemPool[i]].randoItemId == RI_HEART_PIECE) {
                                if (heartPiecesRemoved == 0) {
                                    itemPool[i] = RI_HEART_CONTAINER;
                                } else {
                                    itemPool.erase(itemPool.begin() + i);
                                }

                                removedHeartPieces = true;
                                heartPiecesRemoved++;
                                if (heartPiecesRemoved == 4) {
                                    heartPiecesRemoved = 0;
                                }
                                break;
                            }
                        }

                        if (removedHeartPieces) {
                            continue;
                        }

                        SPDLOG_ERROR("Could not match item pool size to check pool size {}/{}", itemPool.size(),
                                     checkPool.size());
                        throw std::runtime_error("Could not match item pool size to check pool size");
                    }
                }

                // Grant the starting items
                for (RandoItemId startingItem : startingItems) {
                    GiveItem(ConvertItem(startingItem));
                }

                if (RANDO_SAVE_OPTIONS[RO_STARTING_RUPEES]) {
                    gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
                }

                if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_VANILLA) {
                    GiveItem(RI_SWORD_KOKIRI);
                    GiveItem(RI_SHIELD_HERO);

                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
                            RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
                        }
                    }
                } else if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_NO_LOGIC) {
                    Rando::Logic::ApplyNoLogicToSaveContext(checkPool, itemPool);
                } else if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_NEARLY_NO_LOGIC) {
                    Rando::Logic::ApplyNearlyNoLogicToSaveContext(checkPool, itemPool);
                } else if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_GLITCHLESS) {
                    Rando::Logic::ApplyGlitchlessLogicToSaveContext(checkPool, itemPool);
                } else if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_FRENCH_VANILLA) {
                    Rando::Logic::ApplyFrenchVanillaLogicToSaveContext(checkPool, itemPool);
                } else {
                    throw std::runtime_error("Logic option not implemented: " +
                                             std::to_string(RANDO_SAVE_OPTIONS[RO_LOGIC]));
                }

                if (CVarGetInteger("gRando.GenerateSpoiler", 0)) {
                    nlohmann::json spoiler = Rando::Spoiler::GenerateFromSaveContext();
                    spoiler["inputSeed"] = inputSeed;

                    std::string fileName = inputSeed + ".json";
                    Rando::Spoiler::SaveToFile(fileName, spoiler);

                    if (hadInputSeed) {
                        CVarSetString("gRando.SpoilerFile", fileName.c_str());
                    }
                    Rando::Spoiler::RefreshOptions();
                }

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            } else {
                std::string fileName = CVarGetString("gRando.SpoilerFile", "");
                nlohmann::json spoiler = Rando::Spoiler::LoadFromFile(fileName);

                Rando::Spoiler::ApplyToSaveContext(spoiler);

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            }

            RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].eligible = true;
            RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].eligible = true;

        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error with randomizer save creation: {}", e.what());
            Audio_PlaySfx(NA_SE_SY_QUIZ_INCORRECT);
            gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_VANILLA;
            char invalidName[8] = { 18, 23, 31, 10, 21, 18, 13, 62 };
            memcpy(gSaveContext.save.saveInfo.playerData.playerName, invalidName, sizeof(invalidName));
            gSaveContext.save.saveInfo.playerData.newf[0] = '\0';
        }
    }
}
