#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
	Regions[RR_MISCELLANEOUS] = RandoRegion{ .name = "Various Regions", .sceneId = SCENE_SPOT00,
	// This region currently directly houses nothing, but enemy drop checks reference the scene ID so that they can
	// appear together in the check tracker.
	};
}, {});
// clang-format on