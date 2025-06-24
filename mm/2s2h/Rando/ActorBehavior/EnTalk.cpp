#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
}

void ApplyRemainsHint(u16* textId, bool* loadFromMessageTable) {
    static int remainsHintIndex = 0;

    if (remainsHintIndex > 4) {
        remainsHintIndex = 0;
    }

    u8 icon = 0xFE;
    std::string msg;
    RandoItemId randoItemId = RI_NONE;

    if (remainsHintIndex == 0) {
        msg = "        Witcher Wanted:\n"
              "These monsters are tormenting the "
              "local townfolk, will pay good money "
              "for their remains.";
    } else {
        msg = "         %g{{boss}}%w:\n"
              "Last seen in near %y{{location}}%w.";

        switch (remainsHintIndex) {
            case 1:
                CustomMessage::Replace(&msg, "{{boss}}", " Odolwa");
                randoItemId = RI_REMAINS_ODOLWA;
                break;
            case 2:
                CustomMessage::Replace(&msg, "{{boss}}", "  Goht");
                randoItemId = RI_REMAINS_GOHT;
                break;
            case 3:
                CustomMessage::Replace(&msg, "{{boss}}", "  Gyorg");
                randoItemId = RI_REMAINS_GYORG;
                break;
            case 4:
                CustomMessage::Replace(&msg, "{{boss}}", "Twinmold");
                randoItemId = RI_REMAINS_TWINMOLD;
                break;
        }

        icon = Rando::StaticData::GetIconForZMessage(randoItemId);
        RandoCheckId randoCheckId = Rando::FindItemPlacement(randoItemId);
        CustomMessage::Replace(&msg, "{{location}}",
                               Ship_GetSceneName(Rando::StaticData::Checks[randoCheckId].sceneId));
    }

    CustomMessage::Entry entry = {
        .icon = icon,
        .nextMessageID = remainsHintIndex >= 4 ? (u16)0xFFFF : (u16)0x1C06,
        .msg = msg,
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
    remainsHintIndex++;
}

void Rando::ActorBehavior::InitEnTalkBehavior() {
    // "Recruiting Soldiers..." Posters around Clock Town
    COND_ID_HOOK(OnOpenText, 0x1C06, IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_BOSS_REMAINS], ApplyRemainsHint);
}
