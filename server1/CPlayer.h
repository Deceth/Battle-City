#ifndef _CPLAYER
#define _CPLAYER

#include "CServer.h"

class CServer;

class CPlayer {

	public:
		unsigned char id;

		float X;
		float Y;

		string Name;
		string Town;
		string UniqID;
		string IPAddress;

		int playerType;
		unsigned char displayTank;
		unsigned char Tank;
		unsigned char Tank2;
		unsigned char Tank3;
		unsigned char Tank4;
		unsigned char Tank5;
		unsigned char Tank6;
		unsigned char Tank7;
		unsigned char Tank8;
		unsigned char Tank9;
		unsigned char Red;
		unsigned char Green;
		unsigned char Blue;
		unsigned char Member;
		int RentalCity;

		int Points;
		int Orbs;
		int Assists;
		int Deaths;
		int MonthlyPoints;
		int City;
		int Mayor;
		int State;
		float lastMove;
		float lastShot;

		bool isDead;
		float timeDeath;
		bool isCloaked;
		float timeUncloak;

		int Socket;
		char Buffer[2048];
		int BufferLength;

		void JoinGame();
		void StartJoin();
		void LeaveGame(bool showMessage);
		void LeaveGame(bool showMessage, bool transferMayor);
		void LoggedIn(string User);
		void Death(int killer);
		void Clear();
		void Clear(bool triggerServerReset);
		void setCloak(bool isCloaked);
		void setMayor(bool isMayor);
		void setMayor(bool isMayor, bool requireInGame);
		int FindApplyMayor();

		bool isAdmin();
		bool isInApply();
		bool isInChat();
		bool isConnected();
		bool isInGame();
		bool isInGameApplyOrChat();
		int getTileX();
		int getTileY();

		CPlayer(CServer *Server, int id);
		~CPlayer();

	private:
		CServer *p;
		void ResetPlayer();

};

#endif
