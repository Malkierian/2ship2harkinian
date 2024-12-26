#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/Rando/StaticData/StaticData.h"
#include "2s2h/ShipInit.hpp"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"

s32 func_80832558(PlayState* play, Player* player, PlayerFuncD58 arg2);
void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
void Player_StopCutscene(Player* player);
void func_80848294(PlayState* play, Player* player);
Gfx* EnBox_SetRenderMode1(GraphicsContext* gfxCtx);
Gfx* EnBox_SetRenderMode2(GraphicsContext* gfxCtx);
Gfx* EnBox_SetRenderMode3(GraphicsContext* gfxCtx);
void EnBox_Draw(Actor* actor, PlayState* play);
void Player_DrawStrayFairyParticles(PlayState* play, Vec3f* arg1);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
}

static Gfx gBoxChestBaseCopyDL[42];
static Gfx gBoxChestLidCopyDL[48];

#define ENBOX_RC (actor->home.rot.x)
#define ENBOX_SET_ITEM(thisx, newItem) ((thisx)->params = (((thisx)->params & ~(0x7F << 5)) | ((newItem & 0x7F) << 5)))

void Player_Action_65_override(Player* player, PlayState* play) {
    if (PlayerAnimation_Update(play, &player->skelAnime)) {
        Player_StopCutscene(player);
        func_80848294(play, player);
    }
}

void func_80837C78_override(PlayState* play, Player* player) {
    Player_SetAction_PreserveMoveFlags(play, player, Player_Action_65_override, 0);
    player->stateFlags1 |= (PLAYER_STATE1_400 | PLAYER_STATE1_20000000);
}

void EnBox_RandoPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* actor, Gfx** gfx) {
    s32 pad;
    EnBox* enBox = (EnBox*)actor;
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[ENBOX_RC].randoItemId, (RandoCheckId)ENBOX_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerOrnateTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockOrnateTex);
            break;
        case RITYPE_MAJOR:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerGildedTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockGildedTex);
            break;
        case RITYPE_MASK:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerMaskTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockMaskTex);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerSkullTokenTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockSkullTokenTex);
            break;
        case RITYPE_SMALL_KEY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerSmallKeyTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockSmallKeyTex);
            break;
        case RITYPE_STRAY_FAIRY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerStrayFairyTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockStrayFairyTex);
            break;
        default:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockTex);
            break;
    }

    gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (limbIndex == OBJECT_BOX_CHEST_LIMB_01) {
        gSPDisplayList((*gfx)++, (Gfx*)gBoxChestBaseCopyDL);
    } else if (limbIndex == OBJECT_BOX_CHEST_LIMB_03) {
        gSPDisplayList((*gfx)++, (Gfx*)gBoxChestLidCopyDL);
    }
}

void EnBox_RandoDraw(Actor* actor, PlayState* play) {
    s32 pad;
    EnBox* enBox = (EnBox*)actor;

    OPEN_DISPS(play->state.gfxCtx);

    if (enBox->unk_1F4.unk_10 != NULL) {
        enBox->unk_1F4.unk_10(&enBox->unk_1F4, play);
    }
    if (((enBox->alpha == 255) && (enBox->type != ENBOX_TYPE_BIG_INVISIBLE) &&
         (enBox->type != ENBOX_TYPE_SMALL_INVISIBLE)) ||
        (!CHECK_FLAG_ALL(enBox->dyna.actor.flags, ACTOR_FLAG_REACT_TO_LENS) &&
         ((enBox->type == ENBOX_TYPE_BIG_INVISIBLE) || (enBox->type == ENBOX_TYPE_SMALL_INVISIBLE)))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode1(play->state.gfxCtx));
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        POLY_OPA_DISP = SkelAnime_Draw(play, enBox->skelAnime.skeleton, enBox->skelAnime.jointTable, NULL,
                                       EnBox_RandoPostLimbDraw, &enBox->dyna.actor, POLY_OPA_DISP);
    } else if (enBox->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, enBox->alpha);
        if ((enBox->type == ENBOX_TYPE_BIG_INVISIBLE) || (enBox->type == ENBOX_TYPE_SMALL_INVISIBLE)) {
            gSPSegment(POLY_XLU_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode3(play->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode2(play->state.gfxCtx));
        }
        POLY_XLU_DISP = SkelAnime_Draw(play, enBox->skelAnime.skeleton, enBox->skelAnime.jointTable, NULL,
                                       EnBox_RandoPostLimbDraw, &enBox->dyna.actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// This simply prevents the player from getting an item from the chest, but still
// plays the chest opening animation and ensure the treasure chest flag is set
void Rando::ActorBehavior::InitEnBoxBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_CHEST, IS_RANDO, {
        EnBox* enBox = va_arg(args, EnBox*);
        Actor* actor = (Actor*)enBox;
        Player* player = GET_PLAYER(gPlayState);
        if (ENBOX_RC != RC_UNKNOWN) {
            func_80832558(gPlayState, player, func_80837C78_override);
            *should = false;
        }
    });

    // Replace the item in the chest with a recovery heart, to prevent any other item side effects
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_BOX, IS_RANDO, [](Actor* actor, bool* should) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_CHEST, ENBOX_GET_CHEST_FLAG(actor),
                                                                    gPlayState->sceneId);
        RandoCheckId randoCheckId = randoStaticCheck.randoCheckId;

        if (randoCheckId == RC_UNKNOWN || !RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            return;
        }

        ENBOX_RC = randoStaticCheck.randoCheckId;
        actor->params = ((actor->params & ~(0x7F << 5)) | ((GI_RECOVERY_HEART & 0x7F) << 5));

        if (CVarGetInteger("gRando.CSMC", 0)) {
            actor->draw = EnBox_RandoDraw;
        }
    });
}

static RegisterShipInitFunc initFunc(
    []() {
        if (gPlayState == NULL) {
            return;
        }

        Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_CHEST].first;

        while (actor != NULL) {
            if (actor->id == ACTOR_EN_BOX) {
                if (CVarGetInteger("gRando.CSMC", 0) && IS_RANDO) {
                    actor->draw = EnBox_RandoDraw;
                } else if (actor->draw == EnBox_RandoDraw) {
                    actor->draw = EnBox_Draw;
                }
            }

            actor = actor->next;
        }
    },
    { "gRando.CSMC", "IS_RANDO" });

static RegisterShipInitFunc initializeChestCopyDLs([]() {
    Gfx* baseDL = ResourceMgr_LoadGfxByName(gBoxChestBaseOrnateDL);
    memcpy(gBoxChestBaseCopyDL, baseDL, sizeof(gBoxChestBaseCopyDL));
    gBoxChestBaseCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
    gBoxChestBaseCopyDL[8] = gsDPNoOp();
    gBoxChestBaseCopyDL[25] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x0A000000 | 1);
    gBoxChestBaseCopyDL[26] = gsDPNoOp();

    Gfx* lidDL = ResourceMgr_LoadGfxByName(gBoxChestLidOrnateDL);
    memcpy(gBoxChestLidCopyDL, lidDL, sizeof(gBoxChestLidCopyDL));
    gBoxChestLidCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
    gBoxChestLidCopyDL[8] = gsDPNoOp();
}, {});
