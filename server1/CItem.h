#ifndef __CITEM__
#define __CITEM__

#include "CServer.h"

class CItem {

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

		CItem(int x, int y, int type, int City, unsigned short id, CServer *Server);
		~CItem();

		void drop(int x, int y, int owner);
};

class CItemList {

	public:
		CItem *itemListHead;

		CItem *findItem(unsigned short id);

		CItem *newItem(int x, int y, int type, int City, unsigned short id);
		CItem *delItem(CItem *del);
		CItem *remItem(CItem *del);
		
		void deleteItemsByCity(int city);
		void deleteItemsByFactory(int city, int type);
		void deleteItemsByPlayer(int id);

		int itmID;

		CItemList(CServer *Server);
		~CItemList();

		void cycle();

		CServer *p;
};

#endif
