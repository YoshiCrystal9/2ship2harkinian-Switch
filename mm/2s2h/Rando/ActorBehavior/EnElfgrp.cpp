#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "overlays/actors/ovl_En_Elfgrp/z_en_elfgrp.h"
}

void ApplyClockTownGreatFairyHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "%wPlease, find the Stray Fairy who's lost! We will reward you with %g{{article1}}{{item1}}%w and maybe "
               "even %g{{article2}}{{item2}}%w if you are worthy."
    };

    auto& randoStaticItem1 = Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].randoItemId];

    if (!Ship_IsCStringEmpty(randoStaticItem1.article)) {
        CustomMessage::Replace(&entry.msg, "{{article1}}", std::string(randoStaticItem1.article) + " ");
    } else {
        CustomMessage::Replace(&entry.msg, "{{article1}}", "");
    }

    CustomMessage::Replace(&entry.msg, "{{item1}}", randoStaticItem1.name);

    auto& randoStaticItem2 = Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].randoItemId];

    if (!Ship_IsCStringEmpty(randoStaticItem2.article)) {
        CustomMessage::Replace(&entry.msg, "{{article2}}", std::string(randoStaticItem2.article) + " ");
    } else {
        CustomMessage::Replace(&entry.msg, "{{article2}}", "");
    }

    CustomMessage::Replace(&entry.msg, "{{item2}}", randoStaticItem2.name);

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void ApplyGreatFairyHint(u16* textId, bool* loadFromMessageTable, RandoCheckId randoCheckId) {
    CustomMessage::Entry entry = {
        .msg = "%wPlease, find the Stray Fairies who match our color! We will reward you with %g{{article}}{{item}}%w"
    };

    auto& randoStaticItem = Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId];

    if (!Ship_IsCStringEmpty(randoStaticItem.article)) {
        CustomMessage::Replace(&entry.msg, "{{article}}", std::string(randoStaticItem.article) + " ");
    } else {
        CustomMessage::Replace(&entry.msg, "{{article}}", "");
    }

    CustomMessage::Replace(&entry.msg, "{{item}}", randoStaticItem.name);

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

// Handles the Great Fairy checks
void Rando::ActorBehavior::InitEnElfgrpBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, IS_RANDO, {
        *should = false;

        EnElfgrp* elfgrp = va_arg(args, EnElfgrp*);

        switch (elfgrp->type) {
            case ENELFGRP_TYPE_POWER:
                if (!RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_WOODFALL_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_WISDOM:
                if (!RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_SNOWHEAD_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_COURAGE:
                if (!RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_GREAT_BAY_GREAT_FAIRY].eligible = true;
                }
                break;
            case ENELFGRP_TYPE_KINDNESS:
                if (!RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_IKANA_GREAT_FAIRY].eligible = true;
                }
                break;
            default: // ENELFGRP_TYPE_MAGIC
                if (!RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY].eligible = true;
                }
                // In the game this uses an `else`, but in rando we are okay with both of these happening at the same
                // time
                if ((INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU || INV_CONTENT(ITEM_MASK_ZORA) == ITEM_MASK_ZORA ||
                     INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON) &&
                    !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_GREAT_FAIRY_ALT].eligible = true;
                }
                break;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_GREAT_FAIRY, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS, IS_RANDO, { *should = false; });

    COND_ID_HOOK(OnOpenText, 0x578, IS_RANDO, ApplyClockTownGreatFairyHint);
    COND_ID_HOOK(OnOpenText, 0x580, IS_RANDO, ApplyClockTownGreatFairyHint);

    COND_ID_HOOK(OnOpenText, 0x582, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_WOODFALL_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x583, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_WOODFALL_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x585, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_SNOWHEAD_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x586, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_SNOWHEAD_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x588, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_GREAT_BAY_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x589, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_GREAT_BAY_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x58B, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_IKANA_GREAT_FAIRY);
    });
    COND_ID_HOOK(OnOpenText, 0x58C, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        ApplyGreatFairyHint(textId, loadFromMessageTable, RC_IKANA_GREAT_FAIRY);
    });
}
