#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

// Re-definitions to avoid modifying source headers
#define TAKARAYA_WALL_ROWS 11
#define TAKARAYA_WALL_COLUMNS 8

extern "C" {
extern f32 sTakarayaWallHeights[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
extern u8 sTakarayaWallStates[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
}

// Re-definition to avoid modifying source headers
typedef enum { TAKARAYA_WALL_INACTIVE, TAKARAYA_WALL_RISING, TAKARAYA_WALL_FALLING } TakarayaWallCellState;

#define CVAR_NAME "gEnhancements.Minigames.TreasureChestShopShowFullMaze"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterTreasureChestShopFullMaze() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_TAKARAYA_WALL, CVAR, [](Actor* actor) {
        if (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == TIMER_STATE_OFF) {
            return;
        }

        for (int i = 0; i < TAKARAYA_WALL_ROWS; i++) {
            for (int j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
                if (sTakarayaWallHeights[i][j] >= 0.0f) {
                    if (Math_StepToF(&sTakarayaWallHeights[i][j], 120.0f, 15.0f)) {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_INACTIVE;
                    } else {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_RISING;
                    }
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTreasureChestShopFullMaze, { CVAR_NAME });
