#ifndef GAME_INTERACTOR_H
#define GAME_INTERACTOR_H

#include <stdarg.h>

#ifdef __cplusplus
#include <string>
#include <variant>
extern "C" {
#endif
#include "z64.h"
#ifdef __cplusplus
}
#endif

typedef enum {
    FLAG_NONE,
    FLAG_WEEK_EVENT_REG,
    FLAG_WEEK_EVENT_REG_HORSE_RACE,
    FLAG_EVENT_INF,
    FLAG_SCENES_VISIBLE,
    FLAG_OWL_ACTIVATION,
    FLAG_PERM_SCENE_CHEST,
    FLAG_PERM_SCENE_SWITCH,
    FLAG_PERM_SCENE_CLEARED_ROOM,
    FLAG_PERM_SCENE_COLLECTIBLE,
    FLAG_PERM_SCENE_UNK_14,
    FLAG_PERM_SCENE_ROOMS,
    FLAG_CYCL_SCENE_CHEST,
    FLAG_CYCL_SCENE_SWITCH,
    FLAG_CYCL_SCENE_CLEARED_ROOM,
    FLAG_CYCL_SCENE_COLLECTIBLE,
    FLAG_RANDO_INF,
} FlagType;

typedef enum {
    // Vanilla condition: gSaveContext.showTitleCard
    VB_SHOW_TITLE_CARD,
    VB_PLAY_ENTRANCE_CS,
    VB_KALEIDO_UNPAUSE_CLOSE,
    VB_DISABLE_FD_MASK,
    VB_DOGGY_RACE_SET_MAX_SPEED,
    VB_LOWER_RAZOR_SWORD_DURABILITY,
    VB_SET_BLAST_MASK_COOLDOWN_TIMER,
    VB_PATCH_POWER_CROUCH_STAB,
    VB_PATCH_SIDEROLL,
    VB_TATL_CONVERSATION_AVAILABLE,
    VB_PREVENT_MASK_TRANSFORMATION_CS,
    VB_RESET_PUTAWAY_TIMER,
    VB_SET_CLIMB_SPEED,
    VB_PREVENT_CLOCK_DISPLAY,
    VB_SONG_AVAILABLE_TO_PLAY,
    VB_USE_CUSTOM_CAMERA,
    VB_DELETE_OWL_SAVE,
    VB_MSG_SCRIPT_DEL_ITEM,
    VB_CONSIDER_BUNNY_HOOD_EQUIPPED,
    VB_USE_ITEM_EQUIP_MASK,
    VB_KALEIDO_DISPLAY_ITEM_TEXT,
    VB_USE_ITEM_CONSIDER_LINK_HUMAN,
    VB_DRAW_ITEM_EQUIPPED_OUTLINE,
    VB_PLAY_TRANSITION_CS,
    VB_PLAY_SONG_OF_TIME_CS,
    VB_TATL_INTERUPT_MSG,
    VB_TATL_INTERUPT_MSG3,
    VB_TATL_INTERUPT_MSG4,
    VB_TATL_INTERUPT_MSG6,
    VB_ITEM_BE_RESTRICTED,
    VB_APPLY_AIR_CONTROL,
    VB_DISABLE_LETTERBOX,
    VB_START_GREAT_FAIRY_CUTSCENE,
    VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS,
    VB_KILL_CLOCK_TOWN_STRAY_FAIRY,
    VB_CLOCK_TOWER_OPENING_CONSIDER_THIS_FIRST_CYCLE,
    VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY,
    VB_DRAW_SLIME_BODY_ITEM,
    VB_DRAW_OCARINA_IN_STK_HAND,
    VB_DRAW_FILE_SELECT_SMALL_EXTRA_INFO_BOX,
    VB_DRAW_FILE_SELECT_EXTRA_INFO_DETAILS,
    VB_DRAW_FILE_SELECT_OWL_SAVE_ICON,
    VB_OVERRIDE_CHAR02_LIMB,
    VB_STK_HAVE_OCARINA,
    VB_POST_CHAR02_LIMB,
    VB_ZTARGET_SPEED_CHECK,
    VB_GIVE_ITEM_FROM_ELFORG,
    VB_GIVE_ITEM_FROM_ITEM00,
    VB_GIVE_ITEM_FROM_SI,
    VB_GIVE_ITEM_FROM_CHEST,
    VB_POT_DROP_COLLECTIBLE,
    VB_GIVE_ITEM_FROM_COW,
    VB_COW_CONSIDER_EPONAS_SONG_PLAYED,
    VB_GIVE_ITEM_FROM_GREAT_FAIRY,
    VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER,
    VB_OSN_CONSIDER_ELIGIBLE_FOR_SONG_OF_HEALING,
    // Vanilla condition: (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) &&
    // !CHECK_QUEST_ITEM(QUEST_SONG_HEALING)
    VB_OSN_TEACH_SONG_OF_HEALING,
    VB_GIVE_ITEM_FROM_OSN,
    VB_GIVE_ITEM_FROM_MOONS_TEAR,
    VB_REVEAL_MOON_STONE_IN_CRATER,
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG,
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL,
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BEAN_REFILL,
    VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED,
    // Vanilla Condition: INV_CONTENT(ITEM_MASK_KAFEIS_MASK) != ITEM_MASK_KAFEIS_MASK
    VB_MADAME_AROMA_ASK_FOR_HELP,
    VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI,
    VB_HAVE_ROMANI_MASK,
    VB_GIVE_ROMANI_MASK,
    VB_JG_THINK_YOU_KNOW_LULLABY,
    VB_GIVE_ITEM_FROM_OFFER,
    VB_EXEC_MSG_EVENT,
    VB_THIEF_BIRD_STEAL,
    VB_PLAY_CREMIA_HUG_CUTSCENE,
    VB_FD_ALWAYS_WIELD_SWORD,
    VB_SHOULD_PUTAWAY,
    VB_ELEGY_CHECK_SCENE,
    VB_NEED_SCARECROW_SONG,
    VB_CHECK_HELD_ITEM_BUTTON_PRESS,
    VB_MAGIC_SPIN_ATTACK_CHECK_FORM,
    VB_TRANSFORM_THUNDER_MATRIX,
    VB_SAKON_TAKE_DAMAGE,
    VB_HAVE_KAMAROS_MASK,
    VB_START_CUTSCENE,
    VB_QUEUE_CUTSCENE,
    VB_CAMERA_SET_FOCAL_ACTOR,
    VB_GIVE_ITEM_FROM_MNK,
    VB_GIVE_ITEM_FROM_JG,
    VB_TERMINA_FIELD_BE_EMPTY,
    VB_FASTER_FIRST_CYCLE,
    VB_CHECK_FOR_ROOM_KEY,
    VB_DRAW_BOSS_REMAINS,
    VB_SPAWN_BOSS_REMAINS,
    VB_ACTIVATE_BOSS_WARP_PAD,
    VB_BE_HOOKSHOT_SURFACE,
    VB_GIVE_ITEM_FROM_KNIGHT,
    VB_STONE_HEISHI_SET_ACTION,
    VB_CONSIDER_DARMANI_HEALED,
    VB_CONSIDER_MIKAU_HEALED,
    VB_GIVE_ITEM_FROM_DMCHAR05,
    VB_DRAW_ITEM_FROM_DMCHAR05,
    VB_DAMAGE_MULTIPLIER,
    VB_DAMAGE_EFFECT,
    VB_DRAW_DAMAGE_EFFECT,
    VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS,
    VB_CHECK_BUMPER_COLLISION,
    VB_PLAY_HEART_CONTAINER_GET_FANFARE,
    VB_DEKU_GUARD_SHOW_SEARCH_BALLS,
    VB_DISPLAY_SONG_OF_DOUBLE_TIME_PROMPT,
    VB_SMITHY_START_UPGRADING_SWORD,
    VB_SMITHY_CHECK_FOR_RAZOR_SWORD,
    VB_SMITHY_CHECK_FOR_GILDED_SWORD,
    VB_HAVE_BLAST_MASK,
    VB_GIVE_NEW_WAVE_BOSSA_NOVA,
    VB_BANKER_GIVE_REWARD,
    VB_PASS_FIRST_BANK_THRESHOLD,
    VB_PASS_INTEREST_BANK_THRESHOLD,
    VB_PASS_SECOND_BANK_THRESHOLD,
    VB_PASS_SECOND_BANK_THRESHOLD_ALT,
    VB_CLEAR_B_BUTTON_FOR_NO_BOW,
    VB_NOT_AFFORD_TINGLE_MAP,
    VB_ALREADY_HAVE_TINGLE_MAP,
    VB_TINGLE_GIVE_MAP_UNLOCK,
    VB_OWL_STATUE_ACTIVATE,
    VB_OWL_STATUE_BE_ACTIVE,
    VB_HAVE_HEALED_PAMELAS_FATHER,
    VB_WIN_MALON_PRACTICE,
    VB_MALON_CONSIDER_EPONA_SONG_GIVEN,
    VB_GIVE_ITEM_FROM_MALON,
    VB_DOOR_HEALTH_CHECK_FAIL,
    VB_GIVE_LOTTERY_WINNINGS,
    VB_GIVE_HONEY_DARLING_REWARD,
    VB_GS_CONTINUE_TEXTBOX,
    VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED,
    VB_HAVE_GARO_MASK,
    VB_COLLECT_PLAYGROUND_RUPEE,
    VB_GUAY_DROP_RUPEE,
    VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED,
    VB_BLOCK_BEGIN_MOVE,
    VB_BLOCK_BE_FINISHED_PULLING,
    VB_SKATE_BLOCK_BEGIN_MOVE,
    VB_PUSH_BLOCK_SET_SPEED,
    VB_PUSH_BLOCK_SET_TIMER,
    VB_GIVE_DON_GERO_MASK,
    VB_TOILET_HAND_TAKE_ITEM,
    VB_ITEM_GIVE_SWORD_SET_FORM_EQUIP,
    VB_POT_DRAW_BE_OVERRIDDEN,
    VB_CRATE_DRAW_BE_OVERRIDDEN,
    VB_HAVE_MAGIC_FOR_TINGLE,
    VB_GIVE_KEATON_MASK,
    VB_GIVE_LETTER_TO_MAMA,
    VB_BARREL_OR_CRATE_DROP_COLLECTIBLE,
    VB_ALLOW_SONG_DOUBLE_TIME_ON_FINAL_NIGHT,
    VB_OWL_TELL_ABOUT_SHRINE,
    VB_ARCHERY_ADD_BONUS_POINTS,
    VB_HONEY_AND_DARLING_MINIGAME_FINISH,
    VB_MINIMAP_TOGGLE,
    VB_SHIELD_FROM_BUTTON_HOLD,
    VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON,
    VB_KILL_GORON_VILLAGE_OWL,
    VB_MONKEY_WAIT_TO_TALK_AFTER_APPROACH,
    VB_SETUP_EAST_CLOCK_TOWN_BOM_BOWL_MAN,
    VB_BE_ELIGBLE_FOR_BOMBERS_NOTEBOOK,
    VB_BOM_BOWL_MAN_GIVE_ITEM,
    VB_DRAW_ITEM_FROM_SOB1,
    VB_MULTIPLY_INFLICTED_DMG,
    VB_ELEGY_STATUE_FADE_IN_OUT,
    VB_GORON_ROLL_CONSUME_MAGIC,
    VB_GORON_ROLL_INCREASE_SPIKE_LEVEL,
    VB_GORON_ROLL_DISABLE_SPIKE_MODE,
    VB_DEKU_LINK_SPIN_ON_LAST_HOP,
    VB_BALLAD_PLAYED_FORM,
    VB_CONTINUE_BANKER_DIALOGUE,
    VB_FISH2_SPAWN_HEART_PIECE,
    VB_PLAY_DEFEAT_CAPTAIN_SEQUENCE,
    VB_CLAMP_ANIMATION_SPEED,
    VB_LINK_DIVE_OVER_WATER,
    VB_GIBDO_TRADE_SEQUENCE_SUFFICIENT_QUANTITY_PRESENTED,
    VB_GIBDO_TRADE_SEQUENCE_ACCEPT_RED_POTION,
    VB_GIBDO_TRADE_SEQUENCE_TAKE_MORE_THAN_ONE_ITEM,
    VB_GIBDO_TRADE_SEQUENCE_DO_TRADE,
    VB_GET_ITEM_ACTION_FROM_MASK,
    VB_GRANT_MAGIC_UPON_REQUEST,
    VB_SCOPENUTS_CONSIDER_FIRST_CYCLE,
    VB_JS_OVERRIDE_MASK_CHECK,
    VB_JS_CONSIDER_ELIGIBLE_FOR_DEITY,
    VB_MEET_MOON_REQUIREMENTS,
    VB_OPEN_WOODFALL_FROM_SONG,
    VB_OPEN_GREAT_BAY_FROM_SONG,
    VB_OPEN_SNOWHEAD_FROM_SONG,
    VB_GOHT_UNFREEZE,
    VB_PERFORM_AC_COLLISION,
    VB_GIVE_ITEM_FROM_GK_LULLABY,
    VB_BUY_GORMAN_MILK,
    VB_PLAY_LOW_HP_ALARM,
    VB_PLAY_GORON_CHILD_CRY,
} GIVanillaBehavior;

typedef enum {
    GI_INVERT_CAMERA_RIGHT_STICK_X,
    GI_INVERT_CAMERA_RIGHT_STICK_Y,
    GI_INVERT_MOVEMENT_X,
    GI_INVERT_SHIELD_X,
    GI_INVERT_SHOP_X,
    GI_INVERT_HORSE_X,
    GI_INVERT_ZORA_SWIM_X,
    GI_INVERT_DEBUG_DPAD_X,
    GI_INVERT_TELESCOPE_X,
    GI_INVERT_FIRST_PERSON_AIM_X,
    GI_INVERT_FIRST_PERSON_AIM_Y,
    GI_INVERT_FIRST_PERSON_GYRO_X,
    GI_INVERT_FIRST_PERSON_GYRO_Y,
    GI_INVERT_FIRST_PERSON_RIGHT_STICK_X,
    GI_INVERT_FIRST_PERSON_RIGHT_STICK_Y,
    GI_INVERT_FIRST_PERSON_MOVING_X,
} GIInvertType;

typedef enum {
    GI_DPAD_OCARINA,
    GI_DPAD_EQUIP,
} GIDpadType;

typedef enum {
    GI_EVENT_NONE,
    GI_EVENT_GIVE_ITEM,
    GI_EVENT_SPAWN_ACTOR,
    GI_EVENT_TRANSITION,
} GIEventType;

#ifdef __cplusplus

#include "2s2h/CustomMessage/CustomMessage.h"
#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <variant>

#include <version>
#ifdef __cpp_lib_source_location
#include <source_location>
#else
#pragma message("Compiling without <source_location> support, the Hook Debugger will not be available")
#endif

typedef uint32_t HOOK_ID;

enum HookType {
    HOOK_TYPE_NORMAL,
    HOOK_TYPE_ID,
    HOOK_TYPE_PTR,
    HOOK_TYPE_FILTER,
};

struct HookRegisteringInfo {
    bool valid;
    const char* file;
    std::uint_least32_t line;
    std::uint_least32_t column;
    const char* function;
    HookType type;

    HookRegisteringInfo()
        : valid(false), file("unknown file"), line(0), column(0), function("unknown function"), type(HOOK_TYPE_NORMAL) {
    }

    HookRegisteringInfo(const char* _file, std::uint_least32_t _line, std::uint_least32_t _column,
                        const char* _function, HookType _type)
        : valid(true), file(_file), line(_line), column(_column), function(_function), type(_type) {
        // Trim off user parent directories
        const char* trimmed = strstr(_file, "mm/2s2h/");
        if (trimmed != nullptr) {
            file = trimmed;
        }
    }
};

struct HookInfo {
    uint32_t calls;
    HookRegisteringInfo registering;
};

#ifdef __cpp_lib_source_location
#define GET_CURRENT_REGISTERING_INFO(type) \
    (HookRegisteringInfo{ location.file_name(), location.line(), location.column(), location.function_name(), type })
#else
#define GET_CURRENT_REGISTERING_INFO(type) (HookRegisteringInfo{})
#endif

struct GIEventNone {};

struct GIEventGiveItem {
    // Whether or not to show the get item cutscene. If true and the player is in the air, the
    // player will instead be frozen for a few seconds. If this is true you _must_ call
    // CustomMessage::SetActiveCustomMessage in the giveItem function otherwise you'll just see a blank message.
    bool showGetItemCutscene;
    // Arbitrary s16 that can be accessed from within the give/draw functions with CUSTOM_ITEM_PARAM
    s16 param;
    // These are run in the context of an item00 actor. This isn't super important but can be useful in some cases
    ActorFunc giveItem;
    ActorFunc drawItem;
};

struct GIEventSpawnActor {
    s16 actorId;
    f32 posX;
    f32 posY;
    f32 posZ;
    s16 rot;
    s32 params;
    // if true, the coordinates are made relative to the player's position and rotation, 0 rotation is facing the same
    // direction as the player, x+ is to the players right, y+ is up, z+ is in front of the player
    bool relativeCoords;
};

struct GIEventTransition {
    u16 entrance;
    u16 cutsceneIndex;
    s8 transitionTrigger;
    u8 transitionType;
};

typedef std::variant<GIEventNone, GIEventGiveItem, GIEventSpawnActor, GIEventTransition> GIEvent;

class GameInteractor {
  public:
    static GameInteractor* Instance;

    void RegisterOwnHooks();

    // Game State
    std::vector<GIEvent> events = {};
    GIEvent currentEvent = GIEventNone();

    // Game Hooks
    HOOK_ID nextHookId = 1;

    template <typename H> struct RegisteredGameHooks {
        inline static std::unordered_map<HOOK_ID, typename H::fn> functions;
        inline static std::unordered_map<int32_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForID;
        inline static std::unordered_map<uintptr_t, std::unordered_map<HOOK_ID, typename H::fn>> functionsForPtr;
        inline static std::unordered_map<HOOK_ID, std::pair<typename H::filter, typename H::fn>> functionsForFilter;

        // Used for the hook debugger
        inline static std::map<HOOK_ID, HookInfo> hookData;
    };
    template <typename H> struct HooksToUnregister {
        inline static std::vector<HOOK_ID> hooks;
        inline static std::vector<HOOK_ID> hooksForID;
        inline static std::vector<HOOK_ID> hooksForPtr;
        inline static std::vector<HOOK_ID> hooksForFilter;
    };

    template <typename H> std::map<uint32_t, HookInfo>* GetHookData() {
        return &RegisteredGameHooks<H>::hookData;
    }

    // General Hooks
    template <typename H>
#ifdef __cpp_lib_source_location
    HOOK_ID RegisterGameHook(typename H::fn h, const std::source_location location = std::source_location::current()) {
#else
    HOOK_ID RegisterGameHook(typename H::fn h) {
#endif
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functions.find(this->nextHookId) != RegisteredGameHooks<H>::functions.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functions[this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] =
            HookInfo{ 0, GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_NORMAL) };
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHook(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooks.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooks(Args&&... args) {
        // Remove pending hooks for this type
        for (auto& hookId : HooksToUnregister<H>::hooks) {
            RegisteredGameHooks<H>::functions.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooks.clear();
        // Execute hooks
        for (auto& hook : RegisteredGameHooks<H>::functions) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // ID based Hooks
    template <typename H>
#ifdef __cpp_lib_source_location
    HOOK_ID RegisterGameHookForID(int32_t id, typename H::fn h,
                                  std::source_location location = std::source_location::current()) {
#else
    HOOK_ID RegisterGameHookForID(int32_t id, typename H::fn h) {
#endif
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForID[id].find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForID[id].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForID[id][this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{ 0, GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_ID) };
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForID(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForID.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForID(int32_t id, Args&&... args) {
        // Remove pending hooks for this type
        for (auto hookIdIt = HooksToUnregister<H>::hooksForID.begin();
             hookIdIt != HooksToUnregister<H>::hooksForID.end();) {
            bool remove = false;

            if (RegisteredGameHooks<H>::functionsForID[id].size() == 0) {
                break;
            }

            for (auto it = RegisteredGameHooks<H>::functionsForID[id].begin();
                 it != RegisteredGameHooks<H>::functionsForID[id].end();) {
                if (it->first == *hookIdIt) {
                    it = RegisteredGameHooks<H>::functionsForID[id].erase(it);
                    RegisteredGameHooks<H>::hookData.erase(*hookIdIt);
                    remove = true;
                    break;
                } else {
                    ++it;
                }
            }

            if (remove) {
                hookIdIt = HooksToUnregister<H>::hooksForID.erase(hookIdIt);
            } else {
                ++hookIdIt;
            }
        }
        // Execute hooks
        for (auto& hook : RegisteredGameHooks<H>::functionsForID[id]) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // PTR based Hooks
    template <typename H>
#ifdef __cpp_lib_source_location
    HOOK_ID RegisterGameHookForPtr(uintptr_t ptr, typename H::fn h,
                                   const std::source_location location = std::source_location::current()) {
#else
    HOOK_ID RegisterGameHookForPtr(uintptr_t ptr, typename H::fn h) {
#endif
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForPtr[ptr].find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForPtr[ptr].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForPtr[ptr][this->nextHookId] = h;
        RegisteredGameHooks<H>::hookData[this->nextHookId] = HookInfo{ 0, GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_PTR) };
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForPtr(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForPtr.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForPtr(uintptr_t ptr, Args&&... args) {
        // Remove pending hooks for this type
        for (auto hookIdIt = HooksToUnregister<H>::hooksForPtr.begin();
             hookIdIt != HooksToUnregister<H>::hooksForPtr.end();) {
            bool remove = false;

            if (RegisteredGameHooks<H>::functionsForPtr[ptr].size() == 0) {
                break;
            }

            for (auto it = RegisteredGameHooks<H>::functionsForPtr[ptr].begin();
                 it != RegisteredGameHooks<H>::functionsForPtr[ptr].end();) {
                if (it->first == *hookIdIt) {
                    it = RegisteredGameHooks<H>::functionsForPtr[ptr].erase(it);
                    RegisteredGameHooks<H>::hookData.erase(*hookIdIt);
                    remove = true;
                    break;
                } else {
                    ++it;
                }
            }

            if (remove) {
                hookIdIt = HooksToUnregister<H>::hooksForPtr.erase(hookIdIt);
            } else {
                ++hookIdIt;
            }
        }
        // Execute hooks
        for (auto& hook : RegisteredGameHooks<H>::functionsForPtr[ptr]) {
            hook.second(std::forward<Args>(args)...);
            RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
        }
    }

    // Filter based Hooks
    template <typename H>
#ifdef __cpp_lib_source_location
    HOOK_ID RegisterGameHookForFilter(typename H::filter f, typename H::fn h,
                                      const std::source_location location = std::source_location::current()) {
#else
    HOOK_ID RegisterGameHookForFilter(typename H::filter f, typename H::fn h) {
#endif
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForFilter.find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForFilter.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForFilter[this->nextHookId] = std::make_pair(f, h);
        RegisteredGameHooks<H>::hookData[this->nextHookId] =
            HookInfo{ 0, GET_CURRENT_REGISTERING_INFO(HOOK_TYPE_FILTER) };
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForFilter(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForFilter.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForFilter(Args&&... args) {
        // Remove pending hooks for this type
        for (auto& hookId : HooksToUnregister<H>::hooksForFilter) {
            RegisteredGameHooks<H>::functionsForFilter.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooksForFilter.clear();
        // Execute hooks
        for (auto& hook : RegisteredGameHooks<H>::functionsForFilter) {
            if (hook.second.first(std::forward<Args>(args)...)) {
                hook.second.second(std::forward<Args>(args)...);
                RegisteredGameHooks<H>::hookData[hook.first].calls += 1;
            }
        }
    }

    template <typename H> void ProcessUnregisteredHooks() {
        // Normal
        for (auto& hookId : HooksToUnregister<H>::hooks) {
            RegisteredGameHooks<H>::functions.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooks.clear();

        // ID
        for (auto& hookId : HooksToUnregister<H>::hooksForID) {
            for (auto& idGroup : RegisteredGameHooks<H>::functionsForID) {
                for (auto it = idGroup.second.begin(); it != idGroup.second.end();) {
                    if (it->first == hookId) {
                        it = idGroup.second.erase(it);
                        RegisteredGameHooks<H>::hookData.erase(hookId);
                    } else {
                        ++it;
                    }
                }
            }
        }
        HooksToUnregister<H>::hooksForID.clear();

        // Ptr
        for (auto& hookId : HooksToUnregister<H>::hooksForPtr) {
            for (auto& ptrGroup : RegisteredGameHooks<H>::functionsForPtr) {
                for (auto it = ptrGroup.second.begin(); it != ptrGroup.second.end();) {
                    if (it->first == hookId) {
                        it = ptrGroup.second.erase(it);
                        RegisteredGameHooks<H>::hookData.erase(hookId);
                    } else {
                        ++it;
                    }
                }
            }
        }
        HooksToUnregister<H>::hooksForPtr.clear();

        // Filter
        for (auto& hookId : HooksToUnregister<H>::hooksForFilter) {
            RegisteredGameHooks<H>::functionsForFilter.erase(hookId);
            RegisteredGameHooks<H>::hookData.erase(hookId);
        }
        HooksToUnregister<H>::hooksForFilter.clear();
    }

    void RemoveAllQueuedHooks() {
#define DEFINE_HOOK(name, _) ProcessUnregisteredHooks<name>();

#include "GameInteractor_HookTable.h"

#undef DEFINE_HOOK
    }

    class HookFilter {
      public:
        static auto ActorNotPlayer(Actor* actor) {
            return actor->id != ACTOR_PLAYER;
        }
        // For use with Should hooks
        static auto SActorNotPlayer(Actor* actor, bool* result) {
            return actor->id != ACTOR_PLAYER;
        }
        static auto ActorMatchIdAndParams(int16_t id, int16_t params) {
            return [id, params](Actor* actor) { return actor->id == id && actor->params == params; };
        }
        // For use with Should hooks
        static auto SActorMatchIdAndParams(int16_t id, int16_t params) {
            return [id, params](Actor* actor, bool* result) { return actor->id == id && actor->params == params; };
        }
    };

#define DEFINE_HOOK(name, args)                  \
    struct name {                                \
        typedef std::function<void args> fn;     \
        typedef std::function<bool args> filter; \
    };

#include "GameInteractor_HookTable.h"

#undef DEFINE_HOOK
};

extern "C" {
#endif // __cplusplus

void GameInteractor_ExecuteOnGameStateMainStart();
void GameInteractor_ExecuteOnGameStateMainFinish();
void GameInteractor_ExecuteOnGameStateDrawFinish();
void GameInteractor_ExecuteOnGameStateUpdate();
void GameInteractor_ExecuteOnConsoleLogoUpdate();
void GameInteractor_ExecuteOnKaleidoUpdate(PauseContext* pauseCtx);
void GameInteractor_ExecuteBeforeKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex);
void GameInteractor_ExecuteAfterKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex);
void GameInteractor_ExecuteOnSaveInit(s16 fileNum);
void GameInteractor_ExecuteOnSaveLoad(s16 fileNum);
void GameInteractor_ExecuteOnFileSelectSaveLoad(s16 fileNum, bool isOwlSave, SaveContext* saveContext);
void GameInteractor_ExecuteBeforeEndOfCycleSave();
void GameInteractor_ExecuteAfterEndOfCycleSave();
void GameInteractor_ExecuteBeforeMoonCrashSaveReset();
void GameInteractor_ExecuteOnInterfaceDrawStart();
void GameInteractor_ExecuteAfterInterfaceClockDraw();
void GameInteractor_ExecuteBeforeInterfaceClockDraw();

void GameInteractor_ExecuteOnSceneInit(s16 sceneId, s8 spawnNum);
void GameInteractor_ExecuteOnRoomInit(s16 sceneId, s8 roomNum);
void GameInteractor_ExecuteAfterRoomSceneCommands(s16 sceneId, s8 roomNum);
void GameInteractor_ExecuteOnPlayDrawWorldEnd();
void GameInteractor_ExecuteOnPlayDestroy();

bool GameInteractor_ShouldActorInit(Actor* actor);
void GameInteractor_ExecuteOnActorInit(Actor* actor);
bool GameInteractor_ShouldActorUpdate(Actor* actor);
void GameInteractor_ExecuteOnActorUpdate(Actor* actor);
bool GameInteractor_ShouldActorDraw(Actor* actor);
void GameInteractor_ExecuteOnActorDraw(Actor* actor);
void GameInteractor_ExecuteOnActorKill(Actor* actor);
void GameInteractor_ExecuteOnActorDestroy(Actor* actor);
void GameInteractor_ExecuteOnPlayerPostLimbDraw(Player* player, s32 limbIndex);

void GameInteractor_ExecuteOnSceneFlagSet(s16 sceneId, FlagType flagType, u32 flag);
void GameInteractor_ExecuteOnSceneFlagUnset(s16 sceneId, FlagType flagType, u32 flag);
void GameInteractor_ExecuteOnFlagSet(FlagType flagType, u32 flag);
void GameInteractor_ExecuteOnFlagUnset(FlagType flagType, u32 flag);

void GameInteractor_ExecuteAfterCameraUpdate(Camera* camera);
void GameInteractor_ExecuteOnCameraChangeModeFlags(Camera* camera);
void GameInteractor_ExecuteOnCameraChangeSettingsFlags(Camera* camera);

void GameInteractor_ExecuteOnPassPlayerInputs(Input* input);

void GameInteractor_ExecuteOnOpenText(u16* textId, bool* loadFromMessageTable);

bool GameInteractor_ShouldItemGive(u8 item);
void GameInteractor_ExecuteOnItemGive(u8 item);

bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...);
#define REGISTER_VB_SHOULD(flag, body)                                                      \
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldVanillaBehavior>( \
        flag, [](GIVanillaBehavior _, bool* should, va_list originalArgs) {                 \
            va_list args;                                                                   \
            va_copy(args, originalArgs);                                                    \
            body;                                                                           \
            va_end(args);                                                                   \
        })
#define COND_HOOK(hookType, condition, body)                                                     \
    {                                                                                            \
        static HOOK_ID hookId = 0;                                                               \
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::hookType>(hookId);          \
        hookId = 0;                                                                              \
        if (condition) {                                                                         \
            hookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::hookType>(body); \
        }                                                                                        \
    }
#define COND_ID_HOOK(hookType, id, condition, body)                                                       \
    {                                                                                                     \
        static HOOK_ID hookId = 0;                                                                        \
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::hookType>(hookId);              \
        hookId = 0;                                                                                       \
        if (condition) {                                                                                  \
            hookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::hookType>(id, body); \
        }                                                                                                 \
    }
#define COND_VB_SHOULD(id, condition, body)                                                               \
    {                                                                                                     \
        static HOOK_ID hookId = 0;                                                                        \
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(hookId); \
        hookId = 0;                                                                                       \
        if (condition) {                                                                                  \
            hookId = REGISTER_VB_SHOULD(id, body);                                                        \
        }                                                                                                 \
    }

int GameInteractor_InvertControl(GIInvertType type);
uint32_t GameInteractor_Dpad(GIDpadType type, uint32_t buttonCombo);

#ifdef __cplusplus
}

#endif

#endif // GAME_INTERACTOR_H
