#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenPort.h"

extern "C" {
#include "variables.h"
extern Input* sPlayerControlInput;
}

#define CVAR_SPEED_MODIFIER_NAME "gSettings.SpeedModifier.Enable"
#define CVAR_SPEED_MODIFIER_TOGGLE "gSettings.SpeedModifier.Toggle"
#define CVAR_WALK_MODIFIER_NAME "gSettings.SpeedModifier.WalkEnable"
#define CVAR_SWIM_MODIFIER_NAME "gSettings.SpeedModifier.SwimEnable"
#define CVAR_SPEED CVarGetInteger(CVAR_SPEED_MODIFIER_NAME, 0)
#define CVAR_SPEED_TOGGLE CVarGetInteger(CVAR_SPEED_MODIFIER_TOGGLE, 0)
#define CVAR_WALK CVarGetInteger(CVAR_WALK_MODIFIER_NAME, 0)
#define CVAR_SWIM CVarGetInteger(CVAR_SWIM_MODIFIER_NAME, 0)

bool speedToggle1;
bool speedToggle2;

void RegisterLinkSpeedModifier() {

    COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR_WALK && CVAR_SPEED, {
        f32* speedTarget = va_arg(args, f32*);

        if (CVAR_SPEED_TOGGLE) {
            if (speedToggle1) {
                *speedTarget *= CVarGetFloat("gSettings.SpeedModifier.WalkMapping1", 1.0f);
            } else if (speedToggle2) {
                *speedTarget *= CVarGetFloat("gSettings.SpeedModifier.WalkMapping2", 1.0f);
            }
        } else {
            if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_CUSTOM_MODIFIER1)) {
                *speedTarget *= CVarGetFloat("gSettings.SpeedModifier.WalkMapping1", 1.0f);
            } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_CUSTOM_MODIFIER2)) {
                *speedTarget *= CVarGetFloat("gSettings.SpeedModifier.WalkMapping2", 1.0f);
            }
        }
    });

    COND_VB_SHOULD(VB_SPEED_MODIFIER_SWIM, CVAR_SWIM && CVAR_SPEED, {
        *should = false;
        f32* incrStep = va_arg(args, f32*);
        f32* maxSpeed = va_arg(args, f32*);
        f32* speed = va_arg(args, f32*);
        f32* speedTarget = va_arg(args, f32*);
        f32 swimMod = 1.0f;

        if (CVAR_SPEED_TOGGLE) {
            if (speedToggle1) {
                swimMod *= CVarGetFloat("gSettings.SpeedModifier.SwimMapping1", 1.0f);
            } else if (speedToggle2) {
                swimMod *= CVarGetFloat("gSettings.SpeedModifier.SwimMapping2", 1.0f);
            }

            // sControlInput is NULL to prevent inputs while surfacing after obtaining an underwater item so we want
            // to ignore it for that case
        } else if (sPlayerControlInput != NULL) {
            if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_CUSTOM_MODIFIER1)) {
                swimMod *= CVarGetFloat("gSettings.SpeedModifier.SwimMapping1", 1.0f);
            } else if (CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_CUSTOM_MODIFIER2)) {
                swimMod *= CVarGetFloat("gSettings.SpeedModifier.SwimMapping2", 1.0f);
            }
        }

        *maxSpeed *= swimMod;

        Math_AsymStepToF(speed, *speedTarget * 0.8f * swimMod, *incrStep, (fabsf(*speed) * 0.02f) + 0.05f);
    });

    COND_HOOK(OnPassPlayerInputs, CVAR_SPEED && (CVAR_WALK || CVAR_SWIM), [](Input* input) {
        if (CVAR_SPEED_TOGGLE) {

            if (CHECK_BTN_ALL(input->press.button, BTN_CUSTOM_MODIFIER1)) {
                speedToggle1 = !speedToggle1;
                speedToggle2 = false;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_CUSTOM_MODIFIER2)) {
                speedToggle2 = !speedToggle2;
                speedToggle1 = false;
            }
        }
    });

    COND_HOOK(OnConsoleLogoUpdate, CVAR_SPEED && (CVAR_WALK || CVAR_SWIM), []() {
        speedToggle1 = false;
        speedToggle2 = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterLinkSpeedModifier, { CVAR_SPEED_MODIFIER_NAME, CVAR_SPEED_MODIFIER_TOGGLE,
                                                                  CVAR_WALK_MODIFIER_NAME, CVAR_SWIM_MODIFIER_NAME });