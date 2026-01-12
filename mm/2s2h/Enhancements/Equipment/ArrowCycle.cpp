#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "z64horse.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
void Player_InitItemAction(PlayState* play, Player* thisx, PlayerItemAction itemAction);
s32 Player_UpperAction_7(Player* thisx, PlayState* play);
s32 Player_UpperAction_8(Player* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.PlayerActions.ArrowCycle"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Magic arrow costs based on z_player.c
static const s16 sMagicArrowCosts[] = { 4, 4, 8 };

// Button Flash Effect Configuration
static const s16 BUTTON_FLASH_DURATION = 3;
static const s16 BUTTON_FLASH_COUNT = 3;
static const s16 BUTTON_HIGHLIGHT_ALPHA = 128;

// State Variables
static s16 sButtonFlashTimer = 0;
static s16 sButtonFlashCount = 0;
static s8 sJustCycledFrames = 0;

// Arrow Type Definitions
static const PlayerItemAction ARROW_NORMAL = PLAYER_IA_BOW;
static const PlayerItemAction ARROW_FIRE = PLAYER_IA_BOW_FIRE;
static const PlayerItemAction ARROW_ICE = PLAYER_IA_BOW_ICE;
static const PlayerItemAction ARROW_LIGHT = PLAYER_IA_BOW_LIGHT;

static const PlayerItemAction sArrowCycleOrder[] = {
    ARROW_NORMAL,
    ARROW_FIRE,
    ARROW_ICE,
    ARROW_LIGHT,
};

// Utility Functions
static bool IsHoldingBow(Player* player) {
    return player->heldItemAction >= ARROW_NORMAL && player->heldItemAction <= ARROW_LIGHT;
}

static bool IsHoldingMagicBow(Player* player) {
    return player->heldItemAction >= ARROW_FIRE && player->heldItemAction <= ARROW_LIGHT;
}

static bool IsAimingBow(Player* player) {
    return IsHoldingBow(player) && ((player->unk_AA5 == PLAYER_UNKAA5_3) || /* Aiming box in first person */
                                    (player->upperActionFunc == Player_UpperAction_7) /* Arrow pulled back on bow */);
}

static bool HasArrowType(PlayerItemAction arrowType) {
    switch (arrowType) {
        case ARROW_NORMAL:
            return true;
        case ARROW_FIRE:
            return (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE);
        case ARROW_ICE:
            return (INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE);
        case ARROW_LIGHT:
            return (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
        default:
            return false;
    }
}

static s32 GetBowItemForArrow(PlayerItemAction arrowType) {
    switch (arrowType) {
        case ARROW_FIRE:
            return ITEM_BOW_FIRE;
        case ARROW_ICE:
            return ITEM_BOW_ICE;
        case ARROW_LIGHT:
            return ITEM_BOW_LIGHT;
        default:
            return ITEM_BOW;
    }
}

static bool CanCycleArrows() {
    Player* player = GET_PLAYER(gPlayState);

    // Don't allow cycling during bow minigames in specific scenes
    if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE &&
        (gPlayState->sceneId == SCENE_SYATEKI_MIZU || // Town Shooting Gallery
         gPlayState->sceneId == SCENE_SYATEKI_MORI || // Swamp Shooting Gallery
         gPlayState->sceneId == SCENE_20SICHITAI2)) { // Tourist Center boat cruise
        return false;
    }

    return !gHorseIsMounted && player->rideActor == NULL && INV_CONTENT(SLOT_BOW) == ITEM_BOW &&
           (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_ARROW_FIRE || INV_CONTENT(ITEM_ARROW_ICE) == ITEM_ARROW_ICE ||
            INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT);
}

// Arrow Cycling Logic
static s8 GetNextArrowType(s8 currentArrowType) {
    int currentIndex = 0;
    for (int i = 0; i < (int)ARRAY_COUNT(sArrowCycleOrder); i++) {
        if (sArrowCycleOrder[i] == currentArrowType) {
            currentIndex = i;
            break;
        }
    }

    for (int offset = 1; offset <= (int)ARRAY_COUNT(sArrowCycleOrder); offset++) {
        int nextIndex = (currentIndex + offset) % ARRAY_COUNT(sArrowCycleOrder);
        if (HasArrowType(sArrowCycleOrder[nextIndex])) {
            return sArrowCycleOrder[nextIndex];
        }
    }

    return ARROW_NORMAL;
}

// UI Update Functions
static void UpdateButtonAlpha(s16 flashAlpha, bool isButtonBow, s16* buttonAlpha) {
    if (isButtonBow) {
        *buttonAlpha = flashAlpha;
        if (sButtonFlashTimer == 0) {
            *buttonAlpha = 255;
        }
    }
}

static void UpdateFlashEffect(PlayState* play) {
    if (sButtonFlashTimer <= 0) {
        return;
    }

    sButtonFlashTimer--;
    s16 flashAlpha = (sButtonFlashTimer % 3) ? BUTTON_HIGHLIGHT_ALPHA : 255;

    if (sButtonFlashTimer == 0 && sButtonFlashCount < BUTTON_FLASH_COUNT - 1) {
        sButtonFlashTimer = BUTTON_FLASH_DURATION;
        sButtonFlashCount++;
    }

    // Update C-buttons
    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cLeftAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cDownAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) == ITEM_BOW) ||
                          (GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE &&
                           GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.cRightAlpha);

    // Update D-pad
    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dRightAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dLeftAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dDownAlpha);

    UpdateButtonAlpha(flashAlpha,
                      (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) == ITEM_BOW) ||
                          (DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE &&
                           DPAD_GET_CUR_FORM_BTN_ITEM(EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT),
                      &play->interfaceCtx.shipInterface.dpad.dUpAlpha);
}

static void UpdateEquippedBow(PlayState* play, s8 arrowType) {
    s32 bowItem = GetBowItemForArrow(static_cast<PlayerItemAction>(arrowType));

    // Update C-buttons
    for (s32 i = EQUIP_SLOT_C_LEFT; i <= EQUIP_SLOT_C_RIGHT; i++) {
        if ((BUTTON_ITEM_EQUIP(0, i) == ITEM_BOW) ||
            (BUTTON_ITEM_EQUIP(0, i) >= ITEM_BOW_FIRE && BUTTON_ITEM_EQUIP(0, i) <= ITEM_BOW_LIGHT)) {
            BUTTON_ITEM_EQUIP(0, i) = bowItem;
            C_SLOT_EQUIP(0, i) = SLOT_BOW;
            Interface_LoadItemIcon(play, i);
            gSaveContext.buttonStatus[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;
        }
    }

    // Update D-pad
    for (s32 i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++) {
        if ((DPAD_BUTTON_ITEM_EQUIP(0, i) == ITEM_BOW) ||
            (DPAD_BUTTON_ITEM_EQUIP(0, i) >= ITEM_BOW_FIRE && DPAD_BUTTON_ITEM_EQUIP(0, i) <= ITEM_BOW_LIGHT)) {
            DPAD_BUTTON_ITEM_EQUIP(0, i) = bowItem;
            DPAD_SLOT_EQUIP(0, i) = SLOT_BOW;
            Interface_Dpad_LoadItemIcon(play, i);
            gSaveContext.shipSaveContext.dpad.status[i] = BTN_ENABLED;
            sButtonFlashTimer = BUTTON_FLASH_DURATION;
            sButtonFlashCount = 0;
        }
    }

    UpdateFlashEffect(play);
}

// Core Arrow Cycling Function
static void CycleToNextArrow(PlayState* play, Player* player) {
    s8 nextArrow = GetNextArrowType(player->heldItemAction);

    if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
        EnArrow* arrow = (EnArrow*)player->heldActor;

        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }

        Actor_Kill(&arrow->actor);
    }

    Player_InitItemAction(play, player, static_cast<PlayerItemAction>(nextArrow));
    UpdateEquippedBow(play, nextArrow);
    Audio_PlaySfx(NA_SE_PL_CHANGE_ARMS);
}

void ArrowCycleMain() {
    if (sJustCycledFrames > 0) {
        sJustCycledFrames--;
    }

    if (gPlayState == nullptr || !CanCycleArrows()) {
        return;
    }

    UpdateFlashEffect(gPlayState);

    Player* player = GET_PLAYER(gPlayState);
    Input* input = CONTROLLER1(&gPlayState->state);

    // Block camera changes when cycling arrows while drawing the bow
    if ((player->stateFlags3 & PLAYER_STATE3_40) && player->unk_ACE == 0) {
        return;
    }

    if (IsAimingBow(player) && CHECK_BTN_ANY(input->press.button, BTN_R)) {
        if (IsHoldingMagicBow(player) && gSaveContext.magicState != MAGIC_STATE_IDLE && player->heldActor == NULL) {
            Audio_PlaySfx(NA_SE_SY_ERROR);
            return;
        }

        if (player->heldActor != NULL && player->heldActor->id == ACTOR_EN_ARROW) {
            EnArrow* heldArrow = (EnArrow*)player->heldActor;

            // If the held arrow itself is magical, then we should "restore" the consumed magic upon cycling
            if (ARROW_IS_MAGICAL(heldArrow->actor.params)) {
                Magic_Add(gPlayState, sMagicArrowCosts[ARROW_GET_MAGIC_FROM_TYPE(heldArrow->actor.params)]);
            }
        }

        CycleToNextArrow(gPlayState, player);
        // Track that we just cycled for 2 frames to prevent held R input from triggering the shield action when in
        // Z-Target mode as the arrow is respawned (Player_UpperAction_8)
        sJustCycledFrames = 2;
    }
}

// Registration and Hooks
void RegisterArrowCycle() {
    COND_VB_SHOULD(VB_SHIELD_FROM_BUTTON_HOLD, CVAR, {
        if (CanCycleArrows()) {
            Player* player = GET_PLAYER(gPlayState);
            Input* input = CONTROLLER1(&gPlayState->state);

            // Suppress Shield input when holding an arrow in Z-Target mode
            if (IsHoldingBow(player) && sJustCycledFrames > 0 && CHECK_BTN_ANY(input->cur.button, BTN_R)) {
                *should = false;
            }
        }
    });

    COND_VB_SHOULD(VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON, CVAR, {
        if (CanCycleArrows()) {
            Player* player = GET_PLAYER(gPlayState);
            Input* input = CONTROLLER1(&gPlayState->state);

            // Suppress Shield input first person cancel when aiming the bow
            if (IsAimingBow(player) && CHECK_BTN_ANY(input->cur.button, BTN_R)) {
                *should = false;
            }
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) { ArrowCycleMain(); });
}

static RegisterShipInitFunc initFunc(RegisterArrowCycle, { CVAR_NAME });
