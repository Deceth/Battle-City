#include "CServer.h"

/***************************************************************
 * Constructor
 *
 **************************************************************/
CServer::CServer() {

	// Print startup messages
	cout << "Battle City Server Version " << VERSION << endl;
	cout << "(C) Copyright 2005-2008 Looble Network" << endl;
	cout << endl;

	// Create objects
	Winsock = new CSocket(this);
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

	// Pause...
	this->PlatformSleep(1000);

	// Reset the game state
	this->reset();

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

/***************************************************************
 * Function:	Init
 *
 **************************************************************/
void CServer::Init() {

	cout << "Server::Start" << endl;

	// Start the timer, start listening on the socket
	cout << " - Initializing Timer" << endl;
	this->Timer->Initialize();
	cout << " - Initializing Winsock" << endl;
	this->Winsock->InitWinsock();

	// Load the database, files, and news
	cout << " - Checking Files" << endl;
	this->CheckFilesAndPaths();
	cout << " - Loading Database" << endl;
	this->Database->LoadDatabase();
	cout << " - Loading News" << endl;
	this->LoadNews();

	#ifdef WIN32
		timeBeginPeriod(1);
	#endif

	cout << "Server::Start::Success" << endl << endl;
}

/***************************************************************
 * Function:	FreePlayer
 *
 **************************************************************/
int CServer::FreePlayer() {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If no player is connected in that slot,
		if (Player[i]->State == State_Disconnected) {

			// Return that index
			return i;
		}
	}

	return 0;
}

/***************************************************************
 * Function:	PlatformSleep
 *
 * @param ms
 **************************************************************/
void CServer::PlatformSleep(int ms) {
	#ifdef WIN32
		Sleep(ms);
	#else
		sleep(ms);
	#endif
}

/***************************************************************
 * Function:	PlatformCaseCompare
 *
 * @param String1
 * @param String2
 **************************************************************/
int CServer::PlatformCaseCompare(string String1, string String2) {
	#ifdef WIN32
		return stricmp(String1.c_str(), String2.c_str());
	#else
		return strcasecmp(String1.c_str(), String2.c_str());
	#endif
}

/***************************************************************
 * Function:	LoadNews
 *
 **************************************************************/
void CServer::LoadNews() {
	char buffer[1024];

	cout << "News::Load" << endl << endl;

	// No news is bad news
	if (Exists("news.txt") == 0) {
		running = 0;
		cout << "News::Load::News.txt does not exist" << endl;
		return;
	}

	// If you can't open the file, exit
	ifstream NewsFile ("news.txt");
	if (! NewsFile.is_open()) {
		cout << "Error opening file";
		exit (1);
	}

	// Read the file, appending the contents to News
	while (! NewsFile.eof() ) {
		NewsFile.getline (buffer,1024);
		News += buffer;
		News += "\r\n";
	}

	// Log
	cout << News;
	cout << "News::Load::Success" << endl << endl;
}

/***************************************************************
 * Function:	CheckFilesAndPaths
 *
 **************************************************************/
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

/***************************************************************
 * Function:	Exists
 *
 * @param Path
 **************************************************************/
int CServer::Exists(string Path) {
	int Exists = 0;
	fstream fin;

	// If we can open the file, set Exists to 1
	fin.open(Path.c_str(),ios::in);
	if( fin.is_open() ) {
		Exists = 1;
	}
	fin.close();

	return Exists;
}

/***************************************************************
 * Function:	TotalPlayers
 *
 **************************************************************/
int CServer::TotalPlayers() {
	int totalPlayers = 0;

	// For every possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {

		// If the player is in Chat, Game, or Apply, increment the count
		if ( Player[j]->isInGameApplyOrChat() ) {
			totalPlayers++;
		}
	}

	return totalPlayers;
}

/***************************************************************
 * Function:	ChangeNews
 *
 * @param NewNews
 **************************************************************/
void CServer::ChangeNews(string NewNews) {
	// Change the news on the server
	News = NewNews;
	News += "\r\n";

	// Change the news int he file
	ofstream NewsFile ("news.txt");
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
		if (this->Player[i]) {
			this->Player[i]->Clear(false);
		}
		else {
			this->Player[i] = new CPlayer(this, i);
		}
	}

	cout << " - Reloading Map and CCs" << endl;
	this->Map->LoadMap();

	cout << " - Reseting starting CC" << endl;
	this->Send->ResetStartingCC();

	cout << "Reset::Success" << endl << endl;
}
