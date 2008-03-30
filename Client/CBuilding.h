#ifndef __CBUILDING__
#define __CBUILDING__

#include "CGame.h"

class CGame;

class CBuilding {

	public:
		CGame *p;

		unsigned short X;
		unsigned short Y;
		unsigned char City;
		unsigned char pop;
		int Type;
		unsigned short id;
		char Animation;
		float timeAnimation;
		char Smoke;
		float timeSmoke;
		int ItemsLeft;

		CBuilding *next;
		CBuilding *prev;

		CBuilding(unsigned short X, unsigned short Y, int Type, unsigned char City, unsigned short id, CGame *Game);
		~CBuilding();
		bool isCC();
		static bool isCC(int buildingType);

};

class CBuildingList {

	public:
		CBuilding *buildingListHead;

		CBuilding *findBuilding(unsigned short id);

		CBuilding *newBuilding(unsigned short X, unsigned short Y, unsigned char City, int Type, unsigned short id);
		CBuilding *delBuilding(CBuilding *del);
		CBuilding *findBuildingbyLocation(int X, int Y);
		void deleteBuildingsByCity(int city);

		CBuildingList(CGame *game);
		~CBuildingList();

		void DestroyCity(char theCity);
		int inRange();
		void Cycle();
		CGame *p;
};

#endif