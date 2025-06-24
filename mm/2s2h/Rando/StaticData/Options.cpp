#include "StaticData.h"

namespace Rando {

namespace StaticData {

#define RO(id, defaultValue)                             \
    {                                                    \
        id, {                                            \
            id, #id, "gRando.Options." #id, defaultValue \
        }                                                \
    }

// clang-format off
std::map<RandoOptionId, RandoStaticOption> Options = {
    RO(RO_ACCESS_DUNGEONS,             RO_ACCESS_DUNGEONS_FORM_AND_SONG),
    RO(RO_ACCESS_MAJORA_REMAINS_COUNT, 0),
    RO(RO_ACCESS_MOON_REMAINS_COUNT,   4),
    RO(RO_ACCESS_TRIALS,               RO_ACCESS_TRIALS_20_MASKS),
    RO(RO_HINTS_BOSS_REMAINS,          RO_GENERIC_OFF),
    RO(RO_HINTS_GOSSIP_STONES,         RO_GENERIC_OFF),
    RO(RO_HINTS_HOOKSHOT,              RO_GENERIC_OFF),
    RO(RO_HINTS_OATH_TO_ORDER,         RO_GENERIC_OFF),
    RO(RO_HINTS_PURCHASEABLE,          RO_GENERIC_OFF),
    RO(RO_HINTS_SPIDER_HOUSES,         RO_GENERIC_OFF),
    RO(RO_LOGIC,                       RO_LOGIC_GLITCHLESS),
    RO(RO_PLENTIFUL_ITEMS,             RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BARREL_DROPS,        RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BOSS_REMAINS,        RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BOSS_SOULS,          RO_GENERIC_OFF),
    RO(RO_SHUFFLE_COWS,                RO_GENERIC_OFF),
    RO(RO_SHUFFLE_CRATE_DROPS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_FREESTANDING_ITEMS,  RO_GENERIC_OFF),
    RO(RO_SHUFFLE_GOLD_SKULLTULAS,     RO_GENERIC_OFF),
    RO(RO_SHUFFLE_GRASS_DROPS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_OWL_STATUES,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_POT_DROPS,           RO_GENERIC_OFF),
    RO(RO_SHUFFLE_SHOPS,               RO_GENERIC_OFF),
	RO(RO_SHUFFLE_SNOWBALL_DROPS,      RO_GENERIC_OFF),
    RO(RO_SHUFFLE_TINGLE_SHOPS,        RO_GENERIC_OFF),
    RO(RO_STARTING_CONSUMABLES,        RO_GENERIC_OFF),
    RO(RO_STARTING_HEALTH,             3),
    RO(RO_STARTING_ITEMS_1,            0),
    RO(RO_STARTING_ITEMS_2,            2149581312),
    RO(RO_STARTING_ITEMS_3,            2048),
    RO(RO_STARTING_MAPS_AND_COMPASSES, RO_GENERIC_OFF),
    RO(RO_STARTING_RUPEES,             RO_GENERIC_OFF),
};
// clang-format on

RandoOptionId GetOptionIdFromName(const char* name) {
    for (auto& [randoOptionId, randoStaticOption] : Options) {
        if (strcmp(name, randoStaticOption.name) == 0) {
            return randoOptionId;
        }
    }
    return RO_MAX;
}

} // namespace StaticData

} // namespace Rando
