#include "Logic.h"

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNoLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    std::vector<RandoItemId> junkPool;
    for (auto& randoCheckId : checkPool) {
        if (RANDO_SAVE_CHECKS[randoCheckId].skipped) {
            uint32_t index = 0;
            for (auto& item : itemPool) {
                if (Rando::StaticData::Items[item].randoItemType == RITYPE_JUNK) {
                    junkPool.push_back(item);
                    itemPool.erase(itemPool.begin() + index);
                    break;
                }
                index++;
            }
        }
    }

    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
    }

    for (auto& randoCheckId : checkPool) {
        if (randoCheckId == RC_UNKNOWN) {
            continue;
        }

        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
        if (RANDO_SAVE_CHECKS[randoCheckId].skipped == true) {
            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = junkPool.back();
            junkPool.pop_back();
            continue;
        }
        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
        itemPool.pop_back();
    }
}

} // namespace Logic

} // namespace Rando
