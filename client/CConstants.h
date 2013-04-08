#ifndef _CCONSTANTS
#define _CCONSTANTS

#define VERSION "3.5.7"

static const int TCPPORT = 5643;
//static const int TCPPORT = 6643;

// Constants
static const int			COST_BUILDING = 500000;				// synchronized with server
static const int			DAMAGE_LASER = 5;					// synchronized with server
static const int			DAMAGE_MINE = 19;					// synchronized with server
static const int			DAMAGE_ROCKET = 8;					// synchronized with server
static const int			DISTANCE_MAX_FROM_BUILDING = 11;
static const int			DISTANCE_MAX_FROM_CC = 29;
static const string			FILE_CITIES_FOLDER = "cities";
static const string			FILE_CITIES_EXTENSION = ".city";
static const int			MAP_SQUARE_LAVA = 1;
static const int			MAP_SQUARE_ROCK = 2;
static const int			MAX_CITIES = 64;
static const int			MAX_HEALTH = 40;					// synchronized with server
static const int			MAX_PLAYERS = 64;
static const int			MAX_PLAYERS_PER_CITY = 4;
static const float			MOVEMENT_SPEED_ADMIN = 1.0f;
static const float			MOVEMENT_SPEED_FLARE = 0.10f;		// TO SYNCH
static const float			MOVEMENT_SPEED_BULLET = 0.80f;		// synchronized with server
static const float			MOVEMENT_SPEED_PLAYER = 0.38f;		// synchronized with server
static const int			TIMER_CHANGE_TANK = 1000;
static const int			TIMER_CLOAK = 5000;					// synchronized with server
static const int			TIMER_DEMOLISH = 3000;
static const int			TIMER_DFG = 5000;					// synchronized with server
static const int			TIMER_RELOAD_SURFACES = 3000;
static const int			TIMER_RESPAWN = 10000;				// synchronized with server
static const int			TIMER_SHOOT_ADMIN = 50;
static const int			TIMER_SHOOT_LASER = 650;			// synchronized with server
static const int			TIMER_SHOOT_ROCKET = 650;			// synchronized with server
static const int			TIMER_SHOOT_FLARE = 500;			// TO SYNCH
static const int			TIMER_UNDER_ATTACK = 3000;
static const short int		RadarSize = 2400;
static const unsigned char	SectorSize = 16;
static const unsigned char	MaxSectors = (512 / SectorSize);

// Item types
static const int			ITEM_TYPE_CLOAK = 0;
static const int			ITEM_TYPE_ROCKET = 1;
static const int			ITEM_TYPE_MEDKIT = 2;
static const int			ITEM_TYPE_BOMB = 3;
static const int			ITEM_TYPE_MINE = 4;
static const int			ITEM_TYPE_ORB = 5;
static const int			ITEM_TYPE_FLARE = 6;
static const int			ITEM_TYPE_DFG = 7;
static const int			ITEM_TYPE_WALL = 8;
static const int			ITEM_TYPE_TURRET = 9;
static const int			ITEM_TYPE_SLEEPER = 10;
static const int			ITEM_TYPE_PLASMA = 11;



// Newbie tips
static const string			NEWBIE_TIP_CLOAK = "Cloaks remove you from enemy radar.  Press 'C' to use a Cloaking Device.";
static const string			NEWBIE_TIP_ROCKET = "Cougar Missles upgrade your tanks weapons but only fire when you are not moving.";
static const string			NEWBIE_TIP_MEDKIT = "Medkits restore your tank to full health. Press 'H' to use a Medkit.";
static const string			NEWBIE_TIP_BOMB = "Bombs destroy nearby enemy buildings. Press 'B' to drop an activated bomb.";
static const string			NEWBIE_TIP_MINE = "Mines detonate enemy tanks, and cannot be seen by the enemy.";
static const string			NEWBIE_TIP_ORB = "The Orb destroys your enemy when dropped on their Command Center! Press 'O' to drop the orb";
static const string			NEWBIE_TIP_FLARE = "Drop flares behind your tank to damage the enemy! Press CTRL to fire!";
static const string			NEWBIE_TIP_DFG = "DFGs demobilize enemy tanks, and cannot be seen by the enemy.";
static const string			NEWBIE_TIP_WALL = "Walls are strong obstacles. Use them to protect your Command Center!";
static const string			NEWBIE_TIP_TURRET = "Turrets automatically fire at enemy tanks. Press 'D' to drop a Turret.";
static const string			NEWBIE_TIP_SLEEPER = "Sleepers automatically fire at enemy tanks and cannot be seen by the enemy.";
static const string			NEWBIE_TIP_PLASMA = "Plasmas automatically fire at enemy tanks and do more damage than regular Turrets.";
static const string			NEWBIE_TIP_CC = "The Command Center (CC) allows your city to function. Protect it at all costs!";
static const string			NEWBIE_TIP_HOSPITAL = "Hospitals restore your health while you stand on them.";
static const string			NEWBIE_TIP_HOUSE = "Houses provide enough workers to staff 2 other buildings.";
static const string			NEWBIE_TIP_RESEARCH = "Researches let you build Factories, and more advanced Researches.";
static const string			NEWBIE_TIP_FACTORY = "Factories produce Items. Drive onto an Item and press 'U' to pick it up.";
static const string			NEWBIE_TIP_ORBABLE = "Your city can now be destroyed by an enemy orb. Protect your CC at all costs!";

enum States {
	Empty,
	STATE_LOGIN,
	STATE_RECOVER,
	STATE_JOINING,
	STATE_GAME,
	STATE_NEWACCOUNT,
	STATE_EDITING,
	STATE_PERSONALITY,
	STATE_VERIFY,
	STATE_MEETING,
	STATE_INTERVIEW,
};

enum PanelModes {
	modeEmpty,
	modeFinance,
};

// Item names
static const char *ItemList[] = {
	"Cloak",
	"Cougar Missile",
	"MedKit",
	"Bomb",
	"Mine",
	"Orb",
	"Flare Gun",
	"DFG",
	"Wall",
	"Turret",
	"Sleeper Turret",
	"Plasma Turret",
};

// Building types:
// - 2xx means Hosp, 3xx means House, 4xx means Research, 1xx means Factory, xZZ means Item ZZ
// - Controls the build menu order (functionally, not appearance)
static const int buildingTypes[] = {
	200,	// Hospital
	300,	// House
	401,	// Research,	Item 1
	101,	// Factory,		Item 1
	409,	// Research,	Item 9
	109,	// Factory,		Item 9
	400,	// Research,	Item 0
	100,	// Factory,		Item 0
	402,	// Research,	Item 2
	102,	// Factory,		Item 2
	411,	// Research,	Item 11
	111,	// Factory,		Item 11
	404,	// Research,	Item 4
	104,	// Factory,		Item 4
	405,	// Research,	Item 5
	105,	// Factory,		Item 5
	403,	// Research,	Item 3
	103,	// Factory,		Item 3
	410,	// Research,	Item 10
	110,	// Factory,		Item 10
	408,	// Research,	Item 8
	108,	// Factory,		Item 8
	407,	// Research,	Item 7
	107,	// Factory,		Item 7
	406,	// Research,	Item 6
	106,	// Factory,		Item 6
};

// Building names
// - Controls the names in the build menu
static const char *buildNames[] = {
	"Hospital",
	"House",
	"Bazooka Research",
	"Bazooka Factory",
	"Turret Research",
	"Turret Factory",
	"Cloak Research",
	"Cloak Factory",
	"MedKit Research",
	"MedKit Factory",
	"Plasma Turret Research",
	"Plasma Turret Factory",
	"Mine Research",
	"Mine Factory",
	"Orb Research",
	"Orb Factory",
	"Time Bomb Research",
	"Time Bomb Factory",
	"Sleeper Research",
	"Sleeper Factory",
	"Wall Research",
	"Wall Factory",
	"DFG Research",
	"DFG Factory",
	"Flare Gun Research",
	"Flare Gun Factory",
};

// Building order in build menu
// - Controls the item images tied to the build menu
static const int buildButton[] = {
	12,		// Hospital
	0,
	2,		// Item 1 (+1)
	2,		// Item 1 (+1)
	9,		// Item 8 (+1)
	9,		// Item 8 (+1)
	1,		// Item 0 (+1)
	1,		// Item 0 (+1)
	3,
	3,
	10,
	10,
	5,
	5,
	6,
	6,
	4,		// Item 3 (+1)
	4,		// Item 3 (+1)
	8,
	8,
	11,
	11,
	8,
	8,
	7,
	7
};

// City names
static const char *CityList[] = {
	"Balkh",
	"Iqaluit",
	"Reykjavik",
	"Jumarity",
	"Helsinki",
	"Copenhagen",
	"Kiev",
	"Barentsburg",
	"Nunivak",
	"Algiers",
	"Paga Pago",
	"St. Johns",
	"Parana",
	"San Salvador de Jujuy",
	"Tallinn",
	"Bergen",
	"Bangui",
	"Annaba",
	"Andorra-la-Vella",
	"Bahia Blanca",
	"Posadas",
	"Santa Fe",
	"Buckland",
	"Kabul",
	"Lahij",
	"Banta",
	"Benguela",
	"Buenos Aires",
	"Resistencia",
	"Santiago del Estero",
	"Armidale",
	"Harbin",
	"Fajardo",
	"Blida",
	"Huambo",
	"Cordoba",
	"Rio Cuarto",
	"Kumayari",
	"Kuala Lumpur",
	"Mango",
	"Arequipa",
	"Constantine",
	"Luanda",
	"Corrientes",
	"Rosario",
	"Kirovakan",
	"Jakarta",
	"Skopje",
	"Bogota",
	"Canberra",
	"Pretoria",
	"Maracay",
	"Cambridge",
	"Laketown",
	"Hanoi",
	"Bishkek",
	"Tirana",
	"Dakar",
	"Aquin",
	"Bismarck",
	"Albany",
	"Manukau",
	"Utrecht",
	"Admin Inn",
};

// Sounds
enum {
	sLaser,
	sFire,
	sEngine,
	sBuild,
	sDie,
	sExplode,
	sTurretfire,
	sBuzz,
	sClick,
	sBigturret,
	sDemolish,
	sScreech,
	sHit,
	sCloak,
	sFlare,
};

// ???
struct sKicked {
	DWORD tick;
};

// Colors by color
static const COLORREF COLOR_BLUE = RGB(0,0,255);
static const COLORREF COLOR_GRAY = RGB(135, 135, 135);
static const COLORREF COLOR_GREEN = RGB(0,255,0);
static const COLORREF COLOR_OFFWHITE = RGB(235,235,235);
static const COLORREF COLOR_ORANGE = RGB(255, 165, 0);
static const COLORREF COLOR_RED = RGB(255,0,0);
static const COLORREF COLOR_TRANSPARENCY = RGB(255,0,255);
static const COLORREF COLOR_YELLOW = RGB(255, 215, 0);
static const COLORREF COLOR_WHITE = RGB(255,255,255);
// Colors by function
static const COLORREF COLOR_ADMIN_CHAT = COLOR_ORANGE;
static const COLORREF COLOR_ADMIN_NAME = COLOR_YELLOW;
static const COLORREF COLOR_DEAD_CHAT = COLOR_GRAY;
static const COLORREF COLOR_DEAD_NAME = COLOR_GRAY;
static const COLORREF COLOR_ENEMY_CHAT = COLOR_RED;
static const COLORREF COLOR_ENEMY_NAME = COLOR_RED;
static const COLORREF COLOR_LAGGING_NAME = COLOR_BLUE;
static const COLORREF COLOR_GLOBAL_CHAT = COLOR_WHITE;
static const COLORREF COLOR_PROMOTION_CHAT = COLOR_WHITE;
static const COLORREF COLOR_PANEL_HEAD = COLOR_GREEN;
static const COLORREF COLOR_PANEL_BODY = COLOR_OFFWHITE;
static const COLORREF COLOR_TEAM_CHAT = COLOR_GREEN;
static const COLORREF COLOR_TEAM_NAME = COLOR_GREEN;
static const COLORREF COLOR_SYSTEM = COLOR_YELLOW;
static const COLORREF COLOR_WHISPER = COLOR_WHITE;

#endif


// NOTE: When version number is updated, users will receive an outdated version error if their 
// bc.exe does not update. 
// Garbage code to change bc.exe file size so updater works!
// 3.4.0 - 356 kb
// 3.4.5 - 360 kb
// 3.4.6 - 364 kb
// 3.4.7 - 368 kb
// 3.4.7 - 372 kb
// 3.4.8 - 376 kb
// 3.4.8 - 380 kb
// 3.4.9 - 368 kb (but version changed, so users on 3.4.7 at 368kb will get outdated version error!)
