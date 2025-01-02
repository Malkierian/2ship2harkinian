#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_GREAT_BAY_TEMPLE_BABA_CHEST_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_BABA_CHEST, CAN_BE_ZORA || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_COMPASS_ROOM,    true),
            CONNECTION(RR_GREAT_BAY_TEMPLE_MAP_ROOM,        true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_BEFORE_WART] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_01, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_02, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_03, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_04, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_05, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_06, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_07, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_08, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_09, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_10, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_11, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_12, true),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART,  true), // TODO: Key Door
            CONNECTION(RR_GREAT_BAY_TEMPLE_WART,                  true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_BOSS_ROOM] = RandoRegion{ .sceneId = SCENE_SEA_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Gyorg)?
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_HC, CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_WARP, CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_1, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_2, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_3, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_4, CAN_BE_ZORA),
            CHECK(RC_GIANTS_CHAMBER_OATH, CAN_BE_ZORA && HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 9),                             ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(GYORGS_LAIR, 0), // Great Bay Temple Pre Boss Room
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_BARREL,           CAN_BE_GORON || CAN_USE_EXPLOSIVE),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_1,               true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_2,               true),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_UNDERWATER_POT,   CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_GREEN_PIPE_1,            CAN_USE_MAGIC_ARROW(ICE)),
            CONNECTION(RR_GREAT_BAY_TEMPLE_MAP_ROOM,                CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_PRE_BOSS_ROOM,           CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART,    true),
            CONNECTION(RR_GREAT_BAY_TEMPLE_WATER_WHEEL_ROOM,        true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_COMPASS_ROOM_WITH_BOSS_KEY_CHEST] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_BOSS_KEY, true),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_COMPASS_ROOM,    true),
            CONNECTION(RR_GREAT_BAY_TEMPLE_GEKKO,           true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_COMPASS_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS,                    true),
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_UNDERWATER,    CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_1,   CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_2,   CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_3,   CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_COMPASS_ROOM_POT,        CAN_BE_ZORA || (CAN_USE_PROJECTILE && HAS_ITEM(ITEM_MASK_GREAT_FAIRY))),
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_FREESTANDING_RUPEE_01, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_FREESTANDING_RUPEE_02, CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_BABA_CHEST_ROOM,                   true),
            CONNECTION(RR_GREAT_BAY_TEMPLE_COMPASS_ROOM_WITH_BOSS_KEY_CHEST,  CAN_BE_ZORA && CAN_USE_MAGIC_ARROW(ICE)),
            CONNECTION(RR_GREAT_BAY_TEMPLE_GEKKO,                             CAN_USE_MAGIC_ARROW(ICE)),
            CONNECTION(RR_GREAT_BAY_TEMPLE_GREEN_PIPE_2,                      CAN_BE_ZORA),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_ENTRANCE] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_ENTRANCE_CHEST, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 7),                    ENTRANCE(GREAT_BAY_TEMPLE, 0), HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_WATER_WHEEL_ROOM,  true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_GEKKO] = RandoRegion{ .sceneId = SCENE_SEA,
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_COMPASS_ROOM_WITH_BOSS_KEY_CHEST,    CAN_USE_MAGIC_ARROW(ICE)),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_GREEN_PIPE_1] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_CHEST,                 HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_1,                 CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_2,                 CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_3,                 CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_4,                 CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_FREESTANDING_RUPEE_01, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_FREESTANDING_RUPEE_02, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM,  true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_GREEN_PIPE_2] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_LOWER_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_UPPER_CHEST, (HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_MAGIC_ARROW(ICE))),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_1,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_2,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_3,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_4,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_5,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_6,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_7,       CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_8,       CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_COMPASS_ROOM,  CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_GREEN_PIPE_3,  CAN_USE_MAGIC_ARROW(ICE)),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_GREEN_PIPE_3] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_CHEST,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_LOWER,   true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_1, CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_2, CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_GREEN_PIPE_3_BARREL,   CAN_USE_MAGIC_ARROW(FIRE)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_GREEN_PIPE_2,  true),
            CONNECTION(RR_GREAT_BAY_TEMPLE_MAP_ROOM,      CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_MAP_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_MAP,                      HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_MAGIC_ARROW(ICE)),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_1,   true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_2,   true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_3,   CAN_BE_ZORA || CAN_USE_MAGIC_ARROW(ICE)),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_1,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_2,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_3,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_4,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_5,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_6,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_7,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_8,     CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_MAP_ROOM_POT,          CAN_BE_ZORA || CAN_USE_MAGIC_ARROW(ICE)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_BABA_CHEST_ROOM,         CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM,            CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM,    true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_PRE_BOSS_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_1,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_2,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_3,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_4,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_5,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_6,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_7,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_8,          CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_ABOVE_WATER, CAN_USE_PROJECTILE && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_UNDERWATER,  CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GYORGS_LAIR, 0),                           ONE_WAY_EXIT, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_GREAT_BAY_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM, CAN_BE_ZORA),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_1, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_2, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_3, CAN_BE_ZORA),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_4, CAN_BE_ZORA),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM,    CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_BEFORE_WART,     true), // TODO: Key Door
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_MAP_ROOM, true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_WART] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_ICE_ARROW,  true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_1, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_2, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_3, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_4, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_5, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_6, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_7, true),
            CHECK(RC_GREAT_BAY_TEMPLE_POT_WART_8, true),
        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_BEFORE_WART, true),
        },
    };
    Regions[RR_GREAT_BAY_TEMPLE_WATER_WHEEL_ROOM] = RandoRegion{ .sceneId = SCENE_SEA,
        .checks = {
            CHECK(RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_PLATFORM,           CAN_BE_ZORA || (HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_MASK_GREAT_FAIRY))),
            CHECK(RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_SKULLTULA,          true),
            CHECK(RC_GREAT_BAY_TEMPLE_WATER_WHEEL_FREESTANDING_RUPEE_01, true),
            CHECK(RC_GREAT_BAY_TEMPLE_WATER_WHEEL_FREESTANDING_RUPEE_02, true),
            CHECK(RC_GREAT_BAY_TEMPLE_WATER_WHEEL_FREESTANDING_RUPEE_03, true),
            CHECK(RC_GREAT_BAY_TEMPLE_WATER_WHEEL_FREESTANDING_RUPEE_04, true),
            CHECK(RC_GREAT_BAY_TEMPLE_WATER_WHEEL_FREESTANDING_RUPEE_05, true),

        },
        .connections = {
            CONNECTION(RR_GREAT_BAY_TEMPLE_CENTRAL_ROOM,  CAN_BE_ZORA),
            CONNECTION(RR_GREAT_BAY_TEMPLE_ENTRANCE,      true),
        },
    };
}, {});
// clang-format on
