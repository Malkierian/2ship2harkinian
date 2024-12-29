#include <libultraship/libultraship.h>
#include "MiscBehavior.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

// Entry point for the module, run once on game boot
void Rando::MiscBehavior::Init() {
    Rando::MiscBehavior::InitFileSelect();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(Rando::MiscBehavior::OnFileCreate);
}

void Rando::MiscBehavior::OnFileLoad() {
    Rando::MiscBehavior::CheckQueueReset();
    Rando::MiscBehavior::InitKaleidoItemPage();
    Rando::MiscBehavior::InitOfferGetItemBehavior();

    COND_HOOK(OnFlagSet, IS_RANDO, Rando::MiscBehavior::OnFlagSet);
    COND_HOOK(OnSceneFlagSet, IS_RANDO, Rando::MiscBehavior::OnSceneFlagSet);
    COND_HOOK(BeforeEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::BeforeEndOfCycleSave);
    COND_HOOK(AfterEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::AfterEndOfCycleSave);
    COND_HOOK(OnSceneInit, IS_RANDO, Rando::MiscBehavior::OnSceneInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });

    // This overrides the ocarina condition for Termina Field
    COND_VB_SHOULD(VB_TERMINA_FIELD_BE_EMPTY, IS_RANDO, { *should = false; });

    // Override faster first-cycle time speed if you don't have the Ocarina
    COND_VB_SHOULD(VB_FASTER_FIRST_CYCLE, IS_RANDO, { *should = false; });

    // The game normally only checks the trade slot for the room key directly, which would mean the player could be
    // denied entry to the Stock Pot Inn if they have the room key but it isn't assigned as the active item for the
    // slot. In rando, use this flag instead.
    COND_VB_SHOULD(VB_CHECK_FOR_ROOM_KEY, IS_RANDO, { *should = Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY); });

    // In Sram_OpenSave (right before this code runs) for non-owl saves, it overwrites the entrance to
    // ENTRANCE(CUTSCENE, 0), we need to override that with our starting location (Harcoded to South Clock Town)
    if (!gSaveContext.save.isOwlSave && IS_RANDO) {
        gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
    }
}
