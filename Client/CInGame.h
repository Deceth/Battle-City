#ifndef _CInGame
#define _CInGame

#include "CGame.h"

class CGame;

class CInGame
{
public:
	CInGame(CGame *game);
	~CInGame();

	void Cycle();

	void CheckRefVariables();
	void CheckCheatTools();
	void ClearOut();
	void ClearAllSectors();

	unsigned char HasSector[34][34];
	bool RequestedSector[34][34];

	float lastTick;
	float updateTick;
	float hospTick;

	int ShowBuildMenu;
	int CanBuild[30];

	float PingTick;
	int TCPPing;
	int TCPPingValues[5];
	float PingTimer;

	char Minimized;

	char IsBuilding;
	char Successor;

	long Cash;
	long Income;
	long Research;
	long Items;
	long Hospital;

	char HasLaser;
	char HasRocket;
	char HasWalkie;
	char ShowMap;
	float timeDeath;
	float timeLastAttack;

	string chatLine1;
	string chatLine2;
	string chatLine3;
	string chatLine4;
	string chatLine5;
	string chatLine6;
	string chatLine7;
	string chatLine8;
	string ChatLine;
	COLORREF chatColor1;
	COLORREF chatColor2;
	COLORREF chatColor3;
	COLORREF chatColor4;
	COLORREF chatColor5;
	COLORREF chatColor6;
	COLORREF chatColor7;
	COLORREF chatColor8;

	string NewbieTip;

	char IsChatting;
	char BombsAreActivated;

	string ReturnRank(int Points);
	void AppendChat(string ChatText, COLORREF Color);
	void AppendLine(string ChatText, COLORREF Color);
	void RefreshArea();
	void ClearSector(int SectorX, int SectorY);
	void PrintWhoData();
	void PrintFinanceReport();
private:
	CGame *p;
protected:

};

#endif