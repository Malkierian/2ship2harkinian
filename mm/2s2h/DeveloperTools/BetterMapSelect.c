
#include "BetterMapSelect.h"

#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_select/z_select.h"
#include <libultraship/bridge.h>

void BetterMapSelect_LoadGame(MapSelectState* mapSelectState, u32 entrance, s32 spawn);
void BetterMapSelect_LoadFileSelect(MapSelectState* mapSelectState);

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     entranceSceneId, _betterMapSelectIndex, humanName)                                 \
    { humanName, BetterMapSelect_LoadGame, ENTRANCE(entranceSceneId, 0) },
#define DEFINE_SCENE_UNSET(_enumValue)

static SceneSelectEntry sBetterScenes[104] = {
#include "tables/scene_table.h"
    { "File Select", BetterMapSelect_LoadFileSelect, 0 },
    { "Title Screen", MapSelect_LoadConsoleLogo, 0 },
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     entranceSceneId, betterMapSelectIndex, humanName)                                  \
    { humanName, ENTRANCE(entranceSceneId, 0), betterMapSelectIndex },
#define DEFINE_SCENE_UNSET(_enumValue)

typedef struct {
    char* name;
    s32 entrance;
    s32 index;
} BetterMapSelectInfoEntry;

static BetterMapSelectInfoEntry sBetterMapSelectInfo[102] = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

#define STAGE_CURRENT_TIME 0x9000

extern SceneSelectEntry sScenes[143];

static bool sIsBetterMapSelectEnabled = false;

typedef struct {
    u16 entrance;
    s16 roomIndex;
    s16 data;
    s16 yaw;
    s16 playerParams;
    Vec3f pos;
} BetterMapSelectRespawnInfo;

typedef struct {
    BetterMapSelectRespawnInfo grottoRespawn;
    BetterMapSelectRespawnInfo downRespawn;
} BetterMapSelectGrottoRespawnInfo;

#define UNUSED_GROTTO_RESPAWN                                    \
    {                                                            \
        0, 0, 0xFF, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { \
            0, 0, 0                                              \
        }                                                        \
    }

// clang-format off
BetterMapSelectGrottoRespawnInfo sBetterMapSelectGrottoInfo[] = {
    /*  0 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -5644, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -2782, 48, -1654 } },
               { ENTRANCE(TERMINA_FIELD, 0), 0, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -2400, 68, -400 } } },
    /*  1 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -21118, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -1592, -222, 4622 } },
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
    /*  2 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -27125, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 4450, 254, 925 } },
               { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 1672, 68, -394 } } },
    /*  3 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -11287, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 192, 48, -3138 } },
               { ENTRANCE(TERMINA_FIELD, 8), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -400, 48, -2520 } } },
    /*  4 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x3F, -22028, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 1012, -221, 3642 } }, // Reused chest
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
    /*  5 */ { { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0xFF, -21846, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 589, 195, 53 } },
               { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x01, 20024, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -2044, 200, 1288 } } },
    /*  6 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 5, 0, 1433 } } },
    /*  7 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 10558, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -2425, -281, -3291 } },
               { ENTRANCE(TERMINA_FIELD, 8), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -400, 48, -2520 } } },
    /*  8 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 5, 0, 1433 } } },
    /*  9 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -19479, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 3223, 219, 1417 } },
               { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 1672, 68, -394 } } },
    /* 10 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -32768, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -375, -222, 3976 } }, // Reused cow
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
    /* 11 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -5159, -281, -571 } },
               { ENTRANCE(TERMINA_FIELD, 0), 0, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -2400, 68, -400 } } },
    /* 12 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 5, 0, 1433 } } },
    /* 13 */ { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, 21663, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -2317, -221, 3418 } },
               { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
    /* 14 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 5, 0, 1433 } } },
    /* 15 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(DEKU_PALACE, 2), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 5, 0, 1433 } } },
    /* 16 */ { UNUSED_GROTTO_RESPAWN,
               { ENTRANCE(GORON_VILLAGE_WINTER, 3), 0, 0x01, -12015, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 2621, -200, -1389 } } },
};

// TODO: Figure out how to deal with re-used grottos

// Grotto 4 re-use
// Termina Field near Ikana pillar grotto
// { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x9A, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 2367, 315, -192 } },
// { ENTRANCE(TERMINA_FIELD, 7), 0, 0x01, 16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 1672, 68, -394 } } },
// Termina Field near swamp grass grotto
// { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x3F, -22028, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 1012, -221, 3642 } },
// { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
// Road to Southern Swamp near heart piece tree grotto
// { { ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), 0, 0x3E, 4187, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 104, -182, 2202 } },
// { ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), 0, 0x01, -3641, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 331, -143, 245 } } },
// Woods of Mystery Day 2 path grotto
// { { ENTRANCE(WOODS_OF_MYSTERY, 0), 2, 0x5C, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 2, 0, -889 } },
// { ENTRANCE(WOODS_OF_MYSTERY, 0), 1, 0x01, -16384, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 274, 0, 0 } } },
// Southern Swamp behind spider house grotto
// { { ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), 1, 0x3D, -5462, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -1700, 38, 1800 } },
// { ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), 1, 0x01, -2731, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -1049, 12, 2042 } } },
// Mountain Village Spring ramps near Darmani grave grotto
// { { ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 0), 1, 0x3B, -10377, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 2406, 1168, -1197 } },
// { ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 2), 0, 0x01, -22210, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 2089, 15, 939 } } },
// Path to Goron Village ramp hidden grotto
// { { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x99, 12379, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -1309, 320, 143 } },
// { ENTRANCE(PATH_TO_GORON_VILLAGE_WINTER, 0), 0, 0x01, 20024, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -2044, 200, 1288 } } },
// Path to Snowhead snow triangle hidden grotto
// { { ENTRANCE(PATH_TO_SNOWHEAD, 0), 0, 0x33, -19479, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -987, 360, -2339 } },
// { ENTRANCE(PATH_TO_SNOWHEAD, 1), 0, 0x01, 8192, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -2518, 550, -3441 } } },
// Great Bay behind Fisherman hut grotto
// { { ENTRANCE(GREAT_BAY_COAST, 0), 0, 0x37, -11287, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 1359, 80, 5018 } },
// { ENTRANCE(GREAT_BAY_COAST, 4), 0, 0x01, -28217, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 1137, 92, 4635 } } },
// Zora Cape under rock grotto
// { { ENTRANCE(ZORA_CAPE, 0), 0, 0x95, -14018, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -562, 80, 2707 } },
// { ENTRANCE(ZORA_CAPE, 0), 0, 0x01, 3583, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 92, 12, 333 } } },
// Road to Ikana under rock grotto
// { { ENTRANCE(ROAD_TO_IKANA, 0), 0, 0x96, -19479, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -428, 200, -335 } },
// { ENTRANCE(ROAD_TO_IKANA, 0), 0, 0x01, 14563, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -3006, 0, -305 } } },
// Ikana Graveyard rock circle hidden grotto
// { { ENTRANCE(IKANA_GRAVEYARD, 0), 0, 0xB8, -28217, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 106, 314, -1777 } },
// { ENTRANCE(IKANA_GRAVEYARD, 0), 0, 0x01, -32768, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -45, -49, 864 } } },
// Ikana Canyon near Secret Shrine grotto
// { { ENTRANCE(IKANA_CANYON, 0), 2, 0xB4, -28217, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -2475, -505, 2475 } },
// { ENTRANCE(IKANA_CANYON, 12), 2, 0x01, 13653, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -3068, -505, 2690 } } },

// Grotto 10 re-use
// Termina Field Hollow Log hidden grotto
// { { ENTRANCE(TERMINA_FIELD, 0), 0, 0x1F, -32768, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { -375, -222, 3976 } },
// { ENTRANCE(TERMINA_FIELD, 6), 0, 0x01, 0, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { -412, -77, 1681 } } },
// Great Bay Cliffs near Gerudo Fortress grotto
// { { ENTRANCE(GREAT_BAY_COAST, 0), 0, 0xFF, -13654, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_4), { 2077, 333, -215 } },
// { ENTRANCE(GREAT_BAY_COAST, 6), 0, 0x01, 16019, PLAYER_PARAMS(0xFF, PLAYER_INITMODE_D), { 1321, -16, -1029 } } },

// clang-format on

void BetterMapSelect_LoadGame(MapSelectState* mapSelectState, u32 entrance, s32 spawn) {
    CVarSetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", mapSelectState->currentScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.Opt", mapSelectState->opt);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", mapSelectState->topDisplayedScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", mapSelectState->pageDownIndex);
    CVarSave();
    MapSelect_LoadGame(mapSelectState, entrance, spawn);

    // Remove dummy cutscene index for retaining current time
    if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME) {
        gSaveContext.save.cutsceneIndex = 0;
    }

    // Handle Grotto return locations
    if (Entrance_GetSceneIdAbsolute(entrance) == SCENE_KAKUSIANA) {
        if (spawn >= 0 && spawn < ARRAY_COUNT(sBetterMapSelectGrottoInfo)) {
            // Set player void out location
            gSaveContext.respawn[RESPAWN_MODE_DOWN].data = sBetterMapSelectGrottoInfo[spawn].downRespawn.data;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = sBetterMapSelectGrottoInfo[spawn].downRespawn.roomIndex;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = sBetterMapSelectGrottoInfo[spawn].downRespawn.entrance;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = sBetterMapSelectGrottoInfo[spawn].downRespawn.pos;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = sBetterMapSelectGrottoInfo[spawn].downRespawn.yaw;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams =
                sBetterMapSelectGrottoInfo[spawn].downRespawn.playerParams;

            // Copy void out to top
            gSaveContext.respawn[RESPAWN_MODE_TOP] = gSaveContext.respawn[RESPAWN_MODE_DOWN];

            // Set grotto respawn info
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].data = sBetterMapSelectGrottoInfo[spawn].grottoRespawn.data;
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].roomIndex =
                sBetterMapSelectGrottoInfo[spawn].grottoRespawn.roomIndex;
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].entrance =
                sBetterMapSelectGrottoInfo[spawn].grottoRespawn.entrance;
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].pos = sBetterMapSelectGrottoInfo[spawn].grottoRespawn.pos;
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].yaw = sBetterMapSelectGrottoInfo[spawn].grottoRespawn.yaw;
            gSaveContext.respawn[RESPAWN_MODE_UNK_3].playerParams =
                sBetterMapSelectGrottoInfo[spawn].grottoRespawn.playerParams;
        }
    }
}

void BetterMapSelect_LoadFileSelect(MapSelectState* mapSelectState) {
    CVarSetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", mapSelectState->currentScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.Opt", mapSelectState->opt);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", mapSelectState->topDisplayedScene);
    CVarSetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", mapSelectState->pageDownIndex);
    CVarSave();
    gSaveContext.gameMode = GAMEMODE_FILE_SELECT;
    STOP_GAMESTATE(&mapSelectState->state);
    SET_NEXT_GAMESTATE(&mapSelectState->state, FileSelect_Init, sizeof(FileSelectState));
}

void BetterMapSelect_Init(MapSelectState* mapSelectState) {
    static bool sIsInitialized = false;
    sIsBetterMapSelectEnabled = CVarGetInteger("gDeveloperTools.BetterMapSelect.Enabled", 0);

    if (sIsBetterMapSelectEnabled) {
        if (!sIsInitialized) {
            for (s32 i = 0; i < ARRAY_COUNT(sBetterMapSelectInfo); i++) {
                sBetterScenes[sBetterMapSelectInfo[i].index].name = sBetterMapSelectInfo[i].name;
                sBetterScenes[sBetterMapSelectInfo[i].index].entrance = sBetterMapSelectInfo[i].entrance;
            }
            sIsInitialized = true;
        }

        mapSelectState->scenes = sBetterScenes;
        mapSelectState->count = ARRAY_COUNT(sBetterScenes);
        mapSelectState->currentScene = CVarGetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", 0);
        mapSelectState->opt = CVarGetInteger("gDeveloperTools.BetterMapSelect.Opt", 0);
        mapSelectState->topDisplayedScene = CVarGetInteger("gDeveloperTools.BetterMapSelect.TopDisplayedScene", 0);
        mapSelectState->pageDownIndex = CVarGetInteger("gDeveloperTools.BetterMapSelect.PageDownIndex", 0);

        gSaveContext.save.cutsceneIndex = STAGE_CURRENT_TIME;
    } else {
        mapSelectState->scenes = sScenes;
        mapSelectState->count = ARRAY_COUNT(sScenes);
        mapSelectState->currentScene = 0;
        mapSelectState->opt = 0;
        mapSelectState->topDisplayedScene = 0;
        mapSelectState->pageDownIndex = 0;

        if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME) {
            gSaveContext.save.cutsceneIndex = 0;
        }
    }
}

void BetterMapSelect_Update(MapSelectState* mapSelectState) {
    if (sIsBetterMapSelectEnabled != CVarGetInteger("gDeveloperTools.BetterMapSelect.Enabled", 0)) {
        BetterMapSelect_Init(mapSelectState);
    }

    static s32 sScene = -1;
    Input* controller1 = CONTROLLER1(&mapSelectState->state);

    if (sScene == -1) {
        sScene = CVarGetInteger("gDeveloperTools.BetterMapSelect.CurrentScene", 0);
    }

    mapSelectState->opt = CLAMP_MIN(mapSelectState->opt, 0);

    // Reset entrance value when scene changes
    if (sScene != mapSelectState->currentScene) {
        sScene = mapSelectState->currentScene;
        mapSelectState->opt = 0;
    }

    // Update to a normal stage value, then standard MapSelect_Update handles the rest
    if (gSaveContext.save.cutsceneIndex == STAGE_CURRENT_TIME &&
        CHECK_BTN_ANY(controller1->press.button, BTN_R | BTN_Z)) {
        gSaveContext.save.cutsceneIndex = 0;
    }
}

static const char* betterFormLabels[] = {
    "Deity", "Goron", "Zora", "Deku", "Child",
};

void BetterMapSelect_PrintMenu(MapSelectState* mapSelectState, GfxPrint* printer) {
    s32 i;
    s32 sceneIndex;
    char* sceneName;
    char* stageName;
    char* dayName;

    // Header
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    GfxPrint_SetPos(printer, 12, 2);
    GfxPrint_Printf(printer, "Scene Selection");
    GfxPrint_SetColor(printer, 255, 255, 255, 255);

    // Scenes
    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(printer, 3, i + 4);

        sceneIndex = (mapSelectState->topDisplayedScene + i + mapSelectState->count) % mapSelectState->count;
        if (sceneIndex == mapSelectState->currentScene) {
            GfxPrint_SetColor(printer, 255, 100, 100, 255);
        } else {
            GfxPrint_SetColor(printer, 175, 175, 175, 255);
        }

        sceneName = sBetterScenes[sceneIndex].name;
        GfxPrint_Printf(printer, "%3d %s", sceneIndex, sceneName);
    };

    // Entrance
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_SetPos(printer, 28, 26);
    GfxPrint_Printf(printer, "Entrance:");
    GfxPrint_SetColor(printer, 200, 200, 50, 255);
    GfxPrint_Printf(printer, "%2d", mapSelectState->opt);

    // Form
    GfxPrint_SetPos(printer, 26, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "(B)Form:");
    GfxPrint_SetColor(printer, 55, 200, 50, 255);
    GfxPrint_Printf(printer, "%s", betterFormLabels[GET_PLAYER_FORM]);

    // Day
    GfxPrint_SetPos(printer, 1, 25);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "Day:");
    GfxPrint_SetColor(printer, 100, 100, 200, 255);
    switch (gSaveContext.save.day) {
        case 1:
            dayName = "First Day";
            break;
        case 2:
            dayName = "Second Day";
            break;
        case 3:
            dayName = "Final Day";
            break;
        case 4:
            dayName = "Clear Day";
            break;
        default:
            gSaveContext.save.day = 1;
            dayName = "First Day";
            break;
    }
    GfxPrint_Printf(printer, "%s", dayName);

    // Stage
    GfxPrint_SetPos(printer, 1, 26);
    GfxPrint_SetColor(printer, 100, 100, 100, 255);
    GfxPrint_Printf(printer, "(Z/R)Stage:");
    GfxPrint_SetColor(printer, 200, 100, 200, 255);
    switch (gSaveContext.save.cutsceneIndex) {
        case 0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "Afternoon";
            break;
        case 0x8000:
            gSaveContext.save.time = CLOCK_TIME(6, 0) + 1;
            stageName = "Morning";
            break;
        case 0x8800:
            gSaveContext.save.time = CLOCK_TIME(18, 1);
            stageName = "Night";
            break;
        case 0xFFF0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "0";
            break;
        case 0xFFF1:
            stageName = "1";
            break;
        case 0xFFF2:
            stageName = "2";
            break;
        case 0xFFF3:
            stageName = "3";
            break;
        case 0xFFF4:
            stageName = "4";
            break;
        case 0xFFF5:
            stageName = "5";
            break;
        case 0xFFF6:
            stageName = "6";
            break;
        case 0xFFF7:
            stageName = "7";
            break;
        case 0xFFF8:
            stageName = "8";
            break;
        case 0xFFF9:
            stageName = "9";
            break;
        case 0xFFFA:
            stageName = "A";
            break;
        default:
            stageName = "???";
            break;
    }

    if (gSaveContext.save.cutsceneIndex != STAGE_CURRENT_TIME) {
        GfxPrint_Printf(printer, "%s", stageName);
    } else {
        u16 curMinutes = (s32)TIME_TO_MINUTES_F(gSaveContext.save.time) % 60;
        u16 curHours = (s32)TIME_TO_MINUTES_F(gSaveContext.save.time) / 60;
        char* ampm = "";
        char* hourPrefix = "";
        char* minutePrefix = curMinutes < 10 ? "0" : "";

        // Handle 24 or 12 hour time
        if (CVarGetInteger("gEnhancements.Graphics.24HoursClock", 0)) {
            if (curHours < 10) {
                hourPrefix = "0";
            }
        } else {
            ampm = curHours >= 12 ? "pm" : "am";
            curHours = curHours % 12 ? curHours % 12 : 12;
        }

        GfxPrint_Printf(printer, "%s%d:%s%d %s", hourPrefix, curHours, minutePrefix, curMinutes, ampm);
    }
};
