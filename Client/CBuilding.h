#ifndef __CBUILDING__
#define __CBUILDING__

#include "CGame.h"

class CGame;

class CBuilding
{
public:
	CGame *p;

	unsigned short X, Y;
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

	CBuilding(unsigned short X, unsigned short Y, int Type, unsigned char City, unsigned short id, CGame *Game)
	{
		p = Game;
		this->City = City;
		this->X = X;
		this->Y = Y;
		this->Type = Type;
		this->pop = 0;
		this->Animation = rand()%6;
		this->timeAnimation = 0;
		this->Smoke = 0;
		this->timeSmoke = 0;
		this->id = id;
		this->ItemsLeft = 0;

		prev = 0;
		next = 0;
	}
	~CBuilding()
	{
		if (next)
			next->prev = prev;
		if (prev)
			prev->next = next;
	}
};

class CBuildingList
{
public:
	CBuilding *buildings;

	CBuilding *findBuilding(unsigned short id);

	CBuilding *newBuilding(unsigned short X, unsigned short Y, unsigned char City, int Type, unsigned short id);
	CBuilding *delBuilding(CBuilding *del);
	CBuilding *findBuildingbyLocation(int X, int Y);

	CBuildingList(CGame *game)
	{
		p = game;
		buildings = 0;
	}
	~CBuildingList()
	{
		while (buildings)
			delBuilding(buildings);
	}

	void DestroyCity(char theCity);
	int inRange();
	void Cycle();
	CGame *p;
};

#endif