#include "StaticData.h"

namespace Rando {

namespace StaticData {

#define RI(id, article, name, itemId, getItemId, drawId)      \
    {                                                         \
        id, {                                                 \
            id, #id, article, name, itemId, getItemId, drawId \
        }                                                     \
    }

// clang-format off
std::map<RandoItemId, RandoStaticItem> Items = {
    RI(RI_UNKNOWN,                    "",     "Unknown",                    ITEM_NONE,                       GI_NONE,                     GID_NONE),
    RI(RI_ARROW_FIRE,                 "",     "Fire Arrows",                ITEM_ARROW_FIRE,                 GI_ARROW_FIRE,               GID_ARROW_FIRE),
    RI(RI_ARROW_ICE,                  "",     "Ice Arrows",                 ITEM_ARROW_ICE,                  GI_ARROW_ICE,                GID_ARROW_ICE),
    RI(RI_ARROW_LIGHT,                "",     "Light Arrows",               ITEM_ARROW_LIGHT,                GI_ARROW_LIGHT,              GID_ARROW_LIGHT),
    RI(RI_ARROWS_10,                  "",     "10 Arrows",                  ITEM_ARROWS_10,                  GI_ARROWS_10,                GID_ARROWS_SMALL),
    RI(RI_ARROWS_30,                  "",     "30 Arrows",                  ITEM_ARROWS_30,                  GI_ARROWS_30,                GID_ARROWS_MEDIUM),
    RI(RI_ARROWS_50,                  "",     "50 Arrows",                  ITEM_ARROWS_50,                  GI_ARROWS_50,                GID_ARROWS_LARGE),
    RI(RI_BLUE_POTION_REFILL,         "a",    "Blue Potion Refill",         ITEM_POTION_BLUE,                GI_POTION_BLUE,              GID_POTION_BLUE),
    RI(RI_BOMB_BAG_20,                "a",    "Small Bomb Bag",             ITEM_BOMB_BAG_20,                GI_BOMB_BAG_20,              GID_BOMB_BAG_20),
    RI(RI_BOMB_BAG_30,                "a",    "Bigger Bomb Bag",            ITEM_BOMB_BAG_30,                GI_BOMB_BAG_30,              GID_BOMB_BAG_30),
    RI(RI_BOMB_BAG_40,                "the",  "Biggest Bomb Bag",           ITEM_BOMB_BAG_40,                GI_BOMB_BAG_40,              GID_BOMB_BAG_40),
    RI(RI_BOMBCHU_10,                 "",     "10 Bombchus",                ITEM_BOMBCHUS_10,                GI_BOMBCHUS_10,              GID_BOMBCHU),
    RI(RI_BOMBCHU_5,                  "",     "5 Bombchus",                 ITEM_BOMBCHUS_5,                 GI_BOMBCHUS_5,               GID_BOMBCHU),
    RI(RI_BOMBCHU,                    "a",    "Bombchu",                    ITEM_BOMBCHUS_1,                 GI_BOMBCHUS_1,               GID_BOMBCHU), // not sure about this
    RI(RI_BOMBS_10,                   "",     "10 Bombs",                   ITEM_BOMBS_10,                   GI_BOMBS_10,                 GID_BOMB),
    RI(RI_BOMBS_5,                    "",     "5 Bombs",                    ITEM_BOMBS_5,                    GI_BOMBS_5,                  GID_BOMB),
    RI(RI_BOTTLE_EMPTY,               "an",   "Empty Bottle",               ITEM_BOTTLE,                     GI_BOTTLE,                   GID_BOTTLE),
    RI(RI_BOW,                        "a",    "Bow",                        ITEM_BOW,                        GI_QUIVER_30,                GID_BOW),
    RI(RI_CLOCK_TOWN_STRAY_FAIRY,     "a",    "Clock Town Stray Fairy",     ITEM_STRAY_FAIRIES,              GI_STRAY_FAIRY,              GID_NONE),
    RI(RI_DEED_LAND,                  "the",  "Land Title Deed",            ITEM_DEED_LAND,                  GI_DEED_LAND,                GID_DEED_LAND),
    RI(RI_DEED_MOUNTAIN,              "the",  "Mountain Title Deed",        ITEM_DEED_MOUNTAIN,              GI_DEED_MOUNTAIN,            GID_DEED_MOUNTAIN),
    RI(RI_DEED_OCEAN,                 "the",  "Ocean Title Deed",           ITEM_DEED_OCEAN,                 GI_DEED_OCEAN,               GID_DEED_OCEAN),
    RI(RI_DEED_SWAMP,                 "the",  "Swamp Title Deed",           ITEM_DEED_SWAMP,                 GI_DEED_SWAMP,               GID_DEED_SWAMP),
    RI(RI_DEKU_NUT,                   "a",    "Deku Nut",                   ITEM_DEKU_NUT,                   GI_DEKU_NUTS_1,              GID_DEKU_NUTS),
    RI(RI_DEKU_NUTS_10,               "",     "10 Deku Nuts",               ITEM_DEKU_NUTS_10,               GI_DEKU_NUTS_10,             GID_DEKU_NUTS),
    RI(RI_DEKU_NUTS_5,                "",     "5 Deku Nuts",                ITEM_DEKU_NUTS_5,                GI_DEKU_NUTS_5,              GID_DEKU_NUTS),
    RI(RI_DEKU_STICK,                 "a",    "Deku Stick",                 ITEM_DEKU_STICK,                 GI_DEKU_STICKS_1,            GID_DEKU_STICK),
    RI(RI_DOUBLE_DEFENSE,             "",     "Double Defense",             ITEM_NONE,                       GI_NONE,                     GID_HEART_CONTAINER),
    RI(RI_DOUBLE_MAGIC,               "a",    "Magic Upgrade",              ITEM_NONE,                       GI_NONE,                     GID_MAGIC_JAR_BIG),
    RI(RI_FAIRY_REFILL,               "a",    "Fairy",                      ITEM_FAIRY,                      GI_FAIRY,                    GID_FAIRY),
    RI(RI_GREAT_BAY_BOSS_KEY,         "the",  "Great Bay Boss Key",         ITEM_KEY_BOSS,                   GI_KEY_BOSS,                 GID_KEY_BOSS),
    RI(RI_GREAT_BAY_COMPASS,          "the",  "Great Bay Compass",          ITEM_COMPASS,                    GI_COMPASS,                  GID_COMPASS),
    RI(RI_GREAT_BAY_MAP,              "the",  "Great Bay Map",              ITEM_DUNGEON_MAP,                GI_MAP,                      GID_DUNGEON_MAP),
    RI(RI_GREAT_BAY_SMALL_KEY,        "a",    "Great Bay Small Key",        ITEM_KEY_SMALL,                  GI_KEY_SMALL,                GID_KEY_SMALL),
    RI(RI_GREAT_BAY_STRAY_FAIRY,      "a",    "Great Bay Stray Fairy",      ITEM_STRAY_FAIRIES,              GI_STRAY_FAIRY,              GID_NONE),
    RI(RI_GREAT_FAIRY_SWORD,          "the",  "Great Fairy's Sword",        ITEM_SWORD_GREAT_FAIRY,          GI_SWORD_GREAT_FAIRY,        GID_SWORD_GREAT_FAIRY),
    RI(RI_GREAT_SPIN_ATTACK,          "the",  "Great Spin Attack",          ITEM_NONE,                       GI_NONE,                     GID_SWORD_KOKIRI),
    RI(RI_GREEN_POTION_REFILL,        "a",    "Green Potion Refill",        ITEM_POTION_GREEN,               GI_POTION_GREEN,             GID_POTION_GREEN),
    RI(RI_GS_TOKEN_OCEAN,             "an",   "Ocean Gold Skulltula Token", ITEM_SKULL_TOKEN,                GI_SKULL_TOKEN,              GID_SKULL_TOKEN_2),
    RI(RI_GS_TOKEN_SWAMP,             "a",    "Swamp Gold Skulltula Token", ITEM_SKULL_TOKEN,                GI_SKULL_TOKEN,              GID_SKULL_TOKEN_2),
    RI(RI_HEART_CONTAINER,            "a",    "Heart Container",            ITEM_HEART_CONTAINER,            GI_HEART_CONTAINER,          GID_HEART_CONTAINER),
    RI(RI_HEART_PIECE,                "a",    "Heart Piece",                ITEM_HEART_PIECE,                GI_HEART_PIECE,              GID_HEART_PIECE),
    RI(RI_HOOKSHOT,                   "the",  "Hookshot",                   ITEM_HOOKSHOT,                   GI_HOOKSHOT,                 GID_HOOKSHOT),
    RI(RI_LENS,                       "the",  "Lens of Truth",              ITEM_LENS_OF_TRUTH,              GI_LENS_OF_TRUTH,            GID_LENS),
    RI(RI_LETTER_TO_KAFEI,            "the",  "Letter to Kafei",            ITEM_LETTER_TO_KAFEI,            GI_LETTER_TO_KAFEI,          GID_LETTER_TO_KAFEI),
    RI(RI_LETTER_TO_MAMA,             "the",  "Letter to Mama",             ITEM_LETTER_MAMA,                GI_LETTER_TO_MAMA,           GID_LETTER_MAMA),
    RI(RI_MAGIC_BEAN,                 "a",    "Magic Bean",                 ITEM_MAGIC_BEANS,                GI_MAGIC_BEANS,              GID_MAGIC_BEANS),
    RI(RI_MAGIC_JAR_BIG,              "a",    "Large Magic Refill",         ITEM_MAGIC_JAR_BIG,              GI_MAGIC_JAR_BIG,            GID_MAGIC_JAR_BIG),
    RI(RI_MAGIC_JAR_SMALL,            "a",    "Small Magic Refill",         ITEM_MAGIC_JAR_SMALL,            GI_MAGIC_JAR_SMALL,          GID_MAGIC_JAR_SMALL),
    RI(RI_MASK_BLAST,                 "the",  "Blast Mask",                 ITEM_MASK_BLAST,                 GI_MASK_BLAST,               GID_MASK_BLAST),
    RI(RI_MASK_BREMEN,                "the",  "Bremen Mask",                ITEM_MASK_BREMEN,                GI_MASK_BREMEN,              GID_MASK_BREMEN),
    RI(RI_MASK_BUNNY,                 "the",  "Bunny Hood",                 ITEM_MASK_BUNNY,                 GI_MASK_BUNNY,               GID_MASK_BUNNY),
    RI(RI_MASK_CAPTAIN,               "the",  "Captain's Hat",              ITEM_MASK_CAPTAIN,               GI_MASK_CAPTAIN,             GID_MASK_CAPTAIN),
    RI(RI_MASK_DEKU,                  "the",  "Deku Mask",                  ITEM_MASK_DEKU,                  GI_MASK_DEKU,                GID_MASK_DEKU),
    RI(RI_MASK_GIANT,                 "the",  "Giant's Mask",               ITEM_MASK_GIANT,                 GI_MASK_GIANT,               GID_MASK_GIANT),
    RI(RI_MASK_GORON,                 "the",  "Goron Mask",                 ITEM_MASK_GORON,                 GI_MASK_GORON,               GID_MASK_GORON),
    RI(RI_MASK_GREAT_FAIRY,           "the",  "Great Fairy Mask",           ITEM_MASK_GREAT_FAIRY,           GI_MASK_GREAT_FAIRY,         GID_MASK_GREAT_FAIRY),
    RI(RI_MASK_KAFEIS_MASK,           "",     "Kafei's Mask",               ITEM_MASK_KAFEIS_MASK,           GI_MASK_KAFEIS_MASK,         GID_MASK_KAFEIS_MASK),
    RI(RI_MASK_KAMARO,                "",     "Kamaro's Mask",              ITEM_MASK_KAMARO,                GI_MASK_KAMARO,              GID_MASK_KAMARO),
    RI(RI_MASK_ROMANI,                "",     "Romani's Mask",              ITEM_MASK_ROMANI,                GI_MASK_ROMANI,              GID_MASK_ROMANI),
    RI(RI_MASK_STONE,                 "the",  "Stone Mask",                 ITEM_MASK_STONE,                 GI_MASK_STONE,               GID_MASK_STONE),
    RI(RI_MASK_ZORA,                  "the",  "Zora Mask",                  ITEM_MASK_ZORA,                  GI_MASK_ZORA,                GID_MASK_ZORA),
    RI(RI_MILK_REFILL,                "a",    "Milk Refill",                ITEM_MILK,                       GI_MILK,                     GID_MILK),
    RI(RI_MOONS_TEAR,                 "the",  "Moon's Tear",                ITEM_MOONS_TEAR,                 GI_MOONS_TEAR,               GID_MOONS_TEAR),
    RI(RI_NONE,                       "",     "literally nothing",          ITEM_NONE,                       GI_NONE,                     GID_NONE),
    RI(RI_OCARINA,                    "the",  "Ocarina of Time",            ITEM_OCARINA_OF_TIME,            GI_OCARINA_OF_TIME,          GID_OCARINA),
    RI(RI_PENDANT_OF_MEMORIES,        "the",  "Pendant of Memories",        ITEM_PENDANT_OF_MEMORIES,        GI_PENDANT_OF_MEMORIES,      GID_PENDANT_OF_MEMORIES),
    RI(RI_POWDER_KEG,                 "a",    "Powder Keg",                 ITEM_POWDER_KEG,                 GI_POWDER_KEG,               GID_POWDER_KEG),
    RI(RI_PROGRESSIVE_BOMB_BAG,       "a",    "Progressive Bomb Bag",       ITEM_BOMB_BAG_20,                GI_BOMB_BAG_20,              GID_BOMB_BAG_20),
    RI(RI_PROGRESSIVE_BOW,            "a",    "Progressive Bow",            ITEM_BOW,                        GI_QUIVER_30,                GID_BOW),
    RI(RI_PROGRESSIVE_MAGIC,          "",     "Progressive Magic",          ITEM_NONE,                       GI_NONE,                     GID_MAGIC_JAR_SMALL),
    RI(RI_QUIVER_40,                  "a",    "Medium Quiver",              ITEM_QUIVER_40,                  GI_QUIVER_40,                GID_QUIVER_40),
    RI(RI_QUIVER_50,                  "a",    "Large Quiver",               ITEM_QUIVER_50,                  GI_QUIVER_50,                GID_QUIVER_50),
    RI(RI_RECOVERY_HEART,             "a",    "Recovery Heart",             ITEM_RECOVERY_HEART,             GI_RECOVERY_HEART,           GID_RECOVERY_HEART),
    RI(RI_RED_POTION_REFILL,          "a",    "Red Potion Refill",          ITEM_POTION_RED,                 GI_POTION_RED,               GID_POTION_RED),
    RI(RI_REMAINS_GOHT,               "",     "Goht's Remains",             ITEM_REMAINS_GOHT,               GI_REMAINS_GOHT,             GID_REMAINS_GOHT),
    RI(RI_REMAINS_GYORG,              "",     "Gyorg's Remains",            ITEM_REMAINS_GYORG,              GI_REMAINS_GYORG,            GID_REMAINS_GYORG),
    RI(RI_REMAINS_ODOLWA,             "",     "Odolwa's Remains",           ITEM_REMAINS_ODOLWA,             GI_REMAINS_ODOLWA,           GID_REMAINS_ODOLWA),
    RI(RI_REMAINS_TWINMOLD,           "",     "Twinmold's Remains",         ITEM_REMAINS_TWINMOLD,           GI_REMAINS_TWINMOLD,         GID_REMAINS_TWINMOLD),
    RI(RI_ROOM_KEY,                   "the",  "Room Key",                   ITEM_ROOM_KEY,                   GI_ROOM_KEY,                 GID_ROOM_KEY),
    RI(RI_RUPEE_BLUE,                 "a",    "Blue Rupee",                 ITEM_RUPEE_BLUE,                 GI_RUPEE_BLUE,               GID_RUPEE_BLUE),
    RI(RI_RUPEE_GREEN,                "a",    "Green Rupee",                ITEM_RUPEE_GREEN,                GI_RUPEE_GREEN,              GID_RUPEE_GREEN),
    RI(RI_RUPEE_HUGE,                 "a",    "Huge Rupee",                 ITEM_RUPEE_HUGE,                 GI_RUPEE_HUGE,               GID_RUPEE_HUGE),
    RI(RI_RUPEE_PURPLE,               "a",    "Purple Rupee",               ITEM_RUPEE_PURPLE,               GI_RUPEE_PURPLE,             GID_RUPEE_PURPLE),
    RI(RI_RUPEE_RED,                  "a",    "Red Rupee",                  ITEM_RUPEE_RED,                  GI_RUPEE_RED,                GID_RUPEE_RED),
    RI(RI_RUPEE_SILVER,               "a",    "Silver Rupee",               ITEM_RUPEE_SILVER,               GI_RUPEE_SILVER,             GID_RUPEE_SILVER),
    RI(RI_SHIELD_HERO,                "the",  "Hero's Shield",              ITEM_SHIELD_HERO,                GI_SHIELD_HERO,              GID_SHIELD_HERO),
    RI(RI_SHIELD_MIRROR,              "the",  "Mirror Shield",              ITEM_SHIELD_MIRROR,              GI_SHIELD_MIRROR,            GID_SHIELD_MIRROR),
    RI(RI_SINGLE_MAGIC,               "the",  "Power of Magic",             ITEM_NONE,                       GI_NONE,                     GID_MAGIC_JAR_SMALL),
    RI(RI_SNOWHEAD_BOSS_KEY,          "the",  "Snowhead Boss Key",          ITEM_KEY_BOSS,                   GI_KEY_BOSS,                 GID_KEY_BOSS),
    RI(RI_SNOWHEAD_COMPASS,           "the",  "Snowhead Compass",           ITEM_COMPASS,                    GI_COMPASS,                  GID_COMPASS),
    RI(RI_SNOWHEAD_MAP,               "the",  "Snowhead Map",               ITEM_DUNGEON_MAP,                GI_MAP,                      GID_DUNGEON_MAP),
    RI(RI_SNOWHEAD_SMALL_KEY,         "a",    "Snowhead Small Key",         ITEM_KEY_SMALL,                  GI_KEY_SMALL,                GID_KEY_SMALL),
    RI(RI_SNOWHEAD_STRAY_FAIRY,       "a",    "Snowhead Stray Fairy",       ITEM_STRAY_FAIRIES,              GI_STRAY_FAIRY,              GID_NONE),
    RI(RI_SONG_ELEGY,                 "the",  "Elegy of Emptiness",         ITEM_SONG_ELEGY,                 GI_NONE,                     GID_NONE),
    RI(RI_SONG_HEALING,               "the",  "Song of Healing",            ITEM_SONG_HEALING,               GI_NONE,                     GID_NONE),
    RI(RI_SONG_OATH,                  "the",  "Oath to Order",              ITEM_SONG_OATH,                  GI_NONE,                     GID_NONE),
    RI(RI_SONG_SONATA,                "the",  "Sonata of Awakening",        ITEM_SONG_SONATA,                GI_NONE,                     GID_NONE),
    RI(RI_SONG_SUN,                   "the",  "Sun's Song",                 ITEM_SONG_SUN,                   GI_NONE,                     GID_NONE),
    RI(RI_SONG_TIME,                  "the",  "Song of Time",               ITEM_SONG_TIME,                  GI_NONE,                     GID_NONE),
    RI(RI_STONE_TOWER_BOSS_KEY,       "the",  "Stone Tower Boss Key",       ITEM_KEY_BOSS,                   GI_KEY_BOSS,                 GID_KEY_BOSS),
    RI(RI_STONE_TOWER_COMPASS,        "the",  "Stone Tower Compass",        ITEM_COMPASS,                    GI_COMPASS,                  GID_COMPASS),
    RI(RI_STONE_TOWER_MAP,            "the",  "Stone Tower Map",            ITEM_DUNGEON_MAP,                GI_MAP,                      GID_DUNGEON_MAP),
    RI(RI_STONE_TOWER_SMALL_KEY,      "a",    "Stone Tower Small Key",      ITEM_KEY_SMALL,                  GI_KEY_SMALL,                GID_KEY_SMALL),
    RI(RI_STONE_TOWER_STRAY_FAIRY,    "a",    "Stone Tower Stray Fairy",    ITEM_STRAY_FAIRIES,              GI_STRAY_FAIRY,              GID_NONE),
    RI(RI_WOODFALL_BOSS_KEY,          "the",  "Woodfall Boss Key",          ITEM_KEY_BOSS,                   GI_KEY_BOSS,                 GID_KEY_BOSS),
    RI(RI_WOODFALL_COMPASS,           "the",  "Woodfall Compass",           ITEM_COMPASS,                    GI_COMPASS,                  GID_COMPASS),
    RI(RI_WOODFALL_MAP,               "the",  "Woodfall Map",               ITEM_DUNGEON_MAP,                GI_MAP,                      GID_DUNGEON_MAP),
    RI(RI_WOODFALL_SMALL_KEY,         "a",    "Woodfall Small Key",         ITEM_KEY_SMALL,                  GI_KEY_SMALL,                GID_KEY_SMALL),
    RI(RI_WOODFALL_STRAY_FAIRY,       "a",    "Woodfall Stray Fairy",       ITEM_STRAY_FAIRIES,              GI_STRAY_FAIRY,              GID_NONE),
};

RandoItemId GetItemIdFromName(const char* name) {
    for (auto& [randoItemId, randoStaticItem] : Items) {
        if (strcmp(name, randoStaticItem.spoilerName) == 0) {
            return randoItemId;
        }
    }
    return RI_UNKNOWN;
}

// clang-format on

} // namespace StaticData

} // namespace Rando
