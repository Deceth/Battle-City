#include "CMap.h"

CMap::CMap(CGame *Game)
{
	p = Game;
}

CMap::~CMap()
{

}

void CMap::LoadMap()
{
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			this->map[i][j] = 0;
		}
	}

	FILE *file;
	file = fopen("map.dat","r");

	if (file)
	{
		fread(this->map,512,512,file);
		CalculateTiles();
	}

	fclose(file);
	
}

void CMap::CalculateTiles()
{
	/*Calculate ONCE which tiles should be drawn on the map. Since the map is static we only have to do
	this every time the game is started. This determines which tile is drawn based on surrounding tiles.
	copy the X value of the tile to blit to the tiles array. map[][] and tiles[][] can be used in 
	conjunction	with each other to effectivly draw the correct image: Check surrounding 4 Directional tiles*/
	int citIndex = 63;
	for (int j = 0; j <= 511; j++)//y, starting at top of map.
	{
		for (int i = 0; i <= 511; i++)//X, starting at left of map.
		{
			if (this->map[i][j] == 3)
			{
				p->Build->newBuilding(i, j, citIndex, 6, 0); //place City centers
				citIndex--;
			}
				
			if ((this->map[i][j] == 1) || (this->map[i][j] == 2)) //Only check rock or lava tiles
			{
				int currTile = this->map[i][j]; //Variable for the current tile value 
				if (currTile == 1 || currTile == 2) //Only check rock or lava tiles
				{
					/* Below checks each Direction by binary eXpressions, then evaluates the sum 
					into decimal multiplied by the tile size to get an Index to start drawing from
					in the tile file*/
					int left = (i == 511 || this->map[i+1][j] != currTile); 
					int right = (i == 0 || this->map[i-1][j] != currTile); 
					int up = (j == 511 || this->map[i][j+1] != currTile);
					int down = (j == 0 || this->map[i][j-1] != currTile); 
					this->tiles[i][j] = (left + right * 2 + down * 4 + up * 8) * 48; //Calculate 
				}
			}
		}
	}
}