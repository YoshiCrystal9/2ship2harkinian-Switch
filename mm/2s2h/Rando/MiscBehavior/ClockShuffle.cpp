#include "ClockShuffle.h"
#include "Rando/Rando.h"
#include "Rando/Logic/Logic.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64game.h"
#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
#include "overlays/actors/ovl_En_Test4/z_en_test4.h"
#include "functions.h"
}

using namespace Rando::Logic;

namespace Rando {

// ============================================================================
// CLOCK ITEM MANAGEMENT
// ============================================================================

namespace ClockItems {

// Internal half-day indices

// Convert a rando item ID to its corresponding half-day index
int GetHalfDayIndexFromClockItem(RandoItemId clockItemId) {
    switch (clockItemId) {
        case RI_TIME_DAY_1:
            return HALF_DAY1_DAY;
        case RI_TIME_NIGHT_1:
            return HALF_DAY1_NIGHT;
        case RI_TIME_DAY_2:
            return HALF_DAY2_DAY;
        case RI_TIME_NIGHT_2:
            return HALF_DAY2_NIGHT;
        case RI_TIME_DAY_3:
            return HALF_DAY3_DAY;
        case RI_TIME_NIGHT_3:
            return HALF_DAY3_NIGHT;
        default:
            return INVALID;
    }
}

// Convert a half-day index back to its rando item ID
RandoItemId GetClockItemFromHalfDayIndex(int halfDayIndex) {
    if (halfDayIndex < 0 || halfDayIndex >= HALF_COUNT) {
        return RI_UNKNOWN;
    }

    // Map each half-day index to its corresponding rando item
    static const RandoItemId clockItemMap[] = {
        RI_TIME_DAY_1,   // HALF_DAY1_DAY   (index 0)
        RI_TIME_NIGHT_1, // HALF_DAY1_NIGHT (index 1)
        RI_TIME_DAY_2,   // HALF_DAY2_DAY   (index 2)
        RI_TIME_NIGHT_2, // HALF_DAY2_NIGHT (index 3)
        RI_TIME_DAY_3,   // HALF_DAY3_DAY   (index 4)
        RI_TIME_NIGHT_3, // HALF_DAY3_NIGHT (index 5)
    };

    return clockItemMap[halfDayIndex];
}

u8 GetAllOwnedHalfDaysMask() {
    u8 ownedMask = 0;
    for (int i = 0; i < HALF_COUNT; ++i) {
        if (OwnsClockHalfDay(i)) {
            ownedMask |= (1 << i);
        }
    }
    return ownedMask;
}

int FindEarliestOwnedHalfDay(bool searchFromEnd) {
    if (searchFromEnd) {
        for (int i = HALF_COUNT - 1; i >= 0; --i) {
            if (OwnsClockHalfDay(i))
                return i;
        }
    } else {
        for (int i = 0; i < HALF_COUNT; ++i) {
            if (OwnsClockHalfDay(i))
                return i;
        }
    }
    return INVALID;
}

int FindNextOwnedHalfDayAfter(int startHalfDay, u8 ownedMask) {
    if (startHalfDay < 0 || startHalfDay >= HALF_COUNT) {
        return TERMINAL_STATE; // Invalid input, go to terminal state
    }

    // Search for the next owned half-day after the start point
    for (int halfDayIndex = startHalfDay + 1; halfDayIndex < HALF_COUNT; ++halfDayIndex) {
        if (ownedMask & (1 << halfDayIndex)) {
            return halfDayIndex;
        }
    }

    return TERMINAL_STATE; // No owned half-days found after start point
}

// Check if a rando item is a clock item
bool IsClockItem(RandoItemId itemId) {
    return (itemId >= RI_TIME_DAY_1 && itemId <= RI_TIME_NIGHT_3) || itemId == RI_TIME_PROGRESSIVE;
}

// Check if a clock item is a day clock (vs night clock)
bool IsDayClock(RandoItemId itemId) {
    return itemId == RI_TIME_DAY_1 || itemId == RI_TIME_DAY_2 || itemId == RI_TIME_DAY_3;
}

} // namespace ClockItems

namespace ClockShuffle {

// ============================================================================
// INTERNAL TYPES AND DATA
// ============================================================================

// Configuration for each half-day's timing
struct HalfDayTimeConfig {
    u8 dayNumber;  // Which day (1, 2, or 3)
    u16 startTime; // When this half-day begins (6:00 AM or 6:00 PM)
    u16 endTime;   // When this half-day ends (5:59 AM or 5:59 PM)
};

// ============================================================================
// TIME CONFIGURATION DATA
// ============================================================================

constexpr u16 DAWN_TIME = CLOCK_TIME(6, 0);           // 6:00 AM - start of day
constexpr u16 DUSK_TIME = CLOCK_TIME(18, 0);          // 6:00 PM - start of night
constexpr u16 DAWN_END_TIME = CLOCK_TIME(5, 59);      // 5:59 AM - end of night
constexpr u16 DUSK_END_TIME = CLOCK_TIME(17, 59);     // 5:59 PM - end of day
constexpr u16 DAY_0_0559_TIME = CLOCK_TIME(6, 0) - 1; // Day 0, 5:59 AM - Cycle Reset Time
// Vanilla uses CLOCK_TIME(6, 0) - 1 = 16383, NOT CLOCK_TIME(5, 59) = 16338 for cycle resets
// This 45-unit difference has to be accounted for.

// ============================================================================
// TERMINAL TIME HELPER FUNCTIONS
// ============================================================================

// Convert slider minutes (0-359) to CLOCK_TIME format
u16 MinutesToClockTime(int minutes) {
    return CLOCK_TIME(0, minutes); // 00:00 + minutes
}

// Get the configured terminal state time from the saved rando option
u16 GetConfiguredTerminalTime() {
    int terminalMinutes = RANDO_SAVE_OPTIONS[RO_CLOCK_TERMINAL_TIME];
    return MinutesToClockTime(terminalMinutes);
}

// Configuration for each half-day's timing and behavior
constexpr HalfDayTimeConfig HALF_DAY_CONFIGS[] = {
    /* HALF_DAY1_DAY   */ { 1, DAWN_TIME, DUSK_END_TIME }, // Day 1: 6:00 AM - 5:59 PM
    /* HALF_DAY1_NIGHT */ { 1, DUSK_TIME, DAWN_END_TIME }, // Day 1: 6:00 PM - 5:59 AM
    /* HALF_DAY2_DAY   */ { 2, DAWN_TIME, DUSK_END_TIME }, // Day 2: 6:00 AM - 5:59 PM
    /* HALF_DAY2_NIGHT */ { 2, DUSK_TIME, DAWN_END_TIME }, // Day 2: 6:00 PM - 5:59 AM
    /* HALF_DAY3_DAY   */ { 3, DAWN_TIME, DUSK_END_TIME }, // Day 3: 6:00 AM - 5:59 PM
    /* HALF_DAY3_NIGHT */ { 3, DUSK_TIME, DAWN_END_TIME }, // Day 3: 6:00 PM - 5:59 AM
};

// ============================================================================
// TIME DETECTION AND CONFIGURATION
// ============================================================================

const HalfDayTimeConfig* GetHalfDayTimeConfig(int halfDayIndex) {
    if (halfDayIndex < 0 || halfDayIndex >= ClockItems::HALF_COUNT) {
        return nullptr;
    }

    return &HALF_DAY_CONFIGS[halfDayIndex];
}

bool IsCurrentlyNightTime(u16 gameTime) {
    return (gameTime >= DUSK_TIME) || (gameTime < DAWN_TIME);
}

// Check if time value represents night (inline for performance)
inline bool IsNightTime(u16 time) {
    return (time < GAME_TIME_DAY_START) || (time >= GAME_TIME_NIGHT_START);
}

// Check if a given day/time is in the configured terminal state range
bool IsInTerminalRange(s32 day, u16 time) {
    if (day != 3) {
        return false;
    }

    u16 terminalTime = GetConfiguredTerminalTime();

    // Terminal range spans from terminal time until 6:00 AM (may wrap around midnight)
    if (terminalTime >= DUSK_TIME) {
        // Terminal starts in evening (18:00-23:59), range wraps through midnight to dawn
        return (time >= terminalTime) || (time < DAWN_TIME);
    } else {
        // Terminal starts after midnight (0:00-5:59), range goes until dawn
        return (time >= terminalTime && time < DAWN_TIME);
    }
}

// Calculate half-day index from day/time (extracted for reuse)
int GetHalfDayIndexFromTime(s32 day, u16 time) {
    if (day < 1)
        return 0;
    int halfDay = (day - 1) * 2;
    if (IsNightTime(time))
        halfDay++;
    return halfDay;
}

int GetCurrentHalfDayIndex() {
    const u16 currentTime = gSaveContext.save.time;
    const s32 currentDay = gSaveContext.save.day;

    // Handle moon crash/game over sequence: Day 4 or higher means the moon has fallen and the game is ending.
    if (currentDay >= 4) {
        return ClockItems::TERMINAL_STATE; // Use TERMINAL_STATE as a sentinel for post-crash state.
    }

    // The game uses Day 0 as a transition before placing the player at the correct half-day.
    if (currentDay == 0) {
        return ClockItems::TERMINAL_STATE; // Signal: handle Day 0 as a reset/redirect.
    }

    // This is what ClockShuffle treats as a "buffer" period before the moon crash.
    // The buffer length is now configurable via RO_CLOCK_TERMINAL_TIME.
    if (IsInTerminalRange(currentDay, currentTime)) {
        return ClockItems::TERMINAL_STATE;
    }

    const bool isNight = IsCurrentlyNightTime(currentTime);

    // Figure out which half-day we're in:
    // - Each day has two halves: day (even indices) and night (odd indices)
    // - Day 1's day is index 0, night is 1; Day 2's day is 2, night is 3, etc.
    // - So: (currentDay - 1) * 2 gives us the starting index for that day (0 for Day 1, 2 for Day 2, 4 for Day 3)
    // - If it's night, add 1; if it's day, add 0.
    // Example: Day 2 night → (2-1)*2 + 1 = 2 + 1 = 3
    return (currentDay - 1) * 2 + (isNight ? 1 : 0);
}

// ============================================================================
// TIME MANIPULATION FUNCTIONS
// ============================================================================

// Apply a new time to the game, updating all related state
void SetGameTime(u8 day, u16 time) {
    gSaveContext.save.day = day;
    gSaveContext.save.time = time;
    gSaveContext.save.isNight = IsCurrentlyNightTime(time);
    gSaveContext.save.eventDayCount = day;
}

// Set time to half-day start with proper music handling
void SetTimeToHalfDayStart(int halfDayIndex) {
    // Don't try to set time for terminal state
    if (halfDayIndex == ClockItems::TERMINAL_STATE) {
        return;
    }

    // Get the configuration for this half-day
    const HalfDayTimeConfig* config = GetHalfDayTimeConfig(halfDayIndex);
    if (!config) {
        return;
    }

    // Set time to the start of this half-day
    SetGameTime(config->dayNumber, config->startTime);

    // Reset gSceneSeqState to prevent morning sequence from playing for night halves
    // This is needed because DayTelop or previous transitions may have set it to SCENESEQ_MORNING
    if (IsCurrentlyNightTime(config->startTime)) {
        gSceneSeqState = SCENESEQ_DEFAULT;
    }
}

// Check if a scene needs to be reloaded for time skips (whitelist approach)
bool SceneNeedsReloadForTimeSkip(s16 sceneId) {
    // Use a whitelist approach - only reload scenes that actually need it
    // These scenes are extremely time-sensitive and require reload
    switch (sceneId) {
        case SCENE_BOWLING: // Honey & Darling - minigame mode changes based on day/time
            return true;
        case SCENE_CLOCKTOWER: // South Clock Town - Clock Tower platform appears at midnight Night 3
            return true;
        default:
            return false; // Most scenes handle time changes without reload
    }
}

// Force a scene transition to reload the current area
void ForceSceneReload() {
    Player* player = GET_PLAYER(gPlayState);

    // Set up the transition parameters
    gPlayState->nextEntrance = gSaveContext.save.entrance;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_FADE_BLACK_FAST;

    // Set up respawn data to return to the same location
    Play_SetRespawnData(gPlayState, RESPAWN_MODE_RETURN, gSaveContext.save.entrance, gPlayState->roomCtx.curRoom.num,
                        PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B), &player->actor.world.pos, player->actor.world.rot.y);

    // Configure the transition
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
    gSaveContext.respawnFlag = 2;
}

// ============================================================================
// PROACTIVE TIME CHECKING
// ============================================================================
// This is separated into two functions for clarity:
// - ShouldSkipTime: Pure decision logic to determine if a skip is needed
// - CheckAndSkipUnownedTime: Application logic that performs the time skip
//
// Key implementation patterns:
// - Lookahead calculation (current time + 1 minute)
// - Inline night check (time < GAME_TIME_DAY_START || time >= GAME_TIME_NIGHT_START)
// - Day transition detection via prevTime
// - Direct time/day modification followed by day-- trick for dawn
// - prevTime = newTime - 1 minute
//
// Additional features:
// - Terminal state: If no owned clocks, jump to configured terminal time (not Day 4)
// - RandoInf flags for tracking owned half-days
// - GameInteractor hooks for seamless integration

// Decision function: Determines if we should skip time and calculates target half-day
// Returns true if skip is needed, false otherwise
bool ShouldSkipTime(s32 day, u16 time, int* outNextHalfDay) {
    // Early exits
    if (gPlayState->envCtx.sceneTimeSpeed == 0 || Play_InCsMode(gPlayState) || day >= 4) {
        return false;
    }

    // Check terminal range
    if (IsInTerminalRange(day, time)) {
        return false;
    }

    // Calculate and check current half-day ownership
    int currentHalfDay = GetHalfDayIndexFromTime(day, time);
    if (currentHalfDay >= 0 && currentHalfDay < ClockItems::HALF_COUNT && OwnsClockHalfDay(currentHalfDay)) {
        return false;
    }

    // Find next owned half-day
    int nextHalfDay = ClockItems::TERMINAL_STATE;
    for (int i = currentHalfDay + 1; i < ClockItems::HALF_COUNT; ++i) {
        if (OwnsClockHalfDay(i)) {
            nextHalfDay = i;
            break;
        }
    }

    *outNextHalfDay = nextHalfDay;
    return true;
}

// Apply time skip to target half-day (extracted helper)
void ApplyTimeSkip(int nextHalfDay, EnTest4* enTest4) {
    s32 day = (nextHalfDay / 2) + 1;
    u16 time = (nextHalfDay & 1) ? GAME_TIME_NIGHT_START : GAME_TIME_DAY_START;

    // Terminal state override
    if (nextHalfDay == ClockItems::TERMINAL_STATE) {
        day = 3;
        time = GetConfiguredTerminalTime();
    }

    // Apply time change
    gSaveContext.save.day = day;
    gSaveContext.save.time = time;

    // Update actor state
    enTest4->daytimeIndex = IsCurrentlyNightTime(time) ? 1 : 0;

    // Handle scene reload for time-sensitive scenes
    if (SceneNeedsReloadForTimeSkip(gPlayState->sceneId)) {
        ForceSceneReload();
        enTest4->prevTime = time - CLOCK_TIME(0, 1);
        return;
    }

    // Terminal state handling
    if (nextHalfDay == ClockItems::TERMINAL_STATE) {
        enTest4->prevTime = time - CLOCK_TIME(0, 1);
        return;
    }

    // Day transition handling
    if (time == GAME_TIME_DAY_START) {
        gSaveContext.save.day--;
    } else {
        Interface_NewDay(gPlayState, gSaveContext.save.day);
        Environment_NewDay(&gPlayState->envCtx);
    }

    enTest4->prevTime = time - CLOCK_TIME(0, 1);
}

// Application function: Applies the time skip
// Check if time is about to cross into an unowned half-day and skip forward if needed
void CheckAndSkipUnownedTime(Actor* timeActor) {
    // Get EnTest4 actor for state access
    EnTest4* enTest4 = (EnTest4*)timeActor;

    // Skip if eventInf bit 0x0f is set (dog race)
    if (gSaveContext.eventInf[0] & 0x0F) {
        return;
    }

    // Calculate lookahead day/time
    s32 day = gSaveContext.save.day;
    u16 time = gSaveContext.save.time + CLOCK_TIME(0, 1);

    // Check for day transition using actor's prevTime
    // Night check: time < GAME_TIME_DAY_START || time >= GAME_TIME_NIGHT_START
    bool prevWasNight = IsNightTime(enTest4->prevTime);
    bool nextIsNight = IsNightTime(time);
    if (prevWasNight && !nextIsNight) {
        day++;
    }

    if (day >= 4) {
        return; // Don't process moon crash
    }

    // Check if we should skip time (decision logic)
    int nextHalfDay;
    if (!ShouldSkipTime(day, time, &nextHalfDay)) {
        return; // No skip needed
    }

    // Apply the time skip using extracted helper
    ApplyTimeSkip(nextHalfDay, enTest4);
}

// ============================================================================
// PUBLIC API
// ============================================================================

// Check if a specific day/time is owned by the player in ClockShuffle mode
bool IsTimeOwnedForClockShuffle(s32 day, u16 time) {
    if (!RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE]) {
        return true;
    }

    if (day < 1 || day > 3)
        return true;
    if (IsInTerminalRange(day, time))
        return true;

    int halfDayIndex = GetHalfDayIndexFromTime(day, time);
    return OwnsClockHalfDay(halfDayIndex);
}

// Get a formatted description of a time period for error messages
std::string GetTimeDescriptionForMessage(s32 day, u16 time) {
    // Handle terminal state
    if (IsInTerminalRange(day, time)) {
        return "%rFinal Hours%w";
    }

    // Determine if this is night time
    bool isNight = IsCurrentlyNightTime(time);

    std::string description;
    if (isNight) {
        description = "%rNight of the ";
    } else {
        description = "%rDawn of the ";
    }

    // Add day ordinal
    if (day == 1) {
        description += "First";
    } else if (day == 2) {
        description += "Second";
    } else if (day == 3) {
        description += "Third";
    } else {
        description += "Unknown";
    }

    description += " Day%w";
    return description;
}

// Helper for initial file load time correction
void CorrectInitialTime() {
    const int earliestOwnedHalfDay = ClockItems::FindEarliestOwnedHalfDay(false);
    if (earliestOwnedHalfDay == ClockItems::INVALID)
        return;

    bool isDayHalf = (earliestOwnedHalfDay % 2 == 0);
    int targetDay = (earliestOwnedHalfDay / 2) + 1;

    if (isDayHalf) {
        SetGameTime(targetDay - 1, CLOCK_TIME(6, 0) - 1);
    } else {
        SetTimeToHalfDayStart(earliestOwnedHalfDay);
    }
}

void ProcessClockShuffleMessage(u16* textId, bool* loadFromMessageTable, bool isSongOfTime) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

    // Determine target half-day based on which song is used
    int targetHalfDay;
    if (isSongOfTime) {
        // Song of Time: go to earliest owned half-day
        targetHalfDay = ClockItems::FindEarliestOwnedHalfDay(false);
    } else {
        // Song of Double Time: go to next owned half-day after current
        int currentHalfDay = GetCurrentHalfDayIndex();
        u8 ownedHalfDaysMask = ClockItems::GetAllOwnedHalfDaysMask();
        targetHalfDay = ClockItems::FindNextOwnedHalfDayAfter(currentHalfDay, ownedHalfDaysMask);
    }

    std::string destinationText;
    if (targetHalfDay == ClockItems::TERMINAL_STATE) {
        destinationText = "%rFinal Hours%w";
    } else {
        // Convert half-day index to readable text
        int targetDay = (targetHalfDay / 2) + 1;
        bool isNight = (targetHalfDay % 2 == 1);

        if (isNight) {
            destinationText = "%rNight of ";
            if (targetDay == 1)
                destinationText += "First";
            else if (targetDay == 2)
                destinationText += "Second";
            else if (targetDay == 3)
                destinationText += "Third";
            destinationText += " Day%w";
        } else {
            destinationText = "%rDawn of the ";
            if (targetDay == 1)
                destinationText += "First";
            else if (targetDay == 2)
                destinationText += "Second";
            else if (targetDay == 3)
                destinationText += "Third";
            destinationText += " Day%w";
        }
    }

    // Use different message format for each song
    if (isSongOfTime) {
        entry.msg = "Save and return to " + destinationText + "?\n%gYes\nNo\xC2";
    } else { // Song of Double Time
        entry.msg = "Time moves strangely...\nProceed to " + destinationText + "?\n%gYes\nNo\xC2";
    }

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void OnFileLoad() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE];

    // Correct Day 0 time on file load BEFORE scene initialization
    // OnSaveLoad fires before Play_Init, ensuring time is correct before Environment_PlaySceneSequence processes audio
    // This prevents bird chirps from playing when correcting to night half-days on initial spawn
    if (shouldRegister) {
        // Check if this is initial spawn (day=0) and needs correction
        if (!gSaveContext.save.isOwlSave && (gSaveContext.save.day == 0 && gSaveContext.save.time == DAY_0_0559_TIME)) {
            CorrectInitialTime();
        }
    }

    // Hook EnTest4 BEFORE vanilla update to proactively check for time skips
    // This is critical: we must modify time BEFORE vanilla processes it!
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_TEST4, shouldRegister, [](Actor* actor, bool* should) {
        CheckAndSkipUnownedTime(actor);
        *should = true; // Always let vanilla continue with our modified time
    });

    // Hook Song of Time and Song of Double Time message IDs
    COND_ID_HOOK(OnOpenText, 0x1B8A, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        ProcessClockShuffleMessage(textId, loadFromMessageTable, true);
    });

    auto onDoubleTime = [](u16* textId, bool* loadFromMessageTable) {
        ProcessClockShuffleMessage(textId, loadFromMessageTable, false);
    };

    COND_ID_HOOK(OnOpenText, 0x1B91, shouldRegister, onDoubleTime);
    COND_ID_HOOK(OnOpenText, 0x1B90, shouldRegister, onDoubleTime);
    COND_ID_HOOK(OnOpenText, 0x1B8F, shouldRegister, onDoubleTime);
    COND_ID_HOOK(OnOpenText, 0x1B92, shouldRegister, onDoubleTime);
    COND_ID_HOOK(OnOpenText, 0x1B8E, shouldRegister, onDoubleTime);

    COND_VB_SHOULD(VB_TIME_UNTIL_MOON_CRASH_CALCULATION, shouldRegister, {
        *should = false; // Skip vanilla calculation

        // Get the time variable that was passed
        u32* timeVar = va_arg(args, u32*);

        // Calculate owned time remaining
        u8 ownedHalfDaysMask = ClockItems::GetAllOwnedHalfDaysMask();
        u32 totalHours = 0;

        for (int halfDayIndex = 0; halfDayIndex < ClockItems::HALF_COUNT; ++halfDayIndex) {
            if (ownedHalfDaysMask & (1 << halfDayIndex)) {
                totalHours += 12; // Each half-day is 12 hours
            }
        }

        // Add final hours based on configured terminal time if we're in terminal state
        // OR if we don't own Night 3 (which means we'll end up in terminal state)
        bool shouldIncludeTerminalHours =
            (GetCurrentHalfDayIndex() == ClockItems::TERMINAL_STATE) || !OwnsClockHalfDay(ClockItems::HALF_DAY3_NIGHT);

        if (shouldIncludeTerminalHours) {
            // Calculate remaining hours from configured terminal time to 6:00 AM
            u16 terminalTime = GetConfiguredTerminalTime();

            // Calculate time difference (terminal time to dawn)
            u32 terminalZeroed = ZERO_DAY_START(terminalTime);
            u32 timeDiff = (DAY_LENGTH - terminalZeroed) % DAY_LENGTH;

            // Convert to hours (round up to ensure we don't under-calculate)
            u32 terminalHours = (timeDiff + CLOCK_TIME_HOUR - 1) / CLOCK_TIME_HOUR;
            totalHours += terminalHours;
        }

        u32 ownedTime = totalHours * CLOCK_TIME_HOUR;

        // Compensate for the -1 minute offset that vanilla uses
        // Vanilla uses CLOCK_TIME(6, 0) - 1, Clock Shuffle uses CLOCK_TIME(6, 0)
        // So we need to add 1 minute to match vanilla behavior
        ownedTime += CLOCK_TIME_MINUTE;

        // Set the time variable to our calculated value
        *timeVar = ownedTime;
    });

    // Hook scarecrow dance time skip to redirect to next owned half-day
    COND_VB_SHOULD(VB_SCARECROW_DANCE_SET_TIME, shouldRegister, {
        *should = false; // Skip vanilla behavior

        // Calculate next owned half-day after current
        int currentHalfDay = GetCurrentHalfDayIndex();
        u8 ownedHalfDaysMask = ClockItems::GetAllOwnedHalfDaysMask();
        int nextHalfDay = ClockItems::FindNextOwnedHalfDayAfter(currentHalfDay, ownedHalfDaysMask);

        if (nextHalfDay == ClockItems::TERMINAL_STATE) {
            // Jump to terminal time
            gSaveContext.save.day = 3;
            gSaveContext.save.time = GetConfiguredTerminalTime();
            gSaveContext.respawnFlag = -8; // No daytelop for terminal
        } else {
            // Get target half-day configuration
            const HalfDayTimeConfig* config = GetHalfDayTimeConfig(nextHalfDay);
            bool isNightHalf = (nextHalfDay % 2 == 1);
            s32 targetDay = config->dayNumber;
            u16 targetTime = config->startTime;

            if (isNightHalf) {
                // Advancing to night - use respawnFlag -8 (no daytelop)
                gSaveContext.save.day = targetDay;
                gSaveContext.save.time = targetTime;
                gSaveContext.respawnFlag = -8;
            } else {
                // Advancing to dawn - use respawnFlag -4 (triggers daytelop)
                // CRITICAL: Subtract 1 from day because daytelop will increment it
                gSaveContext.save.day = targetDay - 1;
                gSaveContext.save.time = targetTime;
                gSaveContext.respawnFlag = -4;
                SET_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
            }
        }
    });
}

} // namespace ClockShuffle
} // namespace Rando
