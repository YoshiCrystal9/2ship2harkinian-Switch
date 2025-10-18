#ifndef SHIP_UTILS_H
#define SHIP_UTILS_H

#include "PR/ultratypes.h"

#include "gbi.h"

#ifdef __cplusplus
#include <string>
#include <array>
#include <map>
#include <vector>
#include <imgui.h>
#include "Rando/Rando.h"
void LoadGuiTextures();
std::string convertEnumToReadableName(const std::string& input);
std::vector<RandoItemId> convertStartingItemsToRandoItemId(const std::string& input, const std::string& delimiter);
std::string CreateStartingItemsToCvar(std::vector<RandoItemId> startingItemList);
std::string Ship_RemoveSpecialCharacters(const std::string& str);
extern std::array<const char*, 11> digitList;
extern std::string Ship_FormatTimeDisplay(uint32_t value);
extern std::map<uint32_t, ImVec4> itemColorMap;
extern ImVec4 Ship_GetItemColorTint(uint32_t itemId);

extern "C" {
#endif

struct PlayState;
struct Actor;

f32 Ship_GetExtendedAspectRatioMultiplier();
void Ship_ExtendedCullingActorAdjustProjectedZ(Actor* actor);
void Ship_ExtendedCullingActorAdjustProjectedX(Actor* actor);
void Ship_ExtendedCullingActorRestoreProjectedPos(PlayState* play, Actor* actor);
const char* Ship_GetSceneName(s16 sceneId);
bool Ship_IsCStringEmpty(const char* str);
void Ship_CreateQuadVertexGroup(Vtx* vtxList, s32 xStart, s32 yStart, s32 width, s32 height, u8 flippedH);
f32 Ship_GetCharFontWidthNES(u8 character);
TexturePtr Ship_GetCharFontTextureNES(u8 character);
void Ship_Random_Seed(u32 seed);
s32 Ship_Random(s32 min, s32 max);

#ifdef __cplusplus
}
#endif

#endif // SHIP_UTILS_H
