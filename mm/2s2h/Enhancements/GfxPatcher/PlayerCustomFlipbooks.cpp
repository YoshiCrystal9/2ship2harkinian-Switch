#include "PlayerCustomFlipbooks.h"
#include "2s2h/BenPort.h"

extern "C" {
#include "z64player.h"
extern TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX];
extern TexturePtr sPlayerMouthTextures[PLAYER_FORM_MAX][PLAYER_MOUTH_MAX];
uint8_t ResourceMgr_FileExists(const char* resName);
}

static const char* sFDEyesTextures[PLAYER_EYES_MAX] = {
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesOpenTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesHalfTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesClosedTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesRightTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesLeftTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesUpTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesDownTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityEyesWincingTex",
};

static const char* sFDMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthClosedTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthHalfTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthOpenTex",
    "__OTR__objects/object_link_boy/gLinkFierceDeityMouthSmileTex",
};

static const char* sDekuEyesTextures[PLAYER_EYES_MAX] = {
    "__OTR__objects/object_link_nuts/gLinkDekuEyesOpenTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesHalfTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesClosedTex", "__OTR__objects/object_link_nuts/gLinkDekuEyesRightTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesLeftTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesUpTex",
    "__OTR__objects/object_link_nuts/gLinkDekuEyesDownTex",   "__OTR__objects/object_link_nuts/gLinkDekuEyesWincingTex",
};

static const char* sDekuMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_nuts/gLinkDekuMouthClosedTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthHalfTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthOpenTex",
    "__OTR__objects/object_link_nuts/gLinkDekuMouthSmileTex",
};

static const char* sGoronMouthTextures[PLAYER_MOUTH_MAX] = {
    "__OTR__objects/object_link_goron/gLinkGoronMouthClosedTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthHalfTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthOpenTex",
    "__OTR__objects/object_link_goron/gLinkGoronMouthSmileTex",
};

static s32 sFacePatchState = 0;

static void PlayerCustomFlipbooks_PatchOnce(void) {
    if (sFacePatchState != 0) {
        return;
    }

    bool EyesPatch = true;
    bool MouthPatch = true;
    bool DekuEyesPatch = true;
    bool DekuMouthPatch = true;
    bool GoronMouthPatch = true;

    for (s32 i = 0; i < PLAYER_EYES_MAX; i++) {
        if (!ResourceMgr_FileExists(sFDEyesTextures[i])) {
            EyesPatch = false;
            break;
        }
    }

    for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
        if (!ResourceMgr_FileExists(sFDMouthTextures[i])) {
            MouthPatch = false;
            break;
        }
    }

    for (s32 i = 0; i < PLAYER_EYES_MAX; i++) {
        if (!ResourceMgr_FileExists(sDekuEyesTextures[i])) {
            DekuEyesPatch = false;
            break;
        }
    }

    for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
        if (!ResourceMgr_FileExists(sDekuMouthTextures[i])) {
            DekuMouthPatch = false;
            break;
        }
    }

    for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
        if (!ResourceMgr_FileExists(sGoronMouthTextures[i])) {
            GoronMouthPatch = false;
            break;
        }
    }

    if (EyesPatch) {
        for (s32 i = 0; i < PLAYER_EYES_MAX; i++) {
            sPlayerEyesTextures[PLAYER_FORM_FIERCE_DEITY][i] = (TexturePtr)sFDEyesTextures[i];
        }
    }

    if (MouthPatch) {
        for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
            sPlayerMouthTextures[PLAYER_FORM_FIERCE_DEITY][i] = (TexturePtr)sFDMouthTextures[i];
        }
    }

    if (DekuEyesPatch) {
        for (s32 i = 0; i < PLAYER_EYES_MAX; i++) {
            sPlayerEyesTextures[PLAYER_FORM_DEKU][i] = (TexturePtr)sDekuEyesTextures[i];
        }
    }

    if (DekuMouthPatch) {
        for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
            sPlayerMouthTextures[PLAYER_FORM_DEKU][i] = (TexturePtr)sDekuMouthTextures[i];
        }
    }

    if (GoronMouthPatch) {
        for (s32 i = 0; i < PLAYER_MOUTH_MAX; i++) {
            sPlayerMouthTextures[PLAYER_FORM_GORON][i] = (TexturePtr)sGoronMouthTextures[i];
        }
    }

    sFacePatchState = 1;
}

void PlayerCustomFlipbooks_Patch(void) {
    PlayerCustomFlipbooks_PatchOnce();
}
