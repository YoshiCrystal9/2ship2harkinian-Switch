#include "MiscBehavior.h"
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h_assets.h"

extern "C" {
#include "z64save.h"
#include "functions.h"
#include "macros.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "misc/title_static/title_static.h"
extern s16 sWindowContentColors[3];
extern FileSelectState* gFileSelectState;
}

u8 isRando[FILE_NUM_MAX_WITH_OWL_SAVE];

// 5 rectangles per save file:
// Rand Left Aligned
// Rand Left Aligned (shadow offset)
// Rand Center Aligned
// Rand Center Aligned (shadow offset)
// Owl
Vtx sRandVtxData[20 * FILE_NUM_MAX];

constexpr s16 RAND_ICON_HEIGHT = 16;
constexpr s16 RAND_ICON_WIDTH = 32;

// Initialize all vtx data with dummy/default values
void CreateRandSaveTypeVtxData() {
    for (int vtxId = 0; vtxId < ARRAY_COUNT(sRandVtxData); vtxId += 4) {
        // x-coord (left)
        sRandVtxData[vtxId + 0].v.ob[0] = sRandVtxData[vtxId + 2].v.ob[0] = 0;
        // x-coord (right)
        sRandVtxData[vtxId + 1].v.ob[0] = sRandVtxData[vtxId + 3].v.ob[0] = 0;

        // y-coord (top)
        sRandVtxData[vtxId + 0].v.ob[1] = sRandVtxData[vtxId + 1].v.ob[1] = 0;
        // y-coord (bottom)
        sRandVtxData[vtxId + 2].v.ob[1] = sRandVtxData[vtxId + 3].v.ob[1] = 0;

        // z-coordinate
        sRandVtxData[vtxId + 0].v.ob[2] = sRandVtxData[vtxId + 1].v.ob[2] = sRandVtxData[vtxId + 2].v.ob[2] =
            sRandVtxData[vtxId + 3].v.ob[2] = 0;

        // flag
        sRandVtxData[vtxId + 0].v.flag = sRandVtxData[vtxId + 1].v.flag = sRandVtxData[vtxId + 2].v.flag =
            sRandVtxData[vtxId + 3].v.flag = 0;

        // texture coordinates
        sRandVtxData[vtxId + 0].v.tc[0] = sRandVtxData[vtxId + 0].v.tc[1] = sRandVtxData[vtxId + 1].v.tc[1] =
            sRandVtxData[vtxId + 2].v.tc[0] = 0;
        sRandVtxData[vtxId + 1].v.tc[0] = sRandVtxData[vtxId + 2].v.tc[1] = sRandVtxData[vtxId + 3].v.tc[0] =
            sRandVtxData[vtxId + 3].v.tc[1] = 0;

        // alpha
        sRandVtxData[vtxId + 0].v.cn[0] = sRandVtxData[vtxId + 1].v.cn[0] = sRandVtxData[vtxId + 2].v.cn[0] =
            sRandVtxData[vtxId + 3].v.cn[0] = sRandVtxData[vtxId + 0].v.cn[1] = sRandVtxData[vtxId + 1].v.cn[1] =
                sRandVtxData[vtxId + 2].v.cn[1] = sRandVtxData[vtxId + 3].v.cn[1] = sRandVtxData[vtxId + 0].v.cn[2] =
                    sRandVtxData[vtxId + 1].v.cn[2] = sRandVtxData[vtxId + 2].v.cn[2] =
                        sRandVtxData[vtxId + 3].v.cn[2] = sRandVtxData[vtxId + 0].v.cn[3] =
                            sRandVtxData[vtxId + 1].v.cn[3] = sRandVtxData[vtxId + 2].v.cn[3] =
                                sRandVtxData[vtxId + 3].v.cn[3] = 255;
    }
}

// Updates the vtx values on every draw to account for file info moving up/down
void SetRandSaveTypeVtxData() {
    int startY = 44;
    int vtxId = 0;

    for (int i = 0; i < FILE_NUM_MAX; i++, startY -= 16, vtxId += 4) {
        int posY;
        int posX = gFileSelectState->windowPosX + 163;

        // Compute real Y position based on current file select state
        if ((gFileSelectState->configMode == 0x10) && (i == gFileSelectState->copyDestFileIndex)) {
            posY = gFileSelectState->fileNamesY[i] + 0x2C;
        } else if (((gFileSelectState->configMode == 0x11) || (gFileSelectState->configMode == 0x12)) &&
                   (i == gFileSelectState->copyDestFileIndex)) {
            posY = gFileSelectState->buttonYOffsets[i] + startY;
        } else {
            posY = startY + gFileSelectState->buttonYOffsets[i] + gFileSelectState->fileNamesY[i];
        }

        /* Rand Icons */
        // 4 sets: Left aligned, Left aligned (shadow), Center aligned, Center aligned (shadow)
        for (int j = 0; j < 4; j++, vtxId += 4) {
            // x-coord (left)
            sRandVtxData[vtxId + 0].v.ob[0] = sRandVtxData[vtxId + 2].v.ob[0] =
                posX + (j % 2 ? 1 : 0) + ((j > 1) ? 10 : 0);
            // x-coord (right)
            sRandVtxData[vtxId + 1].v.ob[0] = sRandVtxData[vtxId + 3].v.ob[0] =
                sRandVtxData[vtxId + 0].v.ob[0] + RAND_ICON_WIDTH;

            // y-coord (top)
            sRandVtxData[vtxId + 0].v.ob[1] = sRandVtxData[vtxId + 1].v.ob[1] = posY - (j % 2 ? 1 : 0);
            // y-coord (bottom)
            sRandVtxData[vtxId + 2].v.ob[1] = sRandVtxData[vtxId + 3].v.ob[1] =
                sRandVtxData[vtxId + 0].v.ob[1] - RAND_ICON_HEIGHT;

            // texture coordinates
            sRandVtxData[vtxId + 0].v.tc[0] = sRandVtxData[vtxId + 0].v.tc[1] = sRandVtxData[vtxId + 1].v.tc[1] =
                sRandVtxData[vtxId + 2].v.tc[0] = 0;
            sRandVtxData[vtxId + 1].v.tc[0] = sRandVtxData[vtxId + 3].v.tc[0] = RAND_ICON_WIDTH << 5;
            sRandVtxData[vtxId + 2].v.tc[1] = sRandVtxData[vtxId + 3].v.tc[1] = RAND_ICON_HEIGHT << 5;
        }

        /* Owl Icon */
        // x-coord (left)
        sRandVtxData[vtxId + 0].v.ob[0] = sRandVtxData[vtxId + 2].v.ob[0] = posX + 28;
        // x-coord (right)
        sRandVtxData[vtxId + 1].v.ob[0] = sRandVtxData[vtxId + 3].v.ob[0] = sRandVtxData[vtxId + 0].v.ob[0] + 24;

        // y-coord (top)
        sRandVtxData[vtxId + 0].v.ob[1] = sRandVtxData[vtxId + 1].v.ob[1] = posY - 2;
        // y-coord (bottom)
        sRandVtxData[vtxId + 2].v.ob[1] = sRandVtxData[vtxId + 3].v.ob[1] = sRandVtxData[vtxId + 0].v.ob[1] - 12;

        // texture coordinates
        sRandVtxData[vtxId + 0].v.tc[0] = sRandVtxData[vtxId + 0].v.tc[1] = sRandVtxData[vtxId + 1].v.tc[1] =
            sRandVtxData[vtxId + 2].v.tc[0] = 0;
        sRandVtxData[vtxId + 1].v.tc[0] = sRandVtxData[vtxId + 3].v.tc[0] = 24 << 5;
        sRandVtxData[vtxId + 2].v.tc[1] = sRandVtxData[vtxId + 3].v.tc[1] = 12 << 5;
    }
}

void RegisterShoulds() {

    // Renders the small blank info box next to the file info
    REGISTER_VB_SHOULD(VB_DRAW_FILE_SELECT_SMALL_EXTRA_INFO_BOX, {
        int fileIndex = va_arg(args, int);

        // Bail out if not a rando save, or if the save is also an owl save
        // because owl saves already render the small box and large box
        if (!isRando[fileIndex] || gFileSelectState->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            return;
        }

        // We want to force the original small box to render which uses the alpha for the collapsed files
        *should = true;

        OPEN_DISPS(gFileSelectState->state.gfxCtx);

        // But then we also render the same small box again, but using the expanded file info alpha
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                        sWindowContentColors[2], gFileSelectState->fileInfoAlpha[fileIndex]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBlankButtonTex, G_IM_FMT_IA, G_IM_SIZ_16b, 52, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);

        CLOSE_DISPS(gFileSelectState->state.gfxCtx);
    });

    REGISTER_VB_SHOULD(VB_DRAW_FILE_SELECT_EXTRA_INFO_DETAILS, {
        int fileIndex = va_arg(args, int);

        if (!isRando[fileIndex]) {
            return;
        }

        SetRandSaveTypeVtxData();

        OPEN_DISPS(gFileSelectState->state.gfxCtx);

        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        gSPVertex(POLY_OPA_DISP++, (uintptr_t)&sRandVtxData[20 * fileIndex], 20, 0);

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelRandIconTex, G_IM_FMT_I, RAND_ICON_WIDTH, RAND_ICON_HEIGHT, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);

        // Rand Icon (shadow)
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, gFileSelectState->nameAlpha[fileIndex]);

        if (gFileSelectState->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0); // Left aligned
        } else {
            gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0); // Centered
        }

        // Rand Icon
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, gFileSelectState->nameAlpha[fileIndex]);

        if (gFileSelectState->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0); // Left aligned
        } else {
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0); // Centered
        }

        // Owl Icon
        if (gFileSelectState->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelOwlSaveIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 12, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
        }

        CLOSE_DISPS(gFileSelectState->state.gfxCtx);
    });

    // Prevent original owl save icon from rendering
    REGISTER_VB_SHOULD(VB_DRAW_FILE_SELECT_OWL_SAVE_ICON, {
        int fileIndex = va_arg(args, int);

        if (isRando[fileIndex]) {
            *should = false;
        }
    });
}

// Doesn't really look great yet, but the start to how we will augment the file select screen for rando saves
void Rando::MiscBehavior::InitFileSelect() {
    RegisterShoulds();

    CreateRandSaveTypeVtxData();

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFileSelectSaveLoad>(
        [](s16 fileNum, bool isOwlSave, SaveContext* saveContext) {
            isRando[fileNum + (isOwlSave ? FILE_NUM_OWL_SAVE_OFFSET : 0)] =
                saveContext->save.shipSaveInfo.saveType == SAVETYPE_RANDO;
        });
}
