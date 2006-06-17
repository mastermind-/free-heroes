/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#ifndef _MP2MAPS_H
#define _MP2MAPS_H

#include "SDL.h"
#include "agg.h"
#include "object.h"
#include "resource.h"
#include "artifact.h"
#include "heroes.h"
#include "actionevent.h"


/* ************** START MP2 **************** 
*/
/* заголовок карты */
typedef struct {
Uint32  headerMagic;		// address: 0x0000
Uint16  difficultLevel;		// address: 0x0004
Uint8   widthMaps;              // address: 0x0006
Uint8	heightMaps;             // address: 0x0007
Uint8	t1[18];			// address: 0x0008 - 0x0019  ??
Uint8	countHeroes;            // address: 0x001A
Uint8	requiredHumans;		// address: 0x001B
Uint8	maxHumans;		// address: 0x001C
Uint8   conditionsWins;         // address: 0x001D
Uint8	specialWins1;		// address: 0x001E
Uint8	specialWins2;		// address: 0x001F
Uint8	specialWins3;		// address: 0x0020
Uint8	specialWins4;		// address: 0x0021
Uint8   conditionsLoss;         // address: 0x0022
Uint8	specialLoss1;		// address: 0x0023
Uint8	specialLoss2;		// address: 0x0024
Uint8	startWithHeroCastle;	// address: 0x0025
Uint8	t2[20];			// address: 0x0026 - 0x0039  ??
Uint8   longname[16];           // address: 0x003A - 0x0049
Uint8	t3[44];			// address: 0x004A - 0x0075  ??
Uint8   description[143];       // address: 0x0076 - 0x0105
Uint8	uniqName1[159];		// address: 0x0106 - 0x01A3  ??
Uint32	mapsWidth;		// address: 0x01A4
Uint32	mapsHeight;		// address: 0x01A8
} MP2HEADER;			// total:   428 байт

// далее
// mapsWidth * mapsHeight основных ячеек карты
//
/* основная информация о клетке карты */
typedef struct {
Uint16	tileIndex;		// индекс tile (ocean, grass, snow, swamp, lava, desert, dirt, wasteland, beach)
Uint8	objectName1;		// объект 1 уровня (список далее)
Uint8	indexName1;		// 0xFF или индекс спрайта
Uint8	quantity1;		// количество
Uint8	quantity2;		// количество
Uint8	objectName2;		// объект 2 уровня
Uint8	indexName2;		// 0xFF или индекс спрайта
Uint8	shape;			// развернуть спрайт tiles, shape % 4, 0 без изменений, 1 по вертикали, 2 по горизонтали, 3 оба варианта
Uint8	generalObject;		// основной тип объекта на данной клетке или 0
Uint16	indexAddon;		// индекс следующей неотрисованной части объекта иначе 0
Uint32	uniqNumber1;		// уникальный номер целого объекта первого уровня
Uint32	uniqNumber2; 	        // уникальный номер целого объекта второго уровня
} MP2TILEINFO;			// total: 20 байт

// далее
// Uint32	countData количество 15 байтных блоков MP2ADDONTAIL
//		(постоянный инкремент после сохранения, начальное состояние 0x96)

/* дополнительная информация о клетке карты */
typedef struct {
    Uint16	indexAddon;		// индекс следующей неотрисованной части объекта иначе 0
    Uint8	objectNameN1;		// объект следующего 1 уровня
    Uint8	indexNameN1;		// 0xFF или индекс спрайта
    Uint8	quantityN;		// используется для порядка отрисовки % 4
    Uint8	objectNameN2;		// объект следующего 2 уровня
    Uint8	indexNameN2;		// 0xFF или индекс спрайта
    Uint32	uniqNumberN1;		// уникальный номер целого объекта следующего первого уровня
    Uint32	uniqNumberN2;		// уникальный номер целого объекта следующего второго уровня
} MP2ADDONTAIL;

// далее
// блок FF FF FF нахрен нужен непонятно
// дополнительная информация о замках по 0x48 байт
// дополнительная информация о героях по 0x4E байт

// count;
// count;
// count;

typedef struct {

    Uint8	identify;	// 0x46
    Uint8	zero;		// 0x00
    Uint8	color; 		// 00 blue, 01 green, 02 red, 03 yellow, 04 orange, 05 purpl, ff unknown
    BOOL	customBuilding;
    Uint16	building;
		    /*
			0000 0000 0000 0010	Thieve's Guild
			0000 0000 0000 0100	Tavern
			0000 0000 0000 1000	Shipyard
			0000 0000 0001 0000	Well
			0000 0000 1000 0000	Statue
			0000 0001 0000 0000	Left Turret
			0000 0010 0000 0000	Right Turret
			0000 0100 0000 0000	Marketplace
			0000 1000 0000 0000	Farm, Garbage He, Crystal Gar, Waterfall, Orchard, Skull Pile
			0001 0000 0000 0000	Moat
			0010 0000 0000 0000	Fortification, Coliseum, Rainbow, Dungeon, Library, Storm
		    */
    Uint16	dwelling;
		    /*
			0000 0000 0000 1000	dweling1
			0000 0000 0001 0000	dweling2
			0000 0000 0010 0000	dweling3
			0000 0000 0100 0000	dweling4
			0000 0000 1000 0000	dweling5
			0000 0001 0000 0000	dweling6
			0000 0010 0000 0000	upgDweling2
			0000 0100 0000 0000	upgDweling3
			0000 1000 0000 0000	upgDweling4
			0001 0000 0000 0000	upgDweling5
			0010 0000 0000 0000	upgDweling6
		    */
    Uint8	magicTower;
    BOOL	customTroops;
    Uint8	monster1;
    Uint8	monster2;
    Uint8	monster3;
    Uint8	monster4;
    Uint8	monster5;
    Uint16	countMonter1;
    Uint16	countMonter2;
    Uint16	countMonter3;
    Uint16	countMonter4;
    Uint16	countMonter5;
    BOOL	capitan;
    BOOL	customCastleName;
    char	castleName[13];		// name + '\0' // 40 byte
    Uint8	type;			// 00 knight, 01 barb, 02 sorc, 03 warl, 04 wiz, 05 necr, 06 rnd
    BOOL	castle;
    Uint8	allowCastle;		// 00 TRUE, 01 FALSE
    Uint8	xxc[29];

} MP2CASTLE;

/*
typedef struct {

    Uint8	identify;	// 0x4c
    Uint8	00;
    Uint8	00;
    BOOL	customTroops;
    Uint8	monster1;	// 0xff none
    Uint8	monster2;	// 0xff none
    Uint8	monster3;	// 0xff none
    Uint8	monster4;	// 0xff none
    Uint8	monster5;	// 0xff none
    Uint16	countMonter1;
    Uint16	countMonter2;
    Uint16	countMonter3;
    Uint16	countMonter4;
    Uint16	countMonter5;
    Uint8	customPortrate;
    Uint8	portrate;
    Uint8	artifact1;	// 0xff none
    Uint8	artifact2;	// 0xff none
    Uint8	artifact3;	// 0xff none
    Uint8	00;
    Uint32	exerience;
    BOOL	customSkill;
    Uint8	skill1;		// 0xff none
    Uint8	skill2;		// pathfinding, arcgery, logistic, scouting, 
    Uint8	skill3;		// diplomacy, navigation, leadership, wisdom,
    Uint8	skill4;		// mysticism, luck, ballistics, eagle, necromance, estate
    Uint8	skill5;
    Uint8	skill6;
    Uint8	skill7;
    Uint8	skill8;
    Uint8	skillLevel1;
    Uint8	skillLevel2;
    Uint8	skillLevel3;
    Uint8	skillLevel4;
    Uint8	skillLevel5;
    Uint8	skillLevel6;
    Uint8	skillLevel7;
    Uint8	skillLevel8;
    Uint8	00;
    Uint8	customName;
    char	name[13];	// name + '\0'
    BOOL	patrol;
    Uint8	countSquare;	// for patrol
    000000000000 total size 0x4e
} MPHEROES;
*/

// дополнительная информация о карте
// тексты сообщений запрограммированных событий

//Uint32	endCount;


/* ************** END MP2 **************** */

typedef enum {
                DESERT,
                SNOW,
                SWAMP,
                WASTELAND,
                BEACH,
                LAVA,
                DIRT,
                GRASS,
                WATER,
                ROAD

            } E_GROUND;
                                                                                                                                                                            
                                                                                                                                                                            

typedef struct {
                E_GROUND        ground;
                SDL_Surface     *tile;
                BOOL            move;
                E_OBJECT        type;
		Uint16		count;

		union {
		    E_MONSTER	monster;
		    E_RESOURCE  resource;
		    E_ARTIFACT  artifact;
		} object;

		ICNHEADER	*level1;
		ICNHEADER	*level2;
                S_HEROES        *heroes;

                } S_CELLMAPS;

void		FreeMaps(void);
ACTION		InitMaps(char *);
S_CELLMAPS     *GetCELLMAPS(Uint16);
Uint8		GetWidthMaps(void);
Uint8		GetHeightMaps(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// список mines
#define OBJ_MINE_ORE		0x00
#define OBJ_MINE_SULFUR		0x01
#define OBJ_MINE_CRYSTAL	0x02
#define OBJ_MINE_GEMS		0x03
#define OBJ_MINE_GOLD		0x04
/*
// список artifact

#define OBJ_ARTIFACT_ARCANE_NECKLACE			0x11		// The Arcane Necklace of Magic increases your spell power by 4.
#define OBJ_ARTIFACT_CASTER_BRACELET			0x13		// The Caster's Bracelet of Magic increases your spell power by 2.
#define OBJ_ARTIFACT_MAGE_RING				0x15		// The Mage's Ring of Power increases your spell power by 2.
#define OBJ_ARTIFACT_WITCHES_BROACH			0x17		// The Witch's Broach of Magic increases your spell power by 3.
#define OBJ_ARTIFACT_MEDAL_VALOR			0x19		// The Medal of Valor increases your morale.
#define OBJ_ARTIFACT_MEDAL_COURAGE			0x1B		// The Medal of Courage increases your morale.
#define OBJ_ARTIFACT_MEDAL_HONOR			0x1D		// The Medal of Honor increases your morale.
#define OBJ_ARTIFACT_MEDAL_DISTINCTION			0x1F		// The Medal of Distinction increases your morale.
#define OBJ_ARTIFACT_FIZBIN_MISFORTUNE			0x21		// The Fizbin of Misfortune greatly decreases your morale.
#define OBJ_ARTIFACT_THUNDER_MACE			0x23		// The Thunder Mace of Dominion increases your attack skill by 1.
#define OBJ_ARTIFACT_ARMORED_GAUNTLETS			0x25		// The Armored Gauntlets of Protection increase your defense skill by 1.
#define OBJ_ARTIFACT_DEFENDER_HELM			0x27		// The Defender Helm of Protection increases your defense skill by 1.
#define OBJ_ARTIFACT_GIANT_FLAIL			0x29		// The Giant Flail of Dominion increases your attack skill by 1.
#define OBJ_ARTIFACT_BALLISTA				0x2B		// The Ballista of Quickness lets your catapult fire twice per combat round.
#define OBJ_ARTIFACT_STEALTH_SHIELD			0x2D		// The Stealth Shield of Protection increases your defense skill by 2.
#define OBJ_ARTIFACT_DRAGON_SWORD			0x2F		// The Dragon Sword of Dominion increases your attack skill by 3.
#define OBJ_ARTIFACT_POWER_AXE				0x31		// The Power Axe of Dominion increases your attack skill by 2.
#define OBJ_ARTIFACT_DIVINE_BREASTPLATE			0x33		// The Divine Breastplate of Protection increases your defense skill by 3.
#define OBJ_ARTIFACT_MINOR_SCROLL			0x35		// The Minor Scroll of Knowledge increases your knowledge by 2.
#define OBJ_ARTIFACT_MAJOR_SCROLL			0x37		// The Major Scroll of Knowledge increases your knowledge by 3.
#define OBJ_ARTIFACT_SUPERIOR_SCROLL			0x39		// The Superior Scroll of Knowledge increases your knowledge by 4.
#define OBJ_ARTIFACT_FOREMOST_SCROLL			0x3B		// The Foremost Scroll of Knowledge increases your knowledge by 5.
#define OBJ_ARTIFACT_ENDLESS_SACK_GOLD			0x3D		// The Endless Sack of Gold provides you with 1000 gold per day.
#define OBJ_ARTIFACT_ENDLESS_BAG_GOLD			0x3F		// The Endless Bag of Gold provides you with 750 gold per day.
#define OBJ_ARTIFACT_ENDLESS_PURSE_GOLD			0x41		// The Endless Purse of Gold provides you with 500 gold per day.
#define OBJ_ARTIFACT_NOMAD_BOOTS_MOBILITY		0x43		// The Nomad Boots of Mobility increase your movement on land.
#define OBJ_ARTIFACT_TRAVELER_BOOTS_MOBILITY		0x45		// The Traveler's Boots of Mobility increase your movement on land.
#define OBJ_ARTIFACT_RABBIT_FOOT			0x47		// The Lucky Rabbit's Foot increases your luck in combat.
#define OBJ_ARTIFACT_GOLDEN_HORSESHOE			0x49		// The Golden Horseshoe increases your luck in combat.
#define OBJ_ARTIFACT_GAMBLER_LUCKY_COIN			0x4B		// The Gambler's Lucky Coin increases your luck in combat.
#define OBJ_ARTIFACT_FOUR_LEAF_CLOVER			0x4D		// The Four_Leaf Clover increases your luck in combat.
#define OBJ_ARTIFACT_TRUE_COMPASS_MOBILITY		0x4F		// The True Compass of Mobility increases your movement on land and sea.
#define OBJ_ARTIFACT_SAILORS_ASTROLABE_MOBILITY		0x51		// The Sailors' Astrolabe of Mobility increases your movement on sea.
#define OBJ_ARTIFACT_EVIL_EYE				0x53		// The Evil Eye reduces the casting cost of curse spells by half.
#define OBJ_ARTIFACT_ENCHANTED_HOURGLASS		0x55		// The Enchanted Hourglass extends the duration of all your spells by 2 turns.
#define OBJ_ARTIFACT_GOLD_WATCH				0x57		// The Gold Watch doubles the effectiveness of your hypnotize spells.
#define OBJ_ARTIFACT_SKULLCAP				0x59		// The Skullcap halves the casting cost of all mind influencing spells.
#define OBJ_ARTIFACT_ICE_CLOAK				0x5B		// The Ice Cloak halves all damage your troops take from cold spells.
#define OBJ_ARTIFACT_FIRE_CLOAK				0x5D		// The Fire Cloak halves all damage your troops take from fire spells.
#define OBJ_ARTIFACT_LIGHTNING_HELM			0x5F		// The Lightning Helm halves all damage your troops take from lightning spells.
#define OBJ_ARTIFACT_EVERCOLD_ICICLE			0x61		// The Evercold Icicle causes your cold spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_EVERHOT_LAVA_ROCK			0x63		// The Everhot Lava Rock causes your fire spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_LIGHTNING_ROD			0x65		// The Lightning Rod causes your lightning spells to do 50% more damage to enemy troops.
#define OBJ_ARTIFACT_SNAKE_RING				0x67		// The Snake Ring halves the casting cost of all your bless spells.
#define OBJ_ARTIFACT_ANKH				0x69		// The Ankh doubles the effectiveness of all your resurrect and animate spells.
#define OBJ_ARTIFACT_BOOK_ELEMENTS			0x6B		// The Book of Elements doubles the effectiveness of all your summoning spells.
#define OBJ_ARTIFACT_ELEMENTAL_RING			0x6D		// The Elemental Ring halves the casting cost of all summoning spells.
#define OBJ_ARTIFACT_HOLY_PENDANT			0x6F		// The Holy Pendant makes all your troops immune to curse spells.
#define OBJ_ARTIFACT_PENDANT_FREE_WILL			0x71		// The Pendant of Free Will makes all your troops immune to hypnotize spells.
#define OBJ_ARTIFACT_PENDANT_LIFE			0x73		// The Pendant of Life makes all your troops immune to death spells.
#define OBJ_ARTIFACT_SERENITY_PENDANT			0x75		// The Serenity Pendant makes all your troops immune to berserk spells.
#define OBJ_ARTIFACT_SEEING_EYE_PENDANT			0x77		// The Seeing_eye Pendant makes all your troops immune to blindness spells.
#define OBJ_ARTIFACT_KINETIC_PENDANT			0x79		// The Kinetic Pendant makes all your troops immune to paralyze spells.
#define OBJ_ARTIFACT_PENDANT_DEATH			0x7B		// The Pendant of Death makes all your troops immune to holy spells.
#define OBJ_ARTIFACT_WAND_NEGATION			0x7D		// The Wand of Negation protects your troops from the Dispel Magic spell.
#define OBJ_ARTIFACT_GOLDEN_BOW				0x7F		// The Golden Bow eliminates the 50% penalty for your troops shooting past obstacles. (e.g. castle walls)
#define OBJ_ARTIFACT_TELESCOPE				0x81		// The Telescope increases the amount of terrain your hero reveals when adventuring by 1 extra square.
#define OBJ_ARTIFACT_STATESMAN_QUILL			0x83		// The Statesman's Quill reduces the cost of surrender to 10% of the total cost of troops you have in your army.
#define OBJ_ARTIFACT_WIZARD_HAT				0x85		// The Wizard's Hat increases the duration of your spells by 10 turns!
#define OBJ_ARTIFACT_POWER_RING				0x87		// The Power Ring returns 2 extra power points/turn to your hero.
#define OBJ_ARTIFACT_AMMO_CART				0x89		// The Ammo Cart provides endless ammunition for all your troops that shoot.
#define OBJ_ARTIFACT_TAX_LIEN				0x8B		// The Tax Lien costs you 250 gold pieces/turn.
#define OBJ_ARTIFACT_HIDEOUS_MASK			0x8D		// The Hideous Mask prevents all 'wandering' armies from joining your hero.
#define OBJ_ARTIFACT_ENDLESS_POUCH_SULFUR		0x8F		// The Endless Pouch of Sulfur provides 1 unit of sulfur per day.
#define OBJ_ARTIFACT_ENDLESS_VIAL_MERCURY		0x91		// The Endless Vial of Mercury provides 1 unit of mercury per day.
#define OBJ_ARTIFACT_ENDLESS_POUCH_GEMS			0x93		// The Endless Pouch of Gems provides 1 unit of gems per day.
#define OBJ_ARTIFACT_ENDLESS_CORD_WOOD			0x95		// The Endless Cord of Wood provides 1 unit of wood per day.
#define OBJ_ARTIFACT_ENDLESS_CART_ORE			0x97		// The Endless Cart of Ore provides 1 unit of ore per day.
#define OBJ_ARTIFACT_ENDLESS_POUCH_CRYSTAL		0x99		// The Endless Pouch of Crystal provides 1 unit of crystal/day.
#define OBJ_ARTIFACT_SPIKED_HELM			0x9B		// The Spiked Helm increases your attack and defense skills by 1 each.
#define OBJ_ARTIFACT_SPIKED_SHIELD			0x9D		// The Spiked Shield increases your attack and defense skills by 2 each.
#define OBJ_ARTIFACT_WHITE_PEARL			0x9F		// The White Pearl increases your spell power and knowledge by 1 each.
#define OBJ_ARTIFACT_BLACK_PEARL			0xA1		// The Black Pearl increases your spell power and knowledge by 2 each.
*/

/*
#define OBJ_ARTIFACT_ULTIMATE_BOOK			0		// The Ultimate Book of Knowledge increases your knowledge by 12.
#define OBJ_ARTIFACT_ULTIMATE_SWORD			1		// The Ultimate Sword of Dominion increases your attack skill by 12.
#define OBJ_ARTIFACT_ULTIMATE_CLOAK			2		// The Ultimate Cloak of Protection increases your defense skill by 12.
#define OBJ_ARTIFACT_ULTIMATE_WAND			3		// The Ultimate Wand of Magic increases your spell power by 12.
#define OBJ_ARTIFACT_ULTIMATE_SHIELD			4		// The Ultimate Shield increases your attack and defense skills by 6 each.
#define OBJ_ARTIFACT_ULTIMATE_STAFF			5		// The Ultimate Staff increases your spell power and knowledge by 6 each.
#define OBJ_ARTIFACT_ULTIMATE_CROWN			6		// The Ultimate Crown increases each of your basic skills by 4 points.
#define OBJ_ARTIFACT_GOLDEN_GOOSE			7		// The Golden Goose brings in an income of 10,000 gold per turn.
*/

/*
#define OBJ_ARTIFACT_MAGIC_BOOK				81		// The Magic Book enables you to cast spells.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_82			82		// Artifact 82.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_83			83		// Artifact 83.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_84			84		// Artifact 84.
#define OBJ_ARTIFACT_ERROR_ARTIFACT_85			85		// Artifact 85.
#define OBJ_ARTIFACT_SPELL_SCROLL			86		// This Spell Scroll gives your hero the ability to cast the '%s' spell.
#define OBJ_ARTIFACT_ARM_MARTYR				87		// The Arm of the Martyr increases your spell power by 3 but adds the undead morale penalty.
#define OBJ_ARTIFACT_BREASTPLATE_ANDURAN		88		// The Breastplate increases your defense by 5.
#define OBJ_ARTIFACT_BROACH_SHIELDING			89		// The Broach of Shielding provides 50% protection from Armageddon and Elemental Storm, but decreases spell power by 2.
#define OBJ_ARTIFACT_BATTLE_GARB			90		// The Battle Garb of Anduran combines the powers of the three Anduran artifacts.
#define OBJ_ARTIFACT_CRYSTAL_BALL			91		// The Crystal Ball lets you get more specific information about monsters,
#define OBJ_ARTIFACT_HEART_FIRE				92		// The Heart of Fire provides 50% protection from fire, but doubles the damage taken from cold.
#define OBJ_ARTIFACT_HEART_ICE				93		// The Heart of Ice provides 50% protection from cold, but doubles the damage taken from fire.
#define OBJ_ARTIFACT_HELMET_ANDURAN			94		// The Helmet increases your spell power by 5.
#define OBJ_ARTIFACT_HOLY_HAMMER			95		// The Holy Hammer increases your attack skill by 5.
#define OBJ_ARTIFACT_LEGENDARY_SCEPTER			96		// The Legendary Scepter adds 2 points to all attributes.
#define OBJ_ARTIFACT_MASTHEAD				97		// The Masthead boosts your luck and morale by 1 each in sea combat.
#define OBJ_ARTIFACT_SPHERE_NEGATION			98		// The Sphere of Negation disables all spell casting, for both sides, in combat.
#define OBJ_ARTIFACT_STAFF_WIZARDRY			99		// The Staff of Wizardry boosts your spell power by 5.
#define OBJ_ARTIFACT_SWORD_BREAKER			100		// The Sword Breaker increases your defense by 4 and attack by 1.
#define OBJ_ARTIFACT_SWORD_ANDURAN			101		// The Sword increases your attack skill by 5.
#define OBJ_ARTIFACT_SPADE_NECROMANCY			102		// The Spade gives you increased necromancy skill.
#define OBJ_ARTIFACT_NONE				255		// 
*/

#endif
