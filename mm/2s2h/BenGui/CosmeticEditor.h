#ifndef COSMETIC_EDITOR_H
#define COSMETIC_EDITOR_H

#ifdef __cplusplus

class CosmeticEditorWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

extern "C" {
#endif //__cplusplus

void gDPSetPrimColorOverride(Gfx* pkt, u8 m, u8 l, u8 r, u8 g, u8 b, u8 a, u8 elementId);
void gDPSetEnvColorOverride(Gfx* pkt, u8 r, u8 g, u8 b, u8 a, u8 elementId);
Gfx* Gfx_DrawTexRectIA8_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                           s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                           s16 r, s16 g, s16 b, s16 a, u8 elementId);
Gfx* Gfx_DrawRect_DropShadowOverride(Gfx* pkt, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                     u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId);
Gfx* Gfx_DrawTexRectIA16_DropShadowOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                            s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                            u16 dtdy, s16 r, s16 g, s16 b, s16 a, u8 elementId);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffsetOverride(Gfx* pkt, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                                 s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx,
                                                 u16 dtdy, s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects,
                                                 u8 elementId);
Color_RGBA8 CosmeticEditor_GetChangedColor(u8 r, u8 g, u8 b, u8 a, u8 elementId);

typedef enum {
    COSMETIC_ELEMENT_HEART_NORMAL,
    COSMETIC_ELEMENT_HEART_NORMAL_BEATING,
    COSMETIC_ELEMENT_HEART_BORDER,
    COSMETIC_ELEMENT_HEART_DD,
    COSMETIC_ELEMENT_HEART_DD_BEATING,
    COSMETIC_ELEMENT_HEART_DD_BORDER,
    COSMETIC_ELEMENT_MAGIC_NORMAL,
    COSMETIC_ELEMENT_MAGIC_CHATEAU,
    COSMETIC_ELEMENT_MAGIC_CONSUMED,
    COSMETIC_ELEMENT_MAGIC_BORDER,
    COSMETIC_ELEMENT_SMALL_KEY,
    COSMETIC_ELEMENT_RUPEE_ICON,
    COSMETIC_ELEMENT_MINIMAP,
    COSMETIC_ELEMENT_SPIN_SLASH_CHARGE,
    COSMETIC_ELEMENT_SPIN_SLASH_BURST,
    COSMETIC_ELEMENT_GREAT_SPIN_CHARGE,
    COSMETIC_ELEMENT_GREAT_SPIN_BURST,
    COSMETIC_ELEMENT_FIRE_ARROW_PRIMARY,
    COSMETIC_ELEMENT_FIRE_ARROW_SECONDARY,
    COSMETIC_ELEMENT_ICE_ARROW_PRIMARY,
    COSMETIC_ELEMENT_ICE_ARROW_SECONDARY,
    COSMETIC_ELEMENT_LIGHT_ARROW_PRIMARY,
    COSMETIC_ELEMENT_LIGHT_ARROW_SECONDARY,
    COSMETIC_ELEMENT_B_BUTTON,
    COSMETIC_ELEMENT_A_BUTTON,
    COSMETIC_ELEMENT_C_LEFT_BUTTON,
    COSMETIC_ELEMENT_C_DOWN_BUTTON,
    COSMETIC_ELEMENT_C_RIGHT_BUTTON,
    COSMETIC_ELEMENT_D_PAD_BUTTON,
    COSMETIC_ELEMENT_START_BUTTON,
    COSMETIC_ELEMENT_FILE_SELECT_MENU,
    COSMETIC_ELEMENT_FILE_SELECT_PLATES,
    COSMETIC_ELEMENT_HUMAN_TUNIC,
    COSMETIC_ELEMENT_HUMAN_HAIR,
    COSMETIC_ELEMENT_DEKU_TUNIC,
    COSMETIC_ELEMENT_DEKU_HAIR,
    COSMETIC_ELEMENT_GORON_TUNIC,
    COSMETIC_ELEMENT_ZORA_TUNIC,
    COSMETIC_ELEMENT_KAFEI_HAIR,
    COSMETIC_ELEMENT_MAX
} CosmeticEditorElementID;

typedef struct {
    CosmeticEditorElementID id;
    const char* parentName;
    const char* name;
    int32_t defaultR;
    int32_t defaultG;
    int32_t defaultB;
    int32_t defaultA;
    const char* colorCvar;
    const char* colorChangedCvar;
} CosmeticEditorElement;

#define COSMETIC_EDITOR_ELEMENT(id, parentName, name, cvar, defaultR, defaultG, defaultB, defaultA) \
    {                                                                                               \
        id, parentName, name, defaultR, defaultG, defaultB, defaultA, "gCosmetic." cvar ".Color",   \
            "gCosmetic." cvar ".Changed"                                                            \
    }

#ifdef __cplusplus
}
#endif //__cplusplus

#endif