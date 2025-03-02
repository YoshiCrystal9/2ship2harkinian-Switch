
#ifndef ACTOR_LIST_INDEX_H
#define ACTOR_LIST_INDEX_H

#include <libultraship/libultraship.h>
#include "2s2h/ActorExtension/ActorExtension.h"

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

extern ActorExtensionId actorListIndexActorExt;
extern s16 currentActorListIndex;

s16 GetActorListIndex(Actor* actor);
void SetActorListIndex(Actor* actor, s16 index);

#ifdef __cplusplus
}
#endif

#endif // ACTOR_LIST_INDEX_H
