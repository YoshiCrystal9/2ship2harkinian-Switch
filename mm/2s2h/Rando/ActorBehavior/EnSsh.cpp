#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

void ApplySwampSpiderHouseHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "Make me...normal again...I'll give you %g{{article}}{{item}}%w...Please...help me...\xE0",
    };

    auto& randoStaticItem =
        Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].randoItemId];

    if (!Ship_IsCStringEmpty(randoStaticItem.article)) {
        CustomMessage::Replace(&entry.msg, "{{article}}", std::string(randoStaticItem.article) + " ");
    } else {
        CustomMessage::Replace(&entry.msg, "{{article}}", "");
    }

    CustomMessage::Replace(&entry.msg, "{{item}}", randoStaticItem.name);

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void ApplyOceanSpiderHouseHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "Huh? How'd I get up here... Why do I have %g{{article}}{{item}}%w in my pocket...?\xE0",
    };

    auto& randoStaticItem = Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_OCEAN_SPIDER_HOUSE_WALLET].randoItemId];

    if (!Ship_IsCStringEmpty(randoStaticItem.article)) {
        CustomMessage::Replace(&entry.msg, "{{article}}", std::string(randoStaticItem.article) + " ");
    } else {
        CustomMessage::Replace(&entry.msg, "{{article}}", "");
    }

    CustomMessage::Replace(&entry.msg, "{{item}}", randoStaticItem.name);

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void Rando::ActorBehavior::InitEnSshBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_SPIDER_HOUSES];

    // "Recruiting Soldiers..." Posters around Clock Town
    COND_ID_HOOK(OnOpenText, 0x915, shouldRegister, ApplySwampSpiderHouseHint);
    COND_ID_HOOK(OnOpenText, 0x1130, shouldRegister, ApplyOceanSpiderHouseHint);
    COND_ID_HOOK(OnOpenText, 0x1131, shouldRegister, ApplyOceanSpiderHouseHint);

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SSH, IS_RANDO, [](Actor* actor, bool* should) {
        // Skip first dialog
        SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN);
    });
}
