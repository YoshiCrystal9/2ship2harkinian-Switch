#include <vector>
#include <imgui.h>
#include "ship/window/gui/GuiWindow.h"

#define ITEM_TEXTURE_SIZE 46.0f

typedef enum {
    TRACKER_ITEM_RANDO,
    TRACKER_ITEM_SLOT,
    TRACKER_ITEM_SWORD,
    TRACKER_ITEM_SHIELD,
    TRACKER_ITEM_WALLET,
    TRACKER_ITEM_MAGIC,
} TrackerItemType;

typedef struct {
    ImTextureID textureId;
    ImVec4 textureColor;
    ImVec2 textureDimensions;
} TrackerImageObject;

typedef struct {
    std::string name;
    u8 columns;
    float scale;
    std::vector<std::pair<TrackerItemType, u32>> items;
} TrackerGroup;

extern std::vector<TrackerGroup> itemTrackerGroups;
extern bool DrawItemTrackerSlot(TrackerItemType itemType, u32 itemId, float scale, bool clickable);
extern std::string GetItemTrackerItemName(TrackerItemType itemType, u32 itemId);

class ItemTrackerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
