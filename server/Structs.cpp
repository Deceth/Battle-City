/*
===============================================================================

    Battle City - Structs file
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
int maxItems[] =
{
	4,
	4,
	5,
	20,//bombs
	10,
	1,//orb
	4,//walkies
	5,//dfg
	20,//walls
	10,
	5,//sleeper
	5//plasma
};

int itemTypes[] =
{
	0,
	9,
	3,
	2,
	11,
	4,
	5,
	1,
	10,
	8,
	7,
	6
};

int buildTree[] = {
-1, // lazer 0
-1, // turret 1
0, // bomb 2
0, // medkit 3
1, // plasma 4
1, // mine 5
2, // orb 6
2, // cougar 7
4, // sleeper 8
4, // wall 9
5, // dfg 10
6 // walkie talkie 11
};
