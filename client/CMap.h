#ifndef __MAP
#define __MAP

#include "CGame.h"

class CGame;

class CMap
{
public:
	CMap(CGame *Game);
	~CMap();

	void CalculateTiles();
	void LoadMap();

	unsigned char map[512][512];
	short tiles[512][512];
private:
	CGame *p;
protected:
};

#endif