#ifndef CPLAYER
#define CPLAYER

#include "CGame.h"

class CGame;

class CPlayer {

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
		bool isCloaked;
		char isMayor;
		char isShooting;
		char isFrozen;
		char isLagging;
		char isInGame;
		bool isDead;
		float timeFrozen;
		float timeUncloak;

		char playerType;
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
		bool isAdmin();
		void setCloak(bool isCloaked);
		void setMayor(bool isMayor);

		void SetHP(int HP);

		void HitMine();
		void HitDFG();
		int getTileX();
		int getTileY();

		CPlayer(CGame *game, int id);
		virtual ~CPlayer();

	private:
		CGame *p;

};

#endif
