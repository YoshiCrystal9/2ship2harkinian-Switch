#include "ShipUtils.h"
#include "assets/2s2h_assets.h"
#include <string>
#include <bit>
#include <random>
#include <vector>
#include <cassert>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
// Image Icons
#include "assets/interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

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

std::array<const char*, ACTORCAT_MAX> actorCategoryNames = { "Switch", "Background", "Player", "Explosive",
                                                             "NPC",    "Enemy",      "Prop",   "Item/Action",
                                                             "Misc.",  "Boss",       "Door",   "Chest" };

#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _humanName },
#define DEFINE_ACTOR_INTERNAL(_name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _humanName },
#define DEFINE_ACTOR_UNSET(_enumValue) { _enumValue, "Unset" },

std::unordered_map<s16, const char*> actorDescriptions = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_INTERNAL(_name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_UNSET(_enumValue) { _enumValue, "Unset" },

std::unordered_map<s16, const char*> actorDebugNames = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

/*
 * This is identical to `sOwlWarpEntrances` in decomp code, which is a static variable defined in multiple places and
 * cannot be externed. We redundantly define it here and extern it, for use in port enhancements.
 */
extern u16 sOwlWarpEntrancesForMods[OWL_WARP_MAX - 1] = {
    ENTRANCE(GREAT_BAY_COAST, 11),         // OWL_WARP_GREAT_BAY_COAST
    ENTRANCE(ZORA_CAPE, 6),                // OWL_WARP_ZORA_CAPE
    ENTRANCE(SNOWHEAD, 3),                 // OWL_WARP_SNOWHEAD
    ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),  // OWL_WARP_MOUNTAIN_VILLAGE
    ENTRANCE(SOUTH_CLOCK_TOWN, 9),         // OWL_WARP_CLOCK_TOWN
    ENTRANCE(MILK_ROAD, 4),                // OWL_WARP_MILK_ROAD
    ENTRANCE(WOODFALL, 4),                 // OWL_WARP_WOODFALL
    ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // OWL_WARP_SOUTHERN_SWAMP
    ENTRANCE(IKANA_CANYON, 4),             // OWL_WARP_IKANA_CANYON
    ENTRANCE(STONE_TOWER, 3),              // OWL_WARP_STONE_TOWER
};

// These textures are not in existing lists that we iterate over.
std::array<const char*, 31> miscellaneousTextures = {
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
    gTriforcePieceTex,
    gWorldMapOwlFaceTex,
    gameplay_keep_Tex_053140,
    gDungeonMapSkullTex,
    gPauseUnusedCursorTex,
    gWorldMapOwlFaceTex,
    gItemIconTingleMapTex,
    gThreeDayClockSunHourTex,
    gThreeDayClockMoonHourTex,
    gOcarinaATex,
    gOcarinaCDownTex,
    gOcarinaCLeftTex,
    gOcarinaCRightTex,
    gOcarinaCUpTex,
};

std::array<const char*, 11> digitList = { gCounterDigit0Tex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
                                          gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex,
                                          gCounterDigit8Tex, gCounterDigit9Tex, gCounterColonTex };

const char* fairyIconTextures[] = { gDungeonStrayFairyWoodfallIconTex, gDungeonStrayFairySnowheadIconTex,
                                    gDungeonStrayFairyGreatBayIconTex, gDungeonStrayFairyStoneTowerIconTex };

std::vector<std::pair<int16_t, std::string>> itemIdToItemNameMap = {
    // Inventory
    { ITEM_OCARINA_OF_TIME, "Ocarina of Time" },
    { ITEM_BOW, "Bow" },
    { ITEM_ARROW_FIRE, "Fire Arrow" },
    { ITEM_ARROW_ICE, "Ice Arrow" },
    { ITEM_ARROW_LIGHT, "Light Arrow" },
    { ITEM_MOONS_TEAR, "Moon's Tear" },
    { ITEM_BOMB, "Bomb" },
    { ITEM_BOMBCHU, "Bombchu" },
    { ITEM_DEKU_STICK, "Deku Stick" },
    { ITEM_DEKU_NUT, "Deku Nut" },
    { ITEM_MAGIC_BEANS, "Magic Bean" },
    { ITEM_ROOM_KEY, "Room Key" },
    { ITEM_POWDER_KEG, "Powder Keg" },
    { ITEM_PICTOGRAPH_BOX, "Pictograph" },
    { ITEM_LENS_OF_TRUTH, "Lens of Truth" },
    { ITEM_HOOKSHOT, "Hookshot" },
    { ITEM_SWORD_GREAT_FAIRY, "Great Fairy Sword" },
    { ITEM_LETTER_TO_KAFEI, "Letter to Kafei" },
    { ITEM_BOTTLE, "Empty Bottle" },

    // Masks
    { ITEM_MASK_POSTMAN, "Postman's Hat" },
    { ITEM_MASK_ALL_NIGHT, "All-Night Mask" },
    { ITEM_MASK_BLAST, "Blast Mask" },
    { ITEM_MASK_STONE, "Stone Mask" },
    { ITEM_MASK_GREAT_FAIRY, "Great Fairy's Mask" },
    { ITEM_MASK_DEKU, "Deku Mask" },
    { ITEM_MASK_KEATON, "Keaton Mask" },
    { ITEM_MASK_BREMEN, "Bremen Mask" },
    { ITEM_MASK_BUNNY, "Bunny Hood" },
    { ITEM_MASK_DON_GERO, "Don Gero's Mask" },
    { ITEM_MASK_SCENTS, "Mask of Scents" },
    { ITEM_MASK_GORON, "Goron Mask" },
    { ITEM_MASK_ROMANI, "Romani's Mask" },
    { ITEM_MASK_CIRCUS_LEADER, "Circus Leader's Mask" },
    { ITEM_MASK_KAFEIS_MASK, "Kafei's Mask" },
    { ITEM_MASK_COUPLE, "Couple's Mask" },
    { ITEM_MASK_TRUTH, "Mask of Truth" },
    { ITEM_MASK_ZORA, "Zora Mask" },
    { ITEM_MASK_KAMARO, "Kamaro's Mask" },
    { ITEM_MASK_GIBDO, "Gibdo Mask" },
    { ITEM_MASK_GARO, "Garo's Mask" },
    { ITEM_MASK_CAPTAIN, "Captain's Hat" },
    { ITEM_MASK_GIANT, "Giant's Mask" },
    { ITEM_MASK_FIERCE_DEITY, "Fierce Deity's Mask" },

    // Songs
    { ITEM_SONG_TIME, "Song of Time" },
    { ITEM_SONG_HEALING, "Song of Healing" },
    { ITEM_SONG_EPONA, "Epona's Song" },
    { ITEM_SONG_SOARING, "Song of Soaring" },
    { ITEM_SONG_STORMS, "Song of Storms" },
    { ITEM_SONG_SONATA, "Sonata of Awakening" },
    { ITEM_SONG_LULLABY, "Goron Lullaby" },
    { ITEM_SONG_NOVA, "New Wave Bossa Nova" },
    { ITEM_SONG_ELEGY, "Elegy of Emptiness" },
    { ITEM_SONG_OATH, "Oath to Order" },

    // Quest
    { ITEM_REMAINS_ODOLWA, "Odolwa's Remains" },
    { ITEM_REMAINS_GOHT, "Goht's Remains" },
    { ITEM_REMAINS_GYORG, "Gyorg's Remains" },
    { ITEM_REMAINS_TWINMOLD, "Twinmold's Remains" },
    { ITEM_SWORD_KOKIRI, "Kokiri Sword" },
    { ITEM_SHIELD_HERO, "Hero's Shield" },
    { ITEM_WALLET_ADULT, "Adult Wallet" },
    { ITEM_BOMBERS_NOTEBOOK, "Bombers' Notebook" },

    // Upgrade Items
    { ITEM_SWORD_RAZOR, "Razor Sword" },
    { ITEM_SWORD_GILDED, "Gilded Sword" },
    { ITEM_SHIELD_MIRROR, "Mirror Shield" },
    { ITEM_WALLET_GIANT, "Giant Wallet" },

    // Trade Items
    { ITEM_DEED_LAND, "Land Title Deed" },
    { ITEM_DEED_SWAMP, "Swamp Title Deed" },
    { ITEM_DEED_MOUNTAIN, "Mountain Title Deed" },
    { ITEM_DEED_OCEAN, "Ocean Title Deed" },
    { ITEM_LETTER_MAMA, "Letter to Mama" },
    { ITEM_PENDANT_OF_MEMORIES, "Pendant of Memories" },
};

std::string Ship_GetItemNameById(int16_t itemId) {
    std::string itemName = "";
    for (auto& [id, name] : itemIdToItemNameMap) {
        if (id == itemId) {
            itemName = name;
            break;
        }
    }
    return itemName;
};

std::map<uint32_t, ImVec4> itemColorMap = {
    { ITEM_SONG_SONATA, ImVec4(0.588f, 1.0f, 0.392f, 1.0f) },
    { ITEM_SONG_LULLABY, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
    { ITEM_SONG_NOVA, ImVec4(0.392f, 0.588f, 1.0f, 1.0f) },
    { ITEM_SONG_ELEGY, ImVec4(1.0f, 0.627f, 0.0f, 1.0f) },
    { ITEM_SONG_OATH, ImVec4(1.0f, 0.392f, 1.0f, 1.0f) },
    { ITEM_SONG_LULLABY_INTRO, ImVec4(1.0f, 0.313f, 0.156f, 1.0f) },
};

ImVec4 Ship_GetItemColorTint(uint32_t itemId) {
    auto findColor = itemColorMap.find(itemId);
    if (findColor != itemColorMap.end()) {
        return findColor->second;
    } else {
        return ImVec4(1, 1, 1, 1);
    }
}

std::map<uint32_t, ImVec4> randoItemColorMap = {
    { RI_OCARINA_BUTTON_A, ImVec4(0.085f, 0.494f, 0.796f, 1) },
    { RI_OCARINA_BUTTON_C_DOWN, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_LEFT, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_RIGHT, ImVec4(0.84f, 0.768f, 0.089f, 1) },
    { RI_OCARINA_BUTTON_C_UP, ImVec4(0.84f, 0.768f, 0.089f, 1) },
};

ImVec4 Ship_GetRandoItemColorTint(uint32_t randoItemId) {
    auto findColor = randoItemColorMap.find(randoItemId);
    if (findColor != randoItemColorMap.end()) {
        return findColor->second;
    } else {
        return ImVec4(1, 1, 1, 1);
    }
}

std::map<QuestItem, ItemId> questIdToItemMap = {
    { QUEST_REMAINS_ODOLWA, ITEM_REMAINS_ODOLWA },
    { QUEST_REMAINS_GOHT, ITEM_REMAINS_GOHT },
    { QUEST_REMAINS_GYORG, ITEM_REMAINS_GYORG },
    { QUEST_REMAINS_TWINMOLD, ITEM_REMAINS_TWINMOLD },
    { QUEST_SONG_SONATA, ITEM_SONG_SONATA },
    { QUEST_SONG_LULLABY, ITEM_SONG_LULLABY },
    { QUEST_SONG_BOSSA_NOVA, ITEM_SONG_NOVA },
    { QUEST_SONG_ELEGY, ITEM_SONG_ELEGY },
    { QUEST_SONG_OATH, ITEM_SONG_OATH },
    { QUEST_SONG_SARIA, ITEM_SONG_SARIA },
    { QUEST_SONG_TIME, ITEM_SONG_TIME },
    { QUEST_SONG_HEALING, ITEM_SONG_HEALING },
    { QUEST_SONG_EPONA, ITEM_SONG_EPONA },
    { QUEST_SONG_SOARING, ITEM_SONG_SOARING },
    { QUEST_SONG_STORMS, ITEM_SONG_STORMS },
    { QUEST_SONG_SUN, ITEM_SONG_SUN },
    { QUEST_BOMBERS_NOTEBOOK, ITEM_BOMBERS_NOTEBOOK },
};

uint32_t Ship_ConvertQuestIdToItem(uint32_t itemId) {
    auto findQuest = questIdToItemMap.find((QuestItem)itemId);
    if (findQuest != questIdToItemMap.end()) {
        return findQuest->second;
    }
}

uint32_t Ship_ConvertItemIdToQuest(uint32_t itemId) {
    for (auto& quest : questIdToItemMap) {
        if (quest.second == itemId) {
            return quest.first;
        }
    }
}

extern "C" const char* Ship_GetSceneName(s16 sceneId) {
    if (sceneNames.contains(sceneId)) {
        return sceneNames[sceneId];
    }

    return "Unknown";
}

std::string Ship_FormatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

std::string Ship_RemoveSpecialCharacters(const std::string& str) {
    std::string result;
    for (char ch : str) {
        // Only keep alphanumeric characters (letters and digits)
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            result += ch;
        }
    }
    return result;
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
static uint64_t state = 0;
const uint64_t multiplier = 6364136223846793005ULL;
const uint64_t increment = 11634580027462260723ULL;

extern "C" void Ship_Random_Seed(u64 seed) {
    seeded = true;
    state = seed;
}

uint32_t next32() {
    if (!seeded) {
        uint64_t seed = static_cast<uint64_t>(std::random_device{}());
        Ship_Random_Seed(seed);
    }
    state = state * multiplier + increment;
    uint32_t xorshifted = static_cast<uint32_t>(((state >> 18) ^ state) >> 27);
    uint32_t rot = static_cast<int>(state >> 59);
    return std::rotr(xorshifted, rot);
}

extern "C" s32 Ship_Random(s32 min, s32 max) {
    if (min == max) {
        return min;
    }
    assert(max > min);
    uint32_t n = max - min;
    uint32_t cutoff = UINT32_MAX - UINT32_MAX % static_cast<uint32_t>(n);
    for (;;) {
        uint32_t r = next32();
        if (r <= cutoff) {
            return min + r % n;
        }
    }
}

extern uint32_t Ship_Hash(std::string str) {
    // FNV-1a
    const size_t len = str.size();
    uint32_t hval = 0x811c9dc5;
    for (size_t pos = 0; pos < len; pos++) {
        hval ^= (uint32_t)str[pos];
        hval *= 0x01000193;
    }
    return hval;
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

std::string GetActorDescription(u16 actorNum) {
    return actorDescriptions.contains(actorNum) ? actorDescriptions[actorNum] : "???";
}

std::string GetActorDebugName(u16 actorNum) {
    return actorDebugNames.contains(actorNum) ? actorDebugNames[actorNum] : "???";
}

std::string GetActorCategoryName(u8 category) {
    if (category < actorCategoryNames.size()) {
        return actorCategoryNames[category];
    }
    return "Unknown";
}
