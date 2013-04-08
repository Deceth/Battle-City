/*
===============================================================================

    Battle City - CPlayer header file
    Copyright (C) 2005-2013  battlecity.org

    This file is part of Battle City.

    Battle City is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battle City is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battle City.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/
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
