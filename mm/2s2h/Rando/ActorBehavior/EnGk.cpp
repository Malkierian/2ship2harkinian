#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Gk/z_en_gk.h"

void Player_TalkWithPlayer(PlayState* play, Actor* actor);
}

void Rando::ActorBehavior::InitEnGKBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        switch (actor->id) {
            case ACTOR_EN_GK:
                if (RANDO_SAVE_CHECKS[RC_GORON_RACETRACK_GOLD_DUST].obtained) {
                    return;
                }

                *should = false;
                actor->parent = &player->actor;
                player->talkActor = actor;
                player->talkActorDistance = actor->xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;
                Player_TalkWithPlayer(gPlayState, actor);
                break;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_GK_LULLABY, IS_RANDO, {
        // Override vanilla cutscene skip item grant behavior to use rando queue instead
        *should = false;
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId != 9 || actor == NULL || actor->id != ACTOR_EN_GK) {
            return;
        }

        *should = false;

        SET_WEEKEVENTREG(WEEKEVENTREG_24_80); // Ensure Goron Elder check is available
        if (!RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].obtained) {
            RANDO_SAVE_CHECKS[RC_GORON_SHRINE_FULL_LULLABY].eligible = true;
        }
    });

    // Played Full Lullaby for Baby Goron
    COND_HOOK(OnSceneFlagSet, IS_RANDO, [](s16 sceneId, FlagType flagType, u32 flag) {
        if (sceneId == SCENE_16GORON_HOUSE && flagType == FLAG_CYCL_SCENE_SWITCH && flag == 20) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_80); // Ensure Goron Elder check is available
        }
    });
}
