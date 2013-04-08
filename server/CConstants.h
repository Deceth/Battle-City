#ifndef _CCONSTANTS
#define _CCONSTANTS

#define VERSION "3.5.7"

// Note: to change build tree order, you must change the following:
// Server: itemTypes
// Client: buildingTypes
// Server: buildNames
// Server: buildButton
// Server: CCity:addBuilding (if orb/bomb move)

static const int			COST_BUILDING = 500000;				// synchronized with client
static const int			COST_ITEM = 750000;
static const int			COST_INCOME_POPULATION = 10000;
static const int			COST_UPKEEP_RESEARCH = 2000000;
static const int			COST_UPKEEP_HOSPITAL = 2000000;
static const int			DAMAGE_LASER = 5;					// synchronized with client
static const int			DAMAGE_MINE = 19;					// synchronized with client
static const int			DAMAGE_ROCKET = 8;					// synchronized with client
static const int			MAX_HEALTH = 40;					// synchronized with client
static const int			MAX_CITIES = 64;
static const unsigned char	MAX_PLAYERS = 64;
static const int			MAX_PLAYERS_PER_CITY = 4;
static const int			MONEY_MAX_VALUE = 95000000;
static const int			MONEY_STARTING_VALUE = 95000000;
static const float			MOVEMENT_SPEED_FLARE = 0.10f;		// TO SYNCH
static const float			MOVEMENT_SPEED_BULLET = 0.80f;		// synchronized with client
static const float			MOVEMENT_SPEED_PLAYER = 0.38f;		// synchronized with client
static const int			ORBABLE_SIZE = 21;
static const int			POPULATION_MAX_HOUSE = 100;
static const int			POPULATION_MAX_NON_HOUSE = 50;
static const int			TIMER_BOMB = 5000;
static const int			TIMER_CITY_DESTRUCT = 120000;
static const int			TIMER_CLOAK = 5000;					// synchronized with client
static const int			TIMER_DFG = 5000;					// synchronized with client
static const int			TIMER_RESPAWN = 10000;				// synchronized with client
static const int			TIMER_RESEARCH = 10000;
static const int			TIMER_SHOOT_LASER = 650;			// synchronized with client
static const int			TIMER_SHOOT_ROCKET = 650;			// synchronized with client
static const short int		RadarSize = 1800;
static const unsigned char	SectorSize = 16;
static const unsigned char	MaxSectors = (512 / SectorSize);

// Item types
static const int			ITEM_TYPE_CLOAK = 0;
static const int			ITEM_TYPE_ROCKET = 1;
static const int			ITEM_TYPE_MEDKIT = 2;
static const int			ITEM_TYPE_BOMB = 3;
static const int			ITEM_TYPE_MINE = 4;
static const int			ITEM_TYPE_ORB = 5;
static const int			ITEM_TYPE_WALKIE = 6;
static const int			ITEM_TYPE_DFG = 7;
static const int			ITEM_TYPE_WALL = 8;
static const int			ITEM_TYPE_TURRET = 9;
static const int			ITEM_TYPE_SLEEPER = 10;
static const int			ITEM_TYPE_PLASMA = 11;

// Item counts
static const int maxItems[] = {
	4,
	4,
	5,
	20,//bombs
	10,
	1,//orb
	4,//walkies
	5,//dfg
	20,//walls
	10,
	5,//sleeper
	5//plasma
};

// Item types
// - Controls the items buildings produce
// - Tied to index of client's CConstants::buildingTypes
static const int itemTypes[] = {
	ITEM_TYPE_ROCKET, //1,
	ITEM_TYPE_TURRET, //9,
	ITEM_TYPE_CLOAK, //0,
	ITEM_TYPE_MEDKIT, //2,
	ITEM_TYPE_PLASMA, //11,
	ITEM_TYPE_MINE, //4,
	ITEM_TYPE_ORB, //5,
	ITEM_TYPE_BOMB, //3,
	ITEM_TYPE_SLEEPER, //10,
	ITEM_TYPE_WALL, //8,
	ITEM_TYPE_DFG, //7,
	ITEM_TYPE_WALKIE, //6
};

// Building order in tree order
// - Controls order buildings become available in build tree
// - Tied to index of itemTypes
static const int buildTree[] = {
	-1, // rocket 0
	-1, // turret 1
	0, // cloak 2
	0, // medkit 3
	1, // plasma 4
	1, // mine 5
	2, // orb 6
	2, // bomb 7
	4, // sleeper 8
	4, // wall 9
	5, // dfg 10
	6 // walkie talkie 11
};

// Player states
enum {
	State_Disconnected,
	State_Connected,
	State_Verified,
	State_Editing,
	State_Chat,
	State_Game,
	State_Apply
};

#endif
