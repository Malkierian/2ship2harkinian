#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gFixes.FixEponaStealingSword"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFixEponaStealingSword() {
    COND_VB_SHOULD(VB_CLEAR_B_BUTTON_FOR_HORSEBACK, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterFixEponaStealingSword, { CVAR_NAME });
