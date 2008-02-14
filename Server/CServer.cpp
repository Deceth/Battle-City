#include "CServer.h"

CServer::CServer()
{
	running = 1;

	Winsock = new CSocket(this);
	Process = new CProcess(this);
	Account = new CAccount(this);
	Send = new CSend(this);
	Build = new CBuildingList(this);
	Item = new CItemList(this);
	Collision = new CCollision(this);
	Bullet = new CBulletList(this);
	Map = new CMap(this);
	Timer = new CTime();
	Log = new CLog(this);
	EMail = new CEMail(this);
	Database = new CDatabase(this);
	for (int i = 0; i < MaxPlayers; i++)
	{
		Player[i] = new CPlayer(this);
	}
	for (int i = 0; i < 64; i++)
	{
		City[i] = new CCity(this);
	}
}

CServer::~CServer()
{
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
	for (int i = 0; i < MaxPlayers; i++)
	{
		delete Player[i];
	}
	for (int i = 0; i < 64; i++)
	{
		delete City[i];
	}

	#ifdef WIN32
		timeEndPeriod(1);
	#endif
}

void CServer::Init()
{
	cout << "BattleCity Classic Server Version " << VERSION << endl;
	cout << "(C) Copyright 2006 CodeMallet" << endl;
	cout << endl;

	cout << sizeof(WORD);

	cout << "Server::Start" << endl << endl;

	Timer->Initialize();
	Winsock->InitWinsock();

	Map->LoadMap();
	this->CheckFilesAndPaths();
	Database->LoadDatabase();
	this->LoadNews();

	#ifdef WIN32
		timeBeginPeriod(1);
	#endif

	cout << "Server::Start::Success" << endl;
}

int CServer::FreePlayer()
{
	for (int i = 1; i < MaxPlayers; i++)
	{
		if (Player[i]->State == 0)
		{
			return i;
		}
	}

	return 0;
}

void CServer::PlatformSleep(int ms)
{
	#ifdef WIN32
		Sleep(ms);
	#else
		sleep(ms);
	#endif
}

int CServer::PlatformCaseCompare(string String1, string String2)
{
	#ifdef WIN32
		return stricmp(String1.c_str(), String2.c_str());
	#else
		return strcasecmp(String1.c_str(), String2.c_str());
	#endif
}

void CServer::LoadNews()
{
	cout << "News::Load" << endl << endl;

	if (Exists("news.txt") == 0)
	{
		running = 0;
		cout << "News::Load::News.txt does not exist" << endl;
		return;
	}

	char buffer[1024];
	ifstream NewsFile ("news.txt");
	if (! NewsFile.is_open())
	{ cout << "Error opening file"; exit (1); }

	while (! NewsFile.eof() )
	{
		NewsFile.getline (buffer,1024);
		News += buffer;
		News += "\r\n";
	}

	cout << News;

	cout << "News::Load::Success" << endl << endl;
}

void CServer::CheckFilesAndPaths()
{
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

int CServer::Exists(string Path)
{
	int Exists = 0;
	fstream fin;
	fin.open(Path.c_str(),ios::in);
	if( fin.is_open() )
	{
		Exists = 1;
	}
	fin.close();

	return Exists;
}

int CServer::TotalPlayers()
{
	int totalPlayers = 0;
	for (int j = 0; j < MaxPlayers; j++)
	{
		if (Player[j]->State >= State_Chat)
		{
			totalPlayers++;
		}
	}

	return totalPlayers;
}

void CServer::ChangeNews(string NewNews)
{
	News = NewNews;
	News += "\r\n";

	ofstream NewsFile ("news.txt");
	NewsFile.write(News.c_str(), (int)News.length());
	NewsFile.close();

	this->Account->GenerateTop20();
}