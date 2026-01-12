#ifndef SHIP_UTILS_H
#define SHIP_UTILS_H

#include "PR/ultratypes.h"

#include "macros.h" // For CLOCK_TIME and DAY_LENGTH

// Time utilities for 2s2h enhancements
#define MORNING_TIME 0x4000 // 6:00 AM - start of day
// Normalize time so that 6:00 AM is 0 and 5:59 AM is 0xFFFF (wraps around)
#define ZERO_DAY_START(time) (((u16)((time)-MORNING_TIME) % DAY_LENGTH))

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
std::string Ship_RemoveSpecialCharacters(const std::string& str);
extern u16 sOwlWarpEntrancesForMods[];
extern std::array<const char*, 11> digitList;
extern const char* fairyIconTextures[];
extern std::string Ship_FormatTimeDisplay(uint32_t value);
extern std::vector<std::pair<int16_t, std::string>> itemIdToItemNameMap;
extern std::string Ship_GetItemNameById(int16_t itemId);
extern std::map<uint32_t, ImVec4> itemColorMap;
extern ImVec4 Ship_GetItemColorTint(uint32_t itemId);
extern ImVec4 Ship_GetRandoItemColorTint(uint32_t randoItemId);
uint32_t Ship_ConvertQuestIdToItem(uint32_t itemId);
uint32_t Ship_ConvertItemIdToQuest(uint32_t itemId);
extern uint32_t Ship_Hash(std::string str);
extern std::string GetActorDescription(u16 actorNum);
extern std::string GetActorDebugName(u16 actorNum);
extern std::string GetActorCategoryName(u8 category);

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
void Ship_Random_Seed(u64 seed);
s32 Ship_Random(s32 min, s32 max);

#ifdef __cplusplus
}
#endif

#endif // SHIP_UTILS_H
