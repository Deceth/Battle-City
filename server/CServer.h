/*
===============================================================================

    Battle City - CServer header file
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
#ifndef _CSERVER
#define _CSERVER

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>

using namespace std;

#ifndef WIN32
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <termios.h>
	#include <string>
	#include <arpa/inet.h>
#else
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <windows.h>
	#include <process.h>
	#include <direct.h>
	#include <mmsystem.h>
	#include <tchar.h>
	#include <conio.h>
	#include "pthread.h"
#endif

#include "CConstants.h"

#include "NetMessages.h"
#include "CSQLite3.h"

#include "CSocket.h"
#include "CPlayer.h"
#include "CProcess.h"
#include "CAccount.h"
#include "CCity.h"
#include "CBuilding.h"
#include "CBullet.h"
#include "CSend.h"
#include "CItem.h"
#include "CCollision.h"
#include "CMap.h"
#include "CTimer.h"
#include "CLog.h"
#include "CEMail.h"
#include "CDatabase.h"

class CSocket;
class CPlayer;
class CProcess;
class CAccount;
class CCity;
class CBuilding;
class CBuildingList;
class CSend;
class CBullet;
class CBulletList;
class CItem;
class CItemList;
class CCollision;
class CMap;
class CTime;
class CLog;
class CEMail;
class CDatabase;

class CServer {

	public:
		void Init();

		string Top20;
		string News;

		int FreePlayer();
		int TotalPlayers();

		void PlatformSleep(int ms);
		int PlatformCaseCompare(string String1, string String2);

		void CheckFilesAndPaths();
		int Exists(string Path);
		void LoadNews();
		void ChangeNews(string NewNews);
		void respawnPlayers();
		void reset();

		int running;
		
		CServer();
		~CServer();

		CSocket *Winsock;
		CPlayer *Player[MAX_PLAYERS];
		CCity *City[MAX_CITIES];
		CProcess *Process;
		CAccount *Account;
		CBuildingList *Build;
		CItemList *Item;
		CBulletList *Bullet;
		CCollision *Collision;
		CMap *Map;
		CSend *Send;
		CLog *Log;
		CEMail *EMail;
		CDatabase *Database;

		CTime *Timer;
		float Tick;
		float lastTick;
		float TimePassed;

	private:

};

#endif
