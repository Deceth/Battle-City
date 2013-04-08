/*
===============================================================================

    Battle City - CCollision header file
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
#ifndef _CCollision
#define _CCollision

#include "CGame.h"

struct Rect
{
    long X;
    long Y;
    long w;
    long h;
};

class CGame;
class CPlayer;

class CCollision
{
public:
    CCollision(CGame *game);
    ~CCollision();

    int CheckPlayerCollision(unsigned short id);
    int RectCollision(Rect rect1, Rect rect2);
    bool RectCollision(int X, int Y, int width, int height,
            int tolerance1, int X2, int Y2, int width2, int height2,
            int tolerance2);
    int CheckBuildingCollision(int X, int Y);
private:
    CGame *p;
protected:

};

#endif