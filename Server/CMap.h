#ifndef __MAP
#define __MAP

#include "CServer.h"

class CServer;

class CMap
{
public:
	CMap(CServer *Server);
	~CMap();

	void CalculateTiles();
	void LoadMap();

	unsigned char map[512][512];
private:
	CServer *p;
protected:
};

#endif
