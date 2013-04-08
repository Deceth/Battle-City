/*
===============================================================================

    Battle City - CMap file
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
#include "CMap.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CMap::CMap(CServer *Server) {
	this->p = Server;

	// For each row,
	for (int i = 0; i < 512; i++) {

		// For each column,
		for (int j = 0; j < 512; j++) {

			// Set the square to 0
			this->map[i][j] = 0;
		}
	}
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CMap::~CMap() {
}

/***************************************************************
 * Function:	LoadMap
 *
 **************************************************************/
void CMap::LoadMap() {
	FILE *file;

	// If the map file is missing,
	cout << " -- Loading map file" << endl;
	if (this->p->Exists("map.dat") == 0) {

		// Error and return
		this->p->running = 0;
		cout << "Error:  map.dat is missing!" << endl;
		return;
	}

	// Open and read the map file
	cout << " -- Opening map file" << endl;
	file = fopen("map.dat","r");
	
	if (file) {
		cout << " -- Reading map file" << endl;
		fread(this->map,512,512,file);

		cout << " -- Placing CCs" << endl;
		this->CalculateTiles();
	}

	fclose(file);
}

/***************************************************************
 * Function:	CalculateTiles
 *
 **************************************************************/
void CMap::CalculateTiles() {
	int citIndex = 63;

	// For each row,
	for (int j = 0; j < 512; j++) {

		// For each column,
		for (int i = 0; i < 512; i++) {

			// If the map says to place a CC,
			if (this->map[i][j] == 3) {

				// Place the CC
				p->City[citIndex]->x = ((512*48) - (32+(citIndex % 8*64) + 1) * 48);
				p->City[citIndex]->y = ((512*48) - (32+(citIndex / 8*64) + 1) * 48); 

				p->Build->newBuilding(i, j, 0, -1, 0);
				citIndex--;
			}
		}
	}
}
