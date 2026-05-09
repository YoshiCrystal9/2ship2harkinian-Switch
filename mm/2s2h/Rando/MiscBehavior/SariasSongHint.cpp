#include "2s2h/CustomMessage/CustomMessage.h"
#include "MiscBehavior.h"
#include "2s2h/ShipUtils.h"
#include <set>
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include <variables.h>
#include <z64ocarina.h>
s32 Message_ShouldAdvanceSilent(PlayState* play);
extern s16 sOcarinaSongFanfares[17];
extern s16 sLastPlayedSong;
}

static int playedSariasSongState = 0;

std::set<RandoItemId> priorityItems = {
    RI_BOW,       RI_HOOKSHOT,          RI_MASK_BLAST,  RI_BOMB_BAG_20,  RI_MASK_DEKU, RI_MASK_GORON,
    RI_MASK_ZORA, RI_MASK_FIERCE_DEITY, RI_SONG_SONATA, RI_SONG_LULLABY, RI_SONG_NOVA, RI_SONG_SOARING,
};

RandoCheckId GetProgressiveCheckInLogic() {
    std::unordered_map<RandoRegionId, Rando::Logic::RegionTimeState> regionTimeStates =
        Rando::Logic::InitializeRegionTimeStates(RR_MAX);
    std::set<RandoRegionId> reachableRegions = {};
    // Get connected entrances from starting & warp points
    Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions, regionTimeStates);
    // Get connected regions from current entrance (TODO: Make this optional)
    Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                       reachableRegions, regionTimeStates);

    std::vector<RandoCheckId> priorityChecks = {};
    std::vector<RandoCheckId> otherChecks = {};

    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            if (accessLogicFunc.first() && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
                !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
                RandoItemId itemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
                auto type = Rando::StaticData::Items[itemId].randoItemType;

                if (priorityItems.contains(itemId)) {
                    priorityChecks.push_back(randoCheckId);
                } else if (type == RITYPE_MAJOR || type == RITYPE_MASK) {
                    otherChecks.push_back(randoCheckId);
                }
            }
        }
    }

    // First, we try to return a priority check if one is available, in order of the priority items list.
    // If no priority checks are available, we return a random major/mask check.
    return priorityChecks.empty()
               ? (otherChecks.empty() ? RC_UNKNOWN : otherChecks[Ship_Random(0, otherChecks.size() - 1)])
               : priorityChecks[0];
}

void Rando::MiscBehavior::SariasSongHint() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONG_SARIA];

    // Fix vanilla issue where saria's song plays the majoras lair fanfare
    if (shouldRegister) {
        sOcarinaSongFanfares[OCARINA_SONG_SARIAS] = NA_BGM_SARIAS_SONG;
    } else {
        sOcarinaSongFanfares[OCARINA_SONG_SARIAS] = NA_BGM_MAJORAS_LAIR;
    }

    COND_VB_SHOULD(VB_SONG_AVAILABLE_TO_PLAY, shouldRegister, {
        uint8_t* songIndex = va_arg(args, uint8_t*);
        // If the currently played song is Sun's Song, set it to be available to be played.
        if (*songIndex == OCARINA_SONG_SARIAS && CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
            *should = true;
            playedSariasSongState = 1;
        }
    });

    COND_VB_SHOULD(VB_MSG_CAPTURE_MSGMODE_TEXT_DONE, shouldRegister, {
        if (playedSariasSongState && gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_RESTRICTED_SONG) {
            *should = true;

            if (Message_ShouldAdvanceSilent(gPlayState)) {
                if (gPlayState->msgCtx.choiceIndex == 0) {
                    playedSariasSongState = 2;
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    Message_ContinueTextbox(gPlayState, 0x1B95);
                } else {
                    playedSariasSongState = 0;
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    Message_CloseTextbox(gPlayState);
                    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
                }
            }
        }
    });

    COND_VB_SHOULD(VB_MSG_CAPTURE_MSGMODE_TEXT_CLOSING_OCARINA_ACTION, shouldRegister, {
        MessageContext* msgCtx = &gPlayState->msgCtx;

        if (sLastPlayedSong == OCARINA_SONG_SARIAS) {
            *should = true;
            Message_StartTextbox(gPlayState, 0x1B95, NULL);
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x1B95, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        CustomMessage::Entry entry;
        if (playedSariasSongState == 1) {
            entry.nextMessageID = 0x1B95;
            entry.msg = "Call out to an old friend for help? You can only do this once.\x02\x11\xC2Yes\x11No";
        } else if (playedSariasSongState == 2) {
            RandoCheckId randoCheckId = GetProgressiveCheckInLogic();
            entry.textboxType = TEXTBOX_TYPE_2;

            if (randoCheckId == RC_UNKNOWN) {
                entry.msg = "... You call out but there is no response ...";
            } else {
                entry.msg = "%g... Link? Is that you? Where have you been..?! Zelda has been worried sick about you! "
                            "... You need my help?\x10 Alright but just this once. Search %y{{location}}%g, you will "
                            "find what you need. Hurry now!";
                CustomMessage::Replace(&entry.msg, "{{location}}",
                                       Rando::StaticData::GetLocationNameForHint(randoCheckId, true));
                Rando::RemoveItem(RI_SONG_SARIA);
            }

            playedSariasSongState = 0;
        } else if (playedSariasSongState == 0) {
            return;
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
