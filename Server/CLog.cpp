#include "CServer.h"

CLog::CLog(CServer *Server)
{
	p = Server;
}

CLog::~CLog()
{

}

void CLog::logServerError(std::string error)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//debug//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;
	output += linebreak;
	output += ftime;
	output += " - " + error;

	fwrite(output.c_str(), output.size(), 1, file); 

	fclose(file);

	cout << "WARNING:  Server Crash.  Code: " << error << "\n";
}

void CLog::logClientError(std::string error, int Index)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//debug//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;

	output = linebreak;
	output += ftime;
	output += " - ";
	output.append("WARNING:  Client Crash.  Code:  ");
	output.append(error + " from ");
	output.append(p->Player[Index]->Name);

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);

	output.clear();

	output = "WARNING:  Client Crash.  Code: ";
	output.append(error + " from ");
	output.append(p->Player[Index]->Name);
	output.append("\n");
	cout << output << endl;

	error.clear();
}

void CLog::logChat(std::string chat)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//chat//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += chat;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

void CLog::logAdmin(std::string admin, int Index)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//admin//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += p->Player[Index]->Name;
	output += " - ";
	output += admin;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

void CLog::logAccount(std::string account)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//account//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += account;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

void CLog::logOrb(int Orber, int EnemyCity, int BuildCount)
{
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	string filepath;
	filepath = "logs//orb//";
	filepath += fdate;
	filepath += ".log";

	FILE *file;
	file = fopen(filepath.c_str(),"a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	string output;
	string PlayersHome;
	string PlayersEnemy;

	PlayersHome.clear();
	for (int pl = 0; pl < MaxPlayers; pl++)
	{
		if (p->Player[pl]->City == p->Player[Orber]->City)
		{
			PlayersHome += p->Player[pl]->Name;
			PlayersHome += "  ";
		}
	}

	PlayersEnemy.clear();
	for (int pl = 0; pl < MaxPlayers; pl++)
	{
		if (p->Player[pl]->City == EnemyCity)
		{
			PlayersEnemy += p->Player[pl]->Name;
			PlayersEnemy += "   ";
		}
	}

	int pointsgiven = 0;

	if (BuildCount < 15)
	{
		pointsgiven = 20;
	}
	else if (BuildCount < 20)
	{
		pointsgiven = 30;
	}
	else
	{
		pointsgiven = 50;
	}

	stringstream PointsString;
	PointsString << pointsgiven;

	output = linebreak;
	output += ftime;
	output += "  (";
	output += PlayersHome;
	output += ") orbed (";
	output += PlayersEnemy;
	output += ") Points:";
	output += PointsString.str();

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}
