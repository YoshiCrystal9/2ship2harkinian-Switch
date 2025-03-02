
#include "ActorExtension.h"

ActorExtensionId sNextActorExtensionId = 0;

struct ActorExtensionKeyHash {
    std::size_t operator()(const std::pair<Actor*, ActorExtensionId>& key) const {
        return std::hash<Actor*>{}(key.first) ^ (std::hash<ActorExtensionId>{}(key.second) << 1);
    }
};

std::unordered_map<ActorExtensionId, size_t> sGlobalActorExtensionSizes;
std::unordered_map<s16, std::unordered_map<ActorExtensionId, size_t>> sActorExtensionSizes;
std::unordered_map<std::pair<Actor*, ActorExtensionId>, void*, ActorExtensionKeyHash> sActorExtensionData;

void* ActorExtension_Get(Actor* actor, ActorExtensionId id) {
    if (actor == NULL) {
        return NULL;
    }

    auto it = sActorExtensionData.find(std::make_pair(actor, id));
    if (it == sActorExtensionData.end()) {
        return NULL;
    }

    return it->second;
}

ActorExtensionId ActorExtension_CreateForId(s16 actorId, size_t size) {
    ActorExtensionId id = ++sNextActorExtensionId;
    sActorExtensionSizes[actorId][id] = size;
    return id;
}

ActorExtensionId ActorExtension_CreateForAll(size_t size) {
    ActorExtensionId id = ++sNextActorExtensionId;
    sGlobalActorExtensionSizes[id] = size;
    return id;
}

void ActorExtension_Alloc(Actor* actor, s16 actorId) {
    if (actor == NULL) {
        return;
    }

    for (auto& [id, size] : sGlobalActorExtensionSizes) {
        void* data = malloc(size);
        memset(data, 0, size);
        sActorExtensionData[std::make_pair(actor, id)] = data;
    }

    for (auto& [id, size] : sActorExtensionSizes[actorId]) {
        void* data = malloc(size);
        memset(data, 0, size);
        sActorExtensionData[std::make_pair(actor, id)] = data;
    }
}

void ActorExtension_Free(Actor* actor) {
    if (actor == NULL) {
        return;
    }

    for (auto& [id, size] : sGlobalActorExtensionSizes) {
        auto it = sActorExtensionData.find(std::make_pair(actor, id));
        if (it != sActorExtensionData.end()) {
            free(it->second);
            sActorExtensionData.erase(it);
        }
    }

    for (auto& [id, size] : sActorExtensionSizes[actor->id]) {
        auto it = sActorExtensionData.find(std::make_pair(actor, id));
        if (it != sActorExtensionData.end()) {
            free(it->second);
            sActorExtensionData.erase(it);
        }
    }
}
