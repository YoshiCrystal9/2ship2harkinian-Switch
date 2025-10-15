#include "Logic.h"

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNearlyNoLogicToSaveContext(std::unordered_map<RandoCheckId, bool>& checkPool,
                                     std::vector<RandoItemId>& itemPool) {
    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
    }

    std::unordered_map<RandoItemId, RandoCheckId> importantItems;
    std::vector<RandoCheckId> safeChecks;

    std::unordered_map<RandoItemId, std::vector<SceneId>> itemToSceneBlacklist = {
        { RI_MASK_DEKU,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_SONATA,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_MASK_ZORA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_SONG_NOVA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_REMAINS_ODOLWA,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GOHT,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GYORG,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_TWINMOLD,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_OATH,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
    };

    for (auto& [randoCheckId, _] : checkPool) {
        if (randoCheckId == RC_UNKNOWN) {
            continue;
        }

        RandoItemId randoItemId = RI_NONE;
        if (RANDO_SAVE_CHECKS[randoCheckId].skipped) {
            uint32_t index = 0;
            for (auto& item : itemPool) {
                if (Rando::StaticData::Items[item].randoItemType == RITYPE_JUNK) {
                    randoItemId = item;
                    itemPool.erase(itemPool.begin() + index);
                    break;
                }
                index++;
            }
        } else {
            randoItemId = itemPool.back();
            itemPool.pop_back();
        }

        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;

        auto randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

        if (itemToSceneBlacklist.find(randoItemId) != itemToSceneBlacklist.end()) {
            importantItems[randoItemId] = randoCheckId;
        } else if (randoStaticCheck.sceneId != SCENE_MITURIN &&    // Woodfall Temple
                   randoStaticCheck.sceneId != SCENE_MITURIN_BS && // Woodfall Temple Boss
                   randoStaticCheck.sceneId != SCENE_SEA &&        // Great Bay Temple
                   randoStaticCheck.sceneId != SCENE_SEA_BS &&     // Great Bay Temple Boss
                   randoStaticCheck.sceneId != SCENE_LAST_DEKU &&  // Moon Deku
                   randoStaticCheck.sceneId != SCENE_LAST_GORON && // Moon Goron
                   randoStaticCheck.sceneId != SCENE_LAST_ZORA &&  // Moon Goron
                   randoStaticCheck.sceneId != SCENE_LAST_LINK &&  // Moon Human
                   randoStaticCheck.sceneId != SCENE_SOUGEN &&     // Moon
                   randoStaticCheck.sceneId != SCENE_LAST_BS       // Moon Boss
        ) {
            safeChecks.push_back(randoCheckId);
        }
    }

    for (auto& [randoItemId, blacklistedScenes] : itemToSceneBlacklist) {
        if (importantItems.find(randoItemId) != importantItems.end()) {
            auto randoStaticCheck = Rando::StaticData::Checks[importantItems.at(randoItemId)];
            const auto& blacklist = itemToSceneBlacklist.at(randoItemId);

            if (std::find(blacklist.begin(), blacklist.end(), randoStaticCheck.sceneId) != blacklist.end()) {
                auto otherCheckIndex = Ship_Random(0, safeChecks.size() - 1);
                RANDO_SAVE_CHECKS[importantItems[randoItemId]].randoItemId =
                    RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId;
                RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId = randoItemId;
                safeChecks.erase(safeChecks.begin() + otherCheckIndex);
            }
        }
    }
}

} // namespace Logic

} // namespace Rando
