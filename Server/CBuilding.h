#ifndef __CBUILDING__
#define __CBUILDING__

#include "CServer.h"

class CServer;

class CBuilding
{
public:
	CServer *p;
	int x, y;
	int City;

	int AttachedID;
	int pop;
	int AttachedID2;
	int AttachedPop;
	int AttachedPop2;

	int type;
	unsigned short id;
	float ProduceTick;
	float PopulationTick;
	float CashTick;
	float MoneyTick;
	float ResearchTick;

	CBuilding *next;
	CBuilding *prev;

	CBuilding(int x, int y, int type, int City, unsigned short id, CServer *Server);
	~CBuilding();
};

class CBuildingList
{
public:
	CBuilding *buildings;

	CBuilding *findBuilding(unsigned short id);

	CBuilding *newBuilding(int x, int y, int type, int City, unsigned short id);
	CBuilding *delBuilding(CBuilding *del);
	CBuilding *remBuilding(CBuilding *del);

	int bldID;

	CBuildingList(CServer *Server)
	{
		p = Server;
		buildings = 0;
		bldID = 1;
	}
	~CBuildingList()
	{
		while (buildings)
			remBuilding(buildings);
	}

	void cycle();
	int GetBuildingCount(int theCity);
	int GetOrbBuildingCount(int theCity);
	int GetOrbPointCount(int theCity);
	CServer *p;
};

#endif
