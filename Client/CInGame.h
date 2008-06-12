#ifndef _CInGame
#define _CInGame

#include "CGame.h"

class CGame;

class CInGame {
	public:
		CInGame(CGame *game);
		~CInGame();

		void Cycle();

		void SendCheatCheck();
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
		char HasCloak;
		char HasRocket;
		char HasMedkit;
		char HasBomb;
		char HasMine;
		char HasOrb;
		char HasFlare;
		char HasDFG;
		char HasWall;
		char HasTurret;
		char HasSleeper;
		char HasPlasma;		
		char ShowMap;
		float timeDeath;
		float timeLastAttack;
		
		bool isUnderAttack;
		float timeUnderAttack;

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

		string playerInfoLine1;
		string playerInfoLine2;
		string playerInfoLine3;
		string playerInfoLine4;
		COLORREF playerInfoColor1;
		COLORREF playerInfoColor2;
		COLORREF playerInfoColor3;
		COLORREF playerInfoColor4;

		string NewbieTip;

		char IsChatting;
		char BombsAreActivated;

		string ReturnRank(int Points);
		void AppendChat(string ChatText, COLORREF Color);
		void AppendChatLine(string ChatText, COLORREF Color);
		void AppendInfo(string infoText);

		void setPlayerInfo(string infoText);
		void setPlayerInfo(string infoText, int lineIndex);
		void setPlayerInfoLine(string infoText, int lineIndex, COLORREF color);

		void RefreshArea();
		void ClearSector(int SectorX, int SectorY);
		void PrintWhoData();
		void PrintFinanceReport();
		int getGrossIncome();
		void setIsUnderAttack(bool isUnderAttack);

		string getFileNameFromChatLine(string chatLine);
		int* splitStringIntoInts(char* buildingLine);

		void requestAutoBuild(string chatLine);
		void saveCity(string chatLine);
		bool createBuilding(unsigned char type, unsigned short x, unsigned short y, bool isAutoBuild);

		static bool isValidCityIndex(int city);

	private:
		CGame *p;

	protected:

};

#endif