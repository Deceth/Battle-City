#ifndef __CITEM__
#define __CITEM__

#include "CGame.h"

class CGame;
class CBullet;

class CItem
{
public:
	CGame *p;
	CItem(int X, int Y, int Type, int City, unsigned short id, int active, CGame *game);
	~CItem();
	int X;
	int Y;
	int active;
	unsigned short id;
	int Life;
	int Type;
	int City;

	float Angle;
	char status;
	char dmg;
	float lastturn;
	int target;
	CBullet *bullet;

	int Animation;
	float Animationtick;

	CItem *next;
	CItem *prev;
};

class CItemList
{
public:
	CItem *items;

	CItem *findItem(unsigned short id);

	CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active);
	CItem *delItem(CItem *del);
	CItem *findItembyLocation(int X, int Y);
	CItem *findItembyLocationAndType(int X, int Y, int Type);

	CItemList(CGame *game)
	{
		p = game;
		items = 0;
	}
	~CItemList()
	{
		while (items)
			delItem(items);
	}

	void DestroyCity(char theCity);
	void Cycle();
	CGame *p;
};

class CInventory
{
public:
	CItem *items;

	CItem *findItem(unsigned short id);

	CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active );
	CItem *delItem(CItem *del);
	CItem *findItembyType(int Type);

	int SelectedItemType;

	void ItemCheck();
	void Drop();

	CInventory(CGame *game)
	{
		p = game;
		items = 0;

		SelectedItemType = 0;
	}
	~CInventory()
	{
		while (items)
			delItem(items);
	}

	void Cycle();
	CGame *p;
};

#endif