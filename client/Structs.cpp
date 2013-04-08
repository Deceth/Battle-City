/*
===============================================================================

    Battle City - Structs
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
char *CityList[] = {
	"Balkh",
	"Iqaluit",
	"Reykjavik",
	"Jumarity",
	"Helsinki",
	"Copenhagen",
	"Kiev",
	"Barentsburg",
	"Nunivak",
	"Algiers",
	"Paga Pago",
	"St. Johns",
	"Parana",
	"San Salvador de Jujuy",
	"Tallinn",
	"Bergen",
	"Bangui",
	"Annaba",
	"Andorra-la-Vella",
	"Bahia Blanca",
	"Posadas",
	"Santa Fe",
	"Buckland",
	"Kabul",
	"Lahij",
	"Banta",
	"Benguela",
	"Buenos Aires",
	"Resistencia",
	"Santiago del Estero",
	"Armidale",
	"Harbin",
	"Fajardo",
	"Blida",
	"Huambo",
	"Cordoba",
	"Rio Cuarto",
	"Kumayari",
	"Kuala Lumpur",
	"Mango",
	"Arequipa",
	"Constantine",
	"Luanda",
	"Corrientes",
	"Rosario",
	"Kirovakan",
	"Jakarta",
	"Skopje",
	"Bogota",
	"Canberra",
	"Pretoria",
	"Maracay",
	"Cambridge",
	"Laketown",
	"Hanoi",
	"Bishkek",
	"Tirana",
	"Dakar",
	"Aquin",
	"Bismarck",
	"Albany",
	"Manukau",
	"Utrecht",
	"Admin Inn",
};

char *ItemList[] = {
	"Laser",
	"Cougar Missile",
	"MedKit",
	"Bomb",
	"Mine",
	"Orb",
	"Flare Gun",
	"DFG",
	"Wall",
	"Turret",
	"Sleeper Turret",
	"Plasma Turret",
};

int buildingTypes[] = {
	200,300,400,100,409,109,403,103,402,102,411,111,404,104,405,105,401,101,410,110,408,108,407,107,406,106
};

char *buildNames[] = {
	"Hospital",
	"House",
	"Laser Research",
	"Laser Factory",
	"Turret Research",
	"Turret Factory",
	"Time Bomb Research",
	"Time Bomb Factory",
	"MedKit Research",
	"MedKit Factory",
	"Plasma Turret Research",
	"Plasma Turret Factory",
	"Mine Research",
	"Mine Factory",
	"Orb Research",
	"Orb Factory",
	"Bazooka Research",
	"Bazooka Factory",
	"Sleeper Research",
	"Sleeper Factory",
	"Wall Research",
	"Wall Factory",
	"DFG Research",
	"DFG Factory",
	"Flare Gun Research",
	"Flare Gun Factory",
};

int buildButton[] = {
	12,0,1,1,9,9,4,4,3,3,10,10,5,5,6,6,2,2,8,8,11,11,8,8,7,7
};

/* NOTE: When version number is updated, users will receive an outdated version error if their 
bc.exe does not update. 

Garbage code to change bc.exe file size so updater works!

bc.exe in BattleCityInstaller must always be different size than latest release or else installer must be updated!
-> BattleCityInstaller April 7, bc.exe = 380kb 

3.4.0 - 356 kb
3.4.5 - 360 kb
3.4.6 - 364 kb
3.4.7 - 368 kb
3.4.7 - 372 kb
3.4.8 - 376 kb
3.4.8 - 380 kb
3.4.9 - 368 kb
3.5.0 - 372 kb 
3.5.0 - 376 kb
3.5.0 - 380 kb
3.5.1 - 384 kb
3.5.1 - 388 kb
3.5.1 - 392 kb
3.5.2 - 412 kb

*/

// Begin junk for bigger file size.