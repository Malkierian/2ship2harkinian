#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_SNOWHEAD_TEMPLE_BLOCK_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_BLOCK_ROOM, true),
            CHECK(RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_LEDGE_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_2, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR,  true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_COMPASS_ROOM,  HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_ENTRANCE,  true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_BOSS_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            // TODO: CAN_KILL_BOSS(Goht)?
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_HC,           CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_WARP,         CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_01,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_10,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_02,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_03,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_04,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_05,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_06,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_07,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_08,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_09,       CAN_USE_MAGIC_ARROW(FIRE)),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_1,  true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_2,  true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_3,  true),
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_4,  true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 7),               ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(GOHTS_LAIR, 0), // Snowhead Temple Boss Room
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_BRIDGE_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM, ((CAN_BE_ZORA && CAN_USE_SWORD) || (CAN_BE_GORON && CAN_USE_SWORD) || CAN_USE_MAGIC_ARROW(FIRE) || HAS_ITEM(ITEM_HOOKSHOT))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_1, CAN_BE_ZORA || CAN_BE_GORON),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_2, CAN_BE_ZORA || CAN_BE_GORON),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_BRIDGE_PILLAR, CAN_USE_PROJECTILE && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_BRIDGE_UNDER_PLATFORM, CAN_USE_PROJECTILE && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_ENTRANCE, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_MAP_ROOM, true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_CHEST, CAN_BE_GORON),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_2, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_PILLARS_ROOM,             true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR_SWITCH_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_PILLARS_ROOM, CAN_BE_DEKU && CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_BLOCK_ROOM,           true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_BRIDGE_ROOM,          true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM,  true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_ENTRANCE,             HAS_ITEM(ITEM_BOW)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_PILLARS_ROOM,         CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_DOOR] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_2, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_DINOLFOS_ROOM, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_UPPER_WIZZROBE_ROOM, true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SECOND_FLOOR] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_1, CAN_BE_GORON || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_2, CAN_BE_GORON || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_2, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM, (CAN_BE_GORON || CAN_USE_MAGIC_ARROW(FIRE))),
            CONNECTION(RR_SNOWHEAD_TEMPLE_LOWER_WIZZROBE_ROOM, CAN_BE_GORON),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_THIRD_FLOOR, CAN_USE_MAGIC_ARROW(FIRE)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_MAP_ROOM, CAN_BE_GORON || HAS_ITEM(ITEM_HOOKSHOT)),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_THIRD_FLOOR] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_ALCOVE_CHEST, ((CAN_BE_DEKU && CAN_BE_GORON) || HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_ITEM(ITEM_HOOKSHOT))),
        },
        .exits = {
            EXIT(ENTRANCE(GOHTS_LAIR, 0),           ONE_WAY_EXIT, CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_SNOWHEAD_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_BOSS_ROOM, CAN_BE_GORON),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SECOND_FLOOR, CAN_BE_GORON && CAN_USE_MAGIC_ARROW(FIRE)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_SNOW_ROOM, (CAN_BE_GORON || HAS_ITEM(ITEM_HOOKSHOT))),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_COMPASS_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_COMPASS, true),
            CHECK(RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_LEDGE_CHEST, (HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_COMPASS_ROOM_CRATE, (CAN_USE_EXPLOSIVE && HAS_ITEM(ITEM_MASK_GREAT_FAIRY))),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_BLOCK_ROOM,   CAN_BE_ZORA || HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_MAGIC_ARROW(FIRE)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_ENTRANCE,     true), // TODO: Key door
            CONNECTION(RR_SNOWHEAD_TEMPLE_ICICLE_ROOM,  CAN_USE_EXPLOSIVE),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_DINOLFOS_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_2, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_SNOW_ROOM, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_DOOR, true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_DUAL_SWITCHES, ((HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC && HAS_ITEM(ITEM_MASK_GREAT_FAIRY)) && ((HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)) || CAN_BE_DEKU))),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SECOND_FLOOR, CAN_BE_GORON || CAN_USE_MAGIC_ARROW(FIRE)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_ICICLE_ROOM, true), // TODO: Key Door
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_ENTRANCE] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
      .checks = {
          CHECK(RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_1, CAN_BE_GORON),
          CHECK(RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_2, CAN_BE_GORON),
      },
      .exits = { //     TO                                         FROM
          EXIT(ENTRANCE(SNOWHEAD, 1),                     ENTRANCE(SNOWHEAD_TEMPLE, 0), true),
      },
      .connections = {
          CONNECTION(RR_SNOWHEAD_TEMPLE_BLOCK_ROOM,   CAN_BE_GORON && CAN_USE_MAGIC_ARROW(FIRE)),
          CONNECTION(RR_SNOWHEAD_TEMPLE_BRIDGE_ROOM,  CAN_BE_GORON),
          CONNECTION(RR_SNOWHEAD_TEMPLE_COMPASS_ROOM, CAN_BE_GORON), // TODO: Key Door
      },
    };
    Regions[RR_SNOWHEAD_TEMPLE_ICICLE_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_ALCOVE_CHEST, (HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC)),
            CHECK(RC_SNOWHEAD_TEMPLE_ICICLE_ROOM, ((CAN_USE_EXPLOSIVE && HAS_ITEM(ITEM_HOOKSHOT)) || CAN_BE_GORON)),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_COMPASS_ROOM,             true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM,       true), // TODO: Key Door
            CONNECTION(RR_SNOWHEAD_TEMPLE_PILLARS_ROOM,             true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_LOWER_WIZZROBE_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_FIRE_ARROW, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SECOND_FLOOR, true),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_MAP_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_MAP_ALCOVE_CHEST, (HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_MAP, true),
            CHECK(RC_SNOWHEAD_TEMPLE_SF_MAP_ROOM, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_BRIDGE_ROOM, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SECOND_FLOOR, (HAS_MAGIC && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_FIRE))),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_PILLARS_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_CHEST,       (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_5, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_6, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_7, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_1, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_2, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_3, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_4, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_5, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_6, (CAN_BE_DEKU || CAN_USE_MAGIC_ARROW(FIRE))),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR_SWITCH_ROOM, CAN_BE_DEKU && CAN_USE_MAGIC_ARROW(FIRE)),
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_FIRST_FLOOR, CAN_BE_DEKU && CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_SNOW_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_SF_SNOW_ROOM, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC && HAS_ITEM(ITEM_MASK_GREAT_FAIRY) && CAN_USE_PROJECTILE),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_THIRD_FLOOR, true),
            CONNECTION(RR_SNOWHEAD_TEMPLE_DINOLFOS_ROOM, CAN_USE_MAGIC_ARROW(FIRE)),
        },
    };
    Regions[RR_SNOWHEAD_TEMPLE_UPPER_WIZZROBE_ROOM] = RandoRegion{ .sceneId = SCENE_HAKUGIN_BS,
        .checks = {
            CHECK(RC_SNOWHEAD_TEMPLE_BOSS_KEY, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_1, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_2, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_3, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_4, true),
            CHECK(RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_5, true),
        },
        .connections = {
            CONNECTION(RR_SNOWHEAD_TEMPLE_CENTRAL_ROOM_THIRD_FLOOR, true),
        },
    };
}, {});
// clang-format on
