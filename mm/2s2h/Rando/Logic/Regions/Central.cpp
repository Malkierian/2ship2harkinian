#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_ASTRAL_OBSERVATORY_OUTSIDE] = RandoRegion{ .name = "Outside Astral Observatory", .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1),           ENTRANCE(TERMINA_FIELD, 9), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_GROW_BEAN_PLANT)
        },
    };
    Regions[RR_ASTRAL_OBSERVATORY_PASSAGE] = RandoRegion{ .name = "Passage", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, CAN_USE_EXPLOSIVE),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_3, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2),              ENTRANCE(ASTRAL_OBSERVATORY, 0), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY, (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA),
        },
    };
    Regions[RR_ASTRAL_OBSERVATORY] = RandoRegion{ .name = "Inside Astral Observatory", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_POT_1, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_2, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_3, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 9),                ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, (CAN_BE_DEKU && HAS_MAGIC) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA), // TODO: Trick to backflip over balloon
        },
    };
    Regions[RR_BOMB_SHOP] = RandoRegion{ .sceneId = SCENE_BOMYA,
        .checks = {
            CHECK(RC_BOMB_SHOP_ITEM_1, CAN_AFFORD(RC_BOMB_SHOP_ITEM_1)),
            CHECK(RC_BOMB_SHOP_ITEM_2, CAN_AFFORD(RC_BOMB_SHOP_ITEM_2)),
            CHECK(RC_BOMB_SHOP_ITEM_3, CAN_AFFORD(RC_BOMB_SHOP_ITEM_3)),
            // TODO: Bigger bomb bag
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 6),              ENTRANCE(BOMB_SHOP, 0), true),
        },
    };
    Regions[RR_CLOCK_TOWER_INTERIOR] = RandoRegion{ .sceneId = SCENE_INSIDETOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),             ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
        },
    };
    Regions[RR_CLOCK_TOWER_ROOF] = RandoRegion{ .sceneId = SCENE_OKUJOU,
        .checks = {
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, (HAS_MAGIC && CAN_BE_DEKU) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, (HAS_MAGIC && CAN_BE_DEKU) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_1, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_2, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_3, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_4, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, CAN_PLAY_SONG(OATH) && CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) && CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) && CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) && CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)),
        },
        .oneWayEntrances = {
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
    };
    Regions[RR_CLOCK_TOWN_EAST] = RandoRegion{ .sceneId = SCENE_TOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWN_EAST_UPPER_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 7),                ENTRANCE(EAST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7),             ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0),           ENTRANCE(EAST_CLOCK_TOWN, 2), true), // TODO: Bombers Code req
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2),             ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0),          ENTRANCE(EAST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1),             ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),      ENTRANCE(EAST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0),             ENTRANCE(EAST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0),        ENTRANCE(EAST_CLOCK_TOWN, 8), true),
            EXIT(ENTRANCE(STOCK_POT_INN, 0),                ENTRANCE(EAST_CLOCK_TOWN, 9), true), // To lobby
            EXIT(ENTRANCE(STOCK_POT_INN, 1),                ENTRANCE(EAST_CLOCK_TOWN, 10), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0),                     ENTRANCE(EAST_CLOCK_TOWN, 11), HAS_ITEM(ITEM_MASK_ROMANI)),
        },
    };
    Regions[RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Clock Town", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            // TODO: Do we want there to be two checks here? In vanilla you can get one right away but the other requires human form. In rando you start as human. Maybe the other should require any form?
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3),             ENTRANCE(FAIRY_FOUNTAIN, 0), true),
        },
    };
    Regions[RR_CLOCK_TOWN_LAUNDRY] = RandoRegion{ .sceneId = SCENE_ALLEY,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6),             ENTRANCE(LAUNDRY_POOL, 0), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 1),               ENTRANCE(LAUNDRY_POOL, 1), Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI))
        },
    };
    Regions[RR_CLOCK_TOWN_NORTH] = RandoRegion{ .sceneId = SCENE_BACKTOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_1, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_2, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_HP, true),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, CAN_USE_SWORD || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 8),                ENTRANCE(NORTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5),              ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4),             ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0),               ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0),        ENTRANCE(NORTH_CLOCK_TOWN, 4), CAN_BE_DEKU),
        },
    };
    Regions[RR_CLOCK_TOWN_SOUTH] = RandoRegion{ .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, true),
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWN_SOUTH_OWL_STATUE, CAN_USE_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1),         ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6),                ENTRANCE(SOUTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3),              ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2),              ENTRANCE(SOUTH_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2),             ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 5), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0),                 ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0),                   ONE_WAY_EXIT, true),
        },
        .connections = {
            CONNECTION(RR_MAX, true),
        },
        .events = {
            EVENT_OWL_WARP(OWL_WARP_CLOCK_TOWN),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTH_CLOCK_TOWN, 9), // From Song of Soaring
        }
    };
    Regions[RR_CLOCK_TOWN_WEST] = RandoRegion{ .sceneId = SCENE_ICHIBA,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET, true),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_HP, true),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_INTEREST, true),
            CHECK(RC_CLOCK_TOWN_WEST_SISTERS_HP, HAS_ITEM(ITEM_MASK_KAMARO)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 0),                ENTRANCE(WEST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 5),             ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 3),             ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(SWORDMANS_SCHOOL, 0),             ENTRANCE(WEST_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 0),               ENTRANCE(WEST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(TRADING_POST, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(BOMB_SHOP, 0),                    ENTRANCE(WEST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(POST_OFFICE, 0),                  ENTRANCE(WEST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(LOTTERY_SHOP, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 8), true),
        },
    };
    Regions[RR_CURIOSITY_SHOP_BACK] = RandoRegion{ .name = "Back", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add Keaton Mask/Express Letter to Mama checks
            CHECK(RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(LAUNDRY_POOL, 1),                 ENTRANCE(CURIOSITY_SHOP, 1), true)
        },
    };
    Regions[RR_CURIOSITY_SHOP_FRONT] = RandoRegion{ .name = "Front", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            // TODO : Add the shop checks
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 4),              ENTRANCE(CURIOSITY_SHOP, 0), true)
        },
    };
    Regions[RR_HONEY_AND_DARLING] = RandoRegion{ .sceneId = SCENE_BOWLING,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_HONEY_DARLING_ALL_DAYS, HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_BOMBCHU) && HAS_ITEM(ITEM_BOMB)),
            CHECK(RC_CLOCK_TOWN_EAST_HONEY_DARLING_ANY_DAY, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_BOMBCHU) || HAS_ITEM(ITEM_BOMB)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 6),              ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), true),
        },
    };
    Regions[RR_INN] = RandoRegion{ .sceneId = SCENE_YADOYA,
        .checks = {
            // TODO : Add Couples Mask check here.
            CHECK(RC_STOCK_POT_INN_GRANDMA_LONG_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GRANDMA_SHORT_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT)),
            CHECK(RC_STOCK_POT_INN_GUEST_ROOM_CHEST,Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            CHECK(RC_STOCK_POT_INN_LETTER_TO_KAFEI, HAS_ITEM(ITEM_MASK_KAFEIS_MASK)),
            CHECK(RC_STOCK_POT_INN_ROOM_KEY, true),
            CHECK(RC_STOCK_POT_INN_STAFF_ROOM_CHEST, true),
            CHECK(RC_STOCK_POT_INN_TOILET_HAND, 
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) || Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)
            ),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 9),              ENTRANCE(STOCK_POT_INN, 0), true), // From ground floor
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 10),             ENTRANCE(STOCK_POT_INN, 1), true), // From upstairs
        },
        .events = {
            EVENT_ACCESS(RANDO_ACCESS_FISH, true),
            EVENT_ACCESS(RANDO_ACCESS_BUGS, true),
        },
    };
    Regions[RR_LOTTERY_SHOP] = RandoRegion{ .sceneId = SCENE_TAKARAKUJI,
        .checks = {
            // Are we adding the lotter reward as a check?
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 8),              ENTRANCE(LOTTERY_SHOP, 0), true),
        },
    };
    Regions[RR_MAYOR_RESIDENCE] = RandoRegion{ .sceneId = SCENE_SONCHONOIE,
        .checks = {
            CHECK(RC_MAYORS_OFFICE_HP, HAS_ITEM(ITEM_MASK_COUPLE)),
            CHECK(RC_MAYORS_OFFICE_KAFEIS_MASK, true)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 7),              ENTRANCE(MAYORS_RESIDENCE, 0), true),
        },
    };
    Regions[RR_MILK_BAR] = RandoRegion{ .sceneId = SCENE_MILK_BAR,
        .checks = {
            // TODO : Add shop checks/Circus Leader's Mask checks.
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 11),             ENTRANCE(MILK_BAR, 0), true),
        },
    };
    Regions[RR_POST_OFFICE] = RandoRegion{ .sceneId = SCENE_POSTHOUSE,
        .checks = {
            // TODO : Add Heartpiece check
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 7),              ENTRANCE(POST_OFFICE, 0), true),
        },
    };
    Regions[RR_SWORDSMAN_SCHOOL] = RandoRegion{ .sceneId = SCENE_DOUJOU,
        .checks = {
            CHECK(RC_SWORDSMAN_SCHOOL_HP, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_1, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_2, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_3, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_4, CAN_USE_HUMAN_SWORD),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_5, CAN_USE_HUMAN_SWORD),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 3),              ENTRANCE(SWORDMANS_SCHOOL, 0), true),
        },
    };
    Regions[RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST] = RandoRegion{ .name = "Before Great Bay Coast", .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 0),              ENTRANCE(TERMINA_FIELD, 2), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_PLAY_SONG(EPONA)),
        },
    };
    Regions[RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE] = RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0),     ENTRANCE(TERMINA_FIELD, 3), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, HAS_ITEM(ITEM_BOW)),
        },
    };
    Regions[RR_TERMINA_FIELD] = RandoRegion{ .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_TERMINA_FIELD_KAMARO, CAN_PLAY_SONG(HEALING)),
            CHECK(RC_TERMINA_FIELD_POT, CAN_GROW_BEAN_PLANT),
            CHECK(RC_TERMINA_FIELD_TALL_GRASS_CHEST, true),
            CHECK(RC_TERMINA_FIELD_TREE_STUMP_CHEST, CAN_GROW_BEAN_PLANT || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_TERMINA_FIELD_WATER_CHEST, CAN_BE_ZORA),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_01, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_02, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_03, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_04, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_05, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_06, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_07, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_08, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_09, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_10, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_11, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_12, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_13, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_14, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_15, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_16, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_17, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_18, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_19, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
            CHECK(RC_TERMINA_FIELD_GUAY_RUPEE_DROP_20, CAN_PLAY_SONG(SONATA) || CAN_PLAY_SONG(LULLABY) || CAN_PLAY_SONG(BOSSA_NOVA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 0),              ENTRANCE(TERMINA_FIELD, 0), true),
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0),       ENTRANCE(TERMINA_FIELD, 1), true),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 0),                ENTRANCE(TERMINA_FIELD, 4), true),
            EXIT(ENTRANCE(MILK_ROAD, 0),                    ENTRANCE(TERMINA_FIELD, 5), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 1),             ENTRANCE(TERMINA_FIELD, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 0),              ENTRANCE(TERMINA_FIELD, 7), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 0),             ENTRANCE(TERMINA_FIELD, 8), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD_BEFORE_PATH_TO_MOUNTAIN_VILLAGE, HAS_ITEM(ITEM_BOW)),
            CONNECTION(RR_TERMINA_FIELD_BEFORE_GREAT_BAY_COAST, CAN_PLAY_SONG(EPONA)),
            CONNECTION(RR_ASTRAL_OBSERVATORY_OUTSIDE, CAN_BE_DEKU)
        },
    };
    Regions[RR_TOWN_DEKU_PLAYGROUND] = RandoRegion{ .sceneId = SCENE_DEKUTES,
        .checks = {
            // TODO : Add checks for getting best time on all three days/Freestanding items.
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 4),             ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), true),
        },
    };
    Regions[RR_TOWN_SHOOTING_GALLERY] = RandoRegion{ .sceneId = SCENE_SYATEKI_MIZU,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 8),              ENTRANCE(TOWN_SHOOTING_GALLERY, 0), true),
        },
    };
    Regions[RR_TRADING_POST] = RandoRegion{ .sceneId = SCENE_8ITEMSHOP,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_TRADING_POST_POT, true), // Note : Goron has to sidehop to get up.
            CHECK(RC_TRADING_POST_SHOP_ITEM_1, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_1)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_2, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_2)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_3, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_3)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_4, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_4)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_5, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_5)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_6, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_6)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_7, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_7)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_8, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_8)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 5),              ENTRANCE(TRADING_POST, 0), true),
        
        },
    };
    Regions[RR_TREASURE_SHOP] = RandoRegion{ .sceneId = SCENE_TAKARAYA,
        .checks = {
            // TODO : Add check for each form(minus FD)
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 4),              ENTRANCE(TREASURE_CHEST_SHOP, 0), true),
        },
    };
}, {});
// clang-format on
