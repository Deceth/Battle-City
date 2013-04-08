/*
===============================================================================

    Battle City - CPlayer header file
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
#ifndef CPLAYER
#define CPLAYER

#include "CGame.h"

class CGame;

class CPlayer {

	public:
		int isMoving;
		float timeMove;
		int isTurning;
		float timeTurn;
		int Direction;
		float X;
		float Y;
		int SectorX;
		int SectorY;

		char City;
		int CityX;
		int CityY;

		int HP;
		int refHP1;
		int refHP2;

		int Points;
		int Orbs;
		int Deaths;
		int Assists;
		int MonthlyPoints;
		bool isCloaked;
		char isMayor;
		char isShooting;
		char isFrozen;
		char isLagging;
		char isInGame;
		bool isDead;
		float timeFrozen;
		float timeUncloak;

		char playerType;
		bool RainbowName;
		unsigned char Red;
		unsigned char Green;
		unsigned char Blue;
		unsigned char Member;
		unsigned char Tank;
		unsigned char Tank2;
		unsigned char Tank3;
		unsigned char Tank4;

		string Name;
		string NameString;
		string Town;
		string TownString;

		float lastUpdate;

		unsigned short id;

		void Cycle();

		void InGameClear();
		void ClearPlayer();
		void RelocatePlayer();
		void GenerateNameString();
		bool isAdmin();
		void setCloak(bool isCloaked);
		void setMayor(bool isMayor);

		void SetHP(int HP);

		void HitMine();
		void HitDFG();
		int getTileX();
		int getTileY();

		CPlayer(CGame *game, int id);
		virtual ~CPlayer();

	private:
		CGame *p;

};

#endif
