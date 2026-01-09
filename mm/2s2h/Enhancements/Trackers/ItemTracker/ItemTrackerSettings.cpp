#include "ItemTrackerSettings.h"
#include "ItemTracker.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "ShipUtils.h"
#include "ship/config/Config.h"

namespace BenGui {
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
}

void ItemTrackerSettingsWindow::UpdateElement() {
}

#define WIDGET_COLOR UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5))

static const char* windowTypes[2] = { "Floating", "Window" };

bool shouldWindowSplit = false;
int16_t popupSlot = SLOT_OCARINA;
int16_t indexToRemove = ITEM_OCARINA_OF_TIME;
static std::string trackerInputName;
bool shouldTrackerPopUpOpen = false;
bool shouldRemove = false;

std::vector<std::string> listOrder = {
    "Inventory", "Bottles", "Masks", "Songs", "Quest", "Tokens", "Stray Fairies", "Dungeon",
};

std::vector<std::string> randoListOrder = {
    "Frogs", "Ocarina Buttons", "Boss Souls", "Enemy Souls", "Owl Statues", "Time", "Tingle Maps", "Misc",
};

std::map<std::string, std::tuple<int16_t, int16_t, int16_t>> defaultItemLists = {
    { "Inventory", { ITEM_OCARINA_OF_TIME, ITEM_LETTER_TO_KAFEI, 6 } },
    { "Bottles", { ITEM_BOTTLE_1, ITEM_BOTTLE_6, 6 } },
    { "Masks", { ITEM_MASK_POSTMAN, ITEM_MASK_FIERCE_DEITY, 6 } },
    { "Songs", { ITEM_SONG_TIME, ITEM_SONG_OATH, 5 } },
    { "Quest", { ITEM_REMAINS_ODOLWA, ITEM_BOMBERS_NOTEBOOK, 4 } },
    { "Tokens", { ITEM_SKULL_TOKEN_SWAMP, ITEM_SKULL_TOKEN_OCEAN, 2 } },
    { "Stray Fairies", { ITEM_CLOCK_TOWN_STRAY_FAIRY, ITEM_STONE_TOWER_STRAY_FAIRY, 5 } },
    { "Dungeon", { ITEM_WOODFALL_DUNGEON_MAP, ITEM_STONE_TOWER_KEY_BOSS, 4 } },
};

std::map<std::string, std::tuple<int16_t, int16_t, int16_t>> randoItemLists = {
    { "Frogs", { RI_FROG_BLUE, RI_FROG_WHITE, 4 } },
    { "Ocarina Buttons", { RI_OCARINA_BUTTON_A, RI_OCARINA_BUTTON_C_UP, 5 } },
    { "Boss Souls", { RI_SOUL_BOSS_GOHT, RI_SOUL_BOSS_TWINMOLD, 5 } },
    { "Enemy Souls", { RI_SOUL_ENEMY_ALIEN, RI_SOUL_ENEMY_WOLFOS, 6 } },
    { "Owl Statues", { RI_OWL_CLOCK_TOWN_SOUTH, RI_OWL_ZORA_CAPE, 5 } },
    { "Tingle Maps", { RI_TINGLE_MAP_CLOCK_TOWN, RI_TINGLE_MAP_WOODFALL, 6 } },
    { "Time", { RI_TIME_DAY_1, RI_TIME_NIGHT_3, 6 } },
    { "Misc", { RI_TRIFORCE_PIECE, RI_TRIFORCE_PIECE, 1 } },
};

std::vector<std::string> dungeonPrefix = {
    "Woodfall",
    "Snowhead",
    "Great Bay",
    "Stone Tower",
};

std::pair<uint32_t, uint32_t> GetItemMapRange(uint32_t start, uint32_t end) {
    std::pair<uint32_t, uint32_t> indexRange;

    for (size_t i = 0; i < itemIdToItemNameMap.size(); i++) {
        if (itemIdToItemNameMap[i].first == start) {
            indexRange.first = static_cast<int>(i);
        }
        if (itemIdToItemNameMap[i].first == end) {
            indexRange.second = static_cast<int>(i);
        }
    }

    return indexRange;
}

std::string GetItemTrackerItemName(int16_t itemId, bool isRandoItem) {
    std::string itemName = "";
    uint16_t fallBackCheck = 0;
    if (isRandoItem) {
        itemName = Rando::StaticData::Items[(RandoItemId)itemId].name;
    } else {
        if (itemId >= ITEM_BOTTLE_1 && itemId <= ITEM_BOTTLE_6) {
            itemName = "Bottle";
        } else if (itemId == ITEM_SKULL_TOKEN_SWAMP) {
            itemName = "Swamp Token";
        } else if (itemId == ITEM_SKULL_TOKEN_OCEAN) {
            itemName = "Ocean Token";
        } else if (itemId == ITEM_CLOCK_TOWN_STRAY_FAIRY) {
            itemName = "Clock Town Stray Fairy";
        } else if (itemId >= ITEM_WOODFALL_STRAY_FAIRY && itemId <= ITEM_STONE_TOWER_STRAY_FAIRY) {
            itemName = dungeonPrefix[itemId - ITEM_WOODFALL_STRAY_FAIRY] + " Stray Fairy";
        } else if (itemId >= ITEM_WOODFALL_DUNGEON_MAP && itemId <= ITEM_STONE_TOWER_KEY_BOSS) {
            switch (itemId) {
                case ITEM_WOODFALL_DUNGEON_MAP:
                case ITEM_SNOWHEAD_DUNGEON_MAP:
                case ITEM_GREAT_BAY_DUNGEON_MAP:
                case ITEM_STONE_TOWER_DUNGEON_MAP:
                    // Note the Map and Compass are swapped in vanilla code, this is correcting that.
                    itemName = dungeonPrefix[(itemId - ITEM_WOODFALL_DUNGEON_MAP) / 4] + " Compass";
                    break;
                case ITEM_WOODFALL_DUNGEON_COMPASS:
                case ITEM_SNOWHEAD_DUNGEON_COMPASS:
                case ITEM_GREAT_BAY_DUNGEON_COMPASS:
                case ITEM_STONE_TOWER_DUNGEON_COMPASS:
                    itemName = dungeonPrefix[(itemId - ITEM_WOODFALL_DUNGEON_COMPASS) / 4] + " Map";
                    break;
                case ITEM_WOODFALL_KEY_SMALL:
                case ITEM_SNOWHEAD_KEY_SMALL:
                case ITEM_GREAT_BAY_KEY_SMALL:
                case ITEM_STONE_TOWER_KEY_SMALL:
                    itemName = dungeonPrefix[(itemId - ITEM_WOODFALL_KEY_SMALL) / 4] + " Small Key";
                    break;
                case ITEM_WOODFALL_KEY_BOSS:
                case ITEM_SNOWHEAD_KEY_BOSS:
                case ITEM_GREAT_BAY_KEY_BOSS:
                case ITEM_STONE_TOWER_KEY_BOSS:
                    itemName = dungeonPrefix[(itemId - ITEM_WOODFALL_KEY_BOSS) / 4] + " Boss Key";
                    break;
                default:
                    break;
            }
        } else if (itemId == ITEM_MAGIC_JAR_SMALL || itemId == ITEM_MAGIC_JAR_BIG) {
            itemName = gSaveContext.save.saveInfo.playerData.magicLevel > 1 ? "Double Magic" : "Single Magic";
        } else if (itemId == ITEM_HEART_CONTAINER) {
            itemName = "Double Defense";
        } else if (itemId >= ITEM_MOONS_TEAR && itemId <= ITEM_PENDANT_OF_MEMORIES) {
            itemName = Ship_GetItemNameById(gSaveContext.save.saveInfo.inventory.items[gItemSlots[itemId]]);
        } else if (itemId == ITEM_SWORD_KOKIRI) {
            fallBackCheck = ITEM_SWORD_KOKIRI + (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) - EQUIP_VALUE_SWORD_KOKIRI);
            itemName = Ship_GetItemNameById(fallBackCheck <= ITEM_SWORD_KOKIRI ? ITEM_SWORD_KOKIRI : fallBackCheck);
        } else if (itemId == ITEM_SHIELD_HERO) {
            fallBackCheck = ITEM_SHIELD_HERO + (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) - EQUIP_VALUE_SHIELD_HERO);
            itemName = Ship_GetItemNameById(fallBackCheck <= ITEM_SHIELD_HERO ? ITEM_SHIELD_HERO : fallBackCheck);
        } else if (itemId == ITEM_WALLET_ADULT) {
            fallBackCheck = ITEM_WALLET_ADULT + CUR_UPG_VALUE(UPG_WALLET) - 1;
            itemName = Ship_GetItemNameById(fallBackCheck <= ITEM_WALLET_ADULT ? ITEM_WALLET_ADULT : fallBackCheck);
        }
    }

    if (itemName == "") {
        itemName = Ship_GetItemNameById(itemId);
    }

    return itemName;
}

TrackerItemListObject CreateTrackerObject() {
    std::vector<int16_t> itemTrackerList;
    TrackerItemListObject trackerObject = {
        .windowName = trackerInputName.c_str(),
        .columnLength = 6,
        .windowScale = 1.0f,
        .windowOpacity = 0.5f,
        .itemList = itemTrackerList,
    };
    return trackerObject;
}

void ItemTrackerPopUpContext(int16_t itemId, bool isRandoItem) {
    bool shouldClose = false;
    uint32_t availableSlots = 0;
    uint32_t windowIndex = 0;
    std::vector<TrackerItemListObject>* window =
        isRandoItem ? &BenGui::mItemTrackerWindow->randoItemWindows : &BenGui::mItemTrackerWindow->namedItemWindows;
    if (shouldTrackerPopUpOpen && ImGui::BeginPopup("ItemWindowSubMenu")) {
        for (auto& object : *window) {
            ImGui::PushID(windowIndex);
            auto findInObject = std::find(object.itemList.begin(), object.itemList.end(), itemId);
            if (findInObject == object.itemList.end()) {
                std::string windowStr = "Add to ";
                windowStr += object.windowName;
                if (UIWidgets::Button(windowStr.c_str(), { .size = ImVec2(0, 0), .color = WIDGET_COLOR })) {
                    object.itemList.push_back(itemId);
                    shouldClose = true;
                }
                availableSlots++;
            }
            ImGui::PopID();
            windowIndex++;
        }
        if (availableSlots == 0) {
            ImGui::Text("No Slot Available");
        }
        if (shouldClose) {
            ImGui::CloseCurrentPopup();
            shouldTrackerPopUpOpen = false;
        }

        ImGui::EndPopup();
    }
}

void ItemTrackerDragAndDrop(std::vector<int16_t>& itemWindow, size_t i, TrackerImageObject itemObject) {
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("TRACKER_DRAG", &i, sizeof(size_t));
        ImGui::ImageButton(std::to_string(itemWindow[i]).c_str(), itemObject.textureId, itemObject.textureDimensions,
                           ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), itemObject.textureColor);
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TRACKER_DRAG")) {
            size_t srcIndex = *(const size_t*)payload->Data;
            if (srcIndex != i && srcIndex < itemWindow.size()) {
                auto item = itemWindow[srcIndex];
                itemWindow.erase(itemWindow.begin() + srcIndex);

                if (srcIndex < i) {
                    i--;
                }

                itemWindow.insert(itemWindow.begin() + i, item);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void DrawItemTrackerSlot(std::vector<int16_t>& itemList, int16_t itemId, bool shouldAdd, bool isRandoItem) {
    TrackerImageObject imageObject = GetTextureObject(itemId, isRandoItem);
    imageObject.textureColor.w = 1.0f;
    if (ImGui::ImageButton(std::to_string(itemId).c_str(), imageObject.textureId, imageObject.textureDimensions,
                           ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), imageObject.textureColor)) {
        if (shouldAdd) {
            shouldTrackerPopUpOpen = true;
            popupSlot = itemId;
            ImGui::OpenPopup("ItemWindowSubMenu");
        } else {
            shouldRemove = false;
            for (int i = 0; i < itemList.size(); i++) {
                if (itemList[i] == itemId) {
                    indexToRemove = i;
                    shouldRemove = true;
                    break;
                }
            }
        }
    }
    UIWidgets::Tooltip(GetItemTrackerItemName(itemId, isRandoItem).c_str());
}

void DrawItemList(std::string listName, int columns) {
    bool isRandoItem = false;
    if (ImGui::BeginChild(listName.c_str(), ImVec2(0, 0),
                          ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX |
                              ImGuiChildFlags_AutoResizeY)) {
        if (ImGui::BeginTable(listName.c_str(), columns)) {
            ImVec2 framePadding = ImVec2(listName == "Songs" ? ITEM_SONG_PADDING : 0, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
            std::vector<int16_t> emptyList;

            if (listName == "Frogs" || listName == "Ocarina Buttons" || listName == "Boss Souls" ||
                listName == "Enemy Souls" || listName == "Owl Statues" || listName == "Tingle Maps" ||
                listName == "Time" || listName == "Misc") {
                for (int j = std::get<0>(randoItemLists.at(listName)); j <= std::get<1>(randoItemLists.at(listName));
                     j++) {
                    ImGui::TableNextColumn();
                    isRandoItem = true;
                    DrawItemTrackerSlot(emptyList, j, true, true);
                }
            } else if (listName == "Bottles" || listName == "Tokens" || listName == "Stray Fairies" ||
                       listName == "Dungeon") {
                for (int j = std::get<0>(defaultItemLists.at(listName));
                     j <= std::get<1>(defaultItemLists.at(listName)); j++) {
                    ImGui::TableNextColumn();
                    isRandoItem = false;
                    DrawItemTrackerSlot(emptyList, j, true, false);
                }
            } else {
                std::pair<uint32_t, uint32_t> range = GetItemMapRange(std::get<0>(defaultItemLists.at(listName)),
                                                                      std::get<1>(defaultItemLists.at(listName)));
                for (int i = range.first; i <= range.second; i++) {

                    if (itemIdToItemNameMap[i].first == ITEM_WALLET_ADULT) {
                        ImGui::TableNextColumn();
                        DrawItemTrackerSlot(emptyList, ITEM_MAGIC_JAR_SMALL, true, false);
                        ImGui::TableNextColumn();
                        DrawItemTrackerSlot(emptyList, ITEM_HEART_CONTAINER, true, false);
                    }
                    ImGui::TableNextColumn();
                    DrawItemTrackerSlot(emptyList, itemIdToItemNameMap[i].first, true, isRandoItem);
                }
            }
            ImGui::PopStyleVar(2);
            ItemTrackerPopUpContext(popupSlot, isRandoItem);
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

void DrawPreviewPane() {
    bool isRandoItem = false;
    uint32_t imguiId = 0;
    uint32_t listIndex = 0;
    uint32_t windowListIndex = TRACKER_MAIN;
    if (ImGui::BeginChild("List Previews")) {
        std::vector<std::vector<TrackerItemListObject>*> windowList = {
            &BenGui::mItemTrackerWindow->namedItemWindows,
            &BenGui::mItemTrackerWindow->randoItemWindows,
        };

        for (auto* window : windowList) {
            ImGui::SeparatorText(windowListIndex == TRACKER_MAIN ? "Vanilla Tracker Windows" : "Rando Tracker Windows");
            listIndex = 0;
            for (auto& object : *window) {
                ImGui::PushID(imguiId);
                ImGui::SeparatorText(object.windowName.c_str());
                ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::CalcTextSize("Clear Main").x * 1.5f));
                if (UIWidgets::Button("Clear", { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Red })) {
                    object.itemList.clear();
                }
                ImGui::SameLine();
                if (UIWidgets::Button("x", { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Red })) {
                    auto& currentWindow = *windowList[windowListIndex];
                    currentWindow.erase(currentWindow.begin() + listIndex);
                }
                ImGui::PopID();
                if (object.itemList.size() == 0) {
                    listIndex++;
                    imguiId++;
                    continue;
                }
                if (ImGui::BeginTable(std::to_string(listIndex).c_str(), object.columnLength)) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
                    for (int i = 0; i < object.itemList.size(); i++) {
                        ImGui::TableNextColumn();
                        if (windowListIndex == TRACKER_RANDO) {
                            DrawItemTrackerSlot(object.itemList, object.itemList[i], false, true);
                            isRandoItem = true;
                        } else {
                            DrawItemTrackerSlot(object.itemList, (int16_t)object.itemList[i], false, false);
                        }
                        ItemTrackerDragAndDrop(object.itemList, i, GetTextureObject(object.itemList[i], isRandoItem));
                    }
                    if (shouldRemove) {
                        object.itemList.erase(object.itemList.begin() + indexToRemove);
                        shouldRemove = false;
                        indexToRemove = ITEM_OCARINA_OF_TIME;
                    }
                    ImGui::PopStyleVar(2);
                    ImGui::EndTable();
                }
                listIndex++;
                imguiId++;
            }
            windowListIndex++;
        }
    }
    ImGui::EndChild();
}

void DrawTrackerWindowOptions(int32_t windowIndex, TrackerItemListObject& windowObject) {
    ImGui::PushID(windowIndex);
    int32_t columns = windowObject.columnLength;
    float scale = windowObject.windowScale;
    float opacity = windowObject.windowOpacity;
    std::string trackerInputRename;
    if (windowIndex >= 0) {
        if (UIWidgets::InputString("##windowname", &trackerInputRename,
                                   UIWidgets::InputOptions()
                                       .LabelPosition(UIWidgets::LabelPosition::None)
                                       .Color(WIDGET_COLOR)
                                       .PlaceholderText(windowObject.windowName)
                                       .DefaultValue(windowObject.windowName))) {
            windowObject.windowName = trackerInputRename;
        }
    }
    if (UIWidgets::SliderInt("Columns", &columns,
                             UIWidgets::IntSliderOptions()
                                 .Min(1)
                                 .Max(12)
                                 .DefaultValue(6)
                                 .LabelPosition(UIWidgets::LabelPosition::None)
                                 .Format("Columns: %i")
                                 .Color(WIDGET_COLOR)
                                 .Size(ImVec2(ImGui::GetContentRegionAvail().x / 2, 0)))) {
        windowObject.columnLength = columns;
    }
    ImGui::SameLine();
    if (UIWidgets::SliderFloat("Scale", &scale,
                               UIWidgets::FloatSliderOptions()
                                   .Min(0.5f)
                                   .Max(5.0f)
                                   .DefaultValue(1.0f)
                                   .LabelPosition(UIWidgets::LabelPosition::None)
                                   .Format("Scale: %.1f")
                                   .Step(0.5f)
                                   .Color(WIDGET_COLOR)
                                   .Size(ImVec2(ImGui::GetContentRegionAvail().x, 0)))) {
        windowObject.windowScale = scale;
    }
    if (UIWidgets::SliderFloat("Opacity", &opacity,
                               UIWidgets::FloatSliderOptions()
                                   .Min(0)
                                   .Max(1.0f)
                                   .DefaultValue(0.5f)
                                   .LabelPosition(UIWidgets::LabelPosition::None)
                                   .Format("Opacity: %.1f")
                                   .Step(0.1f)
                                   .Color(WIDGET_COLOR)
                                   .Size(ImVec2(ImGui::GetContentRegionAvail().x, 0)))) {
        windowObject.windowOpacity = opacity;
    }
    ImGui::PopID();
    UIWidgets::Separator();
}

void ApplyRandoItemGroup(std::string listName) {
    std::tuple<int16_t, int16_t, int16_t> list = randoItemLists.at(listName);
    TrackerItemListObject itemObject = {
        .windowName = listName,
        .columnLength = std::get<2>(list),
        .windowScale = 1.0f,
        .windowOpacity = 0.5f,
    };
    for (int i = std::get<0>(list); i <= std::get<1>(list); i++) {
        itemObject.itemList.push_back(i);
    }

    BenGui::mItemTrackerWindow->randoItemWindows.push_back(itemObject);
}

void ApplyDefaultItemGroup(std::string listName) {
    std::tuple<int16_t, int16_t, int16_t> list = defaultItemLists.at(listName);
    TrackerItemListObject itemObject = {
        .windowName = listName,
        .columnLength = std::get<2>(list),
        .windowScale = 1.0f,
        .windowOpacity = 0.5f,
    };

    if (listName == "Bottles" || listName == "Tokens" || listName == "Stray Fairies" || listName == "Dungeon") {
        int16_t baseValue = std::get<0>(list);
        int16_t maxValue = std::get<1>(list);
        for (int j = baseValue; j <= maxValue; j++) {
            itemObject.itemList.push_back(j);
        }
    } else {
        std::pair<uint32_t, uint32_t> range = GetItemMapRange(std::get<0>(list), std::get<1>(list));
        for (int i = range.first; i <= range.second; i++) {
            if (itemIdToItemNameMap[i].first == ITEM_WALLET_ADULT) {
                itemObject.itemList.push_back(ITEM_MAGIC_JAR_SMALL);
                itemObject.itemList.push_back(ITEM_HEART_CONTAINER);
            }
            itemObject.itemList.push_back(itemIdToItemNameMap[i].first);
        }
    }
    BenGui::mItemTrackerWindow->namedItemWindows.push_back(itemObject);
}

void ApplyDefaultItemPreset() {
    BenGui::mItemTrackerWindow->namedItemWindows.clear();

    for (int key = 0; key < defaultItemLists.size(); key++) {
        if (listOrder[key] == "Rando") {
            continue;
        }
        ApplyDefaultItemGroup(listOrder[key]);
    }
}

void SaveItemTrackerLayout() {
    std::vector<std::vector<TrackerItemListObject>*> trackerWindows = {
        &BenGui::mItemTrackerWindow->namedItemWindows,
        &BenGui::mItemTrackerWindow->randoItemWindows,
    };

    auto itemTrackerConfig = nlohmann::json::object();

    uint16_t windowType = 0;

    for (auto* window : trackerWindows) {
        if (window->empty()) {
            windowType++;
            continue;
        }

        std::string windowName = windowType == TRACKER_MAIN ? "Main" : "Rando";
        nlohmann::json trackerSaveObject = nlohmann::json::object();

        for (auto& object : *window) {
            trackerSaveObject[object.windowName] = nlohmann::json::object();
            trackerSaveObject[object.windowName]["Name"] = object.windowName;
            trackerSaveObject[object.windowName]["Columns"] = object.columnLength;
            trackerSaveObject[object.windowName]["Scale"] = object.windowScale;
            trackerSaveObject[object.windowName]["Opacity"] = object.windowOpacity;
            trackerSaveObject[object.windowName]["ItemList"] = nlohmann::json::array();

            for (auto& item : object.itemList) {
                trackerSaveObject[object.windowName]["ItemList"].push_back(item);
            }
        }
        itemTrackerConfig[windowName] = trackerSaveObject;
        windowType++;
    }

    Ship::Context::GetInstance()->GetConfig()->SetBlock("ItemTrackerLayout", itemTrackerConfig);
    Ship::Context::GetInstance()->GetConfig()->Save();
}

void LoadItemTrackerLayout() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
    if (allConfig.find("ItemTrackerLayout") == allConfig.end()) {
        return;
    }

    auto& itemTrackerConfig = allConfig["ItemTrackerLayout"];

    for (auto& [windowKey, windowData] : itemTrackerConfig.items()) {
        std::vector<TrackerItemListObject>* window = windowKey == "Main"
                                                         ? &BenGui::mItemTrackerWindow->namedItemWindows
                                                         : &BenGui::mItemTrackerWindow->randoItemWindows;
        for (auto& [dataKey, dataInfo] : windowData.items()) {
            TrackerItemListObject windowObj = { .windowName = dataInfo["Name"].get<std::string>(),
                                                .columnLength = dataInfo["Columns"],
                                                .windowScale = dataInfo["Scale"],
                                                .windowOpacity = dataInfo["Opacity"],
                                                .itemList = dataInfo["ItemList"] };
            window->push_back(windowObj);
        }
    }
}

void DrawTrackerSaveLoadOptions() {
    if (UIWidgets::Button("Save Layout", { .color = WIDGET_COLOR })) {
        SaveItemTrackerLayout();
    }
    if (UIWidgets::Button("Load Layout", { .color = WIDGET_COLOR })) {
        BenGui::mItemTrackerWindow->namedItemWindows.clear();
        BenGui::mItemTrackerWindow->randoItemWindows.clear();
        LoadItemTrackerLayout();
    }
}

void DrawTrackerOptions() {
    int32_t windowIndex = 0;
    ImGui::SeparatorText("Presets");
    if (UIWidgets::Button("Default Preset", { .color = WIDGET_COLOR })) {
        ApplyDefaultItemPreset();
    }
    UIWidgets::Tooltip("Places all Vanilla items in the tracker.");

    ImGui::SeparatorText("Custom Windows");
    if (ImGui::BeginTable("OptionsList", 2)) {
        ImGui::TableNextColumn();
        UIWidgets::CVarCombobox("Window Type", "gSettings.ItemTracker.WindowType", windowTypes,
                                { .alignment = UIWidgets::ComponentAlignment::Right,
                                  .labelPosition = UIWidgets::LabelPosition::Near,
                                  .color = WIDGET_COLOR });
        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Split Window Groups", "gSettings.ItemTracker.WindowGroup");
        UIWidgets::CVarCheckbox("Show Item Counts", "gSettings.ItemTracker.ItemCounts");
        ImGui::EndTable();
    }
    UIWidgets::InputString("Window Name", &trackerInputName,
                           {
                               .labelPosition = UIWidgets::LabelPosition::None,
                               .color = WIDGET_COLOR,
                               .placeholder = "Enter new window name",
                           });
    if (ImGui::BeginTable("WindowCreation", 2)) {
        ImGui::TableNextColumn();
        if (UIWidgets::Button("Create Vanilla Window", { .color = WIDGET_COLOR })) {
            BenGui::mItemTrackerWindow->namedItemWindows.push_back(CreateTrackerObject());
            trackerInputName.clear();
        }
        ImGui::TableNextColumn();
        if (UIWidgets::Button("Create Rando Window", { .color = WIDGET_COLOR })) {
            BenGui::mItemTrackerWindow->randoItemWindows.push_back(CreateTrackerObject());
            trackerInputName.clear();
        }
        ImGui::EndTable();
    }
    ImGui::SeparatorText("Window Options");
    for (auto& window : BenGui::mItemTrackerWindow->namedItemWindows) {
        DrawTrackerWindowOptions(windowIndex, window);
        windowIndex++;
    }
    for (auto& window : BenGui::mItemTrackerWindow->randoItemWindows) {
        DrawTrackerWindowOptions(windowIndex, window);
        windowIndex++;
    }
}

void DrawTrackerCustomizationOptions() {
    if (ImGui::BeginChild("Item Lists")) {
        for (int key = 0; key < defaultItemLists.size(); key++) {
            ImGui::PushID(key);
            std::tuple<int16_t, int16_t, int16_t> list = defaultItemLists.at(listOrder[key]);
            ImGui::SeparatorText(listOrder[key].c_str());
            ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::CalcTextSize("Set All").x * 1.5f));
            if (UIWidgets::Button("Set All", { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Green })) {
                ApplyDefaultItemGroup(listOrder[key]);
            }
            DrawItemList(listOrder[key], std::get<2>(list));
            ImGui::PopID();
        }
        for (int rkey = 0; rkey < randoItemLists.size(); rkey++) {
            ImGui::PushID(rkey + RI_MAX);
            std::tuple<int16_t, int16_t, int16_t> list = randoItemLists.at(randoListOrder[rkey]);
            ImGui::SeparatorText(randoListOrder[rkey].c_str());
            ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::CalcTextSize("Set All").x * 1.5f));
            if (UIWidgets::Button("Set All", { .size = ImVec2(0, 0), .color = UIWidgets::Colors::Green })) {
                ApplyRandoItemGroup(randoListOrder[rkey]);
            }
            DrawItemList(randoListOrder[rkey], std::get<2>(list));
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}

void ItemTrackerSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(733, 472), ImGuiCond_FirstUseEver);
    if (ImGui::BeginChild("Item Tracker Settings")) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

        if (CVarGetInteger("gWindows.ItemTracker", 0)) {
            UIWidgets::WindowButton("Hide Item Tracker", "gWindows.ItemTracker", BenGui::mItemTrackerWindow,
                                    { .size = UIWidgets::Sizes::Inline, .color = WIDGET_COLOR });
        } else {
            UIWidgets::WindowButton("Show Item Tracker", "gWindows.ItemTracker", BenGui::mItemTrackerWindow,
                                    { .size = UIWidgets::Sizes::Inline, .color = WIDGET_COLOR });
        }
        UIWidgets::Separator();
        UIWidgets::PushStyleTabs(WIDGET_COLOR);
        if (ImGui::BeginTable("TrackerTabs", 2)) {
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("TrackerChild")) {
                if (ImGui::BeginTabBar("TrackerTabs")) {
                    if (ImGui::BeginTabItem("Customization")) {
                        if (ImGui::BeginChild("CustomizationChild")) {
                            DrawTrackerCustomizationOptions();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Options")) {
                        if (ImGui::BeginChild("OptionsChild")) {
                            DrawTrackerOptions();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Save/Load")) {
                        if (ImGui::BeginChild("SaveChild")) {
                            DrawTrackerSaveLoadOptions();
                        }
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::EndChild();

            ImGui::TableNextColumn();
            if (ImGui::BeginChild("WindowChild")) {
                if (ImGui::BeginTabBar("WindowTab")) {
                    if (ImGui::BeginTabItem("Window Layouts")) {
                        DrawPreviewPane();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }
            ImGui::EndChild();
            ImGui::EndTable();
        }
        UIWidgets::PopStyleTabs();

        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();
}

void ItemTrackerSettingsWindow::InitElement() {
}
