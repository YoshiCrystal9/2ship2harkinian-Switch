#include "ItemTracker.h"
#include "ItemTrackerSettings.h"

#include "2s2h/BenGui/UIWidgets.hpp"
#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "Rando/MiscBehavior/ClockShuffle.h"

#include "2s2h/ShipUtils.h"
#include <spdlog/fmt/fmt.h>

extern "C" {
#include "z64save.h"
#include "variables.h"
#include <macros.h>
#include <functions.h>
#include "2s2h_assets.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
}

namespace BenGui {
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
}

#define FORMAT_COUNT "{}/{}"

ImVec4 trackerWindowOpacity = ImVec4(0, 0, 0, 0.5f);

std::vector<ImVec4> dungeonKeyColors = {
    { 0.9f, 0.33f, 0.56f, 0.4f },
    { 0.1f, 0.54f, 0.16f, 0.4f },
    { 0.61f, 0.04f, 0.86f, 0.4f },
    { 0.58f, 0.65f, 0.15f, 0.4f },
};

extern TrackerImageObject GetTextureObject(int16_t itemId, bool isRandoItem) {
    int16_t currentItemId = ITEM_NONE;
    int16_t bottleId = 0;
    bool itemObtained = false;

    if (isRandoItem) {
        TrackerImageObject randoImageObject;
        randoImageObject.textureColor = Ship_GetRandoItemColorTint(itemId);

        switch (itemId) {
            case RI_FROG_BLUE:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_33_01);
                break;
            case RI_FROG_CYAN:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_32_40);
                break;
            case RI_FROG_PINK:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_32_80);
                break;
            case RI_FROG_WHITE:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_33_02);
                break;
            case RI_OWL_CLOCK_TOWN_SOUTH:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_CLOCK_TOWN);
                break;
            case RI_OWL_GREAT_BAY_COAST:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_GREAT_BAY_COAST);
                break;
            case RI_OWL_IKANA_CANYON:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_IKANA_CANYON);
                break;
            case RI_OWL_MILK_ROAD:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_MILK_ROAD);
                break;
            case RI_OWL_MOUNTAIN_VILLAGE:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_MOUNTAIN_VILLAGE);
                break;
            case RI_OWL_SNOWHEAD:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_SNOWHEAD);
                break;
            case RI_OWL_SOUTHERN_SWAMP:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_SOUTHERN_SWAMP);
                break;
            case RI_OWL_STONE_TOWER:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_STONE_TOWER);
                break;
            case RI_OWL_WOODFALL:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_WOODFALL);
                break;
            case RI_OWL_ZORA_CAPE:
                itemObtained = GET_OWL_STATUE_ACTIVATED(OWL_WARP_ZORA_CAPE);
                break;
            case RI_SOUL_BOSS_GOHT:
            case RI_SOUL_BOSS_GYORG:
            case RI_SOUL_BOSS_MAJORA:
            case RI_SOUL_BOSS_ODOLWA:
            case RI_SOUL_BOSS_TWINMOLD:
            case RI_SOUL_ENEMY_ALIEN:
            case RI_SOUL_ENEMY_ARMOS:
            case RI_SOUL_ENEMY_BAD_BAT:
            case RI_SOUL_ENEMY_BEAMOS:
            case RI_SOUL_ENEMY_BOE:
            case RI_SOUL_ENEMY_BUBBLE:
            case RI_SOUL_ENEMY_CAPTAIN_KEETA:
            case RI_SOUL_ENEMY_CHUCHU:
            case RI_SOUL_ENEMY_DEATH_ARMOS:
            case RI_SOUL_ENEMY_DEEP_PYTHON:
            case RI_SOUL_ENEMY_DEKU_BABA:
            case RI_SOUL_ENEMY_DEXIHAND:
            case RI_SOUL_ENEMY_DINOLFOS:
            case RI_SOUL_ENEMY_DODONGO:
            case RI_SOUL_ENEMY_DRAGONFLY:
            case RI_SOUL_ENEMY_EENO:
            case RI_SOUL_ENEMY_EYEGORE:
            case RI_SOUL_ENEMY_FREEZARD:
            case RI_SOUL_ENEMY_GARO:
            case RI_SOUL_ENEMY_GEKKO:
            case RI_SOUL_ENEMY_GIANT_BEE:
            case RI_SOUL_ENEMY_GOMESS:
            case RI_SOUL_ENEMY_GUAY:
            case RI_SOUL_ENEMY_HIPLOOP:
            case RI_SOUL_ENEMY_IGOS_DU_IKANA:
            case RI_SOUL_ENEMY_IRON_KNUCKLE:
            case RI_SOUL_ENEMY_KEESE:
            case RI_SOUL_ENEMY_LEEVER:
            case RI_SOUL_ENEMY_LIKE_LIKE:
            case RI_SOUL_ENEMY_MAD_SCRUB:
            case RI_SOUL_ENEMY_NEJIRON:
            case RI_SOUL_ENEMY_OCTOROK:
            case RI_SOUL_ENEMY_PEAHAT:
            case RI_SOUL_ENEMY_PIRATE:
            case RI_SOUL_ENEMY_POE:
            case RI_SOUL_ENEMY_REDEAD:
            case RI_SOUL_ENEMY_SHELLBLADE:
            case RI_SOUL_ENEMY_SKULLFISH:
            case RI_SOUL_ENEMY_SKULLTULA:
            case RI_SOUL_ENEMY_SNAPPER:
            case RI_SOUL_ENEMY_STALCHILD:
            case RI_SOUL_ENEMY_TAKKURI:
            case RI_SOUL_ENEMY_TEKTITE:
            case RI_SOUL_ENEMY_WALLMASTER:
            case RI_SOUL_ENEMY_WART:
            case RI_SOUL_ENEMY_WIZROBE:
            case RI_SOUL_ENEMY_WOLFOS:
                itemObtained = Flags_GetRandoInf(SOUL_RI_TO_RANDO_INF(itemId));
                break;
            case RI_TINGLE_MAP_CLOCK_TOWN:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
                break;
            case RI_TINGLE_MAP_WOODFALL:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
                break;
            case RI_TINGLE_MAP_SNOWHEAD:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
                break;
            case RI_TINGLE_MAP_ROMANI_RANCH:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
                break;
            case RI_TINGLE_MAP_GREAT_BAY:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
                break;
            case RI_TINGLE_MAP_STONE_TOWER:
                itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
                break;
            case RI_TIME_DAY_1:
            case RI_TIME_DAY_2:
            case RI_TIME_DAY_3:
                randoImageObject.textureColor = ImVec4(1.0f, 0.9f, 0.3f, 1.0f); // Yellow/gold for sun
                itemObtained = Flags_GetRandoInf(
                    static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 +
                                          Rando::ClockItems::GetHalfDayIndexFromClockItem((RandoItemId)itemId)));
                break;
            case RI_TIME_NIGHT_1:
            case RI_TIME_NIGHT_2:
            case RI_TIME_NIGHT_3:
                randoImageObject.textureColor = ImVec4(0.5f, 0.7f, 1.0f, 1.0f); // Light blue for moon
                itemObtained = Flags_GetRandoInf(
                    static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 +
                                          Rando::ClockItems::GetHalfDayIndexFromClockItem((RandoItemId)itemId)));
                break;
            case RI_TRIFORCE_PIECE:
                itemObtained = gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces > 0;
                break;
            case RI_OCARINA_BUTTON_A:
            case RI_OCARINA_BUTTON_C_DOWN:
            case RI_OCARINA_BUTTON_C_LEFT:
            case RI_OCARINA_BUTTON_C_RIGHT:
            case RI_OCARINA_BUTTON_C_UP:
                itemObtained = Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A + (itemId - RI_OCARINA_BUTTON_A));
                break;
            default:
                break;
        }
        randoImageObject.textureColor.w = itemObtained ? 1.0f : 0.4f;

        randoImageObject.textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
            (const char*)Rando::StaticData::GetIconTexturePath((RandoItemId)itemId));
        randoImageObject.textureDimensions =
            ImVec2(ITEM_TEXTURE_SIZE,
                   itemId >= RI_OWL_CLOCK_TOWN_SOUTH && itemId <= RI_OWL_ZORA_CAPE ? 32.0f : ITEM_TEXTURE_SIZE);

        return randoImageObject;
    } else {
        if (itemId >= ITEM_BOTTLE_1 && itemId <= ITEM_BOTTLE_6) {
            bottleId = SLOT_BOTTLE_1 + (itemId - ITEM_BOTTLE_1);
            currentItemId = ITEM_BOTTLE;
        }
        if (itemId == ITEM_SKULL_TOKEN_SWAMP || itemId == ITEM_SKULL_TOKEN_OCEAN) {
            currentItemId = ITEM_SKULL_TOKEN;
        }
        if (itemId >= ITEM_CLOCK_TOWN_STRAY_FAIRY && itemId <= ITEM_STONE_TOWER_STRAY_FAIRY) {
            currentItemId = ITEM_STRAY_FAIRIES;
        }
        if (itemId >= ITEM_WOODFALL_DUNGEON_MAP && itemId <= ITEM_STONE_TOWER_KEY_BOSS) {
            const int dungeonIndex = (itemId - ITEM_WOODFALL_DUNGEON_MAP) / 4;
            const int itemTypeIndex = (itemId - ITEM_WOODFALL_DUNGEON_MAP) % 4;

            switch (itemTypeIndex) {
                case 0:
                    currentItemId = ITEM_DUNGEON_MAP;
                    break;
                case 1:
                    currentItemId = ITEM_COMPASS;
                    break;
                case 2:
                    currentItemId = ITEM_KEY_SMALL;
                    break;
                case 3:
                    currentItemId = ITEM_KEY_BOSS;
                    break;
            }
        }
    }

    if (currentItemId == ITEM_NONE) {
        switch (itemId) {
            case ITEM_MOONS_TEAR:
                currentItemId = ITEM_MOONS_TEAR;
                for (int16_t i = ITEM_MOONS_TEAR; i <= ITEM_DEED_OCEAN; i++) {
                    if (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED] == i) {
                        currentItemId = i;
                        break;
                    }
                }
                break;
            case ITEM_ROOM_KEY:
                currentItemId = ITEM_ROOM_KEY;
                for (int16_t i = ITEM_ROOM_KEY; i <= ITEM_LETTER_MAMA; i++) {
                    if (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA] == i) {
                        currentItemId = i;
                        break;
                    }
                }
                break;
            case ITEM_LETTER_TO_KAFEI:
                currentItemId = ITEM_LETTER_TO_KAFEI;
                for (int16_t i = ITEM_LETTER_TO_KAFEI; i <= ITEM_PENDANT_OF_MEMORIES; i++) {
                    if (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE] == i) {
                        currentItemId = i;
                        break;
                    }
                }
                break;
            case ITEM_SWORD_KOKIRI:
                currentItemId = ITEM_SWORD_KOKIRI + (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI);
                if (currentItemId < ITEM_SWORD_KOKIRI) {
                    currentItemId = ITEM_SWORD_KOKIRI;
                }
                break;
            case ITEM_SHIELD_HERO:
                currentItemId = ITEM_SHIELD_HERO + (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) - EQUIP_VALUE_SHIELD_HERO);
                if (currentItemId < ITEM_SHIELD_HERO) {
                    currentItemId = ITEM_SHIELD_HERO;
                }
                break;
            case ITEM_WALLET_ADULT:
                currentItemId = ITEM_WALLET_ADULT + CUR_UPG_VALUE(UPG_WALLET) - 1;
                if (currentItemId < ITEM_WALLET_ADULT) {
                    currentItemId = ITEM_WALLET_ADULT;
                }
                break;
            case ITEM_MAGIC_JAR_SMALL:
                if (gSaveContext.save.saveInfo.playerData.magicLevel <= 1) {
                    currentItemId = ITEM_MAGIC_JAR_SMALL;
                } else {
                    currentItemId = ITEM_MAGIC_JAR_BIG;
                }
                break;
            default:
                currentItemId = itemId;
                break;
        }
    }

    TrackerImageObject imageObject = {
        .textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
            (const char*)gItemIcons[currentItemId]),
        .textureColor = Ship_GetItemColorTint(currentItemId),
        .textureDimensions =
            ImVec2(currentItemId >= ITEM_SONG_SONATA && currentItemId <= ITEM_SONG_SUN ? ITEM_TEXTURE_SIZE / 1.5f
                                                                                       : ITEM_TEXTURE_SIZE,
                   ITEM_TEXTURE_SIZE),
    };

    if (itemId >= ITEM_REMAINS_ODOLWA && itemId <= ITEM_BOMBERS_NOTEBOOK) {
        itemObtained = CHECK_QUEST_ITEM(Ship_ConvertItemIdToQuest(itemId));
    } else if (itemId >= ITEM_SWORD_KOKIRI && itemId <= ITEM_SWORD_GILDED) {
        itemObtained = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI;
    } else if (itemId == ITEM_SHIELD_HERO || itemId == ITEM_SHIELD_MIRROR) {
        itemObtained = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) >= EQUIP_VALUE_SHIELD_HERO;
    } else if (itemId == ITEM_WALLET_ADULT || itemId == ITEM_WALLET_GIANT) {
        itemObtained = CUR_UPG_VALUE(UPG_WALLET) >= 1;
    } else if (itemId == ITEM_MAGIC_JAR_SMALL || itemId == ITEM_MAGIC_JAR_BIG) {
        itemObtained = gSaveContext.save.saveInfo.playerData.magicLevel != 0;
    } else if (itemId == ITEM_HEART_CONTAINER) {
        itemObtained = gSaveContext.save.saveInfo.playerData.doubleDefense;
    } else if (itemId >= ITEM_BOTTLE_1 && itemId <= ITEM_BOTTLE_6) {
        if (gSaveContext.save.saveInfo.inventory.items[bottleId] != ITEM_NONE && gPlayState) {
            imageObject.textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                (const char*)gItemIcons[gSaveContext.save.saveInfo.inventory.items[bottleId]]);
        } else {
            imageObject.textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                (const char*)gItemIcons[ITEM_BOTTLE]);
        }
        itemObtained = gSaveContext.save.saveInfo.inventory.items[bottleId] != ITEM_NONE;
    } else if (itemId == ITEM_SKULL_TOKEN_SWAMP || itemId == ITEM_SKULL_TOKEN_OCEAN) {
        uint32_t tokenCount = 0;
        if (itemId == ITEM_SKULL_TOKEN_SWAMP) {
            tokenCount = Inventory_GetSkullTokenCount(SCENE_KINSTA1);
        } else {
            tokenCount = Inventory_GetSkullTokenCount(SCENE_KINDAN2);
        }
        itemObtained = tokenCount > 0;
    } else if (itemId >= ITEM_CLOCK_TOWN_STRAY_FAIRY && itemId <= ITEM_STONE_TOWER_STRAY_FAIRY) {
        if (itemId == ITEM_CLOCK_TOWN_STRAY_FAIRY) {
            imageObject.textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                (const char*)fairyIconTextures[0]);
            imageObject.textureColor = ImVec4(1.0f, 0.9f, 0.5f, 0.4f);
            itemObtained = CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80);
        } else {
            imageObject.textureId = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                (const char*)fairyIconTextures[itemId - ITEM_WOODFALL_STRAY_FAIRY]);
            itemObtained = gSaveContext.save.saveInfo.inventory.strayFairies[itemId - ITEM_WOODFALL_STRAY_FAIRY] > 0;
        }
    } else if (itemId >= ITEM_WOODFALL_DUNGEON_MAP && itemId <= ITEM_STONE_TOWER_KEY_BOSS) {
        const int dungeonIndex = (itemId - ITEM_WOODFALL_DUNGEON_MAP) / 4;
        const int itemTypeIndex = (itemId - ITEM_WOODFALL_DUNGEON_MAP) % 4;
        switch (itemTypeIndex) {
            case 0:
                itemObtained = CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, dungeonIndex);
                break;
            case 1:
                itemObtained = CHECK_DUNGEON_ITEM(DUNGEON_MAP, dungeonIndex);
                break;
            case 2:
                imageObject.textureColor = dungeonKeyColors[dungeonIndex];
                itemObtained = DUNGEON_KEY_COUNT(dungeonIndex) > 0;
                break;
            case 3:
                imageObject.textureColor = dungeonKeyColors[dungeonIndex];
                itemObtained = CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, dungeonIndex);
                break;
        }
    } else {
        itemObtained = INV_CONTENT(itemId) != ITEM_NONE;
    }
    imageObject.textureColor.w = itemObtained ? 1.0f : 0.4f;

    return imageObject;
}

std::string GetItemCounts(int16_t itemId, bool isRandoItem) {
    std::string countStr = "";
    if (isRandoItem) {
        if (itemId == RI_TRIFORCE_PIECE) {
            int16_t maxPieces = gSaveContext.save.shipSaveInfo.rando.randoSaveOptions[RO_TRIFORCE_PIECES_REQUIRED];
            countStr = fmt::format(FORMAT_COUNT, gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces,
                                   maxPieces == 1000 ? "1k" : std::to_string(maxPieces));
        }
    } else {
        int dungeonIndex = 0;
        switch (itemId) {
            case ITEM_BOW:
            case ITEM_BOMB:
            case ITEM_BOMBCHU:
            case ITEM_DEKU_STICK:
            case ITEM_DEKU_NUT:
            case ITEM_MAGIC_BEANS:
            case ITEM_POWDER_KEG:
                countStr = std::to_string(AMMO(itemId));
                break;
            case ITEM_WALLET_ADULT:
                countStr = std::to_string(gSaveContext.save.saveInfo.playerData.rupees);
                break;
            case ITEM_PICTOGRAPH_BOX:
                countStr = CHECK_QUEST_ITEM(QUEST_PICTOGRAPH) ? "1" : "0";
                break;
            case ITEM_SKULL_TOKEN_SWAMP:
            case ITEM_SKULL_TOKEN_OCEAN:
                countStr = fmt::format(
                    FORMAT_COUNT,
                    Inventory_GetSkullTokenCount(itemId == ITEM_SKULL_TOKEN_SWAMP ? SCENE_KINSTA1 : SCENE_KINDAN2),
                    IS_RANDO ? RANDO_SAVE_OPTIONS[RO_MINIMUM_SKULLTULA_TOKENS] : SPIDER_HOUSE_TOKENS_REQUIRED);
                break;
            case ITEM_WOODFALL_STRAY_FAIRY:
            case ITEM_SNOWHEAD_STRAY_FAIRY:
            case ITEM_GREAT_BAY_STRAY_FAIRY:
            case ITEM_STONE_TOWER_STRAY_FAIRY:
                countStr = fmt::format(
                    FORMAT_COUNT, gSaveContext.save.saveInfo.inventory.strayFairies[itemId - ITEM_WOODFALL_STRAY_FAIRY],
                    IS_RANDO ? RANDO_SAVE_OPTIONS[RO_MINIMUM_STRAY_FAIRIES] : STRAY_FAIRY_SCATTERED_TOTAL);
                break;
            case ITEM_WOODFALL_KEY_SMALL:
            case ITEM_SNOWHEAD_KEY_SMALL:
            case ITEM_GREAT_BAY_KEY_SMALL:
            case ITEM_STONE_TOWER_KEY_SMALL:
                dungeonIndex = (itemId - ITEM_WOODFALL_DUNGEON_MAP) / 4;
                countStr = DUNGEON_KEY_COUNT(dungeonIndex) < 0 ? "0" : std::to_string(DUNGEON_KEY_COUNT(dungeonIndex));
                break;
            default:
                break;
        }
    }
    return countStr;
}

void DrawItemCounts(int16_t itemId, bool isRandoItem, ImVec2 textureSize, float scale, ImVec2 currentPos) {
    std::string itemCount = GetItemCounts(itemId, isRandoItem);

    if (itemCount.empty()) {
        return;
    }
    ImVec2 textSize = ImGui::CalcTextSize(itemCount.c_str());

    ImVec2 textPos =
        ImVec2(currentPos.x + textureSize.x - textSize.x - 2.0f, currentPos.y + textureSize.y - textSize.y - 2.0f);
    ImGui::SetCursorPos(textPos);
    ImGui::SetWindowFontScale(scale);
    ImGui::Text(itemCount.c_str());
}

void DrawItemSlot(int16_t itemId, float scale, bool isRandoItem) {
    ImVec2 currentPos = ImGui::GetCursorPos();
    TrackerImageObject imageObject = GetTextureObject(itemId, isRandoItem);
    ImGui::Image(imageObject.textureId,
                 ImVec2(imageObject.textureDimensions.x * scale, imageObject.textureDimensions.y * scale), ImVec2(0, 0),
                 ImVec2(1, 1), imageObject.textureColor, ImVec4(0, 0, 0, 0));
    UIWidgets::Tooltip(GetItemTrackerItemName(itemId, isRandoItem).c_str());
    if (CVarGetInteger("gSettings.ItemTracker.ItemCounts", 0)) {
        DrawItemCounts(itemId, isRandoItem, imageObject.textureDimensions * scale, scale, currentPos);
    }
}

void DrawItemWindowList(TrackerItemListObject windowObject, bool isRandoItem) {
    int columns = windowObject.columnLength;
    if (windowObject.itemList.size() < windowObject.columnLength) {
        columns = windowObject.itemList.size();
    }

    if (ImGui::BeginTable(windowObject.windowName.c_str(), columns)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        for (auto& item : windowObject.itemList) {
            ImGui::TableNextColumn();
            ImVec2 framePadding = ImVec2(item >= ITEM_SONG_SONATA && item <= ITEM_SONG_SUN ? ITEM_SONG_PADDING : 0, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);
            DrawItemSlot(item, windowObject.windowScale, isRandoItem);
            ImGui::PopStyleVar(1);
        }

        ImGui::PopStyleVar(1);
        ImGui::EndTable();
    }
}

void ItemTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }

    if (!gPlayState) {
        return;
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav |
                                   ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

    if (!CVarGetInteger("gSettings.ItemTracker.WindowType", 0)) {
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking;
    }
    shouldWindowSplit = CVarGetInteger("gSettings.ItemTracker.WindowGroup", 0);

    std::vector<std::vector<TrackerItemListObject>*> itemTrackerWindows = {
        &BenGui::mItemTrackerWindow->namedItemWindows,
        &BenGui::mItemTrackerWindow->randoItemWindows,
    };

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    uint32_t windowIndex = TRACKER_MAIN;
    for (auto* window : itemTrackerWindows) {
        if (!window->empty()) {
            bool singleWindowOpen = false;
            if (!shouldWindowSplit) {
                ImVec4 mainBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
                mainBg.w = (*window)[0].windowOpacity;
                ImGui::PushStyleColor(ImGuiCol_WindowBg, mainBg);
                singleWindowOpen =
                    ImGui::Begin(windowIndex == TRACKER_MAIN ? "Main Tracker" : "Rando Tracker", nullptr, windowFlags);
            }

            uint32_t index = 0;
            for (auto& object : *window) {
                if (object.itemList.empty()) {
                    index++;
                    continue;
                }

                bool isWindowOpen = false;

                if (shouldWindowSplit) {
                    ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
                    bg.w = object.windowOpacity;
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
                    std::string windowName = std::string(object.windowName) + "##" + std::to_string(index);
                    isWindowOpen = ImGui::Begin(windowName.c_str(), nullptr, windowFlags);
                } else {
                    isWindowOpen = singleWindowOpen;
                }

                if (isWindowOpen) {
                    DrawItemWindowList(object, windowIndex == TRACKER_MAIN ? false : true);
                }

                if (shouldWindowSplit) {
                    ImGui::PopStyleColor(1);
                    ImGui::End();
                }

                index++;
            }
            if (!shouldWindowSplit) {
                ImGui::PopStyleColor(1);
                ImGui::End();
            }
        }
        windowIndex++;
    }

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(1);
}

void ItemTrackerWindow::InitElement() {
}

void ItemTrackerWindow::DrawElement() {
}
