/*
===============================================================================

    Battle City - CServer file
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
#include "CServer.h"

/// <summary>
/// Initializes <see cref="CServer"/> class.
/// </summary>
CServer::CServer() {
    //  License information
    cout << endl;
    cout << "Battle City Copyright (C) 2005-2013  battlecity.org" << endl;
    cout << "===========================================================" << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
    cout << "for details visit http://www.gnu.org/licenses/." << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions; visit http://www.gnu.org/licenses" << endl; 
    cout << "for details." << endl;
    cout << "===========================================================" << endl;
    cout << endl;
	cout << "Battle City Server Version " << VERSION << endl;
	cout << endl;
    //  Socket abstraction
	Winsock = new CSocket(this);
    //  Process abstraction
	Process = new CProcess(this);
	Account = new CAccount(this);
	Send = new CSend(this);
	Collision = new CCollision(this);
	Map = new CMap(this);
	Timer = new CTime();
	Log = new CLog(this);
	EMail = new CEMail(this);
	Database = new CDatabase(this);
	//Build = new CBuildingList(this);
	//Item = new CItemList(this);
	//Bullet = new CBulletList(this);

	// Create the players
	for (int i = 0; i < MAX_PLAYERS; i++) {
		this->Player[i] = new CPlayer(this, i);
	}

	// Pause...
	this->PlatformSleep(1000);

	// Reset the game state
	this->reset();

	// Pause...
	this->PlatformSleep(1000);

	// Set running to 1
	this->running = 1;
}

/***************************************************************
 * Destructor
 * 
 **************************************************************/
CServer::~CServer() {
	delete Winsock;
	delete Process;
	delete Account;
	delete Send;
	delete Build;
	delete Item;
	delete Collision;
	delete Bullet;
	delete Map;
	delete Timer;
	delete Log;
	delete EMail;
	delete Database;

	// For each possible player, delete the player object
	for (int i = 0; i < MAX_PLAYERS; i++) {
		delete Player[i];
	}

	// For each possible city, delete the city object
	for (int i = 0; i < MAX_CITIES; i++) {
		delete City[i];
	}

	#ifdef WIN32
		timeEndPeriod(1);
	#endif
}

/// <summary>
/// Initializes CServer object
/// </summary>
void CServer::Init() {
    //  Output to the console
	cout << "CServer::Init()" << endl;
	cout << "+ CServer::Init() @ Timer->Initialize" << endl;
    //  Initialize Timer object
	this->Timer->Initialize();
    cout << "+ CServer::Init() @ Winsock->InitWinsock" << endl;
    //  Initialize Winsock object
	this->Winsock->InitWinsock();
    cout << "+ CServer::Init() @ Check files and paths" << endl;
    //  Verify files and paths
	this->CheckFilesAndPaths();
	cout << "+ CServer::Init() @ Database->LoadDatabase" << endl;
    //  Load database into object
	this->Database->LoadDatabase();
    cout << "+ CServer::Init() @ Loading news" << endl;
    //  Load news into Server object
	this->LoadNews();
    //  Initiate minimum resolution for periodic timers
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/dd757624(v=vs.85).aspx
	#ifdef WIN32
		timeBeginPeriod(1);
	#endif
	cout << "CServer::Init() @ Finish" << endl;
}

/// <summary>
/// Locates an empty player slot
/// </summary>
/// <returns></returns>
int CServer::FreePlayer() {
    //  Loop through available player slots
	for (int i = 0; i < MAX_PLAYERS; i++) {
        //  Upon verifying player slot is empty return slot id
		// If no player is connected in that slot,
		if (Player[i]->State == State_Disconnected) {
			return i;
		}
	}
    //  Return zero meaning no available slots
	return 0;
}


/// <summary>
/// Process goes to sleep for defined time
/// </summary>
/// <param name="ms">Milliseconds</param>
void CServer::PlatformSleep(int ms) {
    //  Suspends the execution of the current thread until the time-out interval elapses
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms686298(v=vs.85).aspx
	#ifdef WIN32
		Sleep(ms);
	#else
		sleep(ms);
	#endif
}

/// <summary>
/// Performs a lowercase comparison of strings
/// </summary>
/// <param name="String1">The string1.</param>
/// <param name="String2">The string2.</param>
/// <returns></returns>
int CServer::PlatformCaseCompare(string String1, string String2) {
	#ifdef WIN32
		return _stricmp(String1.c_str(), String2.c_str());
	#else
		return strcasecmp(String1.c_str(), String2.c_str());
	#endif
}

/// <summary>
/// Load news into Server object
/// </summary>
void CServer::LoadNews() {
	char buffer[1024];
	cout << "CServer::LoadNews() @ Loading" << endl << endl;
    //  Upon verifying news file does not exist create a empty news object
	if (Exists("news.txt") == 0) {
		cout << "CServer::LoadNews() @ news.txt does not exist" << endl << endl;
        //  Set news to empty string
        News = "";
	} else {
        //  Verify news file can be opened
	    ifstream NewsFile ("news.txt");
	    if (! NewsFile.is_open()) {
		    cout << "CServer::LoadNews() @ Failed to open news.txt" << endl << endl;
		    News = "";
	    } else {
	        //  Loop through news file and populate News variable
	        while (! NewsFile.eof() ) {
		        NewsFile.getline (buffer,1024);
		        News += buffer;
		        News += "\r\n";
	        }
        }
    }

	// Log
	cout << News;
	cout << "CServer::LoadNews() @ Finish" << endl << endl;
}

/// <summary>
/// Create directories and files for dat and log
/// </summary>
void CServer::CheckFilesAndPaths() {
	#ifndef WIN32
		mkdir("dat", 777);
		mkdir("logs", 777);
			mkdir("logs//orb", 777);
			mkdir("logs//chat", 777);
			mkdir("logs//debug", 777);
			mkdir("logs//account", 777);
			mkdir("logs//admin", 777);
	#else
		mkdir("dat");
		mkdir("logs");
			mkdir("logs//orb");
			mkdir("logs//chat");
			mkdir("logs//debug");
			mkdir("logs//account");
			mkdir("logs//admin");
	#endif
}


/// <summary>
/// Verify the specified path exists
/// </summary>
/// <param name="Path">File path</param>
/// <returns></returns>
int CServer::Exists(string Path) {
	int Exists = 0;
    //  Set file stream variable
	fstream fin;
    //  Verify that path can be opened using fstream.open
	fin.open(Path.c_str(),ios::in);
	Exists = (fin.is_open() ? 1 : 0);
	fin.close();
	return Exists;
}

/// <summary>
/// Calculate the total number of players online
/// </summary>
/// <returns></returns>
int CServer::TotalPlayers() {
    //  Set total player counter
	int totalPlayers = 0;
    //  Loop through available player slots
	for (int j = 0; j < MAX_PLAYERS; j++) {
        //  Verify player is in-game, applying, or chatting
		if ( Player[j]->isInGameApplyOrChat() ) {
            //  Increment total player value
			totalPlayers++;
		}
	}
	return totalPlayers;
}

/// <summary>
/// Update the news file
/// </summary>
/// <param name="NewNews">Updated news string</param>
void CServer::ChangeNews(string NewNews) {
    //  Store newNews as News and add end line characters
	News = NewNews;
	News += "\r\n";
    //  Open output file stream handler to news.txt
	ofstream NewsFile ("news.txt");
    //  
	NewsFile.write(News.c_str(), (int)News.length());
	NewsFile.close();

	this->Account->GenerateTop20();
}

/***************************************************************
 * Function:	respawnPlayers
 *
 * @param NewNews
 **************************************************************/
void CServer::respawnPlayers() {
	sSMStateGame game;
	char respawnPacket[2];
	CPlayer* player;
	CCity* city;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->Player[i];

		// If the player is in game,
		if (player->isInGame()) {

			// If the player is dead, and the Respawn timer is up,
			if ((player->isDead == true) && ((this->Tick - player->timeDeath) > TIMER_RESPAWN)) {
				city = this->City[player->City];

				// Move the player
				player->lastMove = this->Tick;
				player->X = (float)city->x;
				player->Y = (float)city->y;

				// Tell everyone the player moved
				game.City = player->City;
				game.x = (unsigned short)city->x;
				game.y = (unsigned short)city->y;
				this->Winsock->SendData(i, smWarp,(char *)&game,sizeof(game));

				// Respawn the player
				player->isDead = false;
				respawnPacket[0] = i;
				respawnPacket[1] = 0;
				this->Send->SendAllBut(-1, smRespawn, respawnPacket, 1);
			}
		}
	}
}

/***************************************************************
 * Function:	reset
 *
 **************************************************************/
void CServer::reset() {

	// If any player is connected, return
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (this->Player[i] && this->Player[i]->isConnected()) {
			return;
		}
	}

	cout << "CServer::reset()" << endl;

	// Rebuild the Build list
	cout << " - Rebuilding building list" << endl;
	delete this->Build;
	this->Build = new CBuildingList(this);

	// Rebuild the Item list
	cout << " - Rebuilding item list" << endl;
	delete this->Item;
	this->Item = new CItemList(this);

	// Rebuild the Bullet list
	cout << " - Rebuilding bullet list" << endl;
	delete this->Bullet;
	this->Bullet = new CBulletList(this);

	// Rebuild each possible city (must happen after destroying buildings list)
	cout << " - Rebuilding cities" << endl;
	for (int i = 0; i < MAX_CITIES; i++) {
		delete this->City[i];
		this->City[i] = new CCity(this, i);
	}

	// Rebuild each possible player
	cout << " - Rebuilding players" << endl;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		this->Player[i]->Clear(false);
	}

	cout << " - Reloading Map and CCs" << endl;
	this->Map->LoadMap();

	cout << " - Reseting starting CC" << endl;
	this->Send->ResetStartingCC(true);

	cout << "Reset::Success" << endl << endl;
}
