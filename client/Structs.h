/*
===============================================================================

    Battle City - Structs header file
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
extern char *CityList[];
extern char *buildNames[];
extern char *buildNamestemp[];
extern char *ItemList[];
extern int buildingTypes[];
extern int buildButton[];

enum {
	sLaser,
	sFire,
	sEngine,
	sBuild,
	sDie,
	sExplode,
	sTurretfire,
	sBuzz,
	sClick,
	sBigturret,
	sDemolish,
	sScreech,
	sHit,
	sFlare,
};

struct sKicked
{
	DWORD tick;
};