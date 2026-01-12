#include "2s2h/BenGui/UIWidgets.hpp"
#include "CosmeticEditor.h"
#include "2s2h/ShipInit.hpp"

std::vector<const char*> cosmeticEditorParentElements;

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

// clang-format off
CosmeticEditorElement cosmeticEditorElements[COSMETIC_ELEMENT_MAX] = {
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_NORMAL,              "Hearts",           "Normal",                     "Hearts.Normal",                255, 70,  50,  255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_NORMAL_BEATING,      "Hearts",           "Beating",                    "Hearts.Beating",               255, 70,  50,  255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_BORDER,              "Hearts",           "Border",                     "Hearts.Border",                50,  40,  60,  255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD,                  "Hearts",           "DD",                         "Hearts.DD",                    200, 0,   0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD_BEATING,          "Hearts",           "DD Beating",                 "Hearts.DDBeating",             200, 0,   0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HEART_DD_BORDER,           "Hearts",           "DD Border",                  "Hearts.DDBorder",              255, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_NORMAL,              "Magic Meter",      "Normal",                     "Magic.Normal",                 0,   200, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_CHATEAU,             "Magic Meter",      "Chateau",                    "Magic.Chateau",                0,   0,   200, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_CONSUMED,            "Magic Meter",      "Active",                     "Magic.Active",                 250, 250, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MAGIC_BORDER,              "Magic Meter",      "Border",                     "Magic.Border",                 255, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SMALL_KEY,                 "HUD",              "Small Key",                  "HUD.SmallKey",                 0,   200, 230, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_RUPEE_ICON,                "HUD",              "Rupee Icon",                 "HUD.RupeeIcon",                200, 255, 100, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_MINIMAP,                   "HUD",              "Minimap",                    "HUD.Minimap",                  0,   255, 255, 160),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SPIN_SLASH_CHARGE,         "Effects",          "Spin Slash Charge",          "Effects.SpinSlashCharge",      170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_SPIN_SLASH_BURST,          "Effects",          "Spin Slash Burst",           "Effects.SpinSlashBurst",       170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_GREAT_SPIN_CHARGE,         "Effects",          "Great Spin Charge",          "Effects.GreatSpinCharge",      255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_GREAT_SPIN_BURST,          "Effects",          "Great Spin Burst",           "Effects.GreatSpinBurst",       255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FIRE_ARROW_PRIMARY,        "Effects",          "Fire Arrow Primary",         "Effects.FireArrowPrim",        255, 200, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FIRE_ARROW_SECONDARY,      "Effects",          "Fire Arrow Secondary",       "Effects.FireArrowSec",         255, 0,   0,   128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_ICE_ARROW_PRIMARY,         "Effects",          "Ice Arrow Primary",          "Effects.IceArrowPrim",         170, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_ICE_ARROW_SECONDARY,       "Effects",          "Ice Arrow Secondary",        "Effects.IceArrowSec",          0,   0,   255, 128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_LIGHT_ARROW_PRIMARY,       "Effects",          "Light Arrow Primary",        "Effects.LightArrowPrim",       255, 255, 170, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_LIGHT_ARROW_SECONDARY,     "Effects",          "Light Arrow Secondary",      "Effects.LightArrowSec",        255, 255, 0,   128),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_B_BUTTON,                  "Buttons",          "B",                          "Buttons.B",                    100, 255, 120, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_A_BUTTON,                  "Buttons",          "A",                          "Buttons.A",                    100, 200, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_LEFT_BUTTON,             "Buttons",          "C Left",                     "Buttons.CLeft",                255, 240, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_DOWN_BUTTON,             "Buttons",          "C Down",                     "Buttons.CDown",                255, 240, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_C_RIGHT_BUTTON,            "Buttons",          "C Right",                    "Buttons.CRight",               255, 240, 0,   255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_D_PAD_BUTTON,              "Buttons",          "D Pad",                      "Buttons.DPad",                 255, 255, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_START_BUTTON,              "Buttons",          "Start",                      "Buttons.Start",                255, 130, 60,  255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FILE_SELECT_MENU,          "Menus",            "File Select Window",         "Menus.FileWindow",             100, 150, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_FILE_SELECT_PLATES,        "Menus",            "File Select Plates",         "Menus.FilePlates",             100, 150, 255, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HUMAN_TUNIC,               "Player",           "Human Tunic",                "Player.HumanTunic",            30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_HUMAN_HAIR,                "Player",           "Human Hair",                 "Player.HumanHair",             30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_DEKU_TUNIC,                "Player",           "Deku Tunic",                 "Player.DekuTunic",             30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_DEKU_HAIR,                 "Player",           "Deku Hair",                  "Player.DekuHair",              30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_GORON_TUNIC,               "Player",           "Goron Tunic",                "Player.GoronTunic",            30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_ZORA_TUNIC,                "Player",           "Zora Tunic",                 "Player.ZoraTunic",             30, 105,  27, 255),
    COSMETIC_EDITOR_ELEMENT(COSMETIC_ELEMENT_KAFEI_HAIR,                "Player",           "Kafei Hair",                 "Player.KafeiHair",             30, 105,  27, 255),
};
// clang-format on

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

void CopyFloatArray(CosmeticEditorElementID id, float currentColor[4], bool isChanged) {
    if (isChanged) {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[id].colorCvar, {});
        currentColor[0] = changedColor.r / 255.0f;
        currentColor[1] = changedColor.g / 255.0f;
        currentColor[2] = changedColor.b / 255.0f;
        currentColor[3] = 1.0f;
    } else {
        currentColor[0] = cosmeticEditorElements[id].defaultR / 255.0f;
        currentColor[1] = cosmeticEditorElements[id].defaultG / 255.0f;
        currentColor[2] = cosmeticEditorElements[id].defaultB / 255.0f;
        currentColor[3] = cosmeticEditorElements[id].defaultA / 255.0f;
    }
};

extern "C" Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    CosmeticEditorElement element = cosmeticEditorElements[elementId];

    Color_RGBA8 returnedColor;

    if (CVarGetInteger(element.colorChangedCvar, false)) {
        Color_RGBA8 changedColor = CVarGetColor(element.colorCvar, {});
        returnedColor.r = static_cast<uint8_t>(changedColor.r);
        returnedColor.g = static_cast<uint8_t>(changedColor.g);
        returnedColor.b = static_cast<uint8_t>(changedColor.b);
        returnedColor.a = static_cast<uint8_t>(255);
    } else {
        returnedColor.r = r;
        returnedColor.g = g;
        returnedColor.b = b;
        returnedColor.a = a;
    }

    return returnedColor;
}

extern "C" void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    gDPSetEnvColor(pkt, setColor.r, setColor.g, setColor.b, a);
}

extern "C" void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    gDPSetPrimColor(pkt, m, l, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                      s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                      u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA8_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                         rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}

extern "C" Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight,
                                                u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawRect_DropShadow(pkt, rectLeft, rectTop, rectWidth, rectHeight, dsdx, dtdy, setColor.r, setColor.g,
                                   setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                       s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                       s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b, s16 a,
                                                       u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA16_DropShadow(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                          rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a);
}
extern "C" Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth,
                                                            s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth,
                                                            s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                                            s16 a, s32 masks, s32 rects, u8 elementId) {
    Color_RGBA8 setColor = CosmeticEditor_GetChangedColor(r, g, b, a, elementId);
    return Gfx_DrawTexRectIA8_DropShadowOffset(pkt, texture, textureWidth, textureHeight, rectLeft, rectTop, rectWidth,
                                               rectHeight, dsdx, dtdy, setColor.r, setColor.g, setColor.b, a, masks,
                                               rects);
}

void CosmeticEditorRandomizeElement(CosmeticEditorElement element) {
    Color_RGBA8 colorSelected;
    colorSelected.r = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.g = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.b = static_cast<uint8_t>((rand() % 256) * 255.0f);
    colorSelected.a = static_cast<uint8_t>(255);

    CVarSetColor(element.colorCvar, colorSelected);
    CVarSetInteger(element.colorChangedCvar, true);
    ShipInit::Init(element.colorCvar);
    ShipInit::Init(element.colorChangedCvar);
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void CosmeticEditorRandomizeAllElements() {
    for (auto& element : cosmeticEditorElements) {
        CosmeticEditorRandomizeElement(element);
    }
}

void CosmeticEditorResetAllElements() {
    for (auto& element : cosmeticEditorElements) {
        CVarClear(element.colorCvar);
        CVarClear(element.colorChangedCvar);
        ShipInit::Init(element.colorCvar);
        ShipInit::Init(element.colorChangedCvar);
    }
}

void CosmeticEditorDrawColorTab() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    std::string resetAllText = ICON_FA_UNDO;
    resetAllText += " All";
    std::string randomAllText = ICON_FA_RECYCLE;
    randomAllText += " All";
    if (ImGui::Button(resetAllText.c_str())) {
        CosmeticEditorResetAllElements();
    }
    UIWidgets::Tooltip("Resets All Elements to their Defaults");
    ImGui::SameLine();
    if (ImGui::Button(randomAllText.c_str())) {
        CosmeticEditorRandomizeAllElements();
    }
    UIWidgets::Tooltip("Randomizes All Elements");
    for (auto& parent : cosmeticEditorParentElements) {
        ImGui::SeparatorText(parent);
        ImGui::BeginTable(parent, 2);
        ImGui::TableSetupColumn("Element Name", ImGuiTableColumnFlags_WidthStretch, 1.4f);
        ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        for (auto& entry : cosmeticEditorElements) {
            if (parent != entry.parentName) {
                continue;
            }
            float currentColor[4];

            ImGui::PushID(entry.id);
            ImGui::TableNextColumn();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
                                 ((ImGui::GetFrameHeight() - (ImGui::CalcTextSize(entry.name).y)) / 2.0f));
            ImGui::Text("%s", entry.name);
            ImGui::TableNextColumn();
            CopyFloatArray(entry.id, currentColor, CVarGetInteger(entry.colorChangedCvar, false));
            bool colorChanged =
                ImGui::ColorEdit3("Color", currentColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (colorChanged) {
                Color_RGBA8 colorSelected;
                colorSelected.r = static_cast<uint8_t>(currentColor[0] * 255.0f);
                colorSelected.g = static_cast<uint8_t>(currentColor[1] * 255.0f);
                colorSelected.b = static_cast<uint8_t>(currentColor[2] * 255.0f);
                colorSelected.a = static_cast<uint8_t>(255);

                CVarSetColor(entry.colorCvar, colorSelected);
                CVarSetInteger(entry.colorChangedCvar, true);
                ShipInit::Init(entry.colorCvar);
                ShipInit::Init(entry.colorChangedCvar);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_UNDO, ImVec2(27.0f, 27.0f))) {
                CVarClear(entry.colorCvar);
                CVarClear(entry.colorChangedCvar);
                ShipInit::Init(entry.colorCvar);
                ShipInit::Init(entry.colorChangedCvar);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_RECYCLE, ImVec2(27.0f, 27.0f))) {
                CosmeticEditorRandomizeElement(entry);
            }
            ImGui::SameLine();
            ImGui::TextColored(CVarGetInteger(entry.colorChangedCvar, 0)
                                   ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                   : UIWidgets::ColorValues.at(UIWidgets::Colors::Gray),
                               CVarGetInteger(entry.colorChangedCvar, 0) ? "Modified" : "Default");
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleColor(3);
}

// Tab Bar is unused until other options are available.
void CosmeticEditorDrawTabBar() {
    ImGui::BeginTabBar("Cosmetic Tab Bar");
    if (ImGui::BeginTabItem("Colors")) {
        CosmeticEditorDrawColorTab();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}

void CosmeticEditorWindow::DrawElement() {
    CosmeticEditorDrawColorTab();
}

void CosmeticEditorWindow::InitElement() {
    cosmeticEditorParentElements.clear();
    for (auto& element : cosmeticEditorElements) {
        if (std::find(cosmeticEditorParentElements.begin(), cosmeticEditorParentElements.end(), element.parentName) !=
            cosmeticEditorParentElements.end()) {
            continue;
        }
        cosmeticEditorParentElements.push_back(element.parentName);
    }
}

// COSMETIC_ELEMENT_HUMAN_TUNIC

Gfx humanTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanTunicPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_TUNIC].colorChangedCvar, 0)) {
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_TUNIC].colorChangedCvar });

static RegisterShipInitFunc humanTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_TUNIC].colorCvar, {});
        humanTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_TUNIC].colorCvar });

// COSMETIC_ELEMENT_HUMAN_HAIR

Gfx humanHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc humanHairPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_HAIR].colorChangedCvar, 0)) {
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_HAIR].colorChangedCvar });

static RegisterShipInitFunc humanHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_HAIR].colorCvar, {});
        humanHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_HUMAN_HAIR].colorCvar });

// COSMETIC_ELEMENT_DEKU_TUNIC

Gfx dekuTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuTunicPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_TUNIC].colorChangedCvar, 0)) {
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_TUNIC].colorChangedCvar });

static RegisterShipInitFunc dekuTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_TUNIC].colorCvar, {});
        dekuTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_TUNIC].colorCvar });

// COSMETIC_ELEMENT_DEKU_HAIR

Gfx dekuHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc dekuHairPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_HAIR].colorChangedCvar, 0)) {
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_HAIR].colorChangedCvar });

static RegisterShipInitFunc dekuHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_HAIR].colorCvar, {});
        dekuHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_DEKU_HAIR].colorCvar });

// COSMETIC_ELEMENT_KAFEI_HAIR

Gfx kafeiHair[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc kafeiHairPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_KAFEI_HAIR].colorChangedCvar, 0)) {
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_KAFEI_HAIR].colorChangedCvar });

static RegisterShipInitFunc kafeiHairColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_KAFEI_HAIR].colorCvar, {});
        kafeiHair[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_KAFEI_HAIR].colorCvar });

// COSMETIC_ELEMENT_GORON_TUNIC

Gfx goronTunic[] = {
    gsDPSetPrimColor(0, 0, 0, 0, 0, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static RegisterShipInitFunc goronTunicPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_GORON_TUNIC].colorChangedCvar, 0)) {
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
            Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_GORON_TUNIC].colorCvar, {});
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_GORON_TUNIC].colorChangedCvar });

static RegisterShipInitFunc goronTunicColor(
    []() {
        Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_GORON_TUNIC].colorCvar, {});
        goronTunic[0] = gsDPSetPrimColor(0, 0, changedColor.r, changedColor.g, changedColor.b, 255);
    },
    { cosmeticEditorElements[COSMETIC_ELEMENT_GORON_TUNIC].colorCvar });

// COSMETIC_ELEMENT_ZORA_TUNIC
static const Color_RGBA8 zoraSkinColor = { 197, 247, 247, 255 };
static const Color_RGBA8 zoraTunicBaseColor = { 0, 74, 16, 255 };

static RegisterShipInitFunc zoraTunicPatch(
    []() {
        if (CVarGetInteger(cosmeticEditorElements[COSMETIC_ELEMENT_ZORA_TUNIC].colorChangedCvar, 0)) {
            /*
             * Zora works differently from the other color changes. Other forms apply a grayscale to the green tunic
             * textures and then alter the Gfx commands to set the color. That works because those textures are one
             * basic color. Zora, however, gradually transitions from green to the bluish Zora skin. A further
             * complication is that relevant colors in the TLUTs are not contiguous, so the brightness calculation will
             * not work as intended. Instead of using the palette approach, here we directly apply the custom color to
             * the textures and TLUTs.
             */
            Color_RGBA8 changedColor = CVarGetColor(cosmeticEditorElements[COSMETIC_ELEMENT_ZORA_TUNIC].colorCvar, {});
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
    { cosmeticEditorElements[COSMETIC_ELEMENT_ZORA_TUNIC].colorChangedCvar });
