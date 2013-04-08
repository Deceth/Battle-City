/*
===============================================================================

    Battle City - CItem header file
    Copyright (C) 2005-2013  battlecity.org

    This file is part of Battle City.

    Battle City is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battle City is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battle City.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/
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
