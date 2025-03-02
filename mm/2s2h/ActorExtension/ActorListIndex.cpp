
#include "ActorListIndex.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"

ActorExtensionId actorListIndexActorExt = 0;
s16 currentActorListIndex = -1;

static RegisterShipInitFunc initFunc(
    []() {
        if (actorListIndexActorExt == 0) {
            actorListIndexActorExt = ActorExtension_CreateForAll(sizeof(s16));
        }
    },
    {});

s16 GetActorListIndex(Actor* actor) {
    s16* listIndex = (s16*)ActorExtension_Get(actor, actorListIndexActorExt);
    if (listIndex == NULL) {
        return -1;
    }

    return *listIndex;
}

void SetActorListIndex(Actor* actor, s16 index) {
    s16* listIndex = (s16*)ActorExtension_Get(actor, actorListIndexActorExt);

    if (listIndex == NULL) {
        assert(false);
    } else {
        *listIndex = index;
    }
}
