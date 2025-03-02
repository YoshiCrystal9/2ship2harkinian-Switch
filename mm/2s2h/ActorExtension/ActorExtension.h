
#ifndef ACTOR_EXTENSION_H
#define ACTOR_EXTENSION_H

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

typedef u32 ActorExtensionId;

void* ActorExtension_Get(Actor* actor, ActorExtensionId id);

// These should only ever be called once, before any actors are spawned
ActorExtensionId ActorExtension_CreateForId(s16 actorId, size_t size);
ActorExtensionId ActorExtension_CreateForAll(size_t size);

// Internal, you shouldn't have to touch these
void ActorExtension_Alloc(Actor* actor, s16 actorId);
void ActorExtension_Free(Actor* actor);

#ifdef __cplusplus
}
#endif

#endif // ACTOR_EXTENSION_H
