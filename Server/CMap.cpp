#include "CMap.h"

//short tiles[512][512];

CMap::CMap(CServer *Server)
{
	p = Server;
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

	if (p->Exists("map.dat") == 0) 
	{
		p->running = 0;
		cout << "Error:  map.dat is missing!" << endl;
		return;
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
	int citIndex = 63;
	for (int j = 0; j < 512; j++)
	{
		for (int i = 0; i < 512; i++)
		{
			if (this->map[i][j] == 3)
			{
				p->City[citIndex]->x = i * 48;
				p->City[citIndex]->y = j * 48;
				p->Build->newBuilding(i, j, 0, -1, 0);
				citIndex--;
			}
		}
	}
}
