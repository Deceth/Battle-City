/*
===============================================================================

    Battle City - CBuilding header file
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
        int rawType;
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
        CBuilding *buildingListHead;

        CBuilding *findBuilding(unsigned short id);

        CBuilding *newBuilding(unsigned short X, unsigned short Y, unsigned char City, int Type, unsigned short id);
        CBuilding *delBuilding(CBuilding *del);
        CBuilding *findBuildingbyLocation(int X, int Y);
        void deleteBuildingsByCity(int city);

        CBuildingList(CGame *game);
        ~CBuildingList();

        void DestroyCity(char theCity);
        bool inRange(bool withBuildingProximity);
        void Cycle();
        CGame *p;
        void requestAutoBuild(string filename);
};

#endif