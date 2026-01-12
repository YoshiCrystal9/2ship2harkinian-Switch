#include "StaticData.h"

extern "C" {
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"
}

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
    RO(RO_ACCESS_MAJORA_MASKS_COUNT,   0),
    RO(RO_ACCESS_MAJORA_REMAINS_COUNT, 0),
    RO(RO_ACCESS_MOON_MASKS_COUNT,     0),
    RO(RO_ACCESS_MOON_REMAINS_COUNT,   4),
    RO(RO_ACCESS_TRIALS,               RO_ACCESS_TRIALS_20_MASKS),
    RO(RO_HINTS_BOSS_REMAINS,          RO_GENERIC_OFF),
    RO(RO_HINTS_GOSSIP_STONES,         RO_GENERIC_OFF),
    RO(RO_HINTS_HOOKSHOT,              RO_GENERIC_OFF),
    RO(RO_HINTS_OATH_TO_ORDER,         RO_GENERIC_OFF),
    RO(RO_HINTS_PURCHASEABLE,          RO_GENERIC_OFF),
    RO(RO_HINTS_SPIDER_HOUSES,         RO_GENERIC_OFF),
    RO(RO_TRAP_AMOUNT,                 5),
    RO(RO_LOGIC,                       RO_LOGIC_GLITCHLESS),
    RO(RO_MINIMUM_SKULLTULA_TOKENS,    SPIDER_HOUSE_TOKENS_REQUIRED),
    RO(RO_MINIMUM_STRAY_FAIRIES,       STRAY_FAIRY_SCATTERED_TOTAL),
    RO(RO_PLENTIFUL_ITEMS,             RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BARREL_DROPS,        RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BOSS_REMAINS,        RO_GENERIC_OFF),
    RO(RO_SHUFFLE_BOSS_SOULS,          RO_GENERIC_OFF),
    RO(RO_SHUFFLE_COWS,                RO_GENERIC_OFF),
    RO(RO_SHUFFLE_CRATE_DROPS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_ENEMY_DROPS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_ENEMY_SOULS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_FREESTANDING_ITEMS,  RO_GENERIC_OFF),
    RO(RO_SHUFFLE_FROGS,               RO_GENERIC_OFF),
    RO(RO_SHUFFLE_GOLD_SKULLTULAS,     RO_GENERIC_OFF),
    RO(RO_SHUFFLE_GRASS_DROPS,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_TRAPS,               RO_GENERIC_OFF),
    RO(RO_SHUFFLE_OWL_STATUES,         RO_GENERIC_OFF),
    RO(RO_SHUFFLE_OCARINA_BUTTONS,     RO_GENERIC_OFF),
    RO(RO_SHUFFLE_POT_DROPS,           RO_GENERIC_OFF),
    RO(RO_SHUFFLE_SHOPS,               RO_GENERIC_OFF),
	RO(RO_SHUFFLE_SNOWBALL_DROPS,      RO_GENERIC_OFF),
    RO(RO_SHUFFLE_SWIM,                RO_GENERIC_OFF),
    RO(RO_SHUFFLE_TINGLE_SHOPS,        RO_GENERIC_OFF),
    RO(RO_SHUFFLE_TRIFORCE_PIECES,     RO_GENERIC_OFF),
    RO(RO_STARTING_CONSUMABLES,        RO_GENERIC_OFF),
    RO(RO_STARTING_HEALTH,             3),
    RO(RO_STARTING_MAPS_AND_COMPASSES, RO_GENERIC_OFF),
    RO(RO_STARTING_RUPEES,             RO_GENERIC_OFF),
    RO(RO_TRIFORCE_PIECES_MAX,         DEFAULT_TRIFORCE_PIECES_MAX),
    RO(RO_TRIFORCE_PIECES_REQUIRED,    DEFAULT_TRIFORCE_PIECES_MAX),
    RO(RO_CLOCK_SHUFFLE,               RO_GENERIC_OFF),
    RO(RO_CLOCK_SHUFFLE_PROGRESSIVE,   RO_CLOCK_SHUFFLE_RANDOM),
    RO(RO_CLOCK_TERMINAL_TIME,         0), // Default: 00:00 (midnight)
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
