#ifndef _CGAME
#define _CGAME

#define VERSION "3.4.8"

const unsigned char MaxPlayers = 63;
const unsigned char SectorSize = 16;
const unsigned char MaxSectors = (512 / SectorSize);
const short int RadarSize = 2400;

const int COST_BUILDING = 500000;
const int DISTANCE_MAX_FROM_CC = 1400;
const int TIMER_CHANGE_TANK = 1000;
const int TIMER_DEMOLISH = 3000;
const int TIMER_RELOAD_SURFACES = 3000;
const int TIMER_RESPAWN = 10000;
const int TIMER_SHOOT_ADMIN = 50;
const int TIMER_SHOOT_LASER = 650;
const int TIMER_SHOOT_ROCKET = 650;

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h>
#include <string>
#include <sstream>
#include <iostream> 
#include <fstream>
using namespace std;

#define DIRECTINPUT_VERSION 0X0800
#include <ddraw.h>
#include <dinput.h>

#include "resource.h"

#include "CLogin.h"
#include "CNew.h"
#include "CPersonality.h"
#include "CMeeting.h"
#include "CInGame.h"
#include "CInterview.h"
#include "CPersonnel.h"
#include "COptions.h"
#include "CHelp.h"
#include "CManage.h"
#include "CRecover.h"
#include "CAdmin.h"
#include "CAdminEdit.h"

#include "CDialog.h"
#include "CProcess.h"
#include "CDDraw.h"
#include "CEngine.h"
#include "CSend.h"
#include "CPlayer.h"
#include "CMap.h"
#include "CCollision.h"
#include "CBuilding.h"
#include "CInput.h"
#include "CSound.h"
#include "CBullet.h"
#include "CItem.h"
#include "CWinsock.h"
#include "CExplode.h"
#include "CTimer.h"
#include "CDrawing.h"

#include "NetMessages.h"
#include "Structs.h"

#include "SHA1.h"

class CLogin;
class CNew;
class CPersonality;
class CMeeting;
class CInGame;
class CInterview;
class CPersonnel;
class COptions;
class CHelp;
class CManage;
class CVerify;
class CRecover;
class CAdmin;
class CAdminEdit;

class CDialog;
class CWinsock;
class CProcess;
class CDDraw;
class CEngine;
class CSend;
class CPlayer;
class CMap;
class CCollision;
class CBuilding;
class CBuildingList;
class CInput;
class CSound;
class CBullet;
class CBulletList;
class CItem;
class CItemList;
class CInventory;
class CExplosion;
class CExplosionList;
class CTime;
class CDrawing;

enum States {
	Empty,
	STATE_LOGIN,
	STATE_RECOVER,
	STATE_JOINING,
	STATE_GAME,
	STATE_NEWACCOUNT,
	STATE_EDITING,
	STATE_PERSONALITY,
	STATE_VERIFY,
	STATE_MEETING,
	STATE_INTERVIEW,
};

enum PanelModes {
	modeEmpty,
	modeFinance,
};

class CGame
{
public:
	void Init(HWND hWnd, HINSTANCE hInst);

	string ReturnUniqID();
	int CheckFile(string file);
	int SilentCheckFile(string file);
	void Restructure();

	void Cycle();
	int running;
	int State;
	HWND hWnd;
	HINSTANCE hInst;
	
	CGame();
	~CGame();

	CLogin *Login;
	CNew *NewAccount;
	CPersonality *Personality;
	CMeeting *Meeting;
	CInGame *InGame;
	CInterview *Interview;
	CPersonnel *Personnel;
	COptions *Options;
	CHelp *Help;
	CManage *Manage;
	CVerify *Verify;
	CRecover *Recover;
	CAdmin *Admin;
	CAdminEdit *AdminEdit;

	CDialog *Dialog;
	CWinsock *Winsock;
	CProcess *Process;
	CEngine *Engine;
	CDDraw *DDraw;
	CSend *Send;
	CPlayer *Player[MaxPlayers];
	CMap *Map;
	CCollision *Collision;
	CBuildingList *Build;
	CItemList *Item;
	CInput *Input;
	CSound *Sound;
	CInventory *Inventory;
	CBulletList *Bullet;
	CExplosionList *Explode;
	CDrawing *Draw;

	CTime *Timer;
	float Tick;
	float lastTick;
	float TimePassed;

	int ResolutionX;
	int ResolutionY;
private:
protected:
};

#endif
