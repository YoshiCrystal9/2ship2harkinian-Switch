#include "ActorBehavior.h"
#include "public/bridge/consolevariablebridge.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

std::map<std::pair<float, float>, RandoCheckId> freestandingMap = {
    // Beneath the Graveyard //
    { { -99.000000f, -260.000000f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_01 },
    { { -133.640945f, -280.000610f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_02 },
    { { -133.640945f, -319.999390f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_03 },
    { { -99.000000f, -340.000000f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_04 },
    { { -64.359055f, -319.999390f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_05 },
    { { -64.359055f, -280.000610f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_06 },
    { { -99.000000f, -300.000000f }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_07 },

    // Deku Palace //
    { { 439.640961f, 1368.999390f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_12 },
    { { 439.640961f, 1329.000610f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_13 },
    { { 405.0f, 1309.0f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_14 },
    { { 370.359039f, 1329.000610f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_15 },
    { { 370.359039f, 1368.999390f }, RC_DEKU_PALACE_FREESTANDING_RUPEE_16 },
    { { 405.0, 1389.0 }, RC_DEKU_PALACE_FREESTANDING_RUPEE_17 },
    { { 405.0, 1349.0 }, RC_DEKU_PALACE_FREESTANDING_RUPEE_18 },
};

void Rando::ActorBehavior::InitEnItem00Behavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor, bool* should) {
        EnItem00* item00 = (EnItem00*)actor;

        // If it's one of our items ignore it
        if (item00->actor.params == ITEM00_NOTHING) {
            return;
        }

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                                    ENITEM00_GET_7F00(actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            auto it = freestandingMap.find({ item00->actor.home.pos.x, item00->actor.home.pos.z });
            if (it == freestandingMap.end()) {
                return;
            } else {
                randoStaticCheck = Rando::StaticData::Checks[it->second];
            }
        }

        // Pots handle their own items, ignore them
        if (randoStaticCheck.randoCheckType == RCTYPE_POT) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
            return;
        }

        // Prevent the original item from spawning
        *should = false;

        s16 itemParams = CustomItem::KILL_ON_TOUCH;
        // Freestanding PoH & HC cannot be picked up by boomerangs
        if (randoStaticCheck.randoCheckType == RCTYPE_FREESTANDING) {
            itemParams |= CustomItem::ABLE_TO_ZORA_RANG;
        }
        // The heart piece in the bio baba grotto beehive needs to be tossed to fall to the ground
        if (randoStaticCheck.randoCheckId == RC_TERMINA_FIELD_BIO_BABA_GROTTO) {
            itemParams |= CustomItem::TOSS_ON_SPAWN;
        }

        // If it hasn't been collected yet, spawn a dummy item
        CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0, itemParams, randoStaticCheck.randoCheckId,
            [](Actor* actor, PlayState* play) {
                auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
                switch (randoStaticCheck.flagType) {
                    case FLAG_NONE:
                        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
                            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].eligible = true;
                        }
                        break;
                    case FLAG_CYCL_SCENE_COLLECTIBLE:
                        Flags_SetCollectible(play, randoStaticCheck.flag);
                        break;
                    default:
                        break;
                }
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM), actor);
            });
    });
}
