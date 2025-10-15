#include "Rando/Rando.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

void Rando::GiveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]++;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]++;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]++;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]++;
            break;
        case RI_GREAT_SPIN_ATTACK:
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = true;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
            break;
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            SET_WEEKEVENTREG(WEEKEVENTREG_12_80);
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            SET_WEEKEVENTREG(WEEKEVENTREG_12_80);
            break;
        // Don't love this because it doesn't hit GameInteractor_ExecuteOnItemGive()
        // but not sure how else to account for collecting outside of dungeon
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_MAP:
        case RI_WOODFALL_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]++;
            }
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_MAP:
        case RI_SNOWHEAD_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]++;
            }
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_MAP:
        case RI_GREAT_BAY_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]++;
            }
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_MAP:
        case RI_STONE_TOWER_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]++;
            }
            break;
        // Technically these should never be used, but leaving them here just in case
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
        case RI_PROGRESSIVE_LULLABY:
        case RI_PROGRESSIVE_SWORD:
        case RI_PROGRESSIVE_WALLET:
            Rando::GiveItem(Rando::ConvertItem(randoItemId));
            break;
        case RI_BOMB_BAG_20:
        case RI_BOMB_BAG_30:
        case RI_BOMB_BAG_40:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
            break;
        case RI_WALLET_ADULT:
        case RI_WALLET_GIANT:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            // Fill Rupees to max, this may be opt-in later
            gSaveContext.rupeeAccumulator = CUR_CAPACITY(UPG_WALLET);
            break;
        case RI_GS_TOKEN_SWAMP:
            // Set QUEST_QUIVER to match bug mentioned in z_parameter.c
            SET_QUEST_ITEM(QUEST_QUIVER);
            Inventory_IncrementSkullTokenCount(SCENE_KINSTA1);
            break;
        case RI_GS_TOKEN_OCEAN:
            SET_QUEST_ITEM(QUEST_QUIVER);
            Inventory_IncrementSkullTokenCount(SCENE_KINDAN2);
            break;
        case RI_MOONS_TEAR:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_LAND:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_SWAMP:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_MOUNTAIN:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_OCEAN:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_ROOM_KEY:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_LETTER_TO_MAMA:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_LETTER_TO_KAFEI:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_PENDANT_OF_MEMORIES:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_POWDER_KEG:
            Flags_SetWeekEventReg(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_SWORD_GILDED:
        case RI_SWORD_KOKIRI:
        case RI_SWORD_RAZOR:
            if (STOLEN_ITEM_1 == ITEM_SWORD_KOKIRI || STOLEN_ITEM_1 == ITEM_SWORD_RAZOR) {
                SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
            }
            if (STOLEN_ITEM_2 == ITEM_SWORD_KOKIRI || STOLEN_ITEM_2 == ITEM_SWORD_RAZOR) {
                SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);
            }

            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_TINGLE_MAP_CLOCK_TOWN:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_CLOCK_TOWN);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
            break;
        case RI_TINGLE_MAP_WOODFALL:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_WOODFALL);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
            break;
        case RI_TINGLE_MAP_SNOWHEAD:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_SNOWHEAD);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
            break;
        case RI_TINGLE_MAP_ROMANI_RANCH:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_ROMANI_RANCH);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
            break;
        case RI_TINGLE_MAP_GREAT_BAY:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_GREAT_BAY);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
            break;
        case RI_TINGLE_MAP_STONE_TOWER:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_STONE_TOWER);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
            break;
        case RI_OWL_CLOCK_TOWN_SOUTH:
            Sram_ActivateOwl(OWL_WARP_CLOCK_TOWN);
            break;
        case RI_OWL_GREAT_BAY_COAST:
            Sram_ActivateOwl(OWL_WARP_GREAT_BAY_COAST);
            break;
        case RI_OWL_IKANA_CANYON:
            Sram_ActivateOwl(OWL_WARP_IKANA_CANYON);
            break;
        case RI_OWL_MILK_ROAD:
            Sram_ActivateOwl(OWL_WARP_MILK_ROAD);
            break;
        case RI_OWL_MOUNTAIN_VILLAGE:
            Sram_ActivateOwl(OWL_WARP_MOUNTAIN_VILLAGE);
            break;
        case RI_OWL_SNOWHEAD:
            Sram_ActivateOwl(OWL_WARP_SNOWHEAD);
            break;
        case RI_OWL_SOUTHERN_SWAMP:
            Sram_ActivateOwl(OWL_WARP_SOUTHERN_SWAMP);
            break;
        case RI_OWL_STONE_TOWER:
            Sram_ActivateOwl(OWL_WARP_STONE_TOWER);
            break;
        case RI_OWL_WOODFALL:
            Sram_ActivateOwl(OWL_WARP_WOODFALL);
            break;
        case RI_OWL_ZORA_CAPE:
            Sram_ActivateOwl(OWL_WARP_ZORA_CAPE);
            break;
        case RI_HEART_CONTAINER:
        case RI_HEART_PIECE:
            gSaveContext.healthAccumulator = gSaveContext.save.saveInfo.playerData.healthCapacity + 0x10;
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_BOTTLE_RED_POTION:
            // ITEM_LONGSHOT will give a Red Potion bottle on the first available bottle slot
            // ITEM_POTION_RED will put a Red Potion bottle on the first bottle slot
            Item_Give(gPlayState, ITEM_LONGSHOT);
            break;
        case RI_SOUL_GOHT:
        case RI_SOUL_GYORG:
        case RI_SOUL_MAJORA:
        case RI_SOUL_ODOLWA:
        case RI_SOUL_TWINMOLD:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_GOHT + (randoItemId - RI_SOUL_GOHT));
            break;
        case RI_FROG_BLUE:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_01);
            break;
        case RI_FROG_CYAN:
            SET_WEEKEVENTREG(WEEKEVENTREG_32_40);
            break;
        case RI_FROG_PINK:
            SET_WEEKEVENTREG(WEEKEVENTREG_32_80);
            break;
        case RI_FROG_WHITE:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_02);
            break;
        case RI_ABILITY_SWIM:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_SWIM);
            break;
        case RI_JUNK:
        case RI_NONE:
            break;
        default:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
    }
}
