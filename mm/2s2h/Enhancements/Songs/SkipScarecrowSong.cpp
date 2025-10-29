#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Kakasi/z_en_kakasi.h"
}

#define CVAR_NAME "gEnhancements.Playback.SkipScarecrowSong"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipScarecrowSong() {
    COND_VB_SHOULD(VB_NEED_SCARECROW_SONG, CVAR, {
        EnKakasi* enKakasi = va_arg(args, EnKakasi*);
        /*
         * This is somewhat similar to the condition that the scarecrow normally checks, except it checks if the
         * instrument is being played at all instead of having played the Scarecrow's Song in particular, and it
         * bypasses the check that Link has taught Pierre a song this cycle.
         */
        if ((enKakasi->picto.actor.xzDistToPlayer < enKakasi->songSummonDist) &&
            ((BREG(1) != 0) || (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE))) {
            *should = true;
            // Properly get out of the ocarina playing state
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
            Message_CloseTextbox(gPlayState);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipScarecrowSong, { CVAR_NAME });
