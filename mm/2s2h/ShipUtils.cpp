#include "ShipUtils.h"
#include "assets/2s2h_assets.h"
#include <string>
#include <random>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost_custom/container_hash/hash_32.hpp>
#include "public/bridge/consolevariablebridge.h"
#include "Context.h"
// Image Icons
#include "assets/interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"

extern float OTRGetAspectRatio();

extern f32 sNESFontWidths[160];
extern const char* fontTbl[156];
extern TexturePtr gItemIcons[131];
extern TexturePtr gQuestIcons[14];
extern TexturePtr gBombersNotebookPhotos[24];
}

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, _betterMapSelectIndex, humanName)                               \
    { enumValue, humanName },
#define DEFINE_SCENE_UNSET(_enumValue)

std::unordered_map<s16, const char*> sceneNames = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

// These textures are not in existing lists that we iterate over.
std::array<const char*, 18> miscellaneousTextures = {
    gArcheryScoreIconTex,
    gBarrelTrackerIcon,
    gChestTrackerIcon,
    gCrateTrackerIcon,
    gDungeonStrayFairyGreatBayIconTex,
    gDungeonStrayFairySnowheadIconTex,
    gDungeonStrayFairyStoneTowerIconTex,
    gDungeonStrayFairyWoodfallIconTex,
    gPotTrackerIcon,
    gQuestIconGoldSkulltulaTex,
    gMagicArrowEquipEffectTex,
    gRupeeCounterIconTex,
    gStrayFairyGreatBayIconTex,
    gStrayFairySnowheadIconTex,
    gStrayFairyStoneTowerIconTex,
    gStrayFairyWoodfallIconTex,
    gTimerClockIconTex,
    gWorldMapOwlFaceTex,
};

std::array<const char*, 11> digitList = { gCounterDigit0Tex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
                                          gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex,
                                          gCounterDigit8Tex, gCounterDigit9Tex, gCounterColonTex };

extern "C" const char* Ship_GetSceneName(s16 sceneId) {
    if (sceneNames.contains(sceneId)) {
        return sceneNames[sceneId];
    }

    return "Unknown";
}

constexpr f32 fourByThree = 4.0f / 3.0f;

// Gets the additional ratio of the screen compared to the original 4:3 ratio, clamping to 1 if smaller
extern "C" f32 Ship_GetExtendedAspectRatioMultiplier() {
    f32 currentRatio = OTRGetAspectRatio();
    return MAX(currentRatio / fourByThree, 1.0f);
}

// Enables Extended Culling options on specific actors by applying an inverse ratio of the draw distance slider
// to the projected Z value of the actor. This tricks distance checks without having to replace hardcoded values.
// Requires that Ship_ExtendedCullingActorRestoreProjectedPos is called within the same function scope.
extern "C" void Ship_ExtendedCullingActorAdjustProjectedZ(Actor* actor) {
    s32 multiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    multiplier = MAX(multiplier, 1);
    if (multiplier > 1) {
        actor->projectedPos.z /= multiplier;
    }
}

// Enables Extended Culling options on specific actors by applying an inverse ratio of the widescreen aspect ratio
// to the projected X value of the actor. This tricks distance checks without having to replace hardcoded values.
// Requires that Ship_ExtendedCullingActorRestoreProjectedPos is called within the same function scope.
extern "C" void Ship_ExtendedCullingActorAdjustProjectedX(Actor* actor) {
    if (CVarGetInteger("gEnhancements.Graphics.ActorCullingAccountsForWidescreen", 0)) {
        f32 ratioAdjusted = Ship_GetExtendedAspectRatioMultiplier();
        actor->projectedPos.x /= ratioAdjusted;
    }
}

// Restores the projectedPos values on the actor after modifications from the Extended Culling hacks
extern "C" void Ship_ExtendedCullingActorRestoreProjectedPos(PlayState* play, Actor* actor) {
    f32 invW = 0.0f;
    Actor_GetProjectedPos(play, &actor->world.pos, &actor->projectedPos, &invW);
}

extern "C" bool Ship_IsCStringEmpty(const char* str) {
    return str == NULL || str[0] == '\0';
}

// Build vertex coordinates for a quad command
// In order of top left, top right, bottom left, then bottom right
// Supports flipping the texture horizontally
extern "C" void Ship_CreateQuadVertexGroup(Vtx* vtxList, s32 xStart, s32 yStart, s32 width, s32 height, u8 flippedH) {
    vtxList[0].v.ob[0] = xStart;
    vtxList[0].v.ob[1] = yStart;
    vtxList[0].v.tc[0] = (flippedH ? width : 0) << 5;
    vtxList[0].v.tc[1] = 0 << 5;

    vtxList[1].v.ob[0] = xStart + width;
    vtxList[1].v.ob[1] = yStart;
    vtxList[1].v.tc[0] = (flippedH ? width * 2 : width) << 5;
    vtxList[1].v.tc[1] = 0 << 5;

    vtxList[2].v.ob[0] = xStart;
    vtxList[2].v.ob[1] = yStart + height;
    vtxList[2].v.tc[0] = (flippedH ? width : 0) << 5;
    vtxList[2].v.tc[1] = height << 5;

    vtxList[3].v.ob[0] = xStart + width;
    vtxList[3].v.ob[1] = yStart + height;
    vtxList[3].v.tc[0] = (flippedH ? width * 2 : width) << 5;
    vtxList[3].v.tc[1] = height << 5;
}

extern "C" f32 Ship_GetCharFontWidthNES(u8 character) {
    u8 adjustedChar = character - ' ';

    if (adjustedChar >= ARRAY_COUNTU(sNESFontWidths)) {
        return 0.0f;
    }

    return sNESFontWidths[adjustedChar];
}

extern "C" TexturePtr Ship_GetCharFontTextureNES(u8 character) {
    u8 adjustedChar = character - ' ';

    if (adjustedChar >= ARRAY_COUNTU(sNESFontWidths)) {
        return (TexturePtr)gEmptyTexture;
    }

    return (TexturePtr)fontTbl[adjustedChar];
}

static bool seeded = false;
static boost::random::mt19937 generator;

extern "C" void Ship_Random_Seed(u32 seed) {
    seeded = true;
    generator = boost::random::mt19937{ seed };
}

extern "C" s32 Ship_Random(s32 min, s32 max) {
    if (!seeded) {
        const auto seed = static_cast<uint32_t>(std::random_device{}());
        Ship_Random_Seed(seed);
    }
    boost::random::uniform_int_distribution<uint32_t> distribution(min, max - 1);
    return distribution(generator);
}

void LoadGuiTextures() {
    for (const TexturePtr entry : gItemIcons) {
        auto path = static_cast<const char*>(entry);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(path, path, ImVec4(1, 1, 1, 1));
    }
    for (const TexturePtr entry : gQuestIcons) {
        auto path = static_cast<const char*>(entry);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(path, path, ImVec4(1, 1, 1, 1));
    }
    for (const TexturePtr entry : gBombersNotebookPhotos) {
        auto path = static_cast<const char*>(entry);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(path, path, ImVec4(1, 1, 1, 1));
    }
    for (const auto entry : miscellaneousTextures) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry, entry, ImVec4(1, 1, 1, 1));
    }
    for (const auto entry : digitList) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry, entry, ImVec4(1, 1, 1, 1));
    }
}

std::string convertEnumToReadableName(const std::string& input) {
    std::string result;
    std::string content = input;

    // Step 1: Remove "RC_" prefix if present
    const std::string prefix = "RC_";
    if (content.rfind(prefix, 0) == 0) {
        content = content.substr(prefix.size());
    }

    // Step 2: Split the string by '_'
    std::vector<std::string> words;
    std::string word;
    std::istringstream stream(content);
    while (std::getline(stream, word, '_')) {
        words.push_back(word);
    }

    // Step 3: Capitalize the first letter of each word
    for (auto& w : words) {
        std::transform(w.begin(), w.end(), w.begin(), [](unsigned char c) { return std::tolower(c); });
        if (!w.empty()) {
            if (w == "hp") {
                w = "HP";
            } else {
                w[0] = std::toupper(w[0]);
            }
        }
    }

    // Step 4: Join the words with spaces
    for (size_t i = 0; i < words.size(); ++i) {
        result += words[i];
        if (i < words.size() - 1) {
            result += " ";
        }
    }

    return result;
}
