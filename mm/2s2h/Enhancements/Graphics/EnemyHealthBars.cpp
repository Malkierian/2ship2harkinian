
#include "2s2h/ActorExtension/ActorExtension.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "assets/interface/parameter_static/parameter_static.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "overlays/actors/ovl_Boss_02/z_boss_02.h"
#include "overlays/actors/ovl_Boss_07/z_boss_07.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Graphics.EnemyHealthBars"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

typedef enum {
    ENEMYHEALTH_ANCHOR_ACTOR,
    ENEMYHEALTH_ANCHOR_TOP,
    ENEMYHEALTH_ANCHOR_BOTTOM,
} EnemyHealthBarAnchorType;

ActorExtensionId actorEnemyHealthExtId = 0;

static Vtx sEnemyHealthVtx[16];
static Mtx sEnemyHealthMtx[2];

u8 GetActorMaxHealth(Actor* actor) {
    u8* maxHealth = (u8*)ActorExtension_Get(actor, actorEnemyHealthExtId);
    if (maxHealth == NULL) {
        return 0;
    }

    return *maxHealth;
}

// Draws an enemy health bar using the magic bar textures and positions it in a similar way to Z-Targeting
void Interface_DrawEnemyHealthBar(TargetContext* targetCtx, PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    Player* player = GET_PLAYER(play);
    Actor* actor = targetCtx->lockOnActor;
    Actor* healthActor = actor;

    if (actor == NULL || (actor->category != ACTORCAT_ENEMY && actor->category != ACTORCAT_BOSS) ||
        targetCtx->lockOnAlpha == 0 || ((s32)pauseCtx->state > PAUSE_STATE_OPENING_2)) {
        return;
    }

    Vec3f projTargetCenter;
    f32 projTargetCappedInvW;

    Color_RGBA8 healthBar_red = { 255, 0, 0, 255 };
    Color_RGBA8 healthBar_border = { 255, 255, 255, 255 };
    s16 healthBar_fillWidth = 64;
    s16 healthBar_actorOffset = 40;
    s32 healthBar_offsetX = 0;
    s32 healthBar_offsetY = 0;
    EnemyHealthBarAnchorType anchorType = ENEMYHEALTH_ANCHOR_ACTOR;

    // Twinmold's tail actor is what is targettable, but the health values are on the parent actor (the body)
    if (actor->id == ACTOR_BOSS_02 && actor->params == TWINMOLD_TYPE_TAIL && actor->parent != NULL &&
        (actor->parent->params == TWINMOLD_TYPE_RED || actor->parent->params == TWINMOLD_TYPE_BLUE)) {
        healthActor = actor->parent;
    }

    OPEN_DISPS(play->state.gfxCtx);

    s16 texHeight = 16;
    s16 endTexWidth = 8;
    f32 halfBarWidth = endTexWidth + ((f32)healthBar_fillWidth / 2);
    // Convert health to signed value and clamp to 0 (some actors overflow the health when subtracting damage)
    s8 curHealth = CLAMP_MIN((s8)healthActor->colChkInfo.health, 0);
    u8 maxHealth = GetActorMaxHealth(healthActor);
    s16 healthBarFill = ((f32)curHealth / maxHealth) * healthBar_fillWidth;

    if (anchorType == ENEMYHEALTH_ANCHOR_ACTOR) {
        // Get actor projected position
        Actor_GetProjectedPos(play, &actor->focus.pos, &projTargetCenter, &projTargetCappedInvW);

        projTargetCenter.x = (SCREEN_WIDTH / 2) * (projTargetCenter.x * projTargetCappedInvW);
        projTargetCenter.x = projTargetCenter.x * (CVarGetInteger("gModes.MirroredWorld.State", 0) ? -1 : 1);
        projTargetCenter.x =
            CLAMP(projTargetCenter.x, (-SCREEN_WIDTH / 2) + halfBarWidth, (SCREEN_WIDTH / 2) - halfBarWidth);

        projTargetCenter.y = (SCREEN_HEIGHT / 2) * (projTargetCenter.y * projTargetCappedInvW);
        projTargetCenter.y = projTargetCenter.y - healthBar_offsetY + healthBar_actorOffset;
        projTargetCenter.y =
            CLAMP(projTargetCenter.y, (-SCREEN_HEIGHT / 2) + (texHeight / 2), (SCREEN_HEIGHT / 2) - (texHeight / 2));
    } else if (anchorType == ENEMYHEALTH_ANCHOR_TOP) {
        projTargetCenter.x = healthBar_offsetX;
        projTargetCenter.y = (SCREEN_HEIGHT / 2) - (texHeight / 2) - healthBar_offsetY;
    } else if (anchorType == ENEMYHEALTH_ANCHOR_BOTTOM) {
        projTargetCenter.x = healthBar_offsetX;
        projTargetCenter.y = (-SCREEN_HEIGHT / 2) + (texHeight / 2) - healthBar_offsetY;
    }

    // Health bar border end left
    Ship_CreateQuadVertexGroup(&sEnemyHealthVtx[0], -floorf(halfBarWidth), -texHeight / 2, endTexWidth, texHeight,
                               false);
    // Health bar border middle
    Ship_CreateQuadVertexGroup(&sEnemyHealthVtx[4], -floorf(halfBarWidth) + endTexWidth, -texHeight / 2,
                               healthBar_fillWidth, texHeight, false);
    // Health bar border end right
    Ship_CreateQuadVertexGroup(&sEnemyHealthVtx[8], ceilf(halfBarWidth) - endTexWidth, -texHeight / 2, endTexWidth,
                               texHeight, true);
    // Health bar fill
    Ship_CreateQuadVertexGroup(&sEnemyHealthVtx[12], -floorf(halfBarWidth) + endTexWidth, (-texHeight / 2) + 3,
                               healthBarFill, 7, false);

    if (((!(player->stateFlags1 & PLAYER_STATE1_40)) || (actor != player->lockOnActor)) &&
        targetCtx->lockOnRadius < 500.0f) {
        f32 slideInOffsetY = 0;

        // Slide in the health bar from edge of the screen (mimic the Z-Target triangles fly in)
        if (anchorType == ENEMYHEALTH_ANCHOR_ACTOR && targetCtx->lockOnRadius > 120.0f) {
            slideInOffsetY = (targetCtx->lockOnRadius - 120.0f) / 2;
            // Slide in from the top if the bar is placed on the top half of the screen
            if (healthBar_offsetY - healthBar_actorOffset <= 0) {
                slideInOffsetY *= -1;
            }
        }

        // Setup DL for overlay disp
        Gfx_SetupDL39_Overlay(play->state.gfxCtx);

        Matrix_Translate(projTargetCenter.x, projTargetCenter.y - slideInOffsetY, 0, MTXMODE_NEW);
        Matrix_Scale(0.65f, 0.65f * -1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_ToMtx(&sEnemyHealthMtx[0]);
        gSPMatrix(OVERLAY_DISP++, &sEnemyHealthMtx[0], G_MTX_MODELVIEW | G_MTX_LOAD);

        // Health bar border
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, healthBar_border.r, healthBar_border.g, healthBar_border.b,
                        healthBar_border.a);
        gDPSetEnvColor(OVERLAY_DISP++, 100, 50, 50, 255);

        gSPVertex(OVERLAY_DISP++, (uintptr_t)sEnemyHealthVtx, 16, 0);

        gDPLoadTextureBlock(OVERLAY_DISP++, (uintptr_t)gMagicMeterEndTex, G_IM_FMT_IA, G_IM_SIZ_8b, endTexWidth,
                            texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

        gDPLoadTextureBlock(OVERLAY_DISP++, (uintptr_t)gMagicMeterMidTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, texHeight, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(OVERLAY_DISP++, 4, 6, 7, 5, 0);

        gDPLoadTextureBlock(OVERLAY_DISP++, (uintptr_t)gMagicMeterEndTex, G_IM_FMT_IA, G_IM_SIZ_8b, endTexWidth,
                            texHeight, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(OVERLAY_DISP++, 8, 10, 11, 9, 0);

        // Health bar fill
        Matrix_Translate(-0.375f, -0.5f, 0, MTXMODE_APPLY);
        Matrix_ToMtx(&sEnemyHealthMtx[1]);
        gSPMatrix(OVERLAY_DISP++, &sEnemyHealthMtx[1], G_MTX_MODELVIEW | G_MTX_LOAD);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, healthBar_red.r, healthBar_red.g, healthBar_red.b, healthBar_red.a);

        gDPLoadMultiBlock_4b(OVERLAY_DISP++, (uintptr_t)gMagicMeterFillTex, 0, G_TX_RENDERTILE, G_IM_FMT_I, 16,
                             texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                             G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(OVERLAY_DISP++, 12, 14, 15, 13, 0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static RegisterShipInitFunc initFunc(
    []() {
        // Register actor extension health data and actor init hook once
        if (actorEnemyHealthExtId == 0) {
            actorEnemyHealthExtId = ActorExtension_CreateForAll(sizeof(u8));

            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](Actor* actor) {
                u8* maxHealth = (u8*)ActorExtension_Get(actor, actorEnemyHealthExtId);
                if (maxHealth == NULL) {
                    assert(false && "Actor Extension memory not valid");
                }

                *maxHealth = actor->colChkInfo.health;

                // The remains in the Majora fight get their health set after init for the first time fighting,
                // so we set the expected value now
                if (actor->id == ACTOR_BOSS_07 && actor->params >= MAJORAS_REMAINS) {
                    *maxHealth = 5;
                }
            });
        }

        COND_HOOK(OnInterfaceDrawStart, CVAR, []() {
            if (gPlayState != NULL) {
                Interface_DrawEnemyHealthBar(&gPlayState->actorCtx.targetCtx, gPlayState);
            }
        });
    },
    { CVAR_NAME });
