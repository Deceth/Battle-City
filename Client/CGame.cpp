#include "CGame.h"
#include "NetMessages.h"

CGame::CGame() {
	
	Login = new CLogin(this);
	NewAccount = new CNew(this);
	Personality = new CPersonality(this);
	Meeting = new CMeeting(this);
	InGame = new CInGame(this);
	Interview = new CInterview(this);
	Personnel = new CPersonnel(this);
	Options = new COptions(this);
	Help = new CHelp(this);
	Manage = new CManage(this);
	Recover = new CRecover(this);
	Admin = new CAdmin(this);
	AdminEdit = new CAdminEdit(this);

	Dialog = new CDialog(this);
	Winsock = new CWinsock(this);
	Process = new CProcess(this);
	Engine = new CEngine(this);
	Send = new CSend(this);
	Map = new CMap(this);
	Collision = new CCollision(this);
	Build = new CBuildingList(this);
	Input = new CInput(this);
	Sound = new CSound(this);
	Item = new CItemList(this);
	Inventory = new CInventory(this);
	Bullet = new CBulletList(this);
	Explode = new CExplosionList(this);
	Draw = new CDrawing(this);
	DDraw = new CDDraw(this);

	Timer = new CTime();

	for (int i = 0; i < MAX_PLAYERS; i++) {
		this->Player[i] = new CPlayer(this, i);
	}

	timeBeginPeriod(1);
	running = 1;

	Tick = 0;
	TimePassed = 0;
	lastTick = 0;

	// Load the options so we know the resolution to use
	Options->LoadOptions();
	if (this->Options->resolution1024) {
		ResolutionX = 1024;
		ResolutionY = 768;
	}
	else {
		ResolutionX = 800;
		ResolutionY = 600;
	}
}

CGame::~CGame() {
	running = 0;

	delete Login;
	delete NewAccount;
	delete Personality;
	delete Meeting;
	delete InGame;
	delete Interview;
	delete Personnel;
	delete Options;
	delete Help;
	delete Manage;
	delete Recover;
	delete Admin;
	delete AdminEdit;

	delete Dialog;
	delete Winsock;
	delete Process;
	delete Engine;
	delete Send;
	delete Map;
 	delete Collision;
	delete Build;
	delete Input;
	delete Sound;
	delete Item;
	delete Inventory;
	delete Bullet;
	delete Explode;
	delete Draw;
	delete DDraw;

	delete Timer;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		delete Player[i];
	}

	timeEndPeriod(1);
	DDraw->~CDDraw();
}

void CGame::Init(HWND hWnd, HINSTANCE hInst) {
	FILE *file;
	sKicked kicked;
	CSHA1 mapHash;
	char szReport[96];

	this->hWnd = hWnd;
	this->hInst = hInst;

	Restructure();

	// If any image files are missing,
	if (
		(CheckFile("imgArrows.bmp") == 0) ||
		(CheckFile("imgArrowsRed.bmp") == 0) ||
		(CheckFile("imgBCLogo.bmp") == 0) ||
		(CheckFile("imgBlackNumbers.bmp") == 0) ||
		(CheckFile("imgBtnStaff.bmp") == 0) ||
		(CheckFile("imgBuildIcons.bmp") == 0) ||
		(CheckFile("imgBuildings.bmp") == 0) ||
		(CheckFile("imgBullets.bmp") == 0) ||
		(CheckFile("imgCompany.bmp") == 0) ||
		(CheckFile("imgCursor.bmp") == 0) ||
		(CheckFile("imgDemolish.bmp") == 0) ||
		(CheckFile("imgGround.bmp") == 0) ||
		(CheckFile("imgHealth.bmp") == 0) ||
		(CheckFile("imgInterface.bmp") == 0) ||
		(CheckFile("imgInterfaceBottom.bmp") == 0) ||
		(CheckFile("imgInventorySelection.bmp") == 0) ||
		(CheckFile("imgItems.bmp") == 0) ||
		(CheckFile("imgLava.bmp") == 0) ||
		(CheckFile("imgLExplosion.bmp") == 0) ||
		(CheckFile("imgLoading.bmp") == 0) ||
		(CheckFile("imgMiniMapColors.bmp") == 0) ||
		(CheckFile("imgMoneyBox.bmp") == 0) ||
		(CheckFile("imgMoneyDown.bmp") == 0) ||
		(CheckFile("imgMoneyUp.bmp") == 0) ||
		(CheckFile("imgMuzzleFlash.bmp") == 0) ||
		(CheckFile("imgPopulation.bmp") == 0) ||
		(CheckFile("imgRadarColors.bmp") == 0) ||
		(CheckFile("imgResearch.bmp") == 0) ||
		(CheckFile("imgResearchComplete.bmp") == 0) ||
		(CheckFile("imgRocks.bmp") == 0) ||
		(CheckFile("imgSExplosion.bmp") == 0) ||
		(CheckFile("imgSmoke.bmp") == 0) ||
		(CheckFile("imgStar.bmp") == 0) ||
		(CheckFile("imgTanks.bmp") == 0) ||
		(CheckFile("imgTurretBase.bmp") == 0) ||
		(CheckFile("imgTurretHead.bmp") == 0) 
	) {
			
		// Error and return
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
	}

	// Initialize the timer
	this->Timer->Initialize();

	#ifndef _DEBUG
		Winsock->Init("72.167.115.50");
//		Winsock->Init("localhost");
	#else
		Winsock->Init("72.167.115.50");
//		Winsock->Init("localhost");	
	#endif

	// If fullscreen is not on, open in Windowed mode
	if (Options->fullscreen == 0) {
		this->DDraw->InitWindowed(hWnd);
	}

	// Else (fullscreen is on), open in Fullscreen mode
	else {
		this->DDraw->InitFullscreen(hWnd);
	}

	// Clear the screen, add Logos
	this->DDraw->Clear();
	this->DDraw->Draw(this->DDraw->imgCompany, (this->ResolutionX/2), (this->ResolutionY/2)-240, 320, 240);
	this->DDraw->Draw(this->DDraw->imgCompany, (this->ResolutionX/2)-320, (this->ResolutionY/2), 320, 240);
	this->DDraw->Draw(this->DDraw->imgBCLogo, (this->ResolutionX/2)-320, (this->ResolutionY/2)-240, 320, 240);
	this->DDraw->Draw(this->DDraw->imgBCLogo, (this->ResolutionX/2), (this->ResolutionY/2), 320, 240);
	this->DDraw->Flip();

	// Start the login dialog, load the map, sound, DirectInput, winsock, engine, etc
	this->Dialog->Start();
	this->Map->LoadMap();
	this->Sound->Init();
	this->Input->StartDInput();
	this->Winsock->StartTCP();
	this->Engine->Init();

	// Check whether the user got kicked within the last 10 minutes
	kicked.tick = 0;
	file = fopen("imgHealthy.bmp","r");

	// If imgHealthy was found,
	if (file) {
		fread(&kicked, sizeof(kicked), 1, file);
		fclose(file);

		// If the Kick timer is up, remove the image
		if (GetTickCount() < kicked.tick || GetTickCount() > kicked.tick + 600000) {
			remove("imgHealthy.bmp");
		}

		// Else (Kick timer still going), tell the player he can't log in yet
		else {
			Winsock->Socket = 0;
			MessageBox(hWnd, "You have been kicked within 10 minutes ago.  You must wait before logging on again.", "BattleCity", 0);
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return;
		}
	}

	// Hash the map.dat file
	mapHash.HashFile("map.dat");
	mapHash.Final();
	memset(szReport, 0, sizeof(szReport));
	mapHash.ReportHash(szReport, CSHA1::REPORT_HEX);

	// If the hash doesn't match the string below, tell the user the file is corrupted
	if (strcmp(szReport, "74 2E 00 1A 48 05 AD A1 A8 8B E1 5E 1E 0F 75 1A 82 B6 E8 B7") != 0) {
		this->Winsock->Socket = 0;
		MessageBox(hWnd, "Your map.dat is corrupted.  Please redownload the client at battlecity.looble.com!", "BattleCity", 0);
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
}



string CGame::ReturnUniqID() {
	char UniqID[50];
	memset(UniqID, 0, 50);
	GetPrivateProfileString("UniqID", "ID", "0", UniqID, 50, "froogle");

#ifndef _DEBUG
	if (strlen(UniqID) == 1) //We need to generate the ID
#endif
	{
		string NewID;
		char tmpBuffer[20];
		srand((unsigned int)GetTickCount());
		int rand1 = rand();
		memset(tmpBuffer, 0, 20);
		itoa(rand1, tmpBuffer, 10);
		NewID = tmpBuffer;
		NewID += "-";
		Sleep(10);
		srand((unsigned int)GetTickCount());
		int rand2 = rand();
		memset(tmpBuffer, 0, 20);
		itoa(rand2, tmpBuffer, 10);
		NewID += tmpBuffer;
		NewID += "-";
		Sleep(5);
		srand((unsigned int)Tick);
		int rand3 = rand();
		memset(tmpBuffer, 0, 20);
		itoa(rand3, tmpBuffer, 10);
		NewID += tmpBuffer;
		NewID += "-";
		int rand4 = (int)GetTickCount();
		itoa(rand4, tmpBuffer, 10);
		NewID += tmpBuffer;
		WritePrivateProfileString("UniqID", "ID", NewID.c_str(), "froogle");
		strcpy(UniqID, NewID.c_str());
	}

	string UniqIDStr;
	UniqIDStr = UniqID;

	return UniqIDStr;
}

int CGame::CheckFile(string file)
{
	int flag = 0;
	fstream fin;
	fin.open(file.c_str(),ios::in);
	if( fin.is_open() )
	{
		flag = 1;
	}
	fin.close();

	if (flag == 1) return 1;

	string ErrorString;
	ErrorString = "File " + file + " not found.  Please visit battlecity.looble.com and download the latest client to fix this problem!";

	MessageBox(hWnd, ErrorString.c_str(), "Error", 0);
	return 0;
}

int CGame::SilentCheckFile(string file)
{
	int flag = 0;
	fstream fin;
	fin.open(file.c_str(),ios::in);
	if( fin.is_open() )
	{
		flag = 1;
	}
	fin.close();

	if (flag == 1) return 1;

	return 0;
}

void CGame::Restructure()
{
	if (SilentCheckFile("img//imgArrows.bmp") == 1) rename("img//imgArrows.bmp", "imgArrows.bmp");
	if (SilentCheckFile("img//imgArrowsRed.bmp") == 1) rename("img//imgArrowsRed.bmp", "imgArrowsRed.bmp");
	if (SilentCheckFile("img//imgBCLogo.bmp") == 1) rename("img//imgBCLogo.bmp", "imgBCLogo.bmp");
	if (SilentCheckFile("img//imgBlackNumbers.bmp") == 1) rename("img//imgBlackNumbers.bmp", "imgBlackNumbers.bmp");
	if (SilentCheckFile("img//imgBtnStaff.bmp") == 1) rename("img//imgBtnStaff.bmp", "imgBtnStaff.bmp");
	if (SilentCheckFile("img//imgBuildIcons.bmp") == 1) rename("img//imgBuildIcons.bmp", "imgBuildIcons.bmp");
	if (SilentCheckFile("img//imgBuildings.bmp") == 1) rename("img//imgBuildings.bmp", "imgBuildings.bmp");
	if (SilentCheckFile("img//imgBullets.bmp") == 1) rename("img//imgBullets.bmp", "imgBullets.bmp");
	if (SilentCheckFile("img//imgCompany.bmp") == 1) rename("img//imgCompany.bmp", "imgCompany.bmp");
	if (SilentCheckFile("img//imgCursor.bmp") == 1) rename("img//imgCursor.bmp", "imgCursor.bmp");
	if (SilentCheckFile("img//imgDemolish.bmp") == 1) rename("img//imgDemolish.bmp", "imgDemolish.bmp");
	if (SilentCheckFile("img//imgGround.bmp") == 1) rename("img//imgGround.bmp", "imgGround.bmp");
	if (SilentCheckFile("img//imgHealth.bmp") == 1) rename("img//imgHealth.bmp", "imgHealth.bmp");
	if (SilentCheckFile("img//imgInterface.bmp") == 1) rename("img//imgInterface.bmp", "imgInterface.bmp");
	if (SilentCheckFile("img//imgInterfaceBottom.bmp") == 1) rename("img//imgInterfaceBottom.bmp", "imgInterfaceBottom.bmp");
	if (SilentCheckFile("img//imgInventorySelection.bmp") == 1) rename("img//imgInventorySelection.bmp", "imgInventorySelection.bmp");
	if (SilentCheckFile("img//imgItems.bmp") == 1) rename("img//imgItems.bmp", "imgItems.bmp");
	if (SilentCheckFile("img//imgLava.bmp") == 1) rename("img//imgLava.bmp", "imgLava.bmp");
	if (SilentCheckFile("img//imgLExplosion.bmp") == 1) rename("img//imgLExplosion.bmp", "imgLExplosion.bmp");
	if (SilentCheckFile("img//imgLoading.bmp") == 1) rename("img//imgLoading.bmp", "imgLoading.bmp");
	if (SilentCheckFile("img//imgMiniMapColors.bmp") == 1) rename("img//imgMiniMapColors.bmp", "imgMiniMapColors.bmp");
	if (SilentCheckFile("img//imgMuzzleFlash.bmp") == 1) rename("img//imgMuzzleFlash.bmp", "imgMuzzleFlash.bmp");
	if (SilentCheckFile("img//imgPopulation.bmp") == 1) rename("img//imgPopulation.bmp", "imgPopulation.bmp");
	if (SilentCheckFile("img//imgRadarColors.bmp") == 1) rename("img//imgRadarColors.bmp", "imgRadarColors.bmp");
	if (SilentCheckFile("img//imgRocks.bmp") == 1) rename("img//imgRocks.bmp", "imgRocks.bmp");
	if (SilentCheckFile("img//imgSExplosion.bmp") == 1) rename("img//imgSExplosion.bmp", "imgSExplosion.bmp");
	if (SilentCheckFile("img//imgSmoke.bmp") == 1) rename("img//imgSmoke.bmp", "imgSmoke.bmp");
	if (SilentCheckFile("img//imgTurretBase.bmp") == 1) rename("img//imgTurretBase.bmp", "imgTurretBase.bmp");
	if (SilentCheckFile("img//imgTurretHead.bmp") == 1) rename("img//imgTurretHead.bmp", "imgTurretHead.bmp");
}

/***************************************************************
 * Function:	handleKey
 *
 * @param wParam
 **************************************************************/
void CGame::handleKey(WPARAM wParam) {

	// If the player is in game,
	if (this->State == STATE_GAME) {
		
		// If the player is chatting,
		if (this->InGame->IsChatting == 1) {
			this->handleChatKey(wParam);
		}
		// Else (player is not chatting),
		else {
			this->handleGameKey(wParam);
		}
	}
}

/***************************************************************
 * Function:	handleKey
 *
 * @param wParam
 **************************************************************/
void CGame::handleChatKey(WPARAM wParam) {

	// Key: ENTER
	if (wParam == 13) {

		// Clear out of chat
		this->InGame->IsChatting = 0;

		// If there was text to send,
		if (this->InGame->ChatLine.length() > 0) {

			// If CHAT COMMAND,
			if (this->InGame->ChatLine.find("/",0) == 0) {

				// If GLOBAL and ADMIN,
				if ((this->InGame->ChatLine.find("/g",0) == 0)) {
					if ( this->Player[this->Winsock->MyIndex]->isAdmin()) {
						this->Send->SendGlobal();
					}
					else {
						this->InGame->AppendInfo("Sorry, only admins can use the /g chat command!");
					}
				}

				// Else, if PM,
				else if (
					(this->InGame->ChatLine.find("/pm ",0) == 0)
					||
					(this->InGame->ChatLine.find("/t ",0) == 0)
					||
					(this->InGame->ChatLine.find("/tell ",0) == 0)
					||
					(this->InGame->ChatLine.find("/w ",0) == 0) ) {
					this->Send->SendWhisper();
				}

				// Else, if Load,
				else if (this->InGame->ChatLine.find("/load ",0) == 0) {
					this->Process->RequestAutoBuild(this->InGame->ChatLine);
				}

				// Else, if Save,
				else if (this->InGame->ChatLine.find("/save ",0) == 0) {
					this->Process->RequestSaveCity(this->InGame->ChatLine);
				}
				// Else, Unknown command,
				else {
					this->InGame->AppendInfo("Unknown chat command!  Did you mean /pm, /load, or /save?");
				}
			}

			// Else, NORMAL,
			else {
				this->Send->SendMessage();
			}
			// Clear the chat line
			this->InGame->ChatLine.clear();
		}
	}

	// KEY: backspace
	else if (wParam == 8) {
		// If there is more than one char, delete the last
		if (this->InGame->ChatLine.length() > 1) {
			this->InGame->ChatLine.erase(this->InGame->ChatLine.length() - 1, 1);
		}
		// Else, clear the only char in the chatline
		else {
			this->InGame->ChatLine.clear();
		}
	}

	// Key: ESCAPE
	else if (wParam == 27) {
		// Clear and close the chat
		this->InGame->IsChatting = 0;
		this->InGame->ChatLine.clear();
	}

	// KEY: any
	else {
		// Append the key to the chat line
		char blargh[2];
		blargh[1] = 0;
		blargh[0] = (char)wParam;

		// TODO: test that the char is alphanumeric

		if (this->InGame->ChatLine.length() < 100) {
			this->InGame->ChatLine.append(blargh);
		}
	}
}

/***************************************************************
 * Function:	handleGameKey
 *
 * @param wParam
 **************************************************************/
void CGame::handleGameKey(WPARAM wParam) {

	// KEY: Enter
	if (wParam == 13) {
		// Open chat
		this->InGame->IsChatting = 1;
	}

	// KEY: u or F6 (and user isn't frozen)
	if (((wParam == 85) || (wParam == 117)) && (this->Player[this->Winsock->MyIndex]->isFrozen == 0)) {

		// Get the item under the user's current position
		CItem *item = this->Item->findItembyLocation(((int)this->Player[this->Winsock->MyIndex]->X + 24) / 48, ((int)this->Player[this->Winsock->MyIndex]->Y + 24) / 48);

		// If found, send a command to pick it up
		if (item) {
			sCMItem ii;
			ii.id = item->id;
			ii.active = item->active;
			ii.type = item->Type;
			this->Winsock->SendData(cmItemUp,(char *)&ii,sizeof(ii));
		}
	}

	// KEY: i or NUMPAD9
	else if ((wParam == 73) || (wParam == 105)) {
		// Trigger the info button
		this->Input->InfoButton();
	}

	// KEY: o or NUMPAD/
	else if ((wParam == 79) || (wParam == 111)) {
		// Get an orb from the user's inventory
		CItem *itm = this->Inventory->findItembyType(5);
		// If found, send a command to drop it 
		if (itm) {
			sCMItem ii;
			ii.id = itm->id;
			ii.active = itm->active;
			ii.type = itm->Type;
			this->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
		}
	}

	// KEY: b or NUMPAD2
	else if ((wParam == 66) || (wParam == 98)) {
		// Get a bomb from the user's inventory
		CItem *itm = this->Inventory->findItembyType(3);
		// If found, activate it then send a command to drop it
		if (itm) {
			sCMItem ii;
			ii.id = itm->id;
			ii.active = 1;
			ii.type = itm->Type;
			this->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
		}
	}

	// KEY: h or NUMPAD8
	else if ((wParam == 72) || (wParam == 104) ) {

		// Tell the inventory to trigger a medkit
		this->Inventory->triggerItem(ITEM_TYPE_MEDKIT);
	}

	
	// KEY: c
	else if ((wParam == 67) || (wParam == 99) ) {

		// Tell the inventory to trigger a medkit
		this->Inventory->triggerItem(ITEM_TYPE_CLOAK);
	}

	// KEY: d or NUMPAD4 
	else if ((wParam == 68) || (wParam == 100)) {
		// Drop the selected item
		this->Inventory->Drop();
	}

	// KEY: [
	else if ((wParam == 123) || (wParam == 91)) {
		//Refresh Data
		//this->Player[this->Winsock->MyIndex]->timeFrozen = this->Timer->GetTime() + 60000;
		//this->Player[this->Winsock->MyIndex]->isFrozen = 1;
	}

	// KEY: ]
	else if ((wParam == 93) || wParam == 125) {
		// Reload Surfaces
		if (this->Tick > this->Input->lastRefresh) {
			this->Input->lastRefresh = this->Tick + TIMER_RELOAD_SURFACES;
			this->DDraw->LoadSurfaces();
			this->DDraw->DirectDraw->RestoreAllSurfaces();
			this->InGame->AppendChat("The game's surfaces were reloaded.", COLOR_SYSTEM);
		}
	}

	// KEY: m or NUMPAD-
	else if ((wParam == 77) || (wParam == 109)) {

		// Center the player, freeze the player
		this->Draw->resetPlayerOffset();
		this->Player[this->Winsock->MyIndex]->isMoving = 0;

		// Toggle the minimap, and request it if it is now open
		this->InGame->ShowMap = 1 - this->InGame->ShowMap;
		if (this->InGame->ShowMap == 1) {
			this->Winsock->SendData(cmMiniMap, " ");
		}
	}

	// KEY: p or F1
	else if ((wParam == 80) || (wParam == 112)) {
		// Trigger the points button
		this->Input->PointsButton();
	}
}

									