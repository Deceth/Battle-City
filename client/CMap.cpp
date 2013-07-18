/*
===============================================================================

    Battle City - CMap
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
/// <summary>   Constructor. </summary>
///
/// <param name="Game"> [in,out] If non-null, the game. </param>
CMap::CMap(CGame *Game)
{
	p = Game;
}
/// <summary>   Destructor. </summary>
CMap::~CMap()
{

}
/// <summary>   Loads the map. </summary>
void CMap::LoadMap()
{
    //  Loop to generate zeros for entire map. Map size is set to 512x512
    //  stored in a char array.
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			this->map[i][j] = 0;
		}
	}
    //  Initialize file handler for reading map.dat
	FILE *file;
    //  Open map.dat through file handler
	file = fopen("map.dat","r");
    //  Upon verifying that file handler opened file
    //  continue with reading the file
	if (file)
	{
        //  Populate this->map with file contents
        //  http://msdn.microsoft.com/en-us/library/kt0etdcs(v=vs.100).aspx
		fread(this->map,512,512,file);
        //  Processes this->map to generate city centers, rock tiles, and lava tiles
		CalculateTiles();
	}
    //  Close the file handler
	fclose(file);
	
}

/// <summary>   Calculates the tiles. </summary>
void CMap::CalculateTiles()
{
	/*Calculate ONCE which tiles should be drawn on the map. Since the map is static we only have to do
	this every time the game is started. This determines which tile is drawn based on surrounding tiles.
	copy the X value of the tile to blit to the tiles array. map[][] and tiles[][] can be used in 
	conjunction	with each other to effectivly draw the correct image: Check surrounding 4 Directional tiles*/
    //  Default starting city index passed to newBuilding method 
	int citIndex = 63;
    //  Loop through y-axis, top of the map
	for (int j = 0; j <= 511; j++)
	{
        //  Loop through x axis, left of the map
		for (int i = 0; i <= 511; i++)
		{
            //  Upon verifying that map location is a city initialize a newBuilding
			if (this->map[i][j] == 3)
			{
                //  Initialize a building at the current coordinates
				p->Build->newBuilding(i, j, citIndex, -1, 0); //place City centers
                //  Subtract from the city index
				citIndex--;
			}
			//  Upon verifying that current map location is rock or lava proceed with populating this->tiles
			//  1 - Rock
			//  2 - Lava
			if ((this->map[i][j] == 1) || (this->map[i][j] == 2))
			{
                //  Populate variable with value from this->map
				int currTile = this->map[i][j];
                //  Upon verifying variable is rock or lava proceed with populating surrounding tiles
				if (currTile == 1 || currTile == 2)
				{
                    //  Verify i is at the end of map or the next tile is not the current tile
                    //  1 - Verification passed
                    //  0 - Verification failed                    
					int left = (i == 511 || this->map[i+1][j] != currTile); 
                    //  Verify i is at the end of the map or the next tile is not the current tile
					int right = (i == 0 || this->map[i-1][j] != currTile); 
                    //  Verify i is at the end of the map or the next tile is not the current tile
					int up = (j == 511 || this->map[i][j+1] != currTile);
                    //  Verify i is at the end of the map or the next tile is not the current tile
					int down = (j == 0 || this->map[i][j-1] != currTile); 
                    //  Perform calculations on above values in order to generate a tile value
                    //  Need more documentation on how these values are used
					this->tiles[i][j] = (left + right * 2 + down * 4 + up * 8) * 48;
				}
			}
		}
	}
}