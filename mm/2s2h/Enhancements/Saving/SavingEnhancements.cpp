#include <libultraship/bridge/consolevariablebridge.h>
#include "BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include <variables.h>
#include <functions.h>
}

#define CVAR_REMEMBER_SAVE_LOCATION_NAME "gEnhancements.Saving.RememberSaveLocation"
#define CVAR_REMEMBER_SAVE_LOCATION CVarGetInteger(CVAR_REMEMBER_SAVE_LOCATION_NAME, 0)

static uint32_t autosaveInterval = 0;
static uint32_t iconTimer = 0;
static uint64_t currentTimestamp = 0;
static uint64_t lastSaveTimestamp = GetUnixTimestamp();
static int lastEntrance = -1;
static int entranceToSave = -1;

static HOOK_ID autosaveGameStateUpdateHookId = 0;
static HOOK_ID autosaveGameStateDrawFinishHookId = 0;
static HOOK_ID skipEntranceCutsceneHookId = 0;
static HOOK_ID gameplayStartHookId = 0;

// Used for saving through Autosaves and Pause Menu saves.
extern "C" int SavingEnhancements_GetSaveEntrance() {
    if (CVAR_REMEMBER_SAVE_LOCATION) {
        // Maintain respawn information, used for grottos
        for (int i = 0; i < RESPAWN_MODE_MAX; i++) {
            gSaveContext.save.shipSaveInfo.respawn[i] = gSaveContext.respawn[i];
        }
        // Daytelop on new game, with Time Shuffle, makes it possible for entranceToSave to be -1. Given that the player
        // must be at this entrance in that scenario, just use it as a fallback.
        return entranceToSave < 0 ? ENTRANCE(SOUTH_CLOCK_TOWN, 0) : entranceToSave;
    } else {
        switch (gPlayState->sceneId) {
            // Woodfall Temple + Odolwa
            case SCENE_MITURIN:
            case SCENE_MITURIN_BS:
                return ENTRANCE(WOODFALL_TEMPLE, 0);
            // Snowhead Temple + Goht
            case SCENE_HAKUGIN:
            case SCENE_HAKUGIN_BS:
                return ENTRANCE(SNOWHEAD_TEMPLE, 0);
            // Great Bay Temple + Gyorg
            case SCENE_SEA:
            case SCENE_SEA_BS:
                return ENTRANCE(GREAT_BAY_TEMPLE, 0);
            // Stone Tower Temple
            case SCENE_INISIE_N:
                return ENTRANCE(STONE_TOWER_TEMPLE, 0);
            // Stone Tower Temple (inverted) + Twinmold
            case SCENE_INISIE_R:
            case SCENE_INISIE_BS:
                return ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0);
            default:
                return ENTRANCE(SOUTH_CLOCK_TOWN, 0);
        }
    }
}

extern "C" bool SavingEnhancements_CanSave() {
    // Game State
    if (gPlayState == NULL || GET_PLAYER(gPlayState) == NULL) {
        return false;
    }

    // Owl saving available
    if (!gSaveContext.flashSaveAvailable || gSaveContext.fileNum == 255) {
        return false;
    }

    // Not in a blocking cutscene
    if (Player_InBlockingCsMode(gPlayState, GET_PLAYER(gPlayState))) {
        return false;
    }

    // Not in the middle of dialog
    if (gPlayState->msgCtx.msgMode != 0) {
        return false;
    }

    // Hasn't gotten to clock town yet
    if (gPlayState->sceneId == SCENE_SPOT00 || gPlayState->sceneId == SCENE_LOST_WOODS ||
        gPlayState->sceneId == SCENE_OPENINGDAN) {
        return false;
    }

    // Can't save once you've gone to the moon
    if (gPlayState->sceneId == SCENE_SOUGEN || gPlayState->sceneId == SCENE_LAST_LINK ||
        gPlayState->sceneId == SCENE_LAST_DEKU || gPlayState->sceneId == SCENE_LAST_GORON ||
        gPlayState->sceneId == SCENE_LAST_ZORA || gPlayState->sceneId == SCENE_LAST_BS) {
        return false;
    }

    // Not in minigames that set temporary flags
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_08_01) || CHECK_WEEKEVENTREG(WEEKEVENTREG_82_08) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_90_20) || CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT) ||
        CHECK_EVENTINF(EVENTINF_34) || CHECK_EVENTINF(EVENTINF_41)) {
        return false;
    }

    return true;
}

extern "C" void SavingEnhancements_AdvancePlaytime() {
    if (gSaveContext.save.shipSaveInfo.fileCompletedAt == 0) {
        uint64_t timestamp = GetUnixTimestamp();
        gSaveContext.save.shipSaveInfo.filePlaytime += timestamp - gSaveContext.shipSaveContext.lastTimeLog;
        gSaveContext.shipSaveContext.lastTimeLog = timestamp;
    }
}

void DeleteOwlSave() {
    // Remove Owl Save on time cycle reset, needed when persisting owl saves and/or when
    // creating owl saves without the player being send back to the file select screen.

    // Delete Owl Save
    func_80147314(&gPlayState->sramCtx, gSaveContext.fileNum);

    // Set it to not be an owl save so after reloading the save file it doesn't try to load at the owl's position in
    // clock town
    gSaveContext.save.isOwlSave = false;
}

void DrawAutosaveIcon() {
    // 5 seconds (100 frames) of showing the owl save icon to signify autosave has happened.
    if (iconTimer != 0) {
        float opacity = 255.0;
        // Fade in icon
        if (iconTimer > 80) {
            opacity = 255.0 - (((iconTimer - 80.0) / 20.0) * 255);
            // Fade out icon
        } else if (iconTimer < 20) {
            opacity = (iconTimer / 20.0) * 255.0;
        }
        Interface_DrawAutosaveIcon(gPlayState, uint16_t(opacity));
        iconTimer--;
    }
}

void HandleAutoSave() {
    // Check if the interval has passed in minutes.
    autosaveInterval = CVarGetInteger("gEnhancements.Saving.AutosaveInterval", 5) * 60000;
    currentTimestamp = GetUnixTimestamp();
    if ((currentTimestamp - lastSaveTimestamp) < autosaveInterval) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL) {
        return;
    }

    // If owl save available to create, do it and reset the interval.
    if (SavingEnhancements_CanSave() && gPlayState->pauseCtx.state == 0) {

        // Reset timestamp, set icon timer to show autosave icon for 5 seconds (100 frames)
        lastSaveTimestamp = GetUnixTimestamp();
        iconTimer = 100;

        // Create owl save
        gSaveContext.save.isOwlSave = true;
        gSaveContext.save.shipSaveInfo.pauseSaveEntrance = SavingEnhancements_GetSaveEntrance();
        SavingEnhancements_AdvancePlaytime();
        Play_SaveCycleSceneFlags(gPlayState);
        gSaveContext.save.saveInfo.playerData.savedSceneId = gPlayState->sceneId;
        func_8014546C(&gPlayState->sramCtx);
        Sram_SetFlashPagesOwlSave(&gPlayState->sramCtx,
                                  gFlashOwlSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                  gFlashOwlSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
        Sram_StartWriteToFlashOwlSave(&gPlayState->sramCtx);
        gSaveContext.save.isOwlSave = false;
        gSaveContext.save.shipSaveInfo.pauseSaveEntrance = -1;
    }
}

/*
 * This respawn data is used for multiple things. Beyond the obvious usage for handling player respawns, this structure
 * also maintains state information when entering shared grottos. This code executes from OnSaveLoad, which runs after
 * save data is populated. This must run after that, otherwise the RESPAWN_MODE_DOWN entrance would get set to
 * ENTR_LOAD_OPENING, which in turn would lead to a crash if the save is within a grotto and the player dies before
 * leaving.
 */
void loadRespawnData(s16 fileNum) {
    for (int i = 0; i < RESPAWN_MODE_MAX; i++) {
        gSaveContext.respawn[i] = gSaveContext.save.shipSaveInfo.respawn[i];
    }
}

/*
 * Upon loading a save, skip any cutscenes that would play if the save is from a cutscene entrance (e.g. owl warps, Link
 * bowing at Mikau's grave, etc.). An OnPassPlayerInputs hook is used to detect when gameplay actually starts (any
 * entrance cutscenes are done), at which point the cutscene skip hook is unregistered. This handles any potential cases
 * where multiple cutscenes play in succession.
 */
static void UnregisterEntranceCutsceneSkip() {
    if (skipEntranceCutsceneHookId) {
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(
            skipEntranceCutsceneHookId);
        skipEntranceCutsceneHookId = 0;
    }

    if (gameplayStartHookId) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPassPlayerInputs>(gameplayStartHookId);
        gameplayStartHookId = 0;
    }
}

void skipEntranceCutsceneOnLoad(s16 fileNum) {
    // Clean up any existing hooks first
    UnregisterEntranceCutsceneSkip();
    // Register hook to skip entrance cutscenes - may skip multiple if they chain
    skipEntranceCutsceneHookId = REGISTER_VB_SHOULD(VB_START_CUTSCENE, {
        // Only skip normal cutscenes
        if (gSaveContext.gameMode == GAMEMODE_NORMAL && gPlayState != nullptr && gPlayState->sceneId != SCENE_SPOT00) {
            *should = false;
        }
    });

    // Register hook to detect when gameplay starts (all cutscenes done)
    // OnPassPlayerInputs only fires during normal gameplay, not during cutscenes
    gameplayStartHookId =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPassPlayerInputs>([](Input* input) {
            // Gameplay has started; any entrance cutscenes are done
            // Now unregister both hooks so normal cutscenes can play
            UnregisterEntranceCutsceneSkip();
        });
}

void RegisterSavingEnhancements() {
    REGISTER_VB_SHOULD(VB_DELETE_OWL_SAVE, {
        if (CVarGetInteger("gEnhancements.Saving.PersistentOwlSaves", 0) ||
            gSaveContext.save.shipSaveInfo.pauseSaveEntrance != -1) {
            *should = false;
        }
    });

    COND_HOOK(OnSaveLoad, true, [](s16 fileNum) {
        if (gSaveContext.save.shipSaveInfo.fileCreatedAt == 0) {
            gSaveContext.save.shipSaveInfo.fileCreatedAt = GetUnixTimestamp();
        }
        gSaveContext.shipSaveContext.lastTimeLog = GetUnixTimestamp();
        lastEntrance = entranceToSave = gSaveContext.save.shipSaveInfo.pauseSaveEntrance;
    });

    // Owl statue prompt
    COND_ID_HOOK(OnOpenText, 0xC01, true,
                 [](u16* textId, bool* loadFromMessageTable) { SavingEnhancements_AdvancePlaytime(); });

    // Finished the game, mark fileCompletedAt accordingly
    COND_HOOK(OnGameCompletion, true, []() {
        if (gSaveContext.save.shipSaveInfo.fileCompletedAt == 0) {
            SavingEnhancements_AdvancePlaytime();
            gSaveContext.save.shipSaveInfo.fileCompletedAt = GetUnixTimestamp();
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::BeforeEndOfCycleSave>([]() {
        SavingEnhancements_AdvancePlaytime();
        DeleteOwlSave();
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::BeforeMoonCrashSaveReset>([]() { DeleteOwlSave(); });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(loadRespawnData);
}

void RegisterAutosave() {
    if (autosaveGameStateUpdateHookId) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameStateUpdate>(autosaveGameStateUpdateHookId);
        autosaveGameStateUpdateHookId = 0;
    }

    if (autosaveGameStateDrawFinishHookId) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameStateDrawFinish>(
            autosaveGameStateDrawFinishHookId);
        autosaveGameStateDrawFinishHookId = 0;
    }

    if (CVarGetInteger("gEnhancements.Saving.Autosave", 0)) {
        autosaveGameStateUpdateHookId =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateUpdate>([]() {
                if (gPlayState == nullptr) {
                    return;
                }

                HandleAutoSave();
            });

        autosaveGameStateDrawFinishHookId =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateDrawFinish>([]() {
                if (gPlayState == nullptr) {
                    return;
                }

                DrawAutosaveIcon();
            });
    }
}

void RegisterRememberSaveLocation() {
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR_REMEMBER_SAVE_LOCATION, {
        /*
         * Update the entrance to save, unless we're leaving a grotto. Grottos exit to entrance 0 of the destination
         * scene and adjust the position manually. In effect, there is no real entrance to target for loading purposes,
         * so we just load into the last grotto instead under those circumstances.
         */
        if (lastEntrance != -1 && !(Entrance_GetSceneIdAbsolute(gSaveContext.save.entrance) != SCENE_KAKUSIANA &&
                                    Entrance_GetSceneIdAbsolute(lastEntrance) == SCENE_KAKUSIANA)) {
            entranceToSave = gSaveContext.save.entrance;
        }
        lastEntrance = gSaveContext.save.entrance;
    });

    COND_HOOK(OnSaveLoad, CVAR_REMEMBER_SAVE_LOCATION, skipEntranceCutsceneOnLoad);
}

static RegisterShipInitFunc initFunc(RegisterRememberSaveLocation, { CVAR_REMEMBER_SAVE_LOCATION_NAME });
