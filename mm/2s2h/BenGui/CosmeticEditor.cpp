#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "CosmeticEditor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "macros.h"

void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);
Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                             s16 r, s16 g, s16 b, s16 a);
Gfx* Gfx_DrawTexRectIA16_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                    s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                    s16 a);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffset(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                         s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                         s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects);
void gfx_texture_cache_clear();
}

Color_RGBA8 ColorRGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Color_RGBA8 color = { r, g, b, a };
    return color;
}

static const std::map<CosmeticGroup, const char*> sCosmeticGroupLabels = {
    { COSMETICS_GROUP_PLAYER, "Player" }, { COSMETICS_GROUP_EFFECTS, "Effects" }, { COSMETICS_GROUP_TRAILS, "Trails" },
    { COSMETICS_GROUP_HUD, "HUD" },       { COSMETICS_GROUP_BUTTONS, "Buttons" }, { COSMETICS_GROUP_MENUS, "Menus" },
};

// clang-format off
std::map<std::string, CosmeticOption> cosmeticOptions = {
    COSMETIC_OPTION("HUD.Hearts",                   "Hearts",                   COSMETICS_GROUP_HUD,          ColorRGBA8(255,  70,  50, 255), false, true, false),
    COSMETIC_OPTION("HUD.Magic",                    "Magic",                    COSMETICS_GROUP_HUD,          ColorRGBA8(  0, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("HUD.SmallKey",                 "Small Key",                COSMETICS_GROUP_HUD,          ColorRGBA8(  0, 200, 230, 255), false, true, false),
    COSMETIC_OPTION("HUD.RupeeIcon",                "Rupee Icon",               COSMETICS_GROUP_HUD,          ColorRGBA8(200, 255, 100, 255), false, true, false),
    COSMETIC_OPTION("HUD.Minimap",                  "Minimap",                  COSMETICS_GROUP_HUD,          ColorRGBA8(  0, 255, 255, 160), false, true, false),
    COSMETIC_OPTION("Effects.SpinSlashCharge",      "Spin Slash Charge",        COSMETICS_GROUP_EFFECTS,      ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.SpinSlashBurst",       "Spin Slash Burst",         COSMETICS_GROUP_EFFECTS,      ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.GreatSpinCharge",      "Great Spin Charge",        COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.GreatSpinBurst",       "Great Spin Burst",         COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.FireArrowPrim",        "Fire Arrow Primary",       COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255, 200,   0, 255), false, true, false),
    COSMETIC_OPTION("Effects.FireArrowSec",         "Fire Arrow Secondary",     COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255,   0,   0, 128), false, true, false),
    COSMETIC_OPTION("Effects.IceArrowPrim",         "Ice Arrow Primary",        COSMETICS_GROUP_EFFECTS,      ColorRGBA8(170, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Effects.IceArrowSec",          "Ice Arrow Secondary",      COSMETICS_GROUP_EFFECTS,      ColorRGBA8(  0,   0, 255, 128), false, true, false),
    COSMETIC_OPTION("Effects.LightArrowPrim",       "Light Arrow Primary",      COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255, 255, 170, 255), false, true, false),
    COSMETIC_OPTION("Effects.LightArrowSec",        "Light Arrow Secondary",    COSMETICS_GROUP_EFFECTS,      ColorRGBA8(255, 255,   0, 128), false, true, false),
    COSMETIC_OPTION("Trails.KokiriSwordTrail",      "Kokiri Sword Trail",       COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.RazorSwordTrail",       "Razor Sword Trail",        COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.GildedSwordTrail",      "Gilded Sword Trail",       COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.GreatFairySwordTrail",  "Great Fairy Sword Trail",  COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.FierceDeitySwordTrail", "Fierce Deity Sword Trail", COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.DekuStickTrail",        "Deku Stick Trail",         COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.DekuSpinTrail",         "Deku Spin Trail",          COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraPunchTrail",        "Zora Punch Trail",         COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraKickTrail",         "Zora Kick Trail",          COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Trails.ZoraBoomerangTrail",    "Zora Boomerang Trail",     COSMETICS_GROUP_TRAILS,       ColorRGBA8(255, 255, 100, 255), false, true, false),
    COSMETIC_OPTION("Buttons.B",                    "B",                        COSMETICS_GROUP_BUTTONS,      ColorRGBA8(100, 255, 120, 255), false, true, false),
    COSMETIC_OPTION("Buttons.A",                    "A",                        COSMETICS_GROUP_BUTTONS,      ColorRGBA8(100, 200, 255, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CLeft",                "C Left",                   COSMETICS_GROUP_BUTTONS,      ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CDown",                "C Down",                   COSMETICS_GROUP_BUTTONS,      ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.CRight",               "C Right",                  COSMETICS_GROUP_BUTTONS,      ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Buttons.DPad",                 "D Pad",                    COSMETICS_GROUP_BUTTONS,      ColorRGBA8(255, 255, 255, 255), false, true, false),
    COSMETIC_OPTION("Buttons.Start",                "Start",                    COSMETICS_GROUP_BUTTONS,      ColorRGBA8(255, 130,  60, 255), false, true, false),
    COSMETIC_OPTION("Menus.FileWindow",             "File Select Window",       COSMETICS_GROUP_MENUS,        ColorRGBA8(100, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Menus.FilePlates",             "File Select Plates",       COSMETICS_GROUP_MENUS,        ColorRGBA8(100, 150, 255, 255), false, true, false),
    COSMETIC_OPTION("Player.HumanTunic",            "Human Tunic",              COSMETICS_GROUP_PLAYER,       ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.HumanHair",             "Human Hair",               COSMETICS_GROUP_PLAYER,       ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Player.DekuTunic",             "Deku Tunic",               COSMETICS_GROUP_PLAYER,       ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.DekuHair",              "Deku Hair",                COSMETICS_GROUP_PLAYER,       ColorRGBA8(255, 240,   0, 255), false, true, false),
    COSMETIC_OPTION("Player.GoronTunic",            "Goron Tunic",              COSMETICS_GROUP_PLAYER,       ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.ZoraTunic",             "Zora Tunic",               COSMETICS_GROUP_PLAYER,       ColorRGBA8( 30, 105,  27, 255), false, true, false),
    COSMETIC_OPTION("Player.KafeiHair",             "Kafei Hair",               COSMETICS_GROUP_PLAYER,       ColorRGBA8( 64,   0, 163, 255), false, true, false),
};
// clang-format on

static void CosmeticEditorInitializeCompatFields() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    for (auto& [id, option] : cosmeticOptions) {
        option.parentName = sCosmeticGroupLabels.at(option.group);
    }

    initialized = true;
}

static CosmeticOption& CosmeticEditor_GetOptionMutable(const char* id) {
    CosmeticEditorInitializeCompatFields();
    return cosmeticOptions.at(id);
}

static CosmeticOption& kHumanTunicOption = cosmeticOptions.at("Player.HumanTunic");
static CosmeticOption& kHumanHairOption = cosmeticOptions.at("Player.HumanHair");
static CosmeticOption& kDekuTunicOption = cosmeticOptions.at("Player.DekuTunic");
static CosmeticOption& kDekuHairOption = cosmeticOptions.at("Player.DekuHair");
static CosmeticOption& kKafeiHairOption = cosmeticOptions.at("Player.KafeiHair");
static CosmeticOption& kGoronTunicOption = cosmeticOptions.at("Player.GoronTunic");
static CosmeticOption& kZoraTunicOption = cosmeticOptions.at("Player.ZoraTunic");
static CosmeticOption& kHeartsOption = cosmeticOptions.at("HUD.Hearts");
static CosmeticOption& kMagicOption = cosmeticOptions.at("HUD.Magic");

typedef struct {
    uint16_t data1;
    uint16_t data2;
} OriginalRGB;

std::unordered_map<std::string, std::unordered_map<int, OriginalRGB>> originalRGB;

void PatchPalette(const char* path, int index, uint8_t r, uint8_t g, uint8_t b) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();

    if (!originalRGB.contains(path) || !originalRGB[path].contains(index)) {
        originalRGB[path][index] = {
            data[index * 2],
            data[index * 2 + 1],
        };
    }

    uint16_t col16 = (r << 11) | (g << 6) | (b << 1) | 1;
    data[index * 2] = col16 >> 8;
    data[index * 2 + 1] = col16 & 0xff;
}

void UnpatchPalette(const char* path, int index) {
    if (!originalRGB.contains(path) || !originalRGB[path].contains(index)) {
        return;
    }

    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();

    data[index * 2] = originalRGB[path][index].data1;
    data[index * 2 + 1] = originalRGB[path][index].data2;
}

enum SHADE_MODE {
    MODE_REVERT,
    MODE_AVG,
    MODE_MIN,
    MODE_MAX,
};

Gfx backToWhite[] = {
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx enableGrayscale[] = {
    gsSPGrayscale(1),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx disableGrayscale[] = {
    gsSPGrayscale(0),
    gsSPEndDisplayList(),
};

// First, it pulls all colors from the palette. Then it finds the average color across the range, and calculates the
// difference between the average color and the target color. It then colors the range according to newBase,
// and shades it lighter or darker based on the difference between the average color and the target color.
void ShadePaletteNewBase(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase, SHADE_MODE mode) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();

    uint32_t maxR = 0;
    uint32_t maxG = 0;
    uint32_t maxB = 0;

    for (int i = begin; i <= end; i++) {
        UnpatchPalette(path, i);

        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        maxR = MAX(maxR, r);
        maxG = MAX(maxG, g);
        maxB = MAX(maxB, b);
    }

    if (mode == MODE_REVERT) {
        return;
    }

    for (int i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        int8_t diffR = maxR - r;
        int8_t diffG = maxG - g;
        int8_t diffB = maxB - b;

        int8_t diff = 0;

        if (mode == MODE_AVG) {
            diff = (diffR + diffG + diffB) / 3;
        } else if (mode == MODE_MIN) {
            diff = MIN(MIN(diffR, diffG), diffB);
        } else if (mode == MODE_MAX) {
            diff = MAX(MAX(diffR, diffG), diffB);
        }

        diff = MIN(MAX(31 - diff, 0), 31);

        r = (diff * newBase.r) / 255;
        g = (diff * newBase.g) / 255;
        b = (diff * newBase.b) / 255;

        PatchPalette(path, i, r, g, b);
    }
}

static const Color_RGBA8 whiteBase = { 255, 255, 255, 255 };

void ShadePaletteWhite(const char* path, uint32_t begin, uint32_t end, SHADE_MODE mode) {
    ShadePaletteNewBase(path, begin, end, whiteBase, mode);
}

void ShadePaletteRevert(const char* path, uint32_t begin, uint32_t end) {
    ShadePaletteNewBase(path, begin, end, whiteBase, MODE_REVERT);
}

/*
 * Given the existing base and target color, map the current color to a new blend between the new base and target. The
 * resulting color's channels are in (0-31) format.
 */
Color_RGBA8 mapNewBaseColorToGradient(Color_RGBA8 currentColor, Color_RGBA8 oldBase, Color_RGBA8 newBase,
                                      Color_RGBA8 targetEnd) {
    double gradientRed = targetEnd.r - oldBase.r;
    double gradientGreen = targetEnd.g - oldBase.g;
    double gradientBlue = targetEnd.b - oldBase.b;

    double deltaRed = currentColor.r - oldBase.r;
    double deltaGreen = currentColor.g - oldBase.g;
    double deltaBlue = currentColor.b - oldBase.b;

    double gradient = gradientRed * gradientRed + gradientGreen * gradientGreen + gradientBlue * gradientBlue;
    double projection = gradientRed * deltaRed + gradientGreen * deltaGreen + gradientBlue * deltaBlue;

    // This condition exists in the event that oldBase and targetEnd are equal, leading to a division by zero.
    // Normally shouldn't happen, but just in case.
    double position = 0.0;
    if (gradient != 0.0) {
        position = projection / gradient;
    }

    return { uint8_t(newBase.r + position * (targetEnd.r - newBase.r)),
             uint8_t(newBase.g + position * (targetEnd.g - newBase.g)),
             uint8_t(newBase.b + position * (targetEnd.b - newBase.b)), 31 };
}

/*
 * Recolors textures that gradually transition from one color to another. Given oldBase (e.g. green parts of Zora) and
 * targetEnd (e.g. bluish skin of Zora), recolor so that the resulting texture instead fades from newBase (e.g. custom
 * tunic color) to targetEnd.
 */
void ShadePaletteGradient(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 oldBase, Color_RGBA8 newBase,
                          Color_RGBA8 targetEnd) {
    ShadePaletteRevert(path, begin, end);

    // Convert 0-255 range to 0-31 range
    newBase.r >>= 3;
    newBase.g >>= 3;
    newBase.b >>= 3;
    newBase.a >>= 3;
    targetEnd.r >>= 3;
    targetEnd.g >>= 3;
    targetEnd.b >>= 3;
    targetEnd.a >>= 3;
    oldBase.r >>= 3;
    oldBase.g >>= 3;
    oldBase.b >>= 3;
    oldBase.a >>= 3;

    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();
    for (int i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        Color_RGBA8 currentColor = { r, g, b, a };
        Color_RGBA8 newColor = mapNewBaseColorToGradient(currentColor, oldBase, newBase, targetEnd);
        PatchPalette(path, i, newColor.r, newColor.g, newColor.b);
    }
}

// Patches a single pixel in a raw RGBA16 (RGBA5551) texture, preserving the original alpha bit.
void PatchRGBA16Pixel(const char* path, int index, uint8_t r, uint8_t g, uint8_t b) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();

    if (!originalRGB.contains(path) || !originalRGB[path].contains(index)) {
        originalRGB[path][index] = {
            data[index * 2],
            data[index * 2 + 1],
        };
    }

    // Preserve the original alpha bit so transparent pixels stay transparent.
    uint8_t a = data[index * 2 + 1] & 1;
    uint16_t col16 = (r << 11) | (g << 6) | (b << 1) | a;
    data[index * 2] = col16 >> 8;
    data[index * 2 + 1] = col16 & 0xff;
}

/*
 * Recolors a raw RGBA16 (RGBA5551) texture. Unlike ShadePaletteNewBase, this preserves each
 * pixel's alpha bit and skips fully transparent pixels when computing the luminance reference,
 * so transparent background areas are left untouched.
 */
void ShadeRGBA16NewBase(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase, SHADE_MODE mode) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path);
    auto data = (uint8_t*)res->GetRawPointer();

    uint32_t maxR = 0;
    uint32_t maxG = 0;
    uint32_t maxB = 0;

    for (uint32_t i = begin; i <= end; i++) {
        UnpatchPalette(path, i);

        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        if (a == 0) {
            continue; // skip transparent pixels
        }

        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        maxR = MAX(maxR, r);
        maxG = MAX(maxG, g);
        maxB = MAX(maxB, b);
    }

    if (mode == MODE_REVERT) {
        return;
    }

    for (uint32_t i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        if (a == 0) {
            // Set transparent pixels to black so that bilinear filtering at edges
            // blends toward black (neutral) rather than bleeding the original color.
            // Using full brightness here would create a harsh colored halo at edges.
            PatchRGBA16Pixel(path, i, 0, 0, 0);
            continue;
        }

        int8_t diffR = maxR - r;
        int8_t diffG = maxG - g;
        int8_t diffB = maxB - b;

        int8_t diff = 0;
        if (mode == MODE_AVG) {
            diff = (diffR + diffG + diffB) / 3;
        } else if (mode == MODE_MIN) {
            diff = MIN(MIN(diffR, diffG), diffB);
        } else if (mode == MODE_MAX) {
            diff = MAX(MAX(diffR, diffG), diffB);
        }

        diff = MIN(MAX(31 - diff, 0), 31);

        r = (diff * newBase.r) / 255;
        g = (diff * newBase.g) / 255;
        b = (diff * newBase.b) / 255;

        PatchRGBA16Pixel(path, i, r, g, b);
    }
}

void ShadeRGBA16Revert(const char* path, uint32_t begin, uint32_t end) {
    ShadeRGBA16NewBase(path, begin, end, whiteBase, MODE_REVERT);
}

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColorEx(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                                        f32 modifier) {
    const CosmeticOption& option = CosmeticEditor_GetOptionMutable(cosmeticId);

    Color_RGBA8 returnedColor;

    if (CVarGetInteger(option.changedCvar, false)) {
        Color_RGBA8 changedColor = CVarGetColor(option.valuesCvar, option.defaultColor);
        returnedColor.r = static_cast<uint8_t>(changedColor.r);
        returnedColor.g = static_cast<uint8_t>(changedColor.g);
        returnedColor.b = static_cast<uint8_t>(changedColor.b);
        returnedColor.a = static_cast<uint8_t>(option.supportsAlpha ? changedColor.a : a);

        switch (mode) {
            case COSMETIC_COLOR_MODE_DEFAULT:
                break;
            case COSMETIC_COLOR_MODE_MULTIPLY:
                returnedColor.r = CLAMP_MAX(returnedColor.r * modifier, 255);
                returnedColor.g = CLAMP_MAX(returnedColor.g * modifier, 255);
                returnedColor.b = CLAMP_MAX(returnedColor.b * modifier, 255);
                break;
            case COSMETIC_COLOR_MODE_DIVIDE:
                if (modifier != 0) {
                    returnedColor.r = CLAMP(returnedColor.r / modifier, 0, 255);
                    returnedColor.g = CLAMP(returnedColor.g / modifier, 0, 255);
                    returnedColor.b = CLAMP(returnedColor.b / modifier, 0, 255);
                }
                break;
            case COSMETIC_COLOR_MODE_ADD:
                returnedColor.r = CLAMP(returnedColor.r + modifier, 0, 255);
                returnedColor.g = CLAMP(returnedColor.g + modifier, 0, 255);
                returnedColor.b = CLAMP(returnedColor.b + modifier, 0, 255);
                break;
            case COSMETIC_COLOR_MODE_SUBTRACT:
                returnedColor.r = CLAMP(returnedColor.r - modifier, 0, 255);
                returnedColor.g = CLAMP(returnedColor.g - modifier, 0, 255);
                returnedColor.b = CLAMP(returnedColor.b - modifier, 0, 255);
                break;
            case COSMETIC_COLOR_MODE_ROTATE: {
                // Rotate hue by modifier degrees (0-360). Example: green (0,200,0) + 120 => blue (0,0,200)
                float rf = returnedColor.r / 255.0f;
                float gf = returnedColor.g / 255.0f;
                float bf = returnedColor.b / 255.0f;

                float maxC = std::max({ rf, gf, bf });
                float minC = std::min({ rf, gf, bf });
                float delta = maxC - minC;

                float h = 0.0f, s = 0.0f, v = maxC;
                if (delta > 0.0f) {
                    s = delta / maxC;
                    if (maxC == rf) {
                        h = 60.0f * fmodf((gf - bf) / delta, 6.0f);
                    } else if (maxC == gf) {
                        h = 60.0f * ((bf - rf) / delta + 2.0f);
                    } else {
                        h = 60.0f * ((rf - gf) / delta + 4.0f);
                    }
                    if (h < 0.0f)
                        h += 360.0f;
                }

                h = fmodf(h + modifier, 360.0f);
                if (h < 0.0f)
                    h += 360.0f;

                float c = v * s;
                float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
                float m = v - c;
                float r1 = 0.0f, g1 = 0.0f, b1 = 0.0f;
                if (h < 60.0f) {
                    r1 = c;
                    g1 = x;
                    b1 = 0;
                } else if (h < 120.0f) {
                    r1 = x;
                    g1 = c;
                    b1 = 0;
                } else if (h < 180.0f) {
                    r1 = 0;
                    g1 = c;
                    b1 = x;
                } else if (h < 240.0f) {
                    r1 = 0;
                    g1 = x;
                    b1 = c;
                } else if (h < 300.0f) {
                    r1 = x;
                    g1 = 0;
                    b1 = c;
                } else {
                    r1 = c;
                    g1 = 0;
                    b1 = x;
                }

                returnedColor.r = static_cast<uint8_t>((r1 + m) * 255.0f);
                returnedColor.g = static_cast<uint8_t>((g1 + m) * 255.0f);
                returnedColor.b = static_cast<uint8_t>((b1 + m) * 255.0f);
                break;
            }
        }
    } else {
        returnedColor.r = r;
        returnedColor.g = g;
        returnedColor.b = b;
        returnedColor.a = a;
    }

    return returnedColor;
}

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" void gDPSetEnvColorOverrideEx(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                         f32 modifier) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, mode, modifier);
    gDPSetEnvColor(pkt, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return gDPSetEnvColorOverrideEx(pkt, r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" void gDPSetPrimColorOverrideEx(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId, u8 mode,
                                          f32 modifier) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColorEx(r, g, b, a, cosmeticId, mode, modifier);
    gDPSetPrimColor(pkt, m, l, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, const char* cosmeticId) {
    return gDPSetPrimColorOverrideEx(pkt, m, l, r, g, b, a, cosmeticId, COSMETIC_COLOR_MODE_DEFAULT, 0.0f);
}

extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                      s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                      u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                      const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA8_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                         rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawRect_DropShadow(pkt, rectLeft, rectTop, rectWidth, rectHeight, dsdx, dtdy, setColor.r, setColor.g,
                                   setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                       s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                       s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                       const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA16_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                          rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                            s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                            s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                                            s16 a, s32 masks, s32 rects, const char* cosmeticId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, cosmeticId);
    return Gfx_DrawTexRectIA8_DropShadowOffset(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                               rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a, masks,
                                               rects);
}

const char* kCosmeticRainbowSyncCvar = "gCosmetics.RainbowSync";
const char* kCosmeticRainbowSpeedCvar = "gCosmetics.RainbowSpeed";
const char* kCosmeticRandomizeOnSeedGenCvar = "gCosmetics.RandomizeOnSeedGen";
int sCosmeticRainbowHue = 0;

void CosmeticEditorSave() {
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

Color_RGBA8 CosmeticEditorGetDefaultColor(const CosmeticOption& option) {
    return option.defaultColor;
}

void CosmeticEditorRefreshElement(const CosmeticOption& option) {
    ShipInit::Init(option.valuesCvar);
    ShipInit::Init(option.changedCvar);
    ShipInit::Init(option.rainbowCvar);
}

void CosmeticEditorSetRainbowEnabled(const CosmeticOption& option, bool enabled, bool save = true) {
    CVarSetInteger(option.rainbowCvar, enabled);
    if (enabled) {
        CVarSetInteger(option.changedCvar, 1);
    }
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorSetLocked(const CosmeticOption& option, bool locked, bool save = true) {
    CVarSetInteger(option.lockedCvar, locked);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorResetElement(CosmeticOption& option, bool save = true) {
    if (CVarGetInteger(option.lockedCvar, 0)) {
        return;
    }
    CVarClear(option.valuesCvar);
    CVarClear(option.changedCvar);
    CVarClear(option.rainbowCvar);
    option.currentColor = ImVec4(option.defaultColor.r / 255.0f, option.defaultColor.g / 255.0f,
                                 option.defaultColor.b / 255.0f, option.defaultColor.a / 255.0f);
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorRandomizeElement(CosmeticOption& option, bool save = true) {
    if (CVarGetInteger(option.lockedCvar, 0)) {
        return;
    }
    ImVec4 colorVec = GetRandomValue();
    Color_RGBA8 color = { static_cast<uint8_t>(colorVec.x * 255.0f), static_cast<uint8_t>(colorVec.y * 255.0f),
                          static_cast<uint8_t>(colorVec.z * 255.0f),
                          static_cast<uint8_t>(option.supportsAlpha ? option.currentColor.w * 255.0f : 255) };
    option.currentColor = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    CVarSetColor(option.valuesCvar, color);
    CVarSetInteger(option.changedCvar, 1);
    CVarSetInteger(option.rainbowCvar, 0);
    CosmeticEditorRefreshElement(option);
    if (save) {
        CosmeticEditorSave();
    }
}

void CosmeticEditorRandomizeAllElements() {
    for (auto& [id, option] : cosmeticOptions) {
        CosmeticEditorRandomizeElement(option, false);
    }
    CosmeticEditorSave();
}

void CosmeticEditorResetAllElements() {
    for (auto& [id, option] : cosmeticOptions) {
        CosmeticEditorResetElement(option, false);
    }
    CosmeticEditorSave();
}

void CosmeticEditorSetAllLocked(bool locked) {
    for (auto& [id, option] : cosmeticOptions) {
        CosmeticEditorSetLocked(option, locked, false);
    }
    CosmeticEditorSave();
}

void CosmeticEditorSetAllRainbow(bool enabled) {
    for (auto& [id, option] : cosmeticOptions) {
        if (!option.supportsRainbow || CVarGetInteger(option.lockedCvar, 0)) {
            continue;
        }
        CosmeticEditorSetRainbowEnabled(option, enabled, false);
    }
    CosmeticEditorSave();
}

bool CosmeticEditorMatchesGroup(const CosmeticOption& option, CosmeticGroup group) {
    return option.group == group;
}

void CosmeticEditorRandomizeGroup(CosmeticGroup group) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group)) {
            CosmeticEditorRandomizeElement(option, false);
        }
    }
    CosmeticEditorSave();
}

void CosmeticEditorResetGroup(CosmeticGroup group) {
    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group)) {
            CosmeticEditorResetElement(option, false);
        }
    }
    CosmeticEditorSave();
}

void CosmeticEditorUpdateTick() {
    int index = 0;
    float rainbowSpeed = CVarGetFloat(kCosmeticRainbowSpeedCvar, 0.6f);
    if (rainbowSpeed <= 0.0f) {
        rainbowSpeed = 0.6f;
    }

    bool hasRainbowEntries = false;
    bool syncRainbow = CVarGetInteger(kCosmeticRainbowSyncCvar, 0);

    for (auto& [id, option] : cosmeticOptions) {
        if (!option.supportsRainbow || !CVarGetInteger(option.rainbowCvar, 0)) {
            if (!syncRainbow) {
                index += static_cast<int>(60 * rainbowSpeed);
            }
            continue;
        }

        hasRainbowEntries = true;

        double frequency = 2 * M_PI / (360 * rainbowSpeed);
        Color_RGBA8 color = {
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + 0) * 127) + 128,
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + (2 * M_PI / 3)) * 127) + 128,
            static_cast<uint8_t>(sin(frequency * (sCosmeticRainbowHue + index) + (4 * M_PI / 3)) * 127) + 128,
            static_cast<uint8_t>(option.supportsAlpha ? option.currentColor.w * 255.0f : 255),
        };

        option.currentColor = ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
        CVarSetColor(option.valuesCvar, color);
        CVarSetInteger(option.changedCvar, 1);
        CosmeticEditorRefreshElement(option);

        if (!syncRainbow) {
            index += static_cast<int>(60 * rainbowSpeed);
        }
    }

    if (!hasRainbowEntries) {
        return;
    }

    sCosmeticRainbowHue++;
    if (sCosmeticRainbowHue >= (360 * rainbowSpeed)) {
        sCosmeticRainbowHue = 0;
    }
}

void CosmeticEditorDrawRow(CosmeticOption& option) {
    Color_RGBA8 defaultColor = CosmeticEditorGetDefaultColor(option);

    if (UIWidgets::CVarColorPicker(option.label, option.valuesCvar, defaultColor, option.supportsAlpha,
                                   option.lockedCvar, THEME_COLOR)) {
        Color_RGBA8 changedColor = CVarGetColor(option.valuesCvar, defaultColor);
        option.currentColor =
            ImVec4(changedColor.r / 255.0f, changedColor.g / 255.0f, changedColor.b / 255.0f, changedColor.a / 255.0f);
        CVarSetInteger(option.changedCvar, 1);
        CosmeticEditorRefreshElement(option);
        CosmeticEditorSave();
    }

    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + std::string(option.label)).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeElement(option);
    }

    ImGui::SameLine();
    bool rainbowEnabled = CVarGetInteger(option.rainbowCvar, 0);
    if (UIWidgets::Checkbox(("Rainbow##" + std::string(option.label)).c_str(), &rainbowEnabled,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        CosmeticEditorSetRainbowEnabled(option, rainbowEnabled);
    }

    ImGui::SameLine();
    bool locked = CVarGetInteger(option.lockedCvar, 0);
    if (UIWidgets::Checkbox(("Locked##" + std::string(option.label)).c_str(), &locked,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR))) {
        CosmeticEditorSetLocked(option, locked);
    }

    if (CVarGetInteger(option.changedCvar, 0)) {
        ImGui::SameLine();
        if (UIWidgets::Button(("Reset##" + std::string(option.label)).c_str(),
                              UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
            CosmeticEditorResetElement(option);
        }
    }
}

void CosmeticEditorDrawGroup(CosmeticGroup group, const char* displayName = nullptr) {
    std::string label = displayName != nullptr ? displayName : sCosmeticGroupLabels.at(group);
    ImGui::Text("%s", label.c_str());
    ImGui::SameLine((ImGui::CalcTextSize("Message Light Blue (None No Shadow)").x * 1.0f) + 60.0f);
    if (UIWidgets::Button(
            ("Random##" + label).c_str(),
            UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeGroup(group);
    }
    ImGui::SameLine();
    if (UIWidgets::Button(("Reset##" + label).c_str(),
                          UIWidgets::ButtonOptions().Size(ImVec2(80, 31)).Padding(ImVec2(2.0f, 0.0f)))) {
        CosmeticEditorResetGroup(group);
    }
    UIWidgets::Spacer();

    for (auto& [id, option] : cosmeticOptions) {
        if (CosmeticEditorMatchesGroup(option, group)) {
            CosmeticEditorDrawRow(option);
        }
    }

    UIWidgets::Separator(true, true, 2.0f, 2.0f);
}

void CosmeticEditorWindow::DrawElement() {
    UIWidgets::CVarCheckbox("Sync Rainbow colors", kCosmeticRainbowSyncCvar,
                            UIWidgets::CheckboxOptions()
                                .Color(THEME_COLOR)
                                .Tooltip("Keeps all rainbow-enabled cosmetics on the same hue cycle."));
    UIWidgets::CVarSliderFloat("Rainbow Speed", kCosmeticRainbowSpeedCvar,
                               UIWidgets::FloatSliderOptions()
                                   .Format("%.2f")
                                   .Min(0.01f)
                                   .Max(1.0f)
                                   .DefaultValue(0.6f)
                                   .Step(0.01f)
                                   .Size(ImVec2(300.0f, 0.0f))
                                   .Color(THEME_COLOR));
    UIWidgets::CVarCheckbox(
        "Randomize all Cosmetics on Randomizer Generation", kCosmeticRandomizeOnSeedGenCvar,
        UIWidgets::CheckboxOptions()
            .Color(THEME_COLOR)
            .Tooltip("Randomizes every unlocked cosmetic entry when a new randomizer seed is generated."));

    if (UIWidgets::Button("Randomize All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorRandomizeAllElements();
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorResetAllElements();
    }

    if (UIWidgets::Button("Lock All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllLocked(true);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Unlock All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllLocked(false);
    }

    if (UIWidgets::Button("Rainbow All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllRainbow(true);
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Un-Rainbow All", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        CosmeticEditorSetAllRainbow(false);
    }

    UIWidgets::Spacer(3.0f);

    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("CosmeticsContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Link & Items")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_PLAYER, "Link");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Effects")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_EFFECTS);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_TRAILS);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("HUD")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_HUD);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_BUTTONS);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Pause Menu")) {
            UIWidgets::Separator(true, true, 2.0f, 2.0f);
            CosmeticEditorDrawGroup(COSMETICS_GROUP_MENUS);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();
}

void CosmeticEditorWindow::InitElement() {
    static bool sHooksRegistered = false;
    if (sHooksRegistered) {
        return;
    }

    for (auto& [id, option] : cosmeticOptions) {
        Color_RGBA8 cvarColor = CVarGetColor(option.valuesCvar, option.defaultColor);
        option.currentColor =
            ImVec4(cvarColor.r / 255.0f, cvarColor.g / 255.0f, cvarColor.b / 255.0f, cvarColor.a / 255.0f);
        CosmeticEditorRefreshElement(option);
    }
    CosmeticEditorSave();

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoSeedGeneration>([]() {
        if (CVarGetInteger(kCosmeticRandomizeOnSeedGenCvar, 0)) {
            CosmeticEditorRandomizeAllElements();
        }
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateUpdate>([]() { CosmeticEditorUpdateTick(); });

    sHooksRegistered = true;
}

// Player.HumanTunic

Gfx humanTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanTunicPatch(
    []() {
        if (CVarGetInteger(kHumanTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanWaistDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightThighDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftThighDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim", 92,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHatDL", "setPrim", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanCollarDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim1", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim2", 65,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim1", 10,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim2", 65,
                                       gsSPDisplayList(humanTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanTorsoDL", "setPrim", 5,
                                       gsSPDisplayList(humanTunic));
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightThighDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftThighDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHatDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanCollarDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanLeftShoulderDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanRightShoulderDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanTorsoDL", "setPrim");
        }
    },
    { kHumanTunicOption.colorChangedCvar });

static RegisterShipInitFunc humanTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kHumanTunicOption.colorCvar, {});
        humanTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kHumanTunicOption.colorCvar });

// Player.HumanHair

Gfx humanHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanHairPatch(
    []() {
        if (CVarGetInteger(kHumanHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim1", 162,
                                       gsSPDisplayList(humanHair));
            ResourceMgr_PatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim2", 201,
                                       gsSPDisplayList(backToWhite));

            ShadePaletteWhite("objects/object_link_child/object_link_child_Tex_005400", 0, 127, MODE_AVG);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_child/gLinkHumanHeadDL", "setPrim2");

            ShadePaletteRevert("objects/object_link_child/object_link_child_Tex_005400", 0, 127);
        }
        gfx_texture_cache_clear();
    },
    { kHumanHairOption.colorChangedCvar });

static RegisterShipInitFunc humanHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kHumanHairOption.colorCvar, {});
        humanHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kHumanHairOption.colorCvar });

// Player.DekuTunic

Gfx dekuTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuTunicPatch(
    []() {
        if (CVarGetInteger(kDekuTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuWaistDL", "setPrim", 22,
                                       gsSPDisplayList(dekuTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim1", 55,
                                       gsSPDisplayList(dekuTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim2", 76,
                                       gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHatDL", "setPrim", 29,
                                       gsSPDisplayList(dekuTunic));

            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 243, 254, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHatDL", "setPrim");

            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 243, 254);
        }
        gfx_texture_cache_clear();
    },
    { kDekuTunicOption.colorChangedCvar });

static RegisterShipInitFunc dekuTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kDekuTunicOption.colorCvar, {});
        dekuTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kDekuTunicOption.colorCvar });

// Player.DekuHair

Gfx dekuHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuHairPatch(
    []() {
        if (CVarGetInteger(kDekuHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim3", 22,
                                       gsSPDisplayList(dekuHair));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim4", 42,
                                       gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName("objects/object_link_nuts/object_link_nuts_DL_009C48", "setPrim", 22,
                                       gsSPDisplayList(dekuHair));

            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 109, 122, MODE_MAX);
            ShadePaletteWhite("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 124, 242, MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim3");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/gLinkDekuHeadDL", "setPrim4");
            ResourceMgr_UnpatchGfxByName("objects/object_link_nuts/object_link_nuts_DL_009C48", "setPrim");

            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 109, 122);
            ShadePaletteRevert("objects/object_link_nuts/object_link_nuts_TLUT_003EB0", 124, 242);
        }
        gfx_texture_cache_clear();
    },
    { kDekuHairOption.colorChangedCvar });

static RegisterShipInitFunc dekuHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kDekuHairOption.colorCvar, {});
        dekuHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kDekuHairOption.colorCvar });

// Player.KafeiHair

Gfx kafeiHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc kafeiHairPatch(
    []() {
        if (CVarGetInteger(kKafeiHairOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim1", 101,
                                       gsSPDisplayList(kafeiHair));
            ResourceMgr_PatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim2", 163,
                                       gsSPDisplayList(backToWhite));
            ResourceMgr_PatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim3", 200,
                                       gsSPDisplayList(kafeiHair));
            ResourceMgr_PatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim4", 236,
                                       gsSPDisplayList(backToWhite));

            ShadePaletteWhite("objects/object_test3/gKafeiBody2TLUT", 1, 3, MODE_MIN);
            ShadePaletteWhite("objects/object_test3/gKafeiBody2TLUT", 8, 255, MODE_MIN);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim1");
            ResourceMgr_UnpatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim2");
            ResourceMgr_UnpatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim3");
            ResourceMgr_UnpatchGfxByName("objects/object_test3/gKafeiHeadDL", "setPrim4");

            ShadePaletteRevert("objects/object_test3/gKafeiBody2TLUT", 1, 3);
            ShadePaletteRevert("objects/object_test3/gKafeiBody2TLUT", 8, 255);
        }
        gfx_texture_cache_clear();
    },
    { kKafeiHairOption.colorChangedCvar });

static RegisterShipInitFunc kafeiHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kKafeiHairOption.colorCvar, {});
        kafeiHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kKafeiHairOption.colorCvar });

// Player.GoronTunic

Gfx goronTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc goronTunicPatch(
    []() {
        if (CVarGetInteger(kGoronTunicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_link_goron/gLinkGoronWaistDL", "setPrim", 16,
                                       gsSPDisplayList(goronTunic));
            ResourceMgr_PatchGfxByName("objects/object_link_goron/gLinkGoronHatDL", "setPrim", 17,
                                       gsSPDisplayList(goronTunic));

            ShadePaletteWhite("objects/object_link_goron/object_link_goron_Tex_002780", 0, 127, MODE_MAX);
            /*
             * gLinkGoronCurledDL contains information for applying the hat texture when Link is Goron rolling, but it
             * does not seem to obey color for anything but the necklace beads. Instead, directly set the color of the
             * texture.
             */
            Color_RGBA8 changedColor = CVarGetColor(kGoronTunicOption.colorCvar, {});
            ShadePaletteNewBase("objects/object_link_goron/object_link_goron_Tex_00CEB8", 0, 127, changedColor,
                                MODE_MAX);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_link_goron/gLinkGoronWaistDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_link_goron/gLinkGoronHatDL", "setPrim");

            ShadePaletteRevert("objects/object_link_goron/object_link_goron_Tex_002780", 0, 127);
            ShadePaletteRevert("objects/object_link_goron/object_link_goron_Tex_00CEB8", 0, 127);
        }
        gfx_texture_cache_clear();
    },
    { kGoronTunicOption.colorChangedCvar });

static RegisterShipInitFunc goronTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(kGoronTunicOption.colorCvar, {});
        goronTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { kGoronTunicOption.colorCvar });

// Player.ZoraTunic
static const Color_RGBA8 zoraSkinColor = { 197, 247, 247, 255 };
static const Color_RGBA8 zoraTunicBaseColor = { 0, 74, 16, 255 };

static RegisterShipInitFunc zoraTunicPatch(
    []() {
        if (CVarGetInteger(kZoraTunicOption.colorChangedCvar, 0)) {
            /*
             * Zora works differently from the other color changes. Other forms apply a grayscale to the green tunic
             * textures and then alter the Gfx commands to set the color. That works because those textures are one
             * basic color. Zora, however, gradually transitions from green to the bluish Zora skin. A further
             * complication is that relevant colors in the TLUTs are not contiguous, so the brightness calculation will
             * not work as intended. Instead of using the palette approach, here we directly apply the custom color to
             * the textures and TLUTs.
             */
            Color_RGBA8 changedColor = CVarGetColor(kZoraTunicOption.colorCvar, {});
            // Arms
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 151, 177, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 179, 180, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_00C578", 183, 183, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);

            // Hat/head and pants
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 151, 177, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 179, 180, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_TLUT_005000", 183, 183, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);

            // Shield
            ShadePaletteGradient("objects/object_link_zora/object_link_zora_Tex_010228", 80, 511, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);

            // Boomerangs
            ShadePaletteGradient("objects/gameplay_keep/gameplay_keep_Tex_0700B0", 80, 511, zoraTunicBaseColor,
                                 changedColor, zoraSkinColor);
        } else {
            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 151, 177);
            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 179, 180);
            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_00C578", 183, 183);

            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 151, 177);
            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 179, 180);
            ShadePaletteRevert("objects/object_link_zora/object_link_zora_TLUT_005000", 183, 183);

            ShadePaletteRevert("objects/object_link_zora/object_link_zora_Tex_010228", 80, 511);
            ShadePaletteRevert("objects/gameplay_keep/gameplay_keep_Tex_0700B0", 80, 511);
        }
        gfx_texture_cache_clear();
    },
    { kZoraTunicOption.colorChangedCvar });

// HUD.Hearts

Gfx heartsColorDL[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPSetGrayscaleColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

Gfx heartsEnvColorDL[] = {
    gsDPSetEnvColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc heartsColorDLPatch(
    []() {
        if (CVarGetInteger(kHeartsOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "enableGrayscale", 2,
                                       gsSPDisplayList(enableGrayscale));
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "setPrim", 5,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "disableGrayscale", 36,
                                       gsSPBranchList(disableGrayscale));
            ResourceMgr_PatchGfxByName("objects/gameplay_keep/gHeartPieceInteriorDL", "setPrim", 56,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setPrim", 2,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setPrim", 2,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setPrim", 5,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setEnv", 6,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setPrim", 3,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setEnv", 4,
                                       gsSPDisplayList(heartsEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setPrim", 3,
                                       gsSPDisplayList(heartsColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setEnv", 4,
                                       gsSPDisplayList(heartsEnvColorDL));

            Color_RGBA8 changedColor = CVarGetColor(kHeartsOption.colorCvar, {});
            ShadeRGBA16NewBase("objects/gameplay_keep/gDropRecoveryHeartTex", 0, 1023, changedColor, MODE_AVG);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "enableGrayscale");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_heart/gGiRecoveryHeartDL", "disableGrayscale");
            ResourceMgr_UnpatchGfxByName("objects/gameplay_keep/gHeartPieceInteriorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartPieceDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_hearts/gGiHeartContainerDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_bottle_red/gGiRedPotionBottleDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPotColorDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerRedPatternColorDL", "setEnv");

            ShadeRGBA16Revert("objects/gameplay_keep/gDropRecoveryHeartTex", 0, 1023);
        }
        gfx_texture_cache_clear();
    },
    { kHeartsOption.colorChangedCvar });

static RegisterShipInitFunc heartsColorDLUpdate(
    []() {
        Color_RGBA8 primColor = CosmeticEditor_GetChangedColor(0, 0, 0, 0, "HUD.Hearts");
        heartsColorDL[0] = gsDPSetPrimColor(0, 0, primColor.r, primColor.g, primColor.b, 255);
        heartsColorDL[1] = gsDPSetGrayscaleColor(primColor.r, primColor.g, primColor.b, 255);

        Color_RGBA8 envColor =
            CosmeticEditor_GetChangedColorEx(0, 0, 0, 0, "HUD.Hearts", COSMETIC_COLOR_MODE_DIVIDE, 2.0f);
        heartsEnvColorDL[0] = gsDPSetEnvColor(envColor.r, envColor.g, envColor.b, 255);
    },
    { kHeartsOption.colorCvar });

// HUD.Magic

Gfx magicColorDL[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPSetGrayscaleColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

Gfx magicEnvColorDL[] = {
    gsDPSetEnvColor(0, 0, 0, 0),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc magicColorDLPatch(
    []() {
        if (CVarGetInteger(kMagicOption.colorChangedCvar, 0)) {
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setPrim", 31,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setEnv", 32,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setPrim", 31,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setEnv", 32,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setPrim", 3,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setEnv", 4,
                                       gsSPDisplayList(magicEnvColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setPrim", 3,
                                       gsSPDisplayList(magicColorDL));
            ResourceMgr_PatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setEnv", 4,
                                       gsSPDisplayList(magicEnvColorDL));

            Color_RGBA8 changedColor = CVarGetColor(kMagicOption.colorCvar, {});
            ShadeRGBA16NewBase("objects/gameplay_keep/gDropMagicSmallTex", 0, 1023, changedColor, MODE_AVG);
            ShadeRGBA16NewBase("objects/gameplay_keep/gDropMagicLargeTex", 0, 1023, changedColor, MODE_AVG);
        } else {
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarLargeDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_magicpot/gGiMagicJarSmallDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPotColorDL", "setEnv");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setPrim");
            ResourceMgr_UnpatchGfxByName("objects/object_gi_liquid/gGiPotionContainerGreenPatternColorDL", "setEnv");

            ShadeRGBA16Revert("objects/gameplay_keep/gDropMagicSmallTex", 0, 1023);
            ShadeRGBA16Revert("objects/gameplay_keep/gDropMagicLargeTex", 0, 1023);
        }
        gfx_texture_cache_clear();
    },
    { kMagicOption.colorChangedCvar });

static RegisterShipInitFunc magicColorDLUpdate(
    []() {
        Color_RGBA8 primColor = CosmeticEditor_GetChangedColor(0, 0, 0, 0, "HUD.Magic");
        magicColorDL[0] = gsDPSetPrimColor(0, 0, primColor.r, primColor.g, primColor.b, 255);
        magicColorDL[1] = gsDPSetGrayscaleColor(primColor.r, primColor.g, primColor.b, 255);

        Color_RGBA8 envColor =
            CosmeticEditor_GetChangedColorEx(0, 0, 0, 0, "HUD.Magic", COSMETIC_COLOR_MODE_DIVIDE, 2.0f);
        magicEnvColorDL[0] = gsDPSetEnvColor(envColor.r, envColor.g, envColor.b, 255);
    },
    { kMagicOption.colorCvar });
