/*
===============================================================================

    Battle City - CBullet header file
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
#ifndef __BULLETS__
#define __BULLETS__

#include "CGame.h"

class CGame;

class CBullet {

    public:
        CGame *p;

        float x;
        float y;
        int type;
        int damage;
        unsigned short owner;
        float life;
        char angle;
        char animation;
        unsigned short turretId;

        CBullet *next;
        CBullet *prev;

        CBullet(int x, int y, int type, int angle, unsigned short owner, CGame *game);
        ~CBullet();
};


class CBulletList {
    public:
        CBullet *bulletListHead;

        CBullet *newBullet(int x, int y, int type, int angle, int owner = 999);
        CBullet *delBullet(CBullet *del);

        CBulletList(CGame *game);
        ~CBulletList();

        void Cycle();
        CGame *p;
};

#endif
