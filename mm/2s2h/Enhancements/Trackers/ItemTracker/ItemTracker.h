#include <vector>
#include <imgui.h>
#include "ship/window/gui/GuiWindow.h"

#define ITEM_TEXTURE_SIZE 46.0f
#define ITEM_SONG_PADDING 8.0f

typedef enum {
    TRACKER_MAIN,
    TRACKER_RANDO,
} TrackerWindowTypes;

typedef enum {
    ITEM_BOTTLE_1 = 0x100,
    ITEM_BOTTLE_2,
    ITEM_BOTTLE_3,
    ITEM_BOTTLE_4,
    ITEM_BOTTLE_5,
    ITEM_BOTTLE_6,
    ITEM_SKULL_TOKEN_SWAMP,
    ITEM_SKULL_TOKEN_OCEAN,
    ITEM_CLOCK_TOWN_STRAY_FAIRY,
    ITEM_WOODFALL_STRAY_FAIRY,
    ITEM_SNOWHEAD_STRAY_FAIRY,
    ITEM_GREAT_BAY_STRAY_FAIRY,
    ITEM_STONE_TOWER_STRAY_FAIRY,
    ITEM_WOODFALL_DUNGEON_MAP,
    ITEM_WOODFALL_DUNGEON_COMPASS,
    ITEM_WOODFALL_KEY_SMALL,
    ITEM_WOODFALL_KEY_BOSS,
    ITEM_SNOWHEAD_DUNGEON_MAP,
    ITEM_SNOWHEAD_DUNGEON_COMPASS,
    ITEM_SNOWHEAD_KEY_SMALL,
    ITEM_SNOWHEAD_KEY_BOSS,
    ITEM_GREAT_BAY_DUNGEON_MAP,
    ITEM_GREAT_BAY_DUNGEON_COMPASS,
    ITEM_GREAT_BAY_KEY_SMALL,
    ITEM_GREAT_BAY_KEY_BOSS,
    ITEM_STONE_TOWER_DUNGEON_MAP,
    ITEM_STONE_TOWER_DUNGEON_COMPASS,
    ITEM_STONE_TOWER_KEY_SMALL,
    ITEM_STONE_TOWER_KEY_BOSS,
} TrackerTokenSlots;

typedef struct {
    ImTextureID textureId;
    ImVec4 textureColor;
    ImVec2 textureDimensions;
} TrackerImageObject;

typedef struct {
    std::string windowName;
    int32_t columnLength;
    float windowScale;
    float windowOpacity;
    std::vector<int16_t> itemList;
} TrackerItemListObject;

TrackerImageObject GetTextureObject(int16_t itemId, bool isRandoItem = false);
extern std::string GetItemTrackerItemName(int16_t itemId, bool isRandoItem = false);
extern bool shouldWindowSplit;

class ItemTrackerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;

    std::vector<TrackerItemListObject> namedItemWindows;
    std::vector<TrackerItemListObject> randoItemWindows;

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
