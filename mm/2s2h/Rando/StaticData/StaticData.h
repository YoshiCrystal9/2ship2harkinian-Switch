#ifndef RANDO_STATIC_DATA_H
#define RANDO_STATIC_DATA_H

#include <map>
#include <array>
#include "Rando/Types.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64item.h"
#include "z64scene.h"
}

#define DEFAULT_TRIFORCE_PIECES_MAX 15

namespace Rando {

namespace StaticData {

struct RandoStaticCheck {
    RandoCheckId randoCheckId;
    const char* name;
    RandoCheckType randoCheckType;
    SceneId sceneId;
    FlagType flagType;
    s32 flag;
    RandoItemId randoItemId;
};

extern std::map<RandoCheckId, RandoStaticCheck> Checks;
extern std::array<std::string, RC_MAX> CheckNames;

RandoStaticCheck GetCheckFromFlag(FlagType flagType, s32 flag, s16 sceneId = SCENE_MAX);
RandoCheckId GetCheckIdFromName(const char* name);
void PopulateCheckNames();

struct RandoStaticItem {
    RandoItemId randoItemId;
    const char* spoilerName;
    const char* article;
    const char* name;
    RandoItemType randoItemType;
    ItemId itemId;
    GetItemId getItemId;
    GetItemDrawId drawId;
};

extern std::map<RandoItemId, RandoStaticItem> Items;
extern std::map<StartingItemCategory, std::vector<RandoItemId>> StartingItemsMap;
extern std::map<RandoItemId, u8> MaxStartingItemsMap;

RandoItemId GetItemIdFromName(const char* name);
u8 GetIconForZMessage(RandoItemId itemId);
const char* GetIconTexturePath(RandoItemId itemId);
bool ShouldShowGetItemCutscene(RandoItemId itemId);
std::string GetItemName(RandoItemId randoItemId, bool includeArticle = true);
std::string GetTrapMessage();

struct RandoStaticOption {
    RandoOptionId randoOptionId;
    const char* name;
    const char* cvar;
    u32 defaultValue;
};

extern std::map<RandoOptionId, RandoStaticOption> Options;

RandoOptionId GetOptionIdFromName(const char* name);

struct RandoStaticRegion {
    RandoRegionId randoRegionId;
    const char* name;
    SceneId sceneId;
    std::map<RandoCheckId, std::function<bool()>> checks;
    std::map<RandoRegionId, std::function<bool()>> regions;
};

extern std::map<RandoRegionId, RandoStaticRegion> Regions;

} // namespace StaticData

} // namespace Rando

#endif // RANDO_STATIC_DATA_H
