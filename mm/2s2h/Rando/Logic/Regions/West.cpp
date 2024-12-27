#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_FISHERMANS_HUT] = RandoRegion{ .sceneId = SCENE_FISHERMAN,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 4),              ENTRANCE(FISHERMANS_HUT, 0), true),
        },
        .events = {
            // TODO: Should this be a check?
            EVENT_ACCESS(RANDO_ACCESS_SEAHORSE, RANDO_ACCESS[RANDO_ACCESS_PIRATE_PICTURE] && HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
        },
    };
    Regions[RR_GREAT_BAY_COAST] = RandoRegion{ .sceneId = SCENE_30GYOSON,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_GREAT_BAY_COAST_MIKAU, CAN_PLAY_SONG(HEALING)),
            CHECK(RC_GREAT_BAY_COAST_HP, CAN_HOOK_SCARECROW && CAN_GROW_BEAN_PLANT),
            CHECK(RC_GREAT_BAY_COAST_POT_01, true),
            CHECK(RC_GREAT_BAY_COAST_POT_02, true),
            CHECK(RC_GREAT_BAY_COAST_POT_03, true),
            CHECK(RC_GREAT_BAY_COAST_POT_04, true),
            CHECK(RC_GREAT_BAY_COAST_POT_05, true),
            CHECK(RC_GREAT_BAY_COAST_POT_06, true),
            CHECK(RC_GREAT_BAY_COAST_POT_07, true),
            CHECK(RC_GREAT_BAY_COAST_POT_08, true),
            CHECK(RC_GREAT_BAY_COAST_POT_09, true),
            CHECK(RC_GREAT_BAY_COAST_POT_10, true),
            CHECK(RC_GREAT_BAY_COAST_POT_11, true),
            CHECK(RC_GREAT_BAY_COAST_POT_12, true),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_1, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_2, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_POT_LEDGE_3, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_TINGLE_MAP_1, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_GREAT_BAY_COAST_TINGLE_MAP_2, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 2),                ENTRANCE(GREAT_BAY_COAST, 0), true),
            EXIT(ENTRANCE(ZORA_CAPE, 0),                    ENTRANCE(GREAT_BAY_COAST, 1), true),
            EXIT(ENTRANCE(PINNACLE_ROCK, 0),                ENTRANCE(GREAT_BAY_COAST, 3), true),
            EXIT(ENTRANCE(FISHERMANS_HUT, 0),               ENTRANCE(GREAT_BAY_COAST, 4), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 0),    ENTRANCE(GREAT_BAY_COAST, 5), CAN_BE_ZORA),
            EXIT(ENTRANCE(MARINE_RESEARCH_LAB, 0),          ENTRANCE(GREAT_BAY_COAST, 7), true),
            EXIT(ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0),       ENTRANCE(GREAT_BAY_COAST, 8), true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_GREAT_BAY_COAST),
        },
        .oneWayEntrances = {
            ENTRANCE(GREAT_BAY_COAST, 11), // From Song of Soaring
            ENTRANCE(GREAT_BAY_COAST, 12), // From being captured in Pirate Fortress Moat
        },
    };
    Regions[RR_GREAT_BAY_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .sceneId = SCENE_YOUSEI_IZUMI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 5),                    ENTRANCE(FAIRY_FOUNTAIN, 3), true),
        },
    };
    Regions[RR_MARINE_RESEARCH_LAB] = RandoRegion{ .sceneId = SCENE_LABO,
        .checks = {
            CHECK(RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA, CAN_BE_ZORA && HAS_ITEM(ITEM_OCARINA_OF_TIME) && RANDO_ACCESS[RANDO_ACCESS_ZORA_EGG] >= 7),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 7),              ENTRANCE(MARINE_RESEARCH_LAB, 0), true),
        },
    };
    Regions[RR_PINNACLE_ROCK_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_SINKAI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 3),              ENTRANCE(PINNACLE_ROCK, 0), true),
        },
        .connections = {
            CONNECTION(RR_PINNACLE_ROCK_INNER, RANDO_ACCESS[RANDO_ACCESS_SEAHORSE] && CAN_BE_ZORA)
        }
    };
    Regions[RR_PINNACLE_ROCK_INNER] = RandoRegion{ .name = "Inner", .sceneId = SCENE_SINKAI,
        .checks = {
            CHECK(RC_PINNACLE_ROCK_CHEST_1,     CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_CHEST_2,     CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_01,      CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_02,      CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_03,      CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_04,      CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_05,      CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_PINNACLE_ROCK_POT_06,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_07,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_08,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_09,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_10,      CAN_BE_ZORA),
            CHECK(RC_PINNACLE_ROCK_POT_11,      CAN_BE_ZORA),
            // TODO: Missing HP check to add here later.
        },
        .connections = {
            CONNECTION(RR_PINNACLE_ROCK_ENTRANCE, true)
        },
        .events = {
            EVENT_ACCESS(RANDO_ACCESS_ZORA_EGG, HAS_MAGIC && HAS_BOTTLE && CAN_BE_ZORA),
            EVENT_ACCESS(RANDO_ACCESS_ZORA_EGG, HAS_MAGIC && HAS_BOTTLE && CAN_BE_ZORA),
            EVENT_ACCESS(RANDO_ACCESS_ZORA_EGG, HAS_MAGIC && HAS_BOTTLE && CAN_BE_ZORA),
        },
    };
    Regions[RR_WATERFALL_RAPIDS] = RandoRegion{ .sceneId = SCENE_35TAKI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 4),                    ENTRANCE(WATERFALL_RAPIDS, 0), true),
        },
    };
    Regions[RR_ZORA_CAPE] = RandoRegion{ .sceneId = SCENE_31MISAKI,
        .checks = {
            // TODO: Grottos
            CHECK(RC_ZORA_CAPE_GROTTO,                    CAN_USE_EXPLOSIVE || CAN_BE_GORON),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_1,             HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_DEKU),
            CHECK(RC_ZORA_CAPE_LEDGE_CHEST_2,             HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ZORA_CAPE_UNDERWATER_CHEST,          CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_WATERFALL_HP,              CAN_BE_ZORA),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_1,        true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_BEAVERS_2,        true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_1,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_2,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_3,     true),
            CHECK(RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_4,     true),
            CHECK(RC_ZORA_CAPE_OWL_STATUE,                CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 1),              ENTRANCE(ZORA_CAPE, 0), true),
            EXIT(ENTRANCE(ZORA_HALL, 0),                    ENTRANCE(ZORA_CAPE, 1), CAN_BE_ZORA),
            EXIT(ENTRANCE(ZORA_HALL,        1),             ENTRANCE(ZORA_CAPE, 2), true),
            EXIT(ENTRANCE(WATERFALL_RAPIDS, 0),             ENTRANCE(ZORA_CAPE, 4), HAS_ITEM(ITEM_HOOKSHOT)),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 3),               ENTRANCE(ZORA_CAPE, 5), HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_EXPLOSIVE),
            EXIT(ENTRANCE(GREAT_BAY_TEMPLE, 0),             ENTRANCE(ZORA_CAPE, 7), CAN_BE_ZORA && HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(BOSSA_NOVA)),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_ZORA_CAPE),
        },
        .oneWayEntrances = {
            ENTRANCE(ZORA_CAPE, 6), // From Song of Soaring
            ENTRANCE(ZORA_CAPE, 9), // From Great Bay Temple Blue Warp
        },
    };
    Regions[RR_ZORA_HALL_EVANS_ROOM] = RandoRegion{ .name = "Evan's Room", .sceneId = SCENE_BANDROOM,
        // TODO: Missing HP check to add here later.
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 4),                    ENTRANCE(ZORA_HALL_ROOMS, 3), true),
        },
    };
    Regions[RR_ZORA_HALL_JAPAS_ROOM] = RandoRegion{ .name = "Japa's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 5),                    ENTRANCE(ZORA_HALL_ROOMS, 1), true),
        },
    };
    Regions[RR_ZORA_HALL_LULUS_ROOM] = RandoRegion{ .name = "Lulu's Room", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_HALL_SCRUB_DEED,          Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON),
            CHECK(RC_ZORA_HALL_SCRUB_HP,            Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) && CAN_BE_GORON && CAN_BE_DEKU),
            CHECK(RC_ZORA_HALL_SCRUB_POTION_REFILL, CAN_BE_ZORA),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 3),                    ENTRANCE(ZORA_HALL_ROOMS, 2), true),
        },
        .events = {
            EVENT_ACCESS(RANDO_ACCESS_GREEN_POTION_REFILL, CAN_BE_ZORA),
        },
    };
    Regions[RR_ZORA_HALL_MIKAUS_ROOM] = RandoRegion{ .name = "Mikau's Room", .sceneId = SCENE_BANDROOM,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 6),                    ENTRANCE(ZORA_HALL_ROOMS, 0), true),
        },
    };
    Regions[RR_ZORA_HALL_SHOP] = RandoRegion{ .name = "Shop", .sceneId = SCENE_BANDROOM,
        .checks = {
            CHECK(RC_ZORA_SHOP_ITEM_1, CAN_AFFORD(RC_ZORA_SHOP_ITEM_1)),
            CHECK(RC_ZORA_SHOP_ITEM_2, CAN_AFFORD(RC_ZORA_SHOP_ITEM_2)),
            CHECK(RC_ZORA_SHOP_ITEM_3, CAN_AFFORD(RC_ZORA_SHOP_ITEM_3)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_HALL, 2),                    ENTRANCE(ZORA_HALL_ROOMS, 5), true),
        },
    };
    Regions[RR_ZORA_HALL] = RandoRegion{ .sceneId = SCENE_33ZORACITY,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ZORA_CAPE, 1),                    ENTRANCE(ZORA_HALL, 0), true),           
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 5),              ENTRANCE(ZORA_HALL, 2), true), // To Shop
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 2),              ENTRANCE(ZORA_HALL, 3), CAN_BE_ZORA), // To Lulu's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 3),              ENTRANCE(ZORA_HALL, 4), CAN_BE_ZORA), // To Evan's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 1),              ENTRANCE(ZORA_HALL, 5), CAN_BE_ZORA), // To Japas's Room
            EXIT(ENTRANCE(ZORA_HALL_ROOMS, 0),              ENTRANCE(ZORA_HALL, 6), CAN_BE_ZORA), // To Mikaus's Room
        },
    };
}, {});
// clang-format on
