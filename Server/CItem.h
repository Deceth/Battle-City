#ifndef __CITEM__
#define __CITEM__

#include "CServer.h"

class CItem
{
public:
	CServer *p;
	int x;
	int y;
	int City;
	int holder;
	int active;
	int life;
	unsigned short id;
	int type;
	float explodetick;

	CItem *next;
	CItem *prev;

	CItem(int x, int y, int type, int City, unsigned short id, CServer *Server)
	{
		p = Server;
		prev = 0;
		next = 0;
		this->x = x;
		this->y = y;
		this->City = City;
		this->type = type;
		this->id = id;
		switch (type)
		{
		case 8:
		case 11:
			life = 40;
			break;
		case 9:
			life = 32;
			break;
		case 10:
			life = 16;
			break;
		}
		explodetick = 0;
		holder = -1;
		active = 0;
	}
	~CItem()
	{
		if (next)
			next->prev = prev;
		if (prev)
			prev->next = next;
	}
	void drop(int x, int y, int owner);
};

class CItemList
{
public:
	CItem *items;

	CItem *findItem(unsigned short id);

	CItem *newItem(int x, int y, int type, int City, unsigned short id);
	CItem *delItem(CItem *del);
	CItem *remItem(CItem *del);

	int itmID;

	CItemList(CServer *Server)
	{
		p = Server;
		items = 0;
		itmID = 1;
	}
	~CItemList()
	{
		while (items)
			delItem(items);
	}

	void cycle();
	CServer *p;
};

#endif
