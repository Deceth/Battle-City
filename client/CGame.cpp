/*
===============================================================================

    Battle City - CGame
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
#include "CGame.h"
#include "NetMessages.h"

CGame::CGame() {
	ConnectionManager = new CConnectionManager(this);
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

/// <summary>   Destructor. </summary>
CGame::~CGame() {
    //  Set running state to zero
	running = 0;
    //  Deallocate memory blocks
    //  http://msdn.microsoft.com/en-us/library/h6227113.aspx
    delete ConnectionManager;
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
    //  Deallocate each player memory block
	for (int i = 0; i < MAX_PLAYERS; i++) {
		delete Player[i];
	}
    //  Clears previously set minimum timer resolution
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/dd757626(v=vs.85).aspx
	timeEndPeriod(1);
    //  Destruct the DirectDraw object
	DDraw->~CDDraw();
}

/// <summary>   Initialize CGame object </summary>
///
/// <param name="hWnd">     Window handler </param>
/// <param name="hInst">    Instance handler </param>
void CGame::Init(HWND hWnd, HINSTANCE hInst) {
    //  Set up dereference pointer to file
	FILE *file;
    //  Define kicked enumerated variable
	sKicked kicked;
    //  Define map hash
	CSHA1 mapHash;
    //  @todo document
	char szReport[96];
    //  Set window handler
	this->hWnd = hWnd;
    //  Set instance handler
	this->hInst = hInst;
    //  Move files to parent directory
	Restructure();

	// Verify files have been moved to the appropriate directory
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
		//  Close game upon verifying an image failed to load
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
	}
	//  Initialize the time interval object
	this->Timer->Initialize();
    //  Initialize the Winsock object
    if(!Options->gameServerAddress.empty()) {
	    Winsock->Init(&Options->gameServerAddress[0]);
    }

    //  Upon verifying windowed mode initialize direct draw for windowed mode
    //  Otherwise, initialize direct draw for full screen mode
    //  Pass the window handler to this function
	if (Options->fullscreen == 0) {
		this->DDraw->InitWindowed(hWnd);
	} else {
		this->DDraw->InitFullscreen(hWnd);
	}
    //  Clear the Direct Draw rendering target
	this->DDraw->Clear();
    //  Draw the imgCompany resource at the specified location using the specified size
	this->DDraw->Draw(this->DDraw->imgCompany, (this->ResolutionX/2), (this->ResolutionY/2)-240, 320, 240);
    //  Draw the imgCompany resource at the specified location using the specified size
	this->DDraw->Draw(this->DDraw->imgCompany, (this->ResolutionX/2)-320, (this->ResolutionY/2), 320, 240);
    //  Draw the imgCompany resource at the specified location using the specified size
	this->DDraw->Draw(this->DDraw->imgBCLogo, (this->ResolutionX/2)-320, (this->ResolutionY/2)-240, 320, 240);
    //  Draw the imgCompany resource at the specified location using the specified size
	this->DDraw->Draw(this->DDraw->imgBCLogo, (this->ResolutionX/2), (this->ResolutionY/2), 320, 240);
    // Perform bit block transfer
	this->DDraw->Flip();
    //  Initialize the dialog box engine
	this->Dialog->Start();
    //  Populates this->map and this->tiles with map.dat file
	this->Map->LoadMap();
    //  Initialize the sound engine
	this->Sound->Init();
    //  Initialize DirectInput, acquire devices
	this->Input->StartDInput();
    //  Initialize connection with game server
	this->Winsock->StartTCP();
    //  Initialize the game engine
	this->Engine->Init();
    //  Set kicked time interval to zero
	kicked.tick = 0;
    //  Populate file with file handler for imgHealthy.bmp
	file = fopen("imgHealthy.bmp","r");
	//  Upon verifying file exists confirm user has been kicked
	if (file) {
        //  Populate kicked with file contents
		fread(&kicked, sizeof(kicked), 1, file);
        //  Close file handler
		fclose(file);
		// Upon verifying current time interval is less than kicked time interval or current time interval is greater than kicked time interval plus 600000
		// remove imgHealth.bmp; otherwise, user is still kicked from game server; therefore, cannot log into game server
		if (GetTickCount() < kicked.tick || GetTickCount() > kicked.tick + 600000) {
			remove("imgHealthy.bmp");
		} else {
            //  Close socket to game server
			Winsock->Socket = 0;
            //  Display message box to user informing that they have been kicked.
			MessageBox(hWnd, "You have been kicked within 10 minutes ago.  You must wait before logging on again.", "BattleCity", 0);
            //  Send WM_CLOSE message to window handler
			SendMessage(hWnd, WM_CLOSE, 0, 0);
            //  Break from logic
			return;
		}
	}
    //  Perform 

	// Hashes file contents into the current state
	mapHash.HashFile("map.dat");
    // Computes the final SHA1 message digest
	mapHash.Final();
    //  Populate szReport with zeros
	memset(szReport, 0, sizeof(szReport));
    //  Populate szReport with message digest in hex
	mapHash.ReportHash(szReport, CSHA1::REPORT_HEX);
    //  Upon verifying szReport matches string is not identical disconnect from game server reporting the map.dat file is corrupt
	if (strcmp(szReport, "74 2E 00 1A 48 05 AD A1 A8 8B E1 5E 1E 0F 75 1A 82 B6 E8 B7") != 0) {
        //  Disconnect from game server
		this->Winsock->Socket = 0;
        //  Display message box that map.dat is corrupt
		MessageBox(hWnd, "Your map.dat is corrupted.  Please redownload the client at battlecity.org!", "BattleCity", 0);
        //  Send WM_CLOSE to window handler
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

/// <summary>   Verify existence of file </summary>
///
/// <param name="file"> File to verify </param>
///
/// <returns>  True or false file exists </returns>
int CGame::CheckFile(string file)
{
    //  Set existence indicator to false
	int flag = 0;
    //  Set file stream
	fstream fin;
    //  Open file stream using file argument with read-only attribute
	fin.open(file.c_str(),ios::in);
    //  Verify file is open
	if( fin.is_open() )
	{
		flag = 1;
	}
	fin.close();
    if(flag) return 1;
    //  Define error string for message box
	string ErrorString;
    //  Set error string
	ErrorString = "Unable to load resource, " + file + ".  Please visit battlecity.org and download the latest client to fix this problem!";
    //  Open message box reporting the error message.
	MessageBox(hWnd, ErrorString.c_str(), "Error", 0);
	return 0;
}

/// <summary>   Verifies that a file exists </summary>
///
/// <param name="file"> File to verify existence </param>
///
/// <returns>   false/true </returns>
int CGame::SilentCheckFile(string file)
{
    //  Set existence indicator to false
	int flag = 0;
    //  Set file stream
	fstream fin;
    //  Open file stream using file argument with read-only attribute
	fin.open(file.c_str(),ios::in);
    //  Verify file is open
	if( fin.is_open() )
	{
		flag = 1;
	}
    //  Close file stream
	fin.close();

    return flag;
}
/// <summary>   Method will go through all assets and rename files accordingly </summary>
void CGame::Restructure()
{
	if (SilentCheckFile("data//imgArrows.bmp") == 1) rename("data//imgArrows.bmp", "imgArrows.bmp");
	if (SilentCheckFile("data//imgArrowsRed.bmp") == 1) rename("data//imgArrowsRed.bmp", "imgArrowsRed.bmp");
	if (SilentCheckFile("data//imgBCLogo.bmp") == 1) rename("data//imgBCLogo.bmp", "imgBCLogo.bmp");
	if (SilentCheckFile("data//imgBlackNumbers.bmp") == 1) rename("data//imgBlackNumbers.bmp", "imgBlackNumbers.bmp");
	if (SilentCheckFile("data//imgBtnStaff.bmp") == 1) rename("data//imgBtnStaff.bmp", "imgBtnStaff.bmp");
	if (SilentCheckFile("data//imgBuildIcons.bmp") == 1) rename("data//imgBuildIcons.bmp", "imgBuildIcons.bmp");
	if (SilentCheckFile("data//imgBuildings.bmp") == 1) rename("data//imgBuildings.bmp", "imgBuildings.bmp");
	if (SilentCheckFile("data//imgBullets.bmp") == 1) rename("data//imgBullets.bmp", "imgBullets.bmp");
	if (SilentCheckFile("data//imgCompany.bmp") == 1) rename("data//imgCompany.bmp", "imgCompany.bmp");
	if (SilentCheckFile("data//imgCursor.bmp") == 1) rename("data//imgCursor.bmp", "imgCursor.bmp");
	if (SilentCheckFile("data//imgDemolish.bmp") == 1) rename("data//imgDemolish.bmp", "imgDemolish.bmp");
	if (SilentCheckFile("data//imgGround.bmp") == 1) rename("data//imgGround.bmp", "imgGround.bmp");
	if (SilentCheckFile("data//imgHealth.bmp") == 1) rename("data//imgHealth.bmp", "imgHealth.bmp");
	if (SilentCheckFile("data//imgInterface.bmp") == 1) rename("data//imgInterface.bmp", "imgInterface.bmp");
	if (SilentCheckFile("data//imgInterfaceBottom.bmp") == 1) rename("data//imgInterfaceBottom.bmp", "imgInterfaceBottom.bmp");
	if (SilentCheckFile("data//imgInventorySelection.bmp") == 1) rename("data//imgInventorySelection.bmp", "imgInventorySelection.bmp");
	if (SilentCheckFile("data//imgItems.bmp") == 1) rename("data//imgItems.bmp", "imgItems.bmp");
	if (SilentCheckFile("data//imgLava.bmp") == 1) rename("data//imgLava.bmp", "imgLava.bmp");
	if (SilentCheckFile("data//imgLExplosion.bmp") == 1) rename("data//imgLExplosion.bmp", "imgLExplosion.bmp");
	if (SilentCheckFile("data//imgLoading.bmp") == 1) rename("data//imgLoading.bmp", "imgLoading.bmp");
	if (SilentCheckFile("data//imgMiniMapColors.bmp") == 1) rename("data//imgMiniMapColors.bmp", "imgMiniMapColors.bmp");
	if (SilentCheckFile("data//imgMuzzleFlash.bmp") == 1) rename("data//imgMuzzleFlash.bmp", "imgMuzzleFlash.bmp");
	if (SilentCheckFile("data//imgPopulation.bmp") == 1) rename("data//imgPopulation.bmp", "imgPopulation.bmp");
	if (SilentCheckFile("data//imgRadarColors.bmp") == 1) rename("data//imgRadarColors.bmp", "imgRadarColors.bmp");
	if (SilentCheckFile("data//imgRocks.bmp") == 1) rename("data//imgRocks.bmp", "imgRocks.bmp");
	if (SilentCheckFile("data//imgSExplosion.bmp") == 1) rename("data//imgSExplosion.bmp", "imgSExplosion.bmp");
	if (SilentCheckFile("data//imgSmoke.bmp") == 1) rename("data//imgSmoke.bmp", "imgSmoke.bmp");
	if (SilentCheckFile("data//imgTurretBase.bmp") == 1) rename("data//imgTurretBase.bmp", "imgTurretBase.bmp");
	if (SilentCheckFile("data//imgTurretHead.bmp") == 1) rename("data//imgTurretHead.bmp", "imgTurretHead.bmp");
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

			// If GLOBAL and ADMIN,
			if ((this->InGame->ChatLine.find("/g",0) == 0) && (this->Player[this->Winsock->MyIndex]->isAdmin())) {
				this->Send->SendGlobal();
			}

			// Else, if PM,
			else if (this->InGame->ChatLine.find("/pm",0) == 0) {
				this->Send->SendWhisper();
			}

			// Else, if Load,
			else if (this->InGame->ChatLine.find("/load ",0) == 0) {
				this->InGame->requestAutoBuild(this->InGame->ChatLine);
			}

			// Else, if Save,
			else if (this->InGame->ChatLine.find("/save ",0) == 0) {
				this->InGame->saveCity(this->InGame->ChatLine);
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

									