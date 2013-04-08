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

#include "CGame.h"

class CGame;
class CBullet;

class CItem {

    public:
        CGame *p;
        
        CItem(int X, int Y, int Type, int City, unsigned short id, int active, CGame *game);
        ~CItem();
        void targetNearestEnemy();
        void verifyBulletExists();

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
        int bulletDamage;
        CBullet *bullet;

        int Animation;
        float Animationtick;

        CItem *next;
        CItem *prev;
};

class CItemList {

    public:
        CItem *itemListHead;
        CGame *p;

        CItem *findItem(unsigned short id);

        CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active);
        CItem *delItem(CItem *del);
        CItem *findItembyLocation(int X, int Y);
        CItem *findItembyLocationAndType(int X, int Y, int Type, bool activeOnly);
        void deleteItemsByCity(char theCity);

        CItemList(CGame *game);
        ~CItemList();
        void Cycle();
};

class CInventory {

    public:
        CItem *itemListHead;

        CItem *findItem(unsigned short id);

        CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active );
        CItem *delItem(CItem *del);
        CItem *findItembyType(int Type);

        int SelectedItemType;

        void ItemCheck();
        void Drop();
        void triggerItem(int type);

        CInventory(CGame *game);
        ~CInventory();

        void Cycle();
        CGame *p;
};

#endif