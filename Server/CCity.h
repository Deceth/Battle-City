#ifndef CCITY
#define CCITY

#include "CServer.h"

class CServer;
class CBuildingList;

class CCity
{
protected:
public:
	CCity(CServer *Server);
	~CCity();

	float DestructTimer;

	int Mayor;
	int Successor;
	int Orbs;
	unsigned char id;
	int active;
	int hiring;
	int notHiring;

	long cash;
	long cashresearch;
	long itemproduction;
	long hospital;
	long income;

	float moneyCycle;
	int itemC[12];

	float research[20]; // -1 == complete, 0 == unbuilt, > 0 == tick research will complete
	int canBuild[30];

	int x;
	int y;

	void cycle();
	void setCanBuild(int i, int can);
	void destroy();

	void didOrb(int City, int index);
	void wasOrbed();

	int PlayerCount();
private:
	CServer *p;
};

#endif
