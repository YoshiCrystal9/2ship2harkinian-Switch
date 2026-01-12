#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Js/z_en_js.h"
}

void OverrideSubJsText(u16* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->talkActor == NULL) {
        return;
    }
    s32 jsType = ENJS_GET_TYPE(player->talkActor);
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

    switch (*textId) {
        // Do you want to play? Yes/No
        case 0x2215: {
            switch (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS]) {
                case RO_ACCESS_TRIALS_20_MASKS: {
                    u8 requiredMasks = 0;
                    switch (jsType) {
                        case 1:
                            requiredMasks = 2;
                            break;
                        case 2:
                            requiredMasks = 6;
                            break;
                        case 3:
                            requiredMasks = 12;
                            break;
                        case 4:
                            requiredMasks = 20;
                            break;
                    }

                    if (Rando::Logic::MoonMaskCount() < requiredMasks) {
                        entry.msg =
                            "You need more masks to play... Come back when you have at least {{requiredMasks}}...";
                        entry.nextMessageID = 0x2216;
                        CustomMessage::Replace(&entry.msg, "{{requiredMasks}}", std::to_string(requiredMasks));
                    }
                    break;
                }
                case RO_ACCESS_TRIALS_REMAINS: {
                    QuestItem questItem = QUEST_17; // Unused
                    RandoItemId itemId = RI_NONE;
                    switch (jsType) {
                        case 1:
                            questItem = QUEST_REMAINS_ODOLWA;
                            itemId = RI_REMAINS_ODOLWA;
                            break;
                        case 2:
                            questItem = QUEST_REMAINS_GOHT;
                            itemId = RI_REMAINS_GOHT;
                            break;
                        case 3:
                            questItem = QUEST_REMAINS_GYORG;
                            itemId = RI_REMAINS_GYORG;
                            break;
                        case 4:
                            questItem = QUEST_REMAINS_TWINMOLD;
                            itemId = RI_REMAINS_TWINMOLD;
                            break;
                    }
                    if (questItem != QUEST_17 && !CHECK_QUEST_ITEM(questItem)) {
                        entry.msg = "You need to find {{item}} before you can play...";
                        entry.nextMessageID = 0x2216;
                        CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::GetItemName(itemId));
                    }
                    break;
                }
                case RO_ACCESS_TRIALS_FORMS:
                    if (jsType == 1 && !HAS_ITEM(ITEM_MASK_DEKU)) {
                        entry.msg = "You need to find the Deku Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    } else if (jsType == 2 && !HAS_ITEM(ITEM_MASK_GORON)) {
                        entry.msg = "You need to find the Goron Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    } else if (jsType == 3 && !HAS_ITEM(ITEM_MASK_ZORA)) {
                        entry.msg = "You need to find the Zora Mask before you can play...";
                        entry.nextMessageID = 0x2216;
                    }
                    break;
            }

            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
            break;
        }
        // You don't want to play? oh...
        case 0x2216: {
            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.msg = "...\xE0";
            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
            return;
        }
    }
}

void OverrideMainJsText(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    bool override = false;

    switch (*textId) {
        // Will you play...with me? Yes/No
        case 0x21FC: {
            if (!RANDO_SAVE_CHECKS[RC_MOON_FIERCE_DEITY_MASK].cycleObtained && Rando::Logic::MoonMaskCount() >= 20) {
                RANDO_SAVE_CHECKS[RC_MOON_FIERCE_DEITY_MASK].eligible = true;
                entry.msg = "You... You seem strong... I have a gift for you...";
                entry.nextMessageID = 0x21FD;
                override = true;
            }

            if (Rando::Logic::MoonMaskCount() < RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_MASKS_COUNT] ||
                Rando::Logic::RemainsCount() < RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_REMAINS_COUNT]) {
                entry.msg = "You are not strong enough to play with me...";
                entry.nextMessageID = 0x21FD;
                override = true;
            }
            break;
        }
        // Skip having to say yes twice
        // "You only have weak masks...So...you'll play?"
        case 0x21FE: {
            *textId = 0x2200;
            break;
        }
        // You're a boring kid.
        case 0x21FD: {
            entry.msg = "...\xE0";
            override = true;
            break;
        }
    }
    if (override) {
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    }
}

void Rando::ActorBehavior::InitEnJsBehavior() {
    COND_VB_SHOULD(VB_JS_CONSIDER_ELIGIBLE_FOR_DEITY, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_JS_OVERRIDE_MASK_CHECK, IS_RANDO, {
        s32* jsType = va_arg(args, s32*);
        bool* result = va_arg(args, bool*);

        switch (*jsType) {
            case 1:
            case 2:
            case 3:
            case 4:
                *should = true;
                *result = true;
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                if (gPlayState->sceneId != SCENE_SOUGEN) {
                    *should = true;
                    *result = false;
                }
                break;
            default:
                break;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x2215, IS_RANDO, OverrideSubJsText);
    COND_ID_HOOK(OnOpenText, 0x2216, IS_RANDO, OverrideSubJsText);
    COND_ID_HOOK(OnOpenText, 0x21FC, IS_RANDO, OverrideMainJsText);
    COND_ID_HOOK(OnOpenText, 0x21FE, IS_RANDO, OverrideMainJsText);
    COND_ID_HOOK(OnOpenText, 0x21FD, IS_RANDO, OverrideMainJsText);
}
