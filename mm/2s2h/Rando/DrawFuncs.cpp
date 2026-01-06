#include "DrawFuncs.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "BenPort.h"

extern "C" {
#include <functions.h>
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_obj_tokeidai/object_obj_tokeidai.h"

// Soul Effects
#include "src/overlays/actors/ovl_Obj_Moon_Stone/z_obj_moon_stone.h"
#include "assets/objects/object_gi_reserve00/object_gi_reserve00.h"

s32 EnMinifrog_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* enMini);

// clang-format off
// Boss Includes
/* Goht */      #include "objects/object_boss_hakugin/object_boss_hakugin.h"
/* Gyorg */     #include "objects/object_boss03/object_boss03.h"
/* Odolwa */    #include "objects/object_boss01/object_boss01.h"
/* Twinmold */  #include "objects/object_boss02/object_boss02.h"
/* Majora */    #include "objects/object_boss07/object_boss07.h"

// Other Actor Includes
/* Minifrog */  #include "objects/object_fr/object_fr.h"
/* Clock */     #include "overlays/actors/ovl_Obj_Tokeidai/z_obj_tokeidai.h"
    
// Clock
void ObjTokeidai_RotateOnMinuteChange(ObjTokeidai* thisx, s32 playSfx);
void ObjTokeidai_RotateOnHourChange(ObjTokeidai* thisx, PlayState* play);
// clang-format on
}

// Soul Effects
void DrawEnLight(Color_RGB8 flameColor, Vec3f flameSize) {
    Gfx* sp68;
    static s8 unk_144 = (s8)(Rand_ZeroOne() * 255.0f);
    static u32 lastUpdate = 0;

    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_ReplaceRotation(&gPlayState->billboardMtxF);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(gPlayState->state.gfxCtx, 0, 0, 0, 0x10, 0x20, 1, (unk_144 * 2) & 0x3F,
                                           (unk_144 * -6) & 0x7F, 0x10, 0x20));
    sp68 = (Gfx*)gameplay_keep_DL_01ACF0;
    gDPSetPrimColor(POLY_XLU_DISP++, 0xC0, 0xC0, flameColor.r, flameColor.g, flameColor.b, 0);
    gDPSetEnvColor(POLY_XLU_DISP++, flameColor.r, flameColor.g, flameColor.b, 0);
    Matrix_Scale(flameSize.x, flameSize.y, flameSize.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, sp68);

    CLOSE_DISPS(gPlayState->state.gfxCtx);

    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        unk_144++;
    }
}

// Limb Override Functions
void EnMinifrogPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if ((limbIndex == FROG_LIMB_RIGHT_EYE) || (limbIndex == FROG_LIMB_LEFT_EYE)) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_ReplaceRotation(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, *dList);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

// Boss Souls
extern void DrawGoht() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[33];
    static Vec3s otherTable[33];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gGohtSkel, (AnimationHeader*)&gGohtRunAnim,
                           jointTable, otherTable, 33);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gGohtMetalPlateWithCirclePatternTex);
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
    DrawEnLight({ 10, 138, 46 }, { 30.0f, 30.0f, 30.0f });
}

extern void DrawGyorg() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[15];
    static Vec3s otherTable[15];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gGyorgSkel,
                           (AnimationHeader*)&gGyorgGentleSwimmingAnim, jointTable, otherTable, 15);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
    DrawEnLight({ 19, 99, 165 }, { 3.0f, 3.0f, 3.0f });
}

extern void DrawOdolwa() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[52];
    static Vec3s otherTable[52];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gOdolwaSkel,
                           (AnimationHeader*)&gOdolwaReadyAnim, jointTable, otherTable, 52);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
    DrawEnLight({ 145, 20, 133 }, { 25.0f, 25.0f, 25.0f });
}

extern void DrawTwinmold() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Scale(0.06f, 0.06f, 0.06f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[13];
    static Vec3s otherTable[13];
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gTwinmoldHeadSkel,
                           (AnimationHeader*)&gTwinmoldHeadFlyAnim, jointTable, otherTable, 13);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    Mtx* mtxHead = (Mtx*)GRAPH_ALLOC(gPlayState->state.gfxCtx, 23 * sizeof(Mtx));
    gSPSegment(POLY_OPA_DISP++, 0x0D, (uintptr_t)mtxHead);
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gTwinmoldBlueSkinTex);
    SkelAnime_DrawOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, NULL, NULL, NULL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
    DrawEnLight({ 168, 180, 20 }, { 3.0f, 3.0f, 3.0f });
}

extern void DrawMajora() {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[MAJORAS_MASK_LIMB_MAX];
    static Vec3s morphTable[MAJORAS_MASK_LIMB_MAX];
    static u32 lastUpdate = 0;

    OPEN_DISPS(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);
    Matrix_Translate(0, 0, 0, MTXMODE_APPLY);

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(gPlayState, &skelAnime, (SkeletonHeader*)&gMajorasMaskSkel,
                       (AnimationHeader*)&gMajorasMaskFloatingAnim, jointTable, morphTable, MAJORAS_MASK_LIMB_MAX);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    gSPSegment(POLY_OPA_DISP++, 8, (uintptr_t)gMajorasMaskWithNormalEyesTex);
    SkelAnime_DrawOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, NULL, NULL, NULL);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gMajorasMaskTentacleMaterialDL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
    DrawEnLight({ 232, 128, 21 }, { 3.0f, 3.0f, 3.0f });
}

// Other Actors
extern void DrawMinifrog(RandoItemId randoItemId, Actor* actor) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[FROG_LIMB_MAX];
    static Vec3s otherTable[FROG_LIMB_MAX];
    Color_RGBA8 envColor = { 200, 170, 0, 255 }; // FROG_YELLOW
    static u32 lastUpdate = 0;
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gFrogSkel, (AnimationHeader*)&gFrogIdleAnim,
                           jointTable, otherTable, FROG_LIMB_MAX);
    }
    if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
        lastUpdate = gPlayState->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    switch (randoItemId) {
        case RI_FROG_BLUE:
            envColor = { 120, 130, 230, 255 };
            break;
        case RI_FROG_CYAN:
            envColor = { 0, 170, 200, 255 };
            break;
        case RI_FROG_PINK:
            envColor = { 210, 120, 100, 255 };
            break;
        case RI_FROG_WHITE:
            envColor = { 190, 190, 190, 255 };
            break;
    }

    gDPSetEnvColor(POLY_OPA_DISP++, envColor.r, envColor.g, envColor.b, envColor.a);

    Mtx* mtxHead = (Mtx*)GRAPH_ALLOC(gPlayState->state.gfxCtx, 23 * sizeof(Mtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gFrogIrisOpenTex);
    gSPSegment(POLY_OPA_DISP++, 0x09, (uintptr_t)gFrogIrisOpenTex);
    SkelAnime_DrawFlexOpa(gPlayState, skelAnime.skeleton, skelAnime.jointTable, FROG_LIMB_MAX,
                          EnMinifrog_OverrideLimbDraw, EnMinifrogPostLimbDraw, actor);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern void DrawClock(RandoItemId randoItemId, Actor* actor) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    ObjTokeidai* clockActor = (ObjTokeidai*)actor;
    static u32 lastUpdate = 0;
    static f32 yTranslation = 0;
    static f32 xRotation = 0;
    static int16_t minuteRingOrExteriorGearRotation = 0;
    static f32 clockFaceZTranslation = 0;
    static int16_t clockFaceRotation = 0;
    static int16_t sunMoonPanelRotation = 0;

    switch (randoItemId) {
        case RI_TIME_DAY_1:
        case RI_TIME_DAY_2:
        case RI_TIME_DAY_3:
            clockFaceRotation = 0xC000;
            sunMoonPanelRotation = 0;
            break;
        case RI_TIME_NIGHT_1:
        case RI_TIME_NIGHT_2:
        case RI_TIME_NIGHT_3:
            clockFaceRotation = 0;
            sunMoonPanelRotation = 0x8000;
            break;
        case RI_TIME_PROGRESSIVE:
            clockFaceRotation = gSaveContext.save.isNight ? 0 : 0xC000;
            sunMoonPanelRotation = gSaveContext.save.isNight ? 0x8000 : 0;
            break;
    }

    if (clockActor != nullptr && clockActor->actor.id == ACTOR_OBJ_TOKEIDAI) {
        clockActor->clockTime = gSaveContext.save.time;

        if (gPlayState != NULL && lastUpdate != gPlayState->state.frames) {
            lastUpdate = gPlayState->state.frames;
            ObjTokeidai_RotateOnMinuteChange(clockActor, true);
            ObjTokeidai_RotateOnHourChange(clockActor, gPlayState);
            yTranslation = clockActor->yTranslation;
            xRotation = clockActor->xRotation;
            minuteRingOrExteriorGearRotation = clockActor->minuteRingOrExteriorGearRotation;
            clockFaceZTranslation = clockActor->clockFaceZTranslation;
            clockFaceRotation = clockActor->clockFaceRotation;
            sunMoonPanelRotation = clockActor->sunMoonPanelRotation;
        }
    }

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);
    Matrix_Translate(0.0f, yTranslation, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -1791.0f, MTXMODE_APPLY);
    Matrix_RotateXS(-xRotation, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 1791.0f, MTXMODE_APPLY);

    Matrix_Push();
    Matrix_RotateZS(-minuteRingOrExteriorGearRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gClockTowerMinuteRingDL);
    Matrix_Pop();

    Matrix_Translate(0.0f, 0.0f, clockFaceZTranslation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gClockTowerClockCenterAndHandDL);

    Matrix_RotateZS(-clockFaceRotation * 2, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gClockTowerClockFaceDL);

    Matrix_Translate(0.0f, -1112.0f, -19.6f, MTXMODE_APPLY);
    Matrix_RotateYS(sunMoonPanelRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gClockTowerSunAndMoonPanelDL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}
