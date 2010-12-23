extern char *CityList[];
extern char *buildNames[];
extern char *buildNamestemp[];
extern char *ItemList[];
extern int buildingTypes[];
extern int buildButton[];

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
	sFlare,
};

struct sKicked
{
	DWORD tick;
};