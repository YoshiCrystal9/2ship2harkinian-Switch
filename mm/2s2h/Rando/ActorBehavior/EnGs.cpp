#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

#include <vector>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Gs/z_en_gs.h"
}

#define FIRST_GS_MESSAGE 0x20D1
#define SECOND_GS_MESSAGE 0x20C0

std::vector<std::string> flavorText = {
    "Good luck on your journey ...",
    "I hope you find what you're looking for ...",
    "... Evil is afoot",
    "Beware the moon's gaze",
    " .. It's dangerous to go alone",
};

s32 GetNormalizedCost() {
    s32 obtainedChecks = 0;
    s32 maxChecks = 0;
    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled) {
            maxChecks++;
            if (saveCheck.obtained) {
                obtainedChecks++;
            }
        }
    }

    return MAX(10, MIN(250, 10 + (obtainedChecks * (250 - 10)) / (maxChecks)));
}

RandoCheckId GetRandomCheck(bool repeatableOnlyObtained = false) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->talkActor == nullptr || player->talkActor->id != ACTOR_EN_GS) {
        return RC_UNKNOWN;
    }
    EnGs* enGs = (EnGs*)player->talkActor;

    std::vector<RandoCheckId> availableChecks;
    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled && Rando::StaticData::Items[saveCheck.randoItemId].randoItemType != RITYPE_JUNK &&
            (!repeatableOnlyObtained || !saveCheck.obtained)) {
            availableChecks.push_back(randoCheckId);
        }
    }

    if (availableChecks.empty()) {
        return RC_UNKNOWN;
    }

    if (repeatableOnlyObtained) {
        Ship_Random_Seed(gGameState->frames);
    } else {
        uint32_t seed = gPlayState->sceneId + enGs->actor.home.pos.x + enGs->actor.home.pos.z;
        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + seed);
    }
    return availableChecks[Ship_Random(0, availableChecks.size() - 1)];
}

void Rando::ActorBehavior::InitEnGsBehavior() {
    bool shouldRegister =
        IS_RANDO && (RANDO_SAVE_OPTIONS[RO_HINTS_GOSSIP_STONES] || RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]);

    COND_VB_SHOULD(VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED, shouldRegister, { *should = true; });

    // Override the message ID so that we can control the text
    COND_VB_SHOULD(VB_GS_CONTINUE_TEXTBOX, shouldRegister, {
        *should = false;
        Message_ContinueTextbox(gPlayState, SECOND_GS_MESSAGE);
    });

    COND_ID_HOOK(OnOpenText, FIRST_GS_MESSAGE, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (RANDO_SAVE_OPTIONS[RO_HINTS_GOSSIP_STONES]) {
            RandoCheckId randoCheckId = GetRandomCheck();
            if (randoCheckId == RC_UNKNOWN) {
                return;
            }

            entry.autoFormat = false;
            auto& saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

            entry.msg = "They say %g{{item}}%w is hidden at %y{{location}}%w.";

            CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::GetItemName(saveCheck.randoItemId));
            CustomMessage::Replace(&entry.msg, "{{location}}",
                                   Ship_GetSceneName(Rando::StaticData::Checks[randoCheckId].sceneId));

            // Replace colors before line break calculation
            CustomMessage::ReplaceColorChars(&entry.msg);

            CustomMessage::AddLineBreaks(&entry.msg);

            if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
                entry.msg += "\x10...\x13\x12";
            }
        } else {
            entry.msg = "";
        }

        if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
            entry.msg += "Trade %r{{rupees}} Rupees%w for a hint?\x02\x11\xC2No\x11Yes";
            s32 cost = GetNormalizedCost();
            CustomMessage::Replace(&entry.msg, "{{rupees}}", std::to_string(cost));

            CustomMessage::ReplaceColorChars(&entry.msg);
        }

        CustomMessage::EnsureMessageEnd(&entry.msg);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, SECOND_GS_MESSAGE, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        MessageContext* msgCtx = &gPlayState->msgCtx;
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
            if (msgCtx->choiceIndex == 1) {
                s32 cost = GetNormalizedCost();

                RandoCheckId randoCheckId = GetRandomCheck(true);
                if (gSaveContext.save.saveInfo.playerData.rupees < cost) {
                    entry.msg = "Foolish... You don't have enough rupees...";
                } else if (randoCheckId == RC_UNKNOWN) {
                    entry.msg = "I have no more hints for you...";
                } else {
                    RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

                    entry.msg = "Wise choice... They say %g{{item}}%w is hidden at %y{{location}}%w.";

                    CustomMessage::Replace(&entry.msg, "{{item}}",
                                           Rando::StaticData::GetItemName(saveCheck.randoItemId));
                    CustomMessage::Replace(&entry.msg, "{{location}}",
                                           Ship_GetSceneName(Rando::StaticData::Checks[randoCheckId].sceneId));

                    gSaveContext.rupeeAccumulator -= cost;
                    cost *= 2;
                }
            } else {
                entry.msg = "Foolish... Come back later when you have more sense.";
            }
        } else {
            entry.msg = flavorText[Ship_Random(0, flavorText.size() - 1)];
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Four Gossip Stone Grottos Heart Piece item grant behavior override
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_GS || *item != GI_HEART_PIECE) {
            return;
        }

        *should = false;

        refActor->parent = &player->actor;
    });
}
