#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "objects/object_tsubo/object_tsubo.h"

void ObjTsubo_Draw(Actor* actor, PlayState* play);
}

#define OBJTSUBO_RC (actor->home.rot.x)
#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

RandoCheckId IdentifyPot(Actor* actor) {
    auto randoStaticCheck =
        Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, OBJ_TSUBO_PFE00(actor), gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId != RC_UNKNOWN && randoStaticCheck.randoCheckType == RCTYPE_POT) {
        return randoStaticCheck.randoCheckId;
    }
    RandoCheckId randoCheckId = RC_UNKNOWN;

    switch (gPlayState->sceneId) {
        case SCENE_DANPEI:
            if (IS_AT(1881.0f, 980.0f)) {
                randoCheckId = RC_DEKU_SHRINE_POT_01;
            }
            if (IS_AT(2373.0f, 0.0f)) {
                randoCheckId = RC_DEKU_SHRINE_POT_02;
            }
            break;
        case SCENE_F40:
            if (IS_AT(1204.0f, 1871.0f)) {
                randoCheckId = RC_STONE_TOWER_HIGHER_SCARECROW_POT_02;
            }
            if (IS_AT(1364.0f, 2391.0f)) {
                randoCheckId = RC_STONE_TOWER_HIGHER_SCARECROW_POT_08;
            }
            if (IS_AT(-1116.0f, 2231.0f)) {
                randoCheckId = RC_STONE_TOWER_LOWER_SCARECROW_POT_02;
            }
            if (IS_AT(-1116.0f, 2031.0f)) {
                randoCheckId = RC_STONE_TOWER_LOWER_SCARECROW_POT_04;
            }
            if (IS_AT(-1116.0f, 1511.0f)) {
                randoCheckId = RC_STONE_TOWER_LOWER_SCARECROW_POT_11;
            }
            break;
        case SCENE_HAKASHITA:
            if (IS_AT(1270.0f, 380.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_NIGHT_1_BATS_POT_03;
            }
            if (IS_AT(-1121.0f, -879.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_01;
            }
            if (IS_AT(-1200.0f, -881.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_02;
            }
            if (IS_AT(-1200.0f, -1400.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_03;
            }
            if (IS_AT(-1120.0f, -1400.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_04;
            }
            break;
        case SCENE_HAKUGIN_BS:
            if (IS_AT(-1250.0f, -1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_01;
            }
            if (IS_AT(-1400.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_02;
            }
            if (IS_AT(-1600.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_03;
            }
            if (IS_AT(-1250.0f, 1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_04;
            }
            if (IS_AT(61.0f, 1440.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_05;
            }
            if (IS_AT(61.0f, 1740.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_06;
            }
            if (IS_AT(1250.0f, 1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_07;
            }
            if (IS_AT(1460.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_08;
            }
            if (IS_AT(1660.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_09;
            }
            if (IS_AT(1250.0f, -1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_10;
            }
            if (IS_AT(150.0f, -1170.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_01;
            }
            if (IS_AT(-150.0f, -1170.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_02;
            }
            if (IS_AT(0.0f, -1400.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_03;
            }
            if (IS_AT(0.0f, -1600.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_EARLY_POT_04;
            }
            break;
        case SCENE_HAKUGIN:
            if (IS_AT(-1052.0f, 338.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_POT_01;
            }
            if (IS_AT(-1091.0f, 327.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_POT_02;
            }
            if (IS_AT(930.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_01;
            }
            if (IS_AT(810.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_02;
            }
            if (IS_AT(870.0f, 1005.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_03;
            }
            if (IS_AT(870.0f, 1125.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_04;
            }
            if (IS_AT(870.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_POT_05;
            }
            if (IS_AT(795.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_AFTER_POT_01;
            }
            if (IS_AT(825.0f, -75.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_AFTER_POT_02;
            }
            if (IS_AT(-585.0f, -645.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_POT_01;
            }
            if (IS_AT(-645.0f, -585.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_POT_02;
            }
            if (IS_AT(570.0f, -90.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_POT_01;
            }
            if (IS_AT(510.0f, -90.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_LEVEL_2_POT_02;
            }
            if (IS_AT(435.0f, -570.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_KEY_POT_01;
            }
            if (IS_AT(410.0f, -526.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_NEAR_BOSS_KEY_POT_02;
            }
            if (IS_AT(473.0f, 458.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SCARECROW_POT_01;
            }
            if (IS_AT(519.0f, 535.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_SCARECROW_POT_02;
            }
            if (IS_AT(-1543.0f, 703.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_01;
            }
            if (IS_AT(-1507.0f, 655.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_02;
            }
            if (IS_AT(-351.0f, 1153.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_03;
            }
            if (IS_AT(-378.0f, 1169.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_04;
            }
            if (IS_AT(-1525.0f, 679.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_POT_05;
            }
            if (IS_AT(-727.0f, 993.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_POT_01;
            }
            if (IS_AT(-675.0f, 920.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_POT_02;
            }
            if (IS_AT(225.0f, 975.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_ENTRANCE_POT_01;
            }
            if (IS_AT(225.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_ENTRANCE_POT_02;
            }
            if (IS_AT(-452.0f, -995.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_01;
            }
            if (IS_AT(-473.0f, -958.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_02;
            }
            if (IS_AT(-493.0f, -921.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_03;
            }
            if (IS_AT(-420.0f, -930.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_04;
            }
            if (IS_AT(-1275.0f, -600.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_05;
            }
            if (IS_AT(-1305.0f, -623.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_06;
            }
            if (IS_AT(-1335.0f, -645.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_POT_07;
            }
            if (IS_AT(-870.0f, -105.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_01;
            }
            if (IS_AT(-915.0f, -105.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_02;
            }
            if (IS_AT(135.0f, -1050.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_03;
            }
            if (IS_AT(150.0f, -990.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_04;
            }
            if (IS_AT(-1184.0f, -1316.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_05;
            }
            if (IS_AT(-1116.0f, -1374.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_POT_06;
            }
            if (IS_AT(1320.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_01;
            }
            if (IS_AT(1440.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_02;
            }
            if (IS_AT(1380.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_03;
            }
            if (IS_AT(1440.0f, -630.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_04;
            }
            if (IS_AT(1440.0f, -570.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_WIZZROBE_POT_05;
            }
            break;
        case SCENE_LAST_BS:
            if (IS_AT(80.0f, 800.0f)) {
                randoCheckId = RC_MOON_MAJORA_POT_01;
            }
            if (IS_AT(-80.0f, 800.0f)) {
                randoCheckId = RC_MOON_MAJORA_POT_02;
            }
            break;
        case SCENE_LAST_GORON:
            if (IS_AT(-5670.0f, -2280.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_01;
            }
            if (IS_AT(-5610.0f, -2280.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_02;
            }
            if (IS_AT(-2700.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_03;
            }
            if (IS_AT(-2640.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_04;
            }
            if (IS_AT(-2580.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_05;
            }
            if (IS_AT(-7200.0f, -1290.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_06;
            }
            if (IS_AT(-7260.0f, -1290.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_07;
            }
            if (IS_AT(-7200.0f, -1410.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_08;
            }
            if (IS_AT(-7260.0f, -1410.0)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_09;
            }
            if (IS_AT(-4890.0f, -1380.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_10;
            }
            if (IS_AT(-4890.0f, -1320.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_11;
            }
            if (IS_AT(-420.0f, -1425.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_EARLY_POT_01;
            }
            if (IS_AT(-480.0f, -1425.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_EARLY_POT_02;
            }
            if (IS_AT(-420.0f, -1275.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_EARLY_POT_03;
            }
            if (IS_AT(-480.0f, -1275.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_EARLY_POT_04;
            }
            break;
        case SCENE_LAST_LINK:
            if (IS_AT(-270.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_01;
            }
            if (IS_AT(-270.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_02;
            }
            if (IS_AT(-210.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_03;
            }
            if (IS_AT(-210.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_04;
            }
            if (IS_AT(210.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_05;
            }
            if (IS_AT(210.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_06;
            }
            if (IS_AT(270.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_07;
            }
            if (IS_AT(270.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_08;
            }
            break;
        case SCENE_PIRATE:
            if (IS_AT(4020.0f, -1860.0f)) {
                randoCheckId = RC_PIRATE_FORTRESS_INTERIOR_GUARDED_POT_01;
            }
            if (IS_AT(2040.0f, -1380.0f)) {
                randoCheckId = RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_POT_02;
            }
            if (IS_AT(1920.0f, -640.0f)) {
                randoCheckId = RC_PIRATE_FORTRESS_SEWERS_WATERWAY_POT_01;
            }
            break;
        case SCENE_REDEAD:
            if (IS_AT(1620.0f, -960.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_BIG_POE_POT_01;
            }
            if (IS_AT(1620.0f, -1560.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_BIG_POE_POT_02;
            }
            if (IS_AT(2280.0f, -1560.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_BIG_POE_POT_03;
            }
            if (IS_AT(2280.0f, -960.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_BIG_POE_POT_04;
            }
            if (IS_AT(1185.0f, -915.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_01;
            }
            if (IS_AT(1185.0f, -945.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_02;
            }
            if (IS_AT(1185.0f, -975.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_03;
            }
            if (IS_AT(1185.0f, -1005.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_04;
            }
            if (IS_AT(1185.0f, -1035.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_05;
            }
            if (IS_AT(1185.0f, -1065.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_06;
            }
            if (IS_AT(1185.0f, -1095.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_07;
            }
            if (IS_AT(1185.0f, -1125.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_08;
            }
            if (IS_AT(1185.0f, -1155.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_09;
            }
            if (IS_AT(1185.0f, -1185.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_MIDDLE_POT_10;
            }
            break;
        case SCENE_MITURIN:
            if (IS_AT(120.0f, 2100.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_ENTRANCE_POT;
            }
            if (IS_AT(660.0f, -120.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_01;
            }
            if (IS_AT(660.0f, 120.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_02;
            }
            if (IS_AT(630.0f, 510.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_03;
            }
            if (IS_AT(570.0f, 570.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_04;
            }
            if (IS_AT(510.0f, 630.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_05;
            }
            if (IS_AT(510.0f, 510.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_POT_06;
            }
            if (IS_AT(675.0f, 255.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_UPPER_POT_01;
            }
            if (IS_AT(645.0f, 255.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAIN_ROOM_UPPER_POT_02;
            }
            if (IS_AT(-780.0f, -165.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAZE_POT_01;
            }
            if (IS_AT(-780.0f, -105.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_MAZE_POT_02;
            }
            if (IS_AT(-120.0f, -2220.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_PRE_BOSS_POT_01;
            }
            if (IS_AT(120.0f, -2220.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_PRE_BOSS_POT_02;
            }
            if (IS_AT(1335.0f, 585.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_WATER_ROOM_POT_01;
            }
            if (IS_AT(1335.0f, 615.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_WATER_ROOM_POT_02;
            }
            if (IS_AT(1335.0f, 645.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_WATER_ROOM_POT_03;
            }
            if (IS_AT(1335.0f, 675.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_WATER_ROOM_POT_04;
            }
            break;
        case SCENE_KINSTA1:
            if (IS_AT(-30.0f, -1650.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_LOWER_POT_01;
            }
            if (IS_AT(30.0f, -1650.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_LOWER_POT_02;
            }
            if (IS_AT(330.0f, -1651.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_01;
            }
            if (IS_AT(330.0f, -1590.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_02;
            }
            if (IS_AT(330.0f, -1531.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_03;
            }
            if (IS_AT(330.0f, -1470.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_POT_04;
            }
            if (IS_AT(544.0f, -569.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_01;
            }
            if (IS_AT(483.0f, -569.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_02;
            }
            if (IS_AT(575.0f, -538.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_03;
            }
            if (IS_AT(514.0f, -539.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_04;
            }
            if (IS_AT(454.0f, -539.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_05;
            }
            if (IS_AT(451.0f, -1232.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_06;
            }
            if (IS_AT(451.0f, -1290.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_POT_07;
            }
            if (IS_AT(90.0f, -209.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_01;
            }
            if (IS_AT(89.0f, -269.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_02;
            }
            if (IS_AT(-90.0f, -270.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_LOWER_POT_03;
            }
            if (IS_AT(330.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_LEFT_POT_01;
            }
            if (IS_AT(270.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_LEFT_POT_02;
            }
            if (IS_AT(-270.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_RIGHT_POT_01;
            }
            if (IS_AT(-330.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_RIGHT_POT_02;
            }
            if (IS_AT(-871.0f, -872.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_POT_01;
            }
            if (IS_AT(-809.0f, -870.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_POT_02;
            }
            break;
        case SCENE_KINDAN2:
            if (IS_AT(64.0f, -1650.0)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_MAIN_ROOM_POT_01;
            }
            if (IS_AT(2.0f, -1722.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_MAIN_ROOM_WEB_POT;
            }
            if (IS_AT(-569.0f, -1200.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_STORAGE_POT_02;
            }
            if (IS_AT(-630.f, -1200.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_STORAGE_POT_03;
            }
            break;
        case SCENE_SEA_BS:
            if (IS_AT(150.0f, 150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_01;
            }
            if (IS_AT(-150.0f, 150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_02;
            }
            if (IS_AT(-150.0f, -150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_03;
            }
            if (IS_AT(150.0f, -150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_04;
            }
            if (IS_AT(1200.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_01;
            }
            if (IS_AT(1200.0f, 1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_02;
            }
            if (IS_AT(-1200.0f, 1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_03;
            }
            if (IS_AT(-1200.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_UNDERWATER_POT_04;
            }
            break;
        case SCENE_SEA:
            if (IS_AT(-1905.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_03;
            }
            if (IS_AT(-1755.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_06;
            }
            if (IS_AT(-1755.0f, -1680.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_07;
            }
            if (IS_AT(-1755.0f, -1725.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_08;
            }
            if (IS_AT(-1755.0f, -1770.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_09;
            }
            if (IS_AT(-1905.0f, -1680.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_10;
            }
            if (IS_AT(-1905.0f, -1725.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_11;
            }
            if (IS_AT(-1905.0f, -1770.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BEFORE_WART_POT_12;
            }
            if (IS_AT(600.0f, -30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_CENTRAL_ROOM_POT_01;
            }
            if (IS_AT(600.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_CENTRAL_ROOM_POT_02;
            }
            if (IS_AT(1265.0f, -785.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_01;
            }
            if (IS_AT(1235.0f, -815.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_02;
            }
            if (IS_AT(1575.0f, -1155.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_03;
            }
            if (IS_AT(1605.0f, -1125.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_SURFACE_POT_04;
            }
            if (IS_AT(1170.0f, -1078.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_01;
            }
            if (IS_AT(1310.0f, -1078.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_02;
            }
            if (IS_AT(1310.0f, -1218.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_WATER_POT_03;
            }
            if (IS_AT(540.0f, -930.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_01;
            }
            if (IS_AT(-540.0f, -930.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_02;
            }
            if (IS_AT(-540.0f, -2070.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_03;
            }
            if (IS_AT(540.0f, -2070.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_POT_04;
            }
            if (IS_AT(-479.0f, 2141.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_01;
            }
            if (IS_AT(-524.0f, 2141.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_02;
            }
            if (IS_AT(-391.0f, 1959.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_03;
            }
            if (IS_AT(-436.0f, 1959.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_04;
            }
            if (IS_AT(-391.0f, 1810.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_05;
            }
            if (IS_AT(-436.0f, 1810.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_06;
            }
            if (IS_AT(-524.0f, 1628.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_07;
            }
            if (IS_AT(-479.0f, 1628.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_PRE_BOSS_POT_08;
            }
            if (IS_AT(2970.0f, 1020.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_UPPER_POT_01;
            }
            if (IS_AT(2970.0f, 960.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_UPPER_POT_02;
            }
            if (IS_AT(1500.0f, 540.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_01;
            }
            if (IS_AT(1590.0f, 540.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_02;
            }
            if (IS_AT(1712.0f, 1215.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_SURFACE_POT_03;
            }
            if (IS_AT(1500.0f, 1030.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_03;
            }
            if (IS_AT(1590.0f, 1250.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_04;
            }
            if (IS_AT(1635.0f, 1620.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_05;
            }
            if (IS_AT(1470.0f, 1665.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_07;
            }
            if (IS_AT(1455.0f, 1620.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_MAP_ROOM_WATER_POT_08;
            }
            if (IS_AT(2835.0f, -555.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_01;
            }
            if (IS_AT(2835.0f, -495.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_02;
            }
            if (IS_AT(2745.0f, -495.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_03;
            }
            if (IS_AT(2745.0f, -555.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_04;
            }
            if (IS_AT(2730.0f, -975.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_05;
            }
            if (IS_AT(2775.0f, -975.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_06;
            }
            if (IS_AT(3555.0f, -720.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_07;
            }
            if (IS_AT(3555.0f, -675.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_POT_08;
            }
            if (IS_AT(-1530.0f, 60.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_01;
            }
            if (IS_AT(-1560.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_02;
            }
            if (IS_AT(-2130.0f, 60.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_03;
            }
            if (IS_AT(-2100.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_RED_PIPE_BEFORE_WART_POT_04;
            }
            if (IS_AT(-2250.0f, -1890.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_01;
            }
            if (IS_AT(-2280.0f, -1920.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_02;
            }
            if (IS_AT(-2280.0f, -2760.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_03;
            }
            if (IS_AT(-2250.0f, -2790.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_04;
            }
            if (IS_AT(-1410.0f, -2790.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_05;
            }
            if (IS_AT(-1380.0f, -2760.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_06;
            }
            if (IS_AT(-1380.0f, -1920.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_07;
            }
            if (IS_AT(-1410.0f, -1890.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_WART_POT_08;
            }
            break;
        case SCENE_INISIE_N:
            if (IS_AT(-30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_01;
            }
            if (IS_AT(-30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_02;
            }
            if (IS_AT(-30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_03;
            }
            if (IS_AT(-30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_04;
            }
            if (IS_AT(30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_05;
            }
            if (IS_AT(30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_06;
            }
            if (IS_AT(30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_07;
            }
            if (IS_AT(30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_SPIKED_BAR_ROOM_POT_08;
            }
            if (IS_AT(45.0f, -690.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_ENTRANCE_POT_01;
            }
            if (IS_AT(-45.0f, -690.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_ENTRANCE_POT_02;
            }
            if (IS_AT(-1275.0f, -1395.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_01;
            }
            if (IS_AT(-1275.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_02;
            }
            if (IS_AT(-1425.0f, -1395.0)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_03;
            }
            if (IS_AT(-1425.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_POT_04;
            }
            if (IS_AT(-1275.0f, -705.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_01;
            }
            if (IS_AT(-1275.0f, -615.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_02;
            }
            if (IS_AT(-1425.0f, -705.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_03;
            }
            if (IS_AT(-1425.0f, -615.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_LAVA_ROOM_AFTER_BLOCK_POT_04;
            }
            if (IS_AT(1350.0f, -1770.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_MIRROR_ROOM_POT_01;
            }
            if (IS_AT(1230.0f, -1770.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_MIRROR_ROOM_POT_02;
            }
            if (IS_AT(810.0f, -1260.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_01;
            }
            if (IS_AT(750.0f, -1260.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_02;
            }
            if (IS_AT(810.0f, -1320.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_LOWER_POT_03;
            }
            if (IS_AT(1800.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_UPPER_POT_01;
            }
            if (IS_AT(1800.0f, -840.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WATER_ROOM_UNDERWATER_UPPER_POT_02;
            }
            if (IS_AT(615.0f, -3375.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_01;
            }
            if (IS_AT(585.0f, -3375.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_02;
            }
            if (IS_AT(615.0f, -3465.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_03;
            }
            if (IS_AT(585.0f, -3465.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_WIND_ROOM_POT_04;
            }
            break;
        case SCENE_INISIE_R:
            if (IS_AT(2070.0f, -1620.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_01;
            }
            if (IS_AT(2790.0f, -1620.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_02;
            }
            if (IS_AT(2790.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_03;
            }
            if (IS_AT(2070.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_GOMESS_POT_04;
            }
            if (IS_AT(-735.0f, -1215.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POE_MAZE_SIDE_POT_01;
            }
            if (IS_AT(-735.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POE_MAZE_SIDE_POT_02;
            }
            if (IS_AT(-1605.0f, -1395.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POE_WIZZROBE_SIDE_POT_01;
            }
            if (IS_AT(-1605.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POE_WIZZROBE_SIDE_POT_02;
            }
            if (IS_AT(-30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_01;
            }
            if (IS_AT(-30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_02;
            }
            if (IS_AT(-30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_03;
            }
            if (IS_AT(-30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_04;
            }
            if (IS_AT(30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_05;
            }
            if (IS_AT(30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_06;
            }
            if (IS_AT(30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_07;
            }
            if (IS_AT(30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_PRE_BOSS_POT_08;
            }
            if (IS_AT(1350.0f, -630.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_BRIDGE_POT_01;
            }
            if (IS_AT(1230.0f, -630.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_BRIDGE_POT_02;
            }
            if (IS_AT(735.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_UPDRAFTS_LEDGE_POT_03;
            }
            if (IS_AT(-1290.0f, -510.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_01;
            }
            if (IS_AT(-1290.0f, -450.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_02;
            }
            if (IS_AT(-1410.0f, -450.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_03;
            }
            if (IS_AT(-1410.0f, -510.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_POT_04;
            }
            break;
        default:
            break;
    }
    return randoCheckId;
}

void ObjTsubo_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gPotStandardDL);
        return;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[OBJTSUBO_RC].randoItemId, (RandoCheckId)OBJTSUBO_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotBossKeyDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotSmallKeyDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gPotHeartDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMinorDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMajorDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gPotMaskDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gPotTokenDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gPotFairyDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gPotStandardDL);
            break;
    }
}

void Rando::ActorBehavior::InitObjTsuboBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TSUBO, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = IdentifyPot(actor);
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            return;
        }

        OBJTSUBO_RC = randoCheckId;
    });

    COND_VB_SHOULD(VB_POT_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        if (OBJTSUBO_RC != RC_UNKNOWN) {
            *should = false;
            actor->draw = ObjTsubo_RandoDraw;
        }
    });

    COND_VB_SHOULD(VB_POT_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = (RandoCheckId)OBJTSUBO_RC;
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG, randoCheckId,
            [](Actor* actor, PlayState* play) {
                RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                randoSaveCheck.eligible = true;
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM), actor);
            });
        *should = false;

        // Clear the stored Check ID for pots that are on a timed respawn
        OBJTSUBO_RC = RC_UNKNOWN;
        actor->draw = ObjTsubo_Draw;
    });
}
