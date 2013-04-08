#ifndef _CGAME
#define _CGAME

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
#define DIRECTINPUT_VERSION 0X0800
#include <ddraw.h>
#include <dinput.h>
using namespace std;
#include "resource.h"

#include "CConstants.h"

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

class CGame
{
public:
	void Init(HWND hWnd, HINSTANCE hInst);

	string ReturnUniqID();
	int CheckFile(string file);
	int SilentCheckFile(string file);
	void Restructure();

	void Cycle();
	void handleKey(WPARAM wParam);
	void handleChatKey(WPARAM wParam);
	void handleGameKey(WPARAM wParam);

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
	CPlayer *Player[MAX_PLAYERS];
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
