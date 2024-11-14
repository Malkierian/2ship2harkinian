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
    VB_TATL_INTERUPT_MSG3,
    VB_TATL_INTERUPT_MSG6,
    VB_ITEM_BE_RESTRICTED,
    VB_FLIP_HOP_VARIABLE,
    VB_DISABLE_LETTERBOX,
    VB_CHEST_SPAWN_FAIRY,
    VB_START_GREAT_FAIRY_CUTSCENE,
    VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS,
    VB_KILL_CLOCK_TOWN_STRAY_FAIRY,
    VB_CLOCK_TOWER_OPENING_CONSIDER_THIS_FIRST_CYCLE,
    VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY,
    VB_DRAW_SLIME_BODY_ITEM,
    VB_DRAW_OCARINA_IN_STK_HAND,
    VB_DRAW_FILE_SELECT_SAVE_TYPE_BOX,
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
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG,
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL,
    VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED,
    // Vanilla Condition: INV_CONTENT(ITEM_MASK_KAFEIS_MASK) != ITEM_MASK_KAFEIS_MASK
    VB_MADAME_AROMA_ASK_FOR_HELP,
    VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI,
    VB_HAVE_ROMANI_MASK,
    VB_GIVE_ROMANI_MASK,
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
    VB_GIVE_ITEM_FROM_MNK,
    VB_TERMINA_FIELD_BE_EMPTY,
    VB_FASTER_FIRST_CYCLE,
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
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <variant>

typedef uint32_t HOOK_ID;

#define DEFINE_HOOK(name, args)                  \
    struct name {                                \
        typedef std::function<void args> fn;     \
        typedef std::function<bool args> filter; \
    }

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
    };
    template <typename H> struct HooksToUnregister {
        inline static std::vector<HOOK_ID> hooks;
        inline static std::vector<HOOK_ID> hooksForID;
        inline static std::vector<HOOK_ID> hooksForPtr;
        inline static std::vector<HOOK_ID> hooksForFilter;
    };

    // General Hooks
    template <typename H> HOOK_ID RegisterGameHook(typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functions.find(this->nextHookId) != RegisteredGameHooks<H>::functions.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functions[this->nextHookId] = h;
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHook(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooks.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooks(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooks) {
            RegisteredGameHooks<H>::functions.erase(hookId);
        }
        HooksToUnregister<H>::hooks.clear();
        for (auto& hook : RegisteredGameHooks<H>::functions) {
            hook.second(std::forward<Args>(args)...);
        }
    }

    // ID based Hooks
    template <typename H> HOOK_ID RegisterGameHookForID(int32_t id, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForID[id].find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForID[id].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForID[id][this->nextHookId] = h;
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForID(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForID.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForID(int32_t id, Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForID) {
            for (auto it = RegisteredGameHooks<H>::functionsForID[id].begin();
                 it != RegisteredGameHooks<H>::functionsForID[id].end();) {
                if (it->first == hookId) {
                    it = RegisteredGameHooks<H>::functionsForID[id].erase(it);
                    HooksToUnregister<H>::hooksForID.erase(std::remove(HooksToUnregister<H>::hooksForID.begin(),
                                                                       HooksToUnregister<H>::hooksForID.end(), hookId),
                                                           HooksToUnregister<H>::hooksForID.end());
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForID[id]) {
            hook.second(std::forward<Args>(args)...);
        }
    }

    // PTR based Hooks
    template <typename H> HOOK_ID RegisterGameHookForPtr(uintptr_t ptr, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForPtr[ptr].find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForPtr[ptr].end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForPtr[ptr][this->nextHookId] = h;
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForPtr(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForPtr.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForPtr(uintptr_t ptr, Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForPtr) {
            for (auto it = RegisteredGameHooks<H>::functionsForPtr[ptr].begin();
                 it != RegisteredGameHooks<H>::functionsForPtr[ptr].end();) {
                if (it->first == hookId) {
                    it = RegisteredGameHooks<H>::functionsForPtr[ptr].erase(it);
                    HooksToUnregister<H>::hooksForPtr.erase(std::remove(HooksToUnregister<H>::hooksForPtr.begin(),
                                                                        HooksToUnregister<H>::hooksForPtr.end(),
                                                                        hookId),
                                                            HooksToUnregister<H>::hooksForPtr.end());
                } else {
                    ++it;
                }
            }
        }
        for (auto& hook : RegisteredGameHooks<H>::functionsForPtr[ptr]) {
            hook.second(std::forward<Args>(args)...);
        }
    }

    // Filter based Hooks
    template <typename H> HOOK_ID RegisterGameHookForFilter(typename H::filter f, typename H::fn h) {
        if (this->nextHookId == 0 || this->nextHookId >= UINT32_MAX)
            this->nextHookId = 1;
        while (RegisteredGameHooks<H>::functionsForFilter.find(this->nextHookId) !=
               RegisteredGameHooks<H>::functionsForFilter.end()) {
            this->nextHookId++;
        }

        RegisteredGameHooks<H>::functionsForFilter[this->nextHookId] = std::make_pair(f, h);
        return this->nextHookId++;
    }
    template <typename H> void UnregisterGameHookForFilter(HOOK_ID hookId) {
        if (hookId == 0)
            return;
        HooksToUnregister<H>::hooksForFilter.push_back(hookId);
    }
    template <typename H, typename... Args> void ExecuteHooksForFilter(Args&&... args) {
        for (auto& hookId : HooksToUnregister<H>::hooksForFilter) {
            RegisteredGameHooks<H>::functionsForFilter.erase(hookId);
        }
        HooksToUnregister<H>::hooksForFilter.clear();
        for (auto& hook : RegisteredGameHooks<H>::functionsForFilter) {
            if (hook.second.first(std::forward<Args>(args)...)) {
                hook.second.second(std::forward<Args>(args)...);
            }
        }
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

    DEFINE_HOOK(OnFileDropped, (std::string path));

    DEFINE_HOOK(OnGameStateMainFinish, ());
    DEFINE_HOOK(OnGameStateDrawFinish, ());
    DEFINE_HOOK(OnGameStateUpdate, ());
    DEFINE_HOOK(OnConsoleLogoUpdate, ());
    DEFINE_HOOK(OnKaleidoUpdate, (PauseContext * pauseCtx));
    DEFINE_HOOK(BeforeKaleidoDrawPage, (PauseContext * pauseCtx, u16 pauseIndex));
    DEFINE_HOOK(AfterKaleidoDrawPage, (PauseContext * pauseCtx, u16 pauseIndex));
    DEFINE_HOOK(OnSaveInit, (s16 fileNum));
    DEFINE_HOOK(OnSaveLoad, (s16 fileNum));
    DEFINE_HOOK(OnFileSelectSaveLoad, (s16 fileNum, bool isOwlSave, SaveContext* saveContext));
    DEFINE_HOOK(BeforeEndOfCycleSave, ());
    DEFINE_HOOK(AfterEndOfCycleSave, ());
    DEFINE_HOOK(BeforeMoonCrashSaveReset, ());

    DEFINE_HOOK(OnSceneInit, (s8 sceneId, s8 spawnNum));
    DEFINE_HOOK(OnRoomInit, (s8 sceneId, s8 roomNum));
    DEFINE_HOOK(AfterRoomSceneCommands, (s8 sceneId, s8 roomNum));
    DEFINE_HOOK(OnPlayDrawWorldEnd, ());
    DEFINE_HOOK(OnPlayDestroy, ());

    DEFINE_HOOK(ShouldActorInit, (Actor * actor, bool* should));
    DEFINE_HOOK(OnActorInit, (Actor * actor));
    DEFINE_HOOK(ShouldActorUpdate, (Actor * actor, bool* should));
    DEFINE_HOOK(OnActorUpdate, (Actor * actor));
    DEFINE_HOOK(ShouldActorDraw, (Actor * actor, bool* should));
    DEFINE_HOOK(OnActorDraw, (Actor * actor));
    DEFINE_HOOK(OnActorKill, (Actor * actor));
    DEFINE_HOOK(OnActorDestroy, (Actor * actor));
    DEFINE_HOOK(OnPlayerPostLimbDraw, (Player * player, s32 limbIndex));

    DEFINE_HOOK(OnSceneFlagSet, (s16 sceneId, FlagType flagType, u32 flag));
    DEFINE_HOOK(OnSceneFlagUnset, (s16 sceneId, FlagType flagType, u32 flag));
    DEFINE_HOOK(OnFlagSet, (FlagType flagType, u32 flag));
    DEFINE_HOOK(OnFlagUnset, (FlagType flagType, u32 flag));

    DEFINE_HOOK(AfterCameraUpdate, (Camera * camera));
    DEFINE_HOOK(OnCameraChangeModeFlags, (Camera * camera));
    DEFINE_HOOK(OnCameraChangeSettingsFlags, (Camera * camera));

    DEFINE_HOOK(OnPassPlayerInputs, (Input * input));

    DEFINE_HOOK(OnOpenText, (u16 * textId, bool* loadFromMessageTable));

    DEFINE_HOOK(ShouldItemGive, (u8 item, bool* should));
    DEFINE_HOOK(OnItemGive, (u8 item));

    DEFINE_HOOK(ShouldVanillaBehavior, (GIVanillaBehavior flag, bool* should, va_list originalArgs));
};

extern "C" {
#endif // __cplusplus

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
