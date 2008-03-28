#ifndef __CBUILDING__
#define __CBUILDING__

#include "CServer.h"

class CServer;

class CBuilding {

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

		bool hasMaxPopulation();
		bool isCC();
		static bool isCC(int buildingType);
		bool isFactory();
		static bool isFactory(int buildingType);
		bool isHospital();
		static bool isHospital(int buildingType);
		bool isHouse();
		static bool isHouse(int buildingType);
		bool isResearch();
		static bool isResearch(int buildingType);

};

class CBuildingList {

	public:
		CBuilding *buildings;

		CBuilding *findBuilding(unsigned short id);

		CBuilding *newBuilding(int x, int y, int type, int City, unsigned short id);
		CBuilding *delBuilding(CBuilding *del);
		CBuilding *remBuilding(CBuilding *del);

		int bldID;

		CBuildingList(CServer *Server) {
			p = Server;
			buildings = 0;
			bldID = 1;
		}

		~CBuildingList() {
			while (buildings) {
				remBuilding(buildings);
			}
		}

		void cycle();

// Moved to CCity
/*
		int GetBuildingCount(int theCity);
		int GetOrbBuildingCount(int theCity);
		int GetOrbPointCount(int theCity);
*/
		CServer *p;
	};

#endif
