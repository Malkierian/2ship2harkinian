#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Dblue_Movebg/z_bg_dblue_movebg.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"
}

#define CVAR_NAME "gEnhancements.Player.FasterPushAndPull"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFasterPushAndPull() {
    COND_VB_SHOULD(VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED, CVAR, {
        BgDblueMovebg* bgDblueMovebg = va_arg(args, BgDblueMovebg*);
        *should = false;
        bgDblueMovebg->unk_188 = 20;
    });

    COND_VB_SHOULD(VB_PUSH_BLOCK_SET_SPEED, CVAR, {
        ObjOshihiki* objOshihiki = va_arg(args, ObjOshihiki*);
        objOshihiki->pushSpeed = 5.0f;
        *should = false;
    });

    COND_VB_SHOULD(VB_PUSH_BLOCK_SET_TIMER, CVAR, {
        ObjOshihiki* objOshihiki = va_arg(args, ObjOshihiki*);
        objOshihiki->timer = 2;
        *should = false;
    });

    COND_VB_SHOULD(VB_SKATE_BLOCK_BEGIN_MOVE, CVAR, { *should = true; });

    COND_VB_SHOULD(VB_BLOCK_BEGIN_MOVE, CVAR, { *should = true; });

    COND_VB_SHOULD(VB_BLOCK_BE_FINISHED_PULLING, CVAR, {
        f32* pValue = va_arg(args, f32*);
        f32 target = (f32)va_arg(args, f64);
        f32 step = (f32)va_arg(args, f64);
        f32 maxStep = (f32)va_arg(args, f64);
        step = CLAMP_MAX(step, maxStep);
        // This is actually the same exact condition, but because we're hooking and running it here it effectively
        // doubles the speed
        *should = Math_StepToF(pValue, target, step);
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterPushAndPull, { CVAR_NAME });
