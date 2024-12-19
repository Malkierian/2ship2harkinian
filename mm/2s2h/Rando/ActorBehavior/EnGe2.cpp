#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnGe2 is initialized this means the player should now be considered to have access to a Pirate. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnGe2Behavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GE2, IS_RANDO, [](Actor* actor) {
        if (!RANDO_ACCESS[RANDO_ACCESS_PIRATE_PICTURE]) {
            RANDO_ACCESS[RANDO_ACCESS_PIRATE_PICTURE]++;
        }
    });
}
