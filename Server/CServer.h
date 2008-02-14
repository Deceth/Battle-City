#ifndef _CSERVER
#define _CSERVER

#define VERSION "3.4.6"

const unsigned char MaxPlayers = 63;
const unsigned char SectorSize = 16;
const unsigned char MaxSectors = (512 / SectorSize);
const short int RadarSize = 1800;

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

enum {State_Disconnected, State_Connected, State_Verified, State_Editing, State_Chat, State_Game, State_Apply};

#include "NetMessages.h"
#include "Structs.h"
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

class CServer
{
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

	int running;
	
	CServer();
	~CServer();

	CSocket *Winsock;
	CPlayer *Player[MaxPlayers];
	CCity *City[64];
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
protected:
};

#endif
