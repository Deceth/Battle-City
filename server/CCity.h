/*
===============================================================================

    Battle City - CCity header file
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
#ifndef CCITY
#define CCITY

#include "CServer.h"

class CServer;
class CBuildingList;

class CCity {

	public:
		CCity(CServer *Server, int id);
		~CCity();

		float DestructTimer;
		float moneyTick;
		float cityVerificationTick;

		int Mayor;
		int Successor;
		int Orbs;
		unsigned char id;
		int active;
		int hiring;
		int notHiring;
		int currentBuildingCount;
		int maxBuildingCount;
		float startTime;
		bool hadBombFactory;
		bool hadOrbFactory;

		long cash;
		long cashresearch;
		long itemproduction;
		long hospital;
		long income;
		int itemC[12];

		float research[20]; // -1 == complete, 0 == unbuilt, > 0 == tick research will complete
		int canBuild[30];

		int x;
		int y;

		void cycle();
		void resetTimers();

		void setCanBuild(int i, int can, bool allowReset);
		void resetToDefault();
		void destroy();

		void didOrb(int City, int index);
		void wasOrbed();
		void addBuilding(int type);
		void subtractBuilding(int type);
		int getOrbValue();
		bool isOrbable();
		int getUptime();
		int getUptimeInSeconds();
		int getUptimeInMinutes();

		int PlayerCount();
		static bool isValidCityIndex(int city);

	protected:

	private:
		CServer *p;
};

#endif
