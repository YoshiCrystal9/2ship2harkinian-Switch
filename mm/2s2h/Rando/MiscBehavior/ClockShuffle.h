#ifndef RANDO_CLOCK_SHUFFLE_H
#define RANDO_CLOCK_SHUFFLE_H

#include "Rando/Types.h"
#include <string>
#include <vector>

extern "C" {
#include "variables.h"
#include "functions.h"
}

namespace Rando {
namespace ClockItems {

// Internal half-day indices
enum ClockHalfIndex : int {
    INVALID = -1,        // Invalid/not found/uninitialized half-day index
    HALF_DAY1_DAY = 0,   // Day 1, 6:00 AM - 5:59 PM
    HALF_DAY1_NIGHT = 1, // Day 1, 6:00 PM - 5:59 AM
    HALF_DAY2_DAY = 2,   // Day 2, 6:00 AM - 5:59 PM
    HALF_DAY2_NIGHT = 3, // Day 2, 6:00 PM - 5:59 AM
    HALF_DAY3_DAY = 4,   // Day 3, 6:00 AM - 5:59 PM
    HALF_DAY3_NIGHT = 5, // Day 3, 6:00 PM - 5:59 AM
    TERMINAL_STATE = 6,  // Terminal state (fallback for invalid/end states)
    HALF_COUNT = 6,      // Total number of regular half-days (0-5)
};

RandoItemId GetClockItemFromHalfDayIndex(int halfDayIndex);
int GetHalfDayIndexFromClockItem(RandoItemId clockItemId);
int FindEarliestOwnedHalfDay(bool searchFromEnd = false);
u8 GetAllOwnedHalfDaysMask();
bool IsClockItem(RandoItemId itemId);
bool IsDayClock(RandoItemId itemId);

} // namespace ClockItems

namespace ClockShuffle {

void OnFileLoad();
void SetTimeToHalfDayStart(int halfDayIndex);

bool IsTimeOwnedForClockShuffle(s32 day, u16 time);
int GetHalfDayIndexFromTime(s32 day, u16 time);
std::string GetTimeDescriptionForMessage(s32 day, u16 time);

} // namespace ClockShuffle
} // namespace Rando

#endif // RANDO_CLOCK_SHUFFLE_H
