#ifndef CPLAYER
#define CPLAYER

#include "CGame.h"

class CGame;

class CPlayer
{
private:
	CGame *p;
protected:
public:
	int isMoving;
	float timeMove;
	int isTurning;
	float timeTurn;
	int Direction;
	float X;
	float Y;
	int SectorX;
	int SectorY;

	char City;
	int CityX;
	int CityY;

	int HP;
	int refHP1;
	int refHP2;

	int Points;
	int Orbs;
	int Deaths;
	int Assists;
	int MonthlyPoints;
	char isMayor;
	char isShooting;
	char isFrozen;
	char isLagging;
	char isInGame;
	bool isDead;
	float timeFrozen;

	char isAdmin;
	bool RainbowName;
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Member;
	unsigned char Tank;
	unsigned char Tank2;
	unsigned char Tank3;
	unsigned char Tank4;

	string Name;
	string NameString;
	string Town;
	string TownString;

	float lastUpdate;

	unsigned short id;

	void Cycle();

	void InGameClear();
	void ClearPlayer();
	void RelocatePlayer();
	void GenerateNameString();

	void SetHP(int HP);

	void HitMine();
	void HitDFG();

	CPlayer(CGame *game, int initid);
	virtual ~CPlayer();
};

#endif
