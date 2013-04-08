/*
===============================================================================

    Battle City - CLog file
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

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CLog::CLog(CServer *Server) {
	p = Server;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CLog::~CLog() {
}

/***************************************************************
 * Function:	logServerError
 *
 * @param error
 **************************************************************/
void CLog::logServerError(std::string error) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	char linebreak[3];
	string output;
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//debug//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	output += linebreak;
	output += ftime;
	output += " - " + error;

	fwrite(output.c_str(), output.size(), 1, file); 

	fclose(file);

	cout << "WARNING:  Server Crash.  Code: " << error << "\n";
}

/***************************************************************
 * Function:	logClientError
 *
 * @param error
 * @param Index
 **************************************************************/
void CLog::logClientError(std::string error, int Index) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	char linebreak[3];
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//debug//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
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

/***************************************************************
 * Function:	logChat
 *
 * @param Server
 **************************************************************/
void CLog::logChat(std::string chat) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	char linebreak[3];
	string output;

	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//chat//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += chat;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

/***************************************************************
 * Function:	logAdmin
 *
 * @param Server
 **************************************************************/
void CLog::logAdmin(std::string admin, int Index) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	char linebreak[3];
	string output;

	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//admin//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += p->Player[Index]->Name;
	output += " - ";
	output += admin;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

/***************************************************************
 * Function:	logAccount
 *
 * @param account
 **************************************************************/
void CLog::logAccount(std::string account) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	char linebreak[3];
	string output;
	
	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//account//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	output = linebreak;
	output += ftime;
	output += " - ";
	output += account;

	fwrite(output.c_str(), output.size(), 1, file); 
	fclose(file);
}

/***************************************************************
 * Function:	logOrb
 *
 * @param Orber
 * @param EnemyCity
 * @param BuildCount
 **************************************************************/
void CLog::logOrb(int Orber, int EnemyCity, int BuildCount) {
	time_t mytime;
	struct tm *today;
	char ftime[30];
	char fdate[30];
	string filepath;
	FILE *file;
	string output;
	string PlayersHome;
	string PlayersEnemy;
	char linebreak[3];
	int pointsgiven;
	stringstream PointsString;

	time(&mytime);
	today = localtime(&mytime);
	strftime(ftime, 30, "%Y-%b-%d.%H.%M.%S", today );
	strftime(fdate, 30, "%Y-%b-%d", today );

	filepath = "logs//orb//";
	filepath += fdate;
	filepath += ".log";

	file = fopen(filepath.c_str(),"a");
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	PlayersHome.clear();
	PlayersEnemy.clear();

	// For each possible player,
	for (int pl = 0; pl < MAX_PLAYERS; pl++) {

		// If the player is in game,
		if (p->Player[pl]->isInGame()) {

			// If the player was in the city that orbed, append the player's name to playersHome
			if (p->Player[pl]->City == p->Player[Orber]->City) {
				PlayersHome += p->Player[pl]->Name;
				PlayersHome += "  ";
			}

			// Else if the player was in the city that got orbed, append the player's name to playersEnemy
			else if (p->Player[pl]->City == EnemyCity) {
				PlayersEnemy += p->Player[pl]->Name;
				PlayersEnemy += "   ";
			}
		}
	}

	// Log the points given for the orb
	// TODO: make consistent with other pointsGiven
	if (BuildCount < 15) {
		pointsgiven = 20;
	}
	else if (BuildCount < 20) {
		pointsgiven = 30;
	}
	else {
		pointsgiven = 50;
	}
	PointsString << pointsgiven;

	// Log the orb
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
