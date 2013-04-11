/*
===============================================================================

    Battle City - Main file
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

/** @file Main.cpp Functions related to start bcserver.exe */

#include "CServer.h"
#include <exception> 

CServer Server;

/***************************************************************
 * Function:	main
 *
 * @param argc
 * @param argv
 **************************************************************/
int main(int argc, char *argv[]) {
	float LastBulletTick = 0;
	float ItemTick = 0;
	float BuildingTick = 0;
	float BulletTick = 0;
	float RespawnTick = 0;
	float Top20Tick = 0;
	float CityTick = 0;
	bool printServerInfo = false;
	stringstream crashMessage;
	
	// Initalize the server
	Server.Init();

	try {

		// While the server is running,
		while (Server.running == 1) {

			// Increment the timer values
			Server.lastTick = Server.Tick;
			Server.Tick = Server.Timer->GetTime();

			// Tell the server to run its cycle
			Server.Winsock->Cycle();

			// If the Item timer is up, tell the server to run its Item cycle
			if (Server.Tick > ItemTick) {
				Server.Item->cycle();
				ItemTick = Server.Tick + 100;
			}

			// If the Building timer is up, tell the server to run its Building cycle
			if (Server.Tick > BuildingTick) {
				Server.Build->cycle();
				BuildingTick = BuildingTick + 50;
			}

			// If the Bullet timer is up, tell the server to run its Bullet cycle
			if (Server.Tick > BulletTick) {
				Server.TimePassed = Server.Tick - LastBulletTick;
				Server.Bullet->cycle();
				LastBulletTick = Server.Tick;
				BulletTick = Server.Tick + 12;
			}

			// If the Top20 timer is up, tell the server to run its Top20 cycle
			if (Server.Tick > Top20Tick) {
				Server.Account->GenerateTop20();
				Top20Tick = Server.Tick + 300000;
			}

			// If the City timer is up, 
			if (Server.Tick > CityTick) {

				// For each possible city,
				for (int i = 0; i < MAX_CITIES; i++) {

					// Run the City cycle
					Server.City[i]->cycle();
				}
				CityTick = Server.Tick + 1000;
			}

			// If the Respawn timer is up,
			if (Server.Tick > RespawnTick) {

				// Check for players to respawn, reset the Respawn timer
				Server.respawnPlayers();
				RespawnTick = Server.Tick + 1000;
			}

			// Sleep
			Server.PlatformSleep(1);
		}
	}

	// Catch basic exceptions
	catch (std::exception& e) {
		crashMessage << endl << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "SERVER CRASH: EXCEPTION CAUGHT" << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "Exception details:" << endl;
		crashMessage << e.what() << endl;
		crashMessage << "------------------------------" << endl;

		printServerInfo = true;
	}

	// Catch everything else
	catch (...) {
		crashMessage << endl << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "SERVER CRASH: no exception :(" << endl;
		crashMessage << "------------------------------" << endl;
		printServerInfo = true;
	}

	// If an exception occurred,
	if (printServerInfo) {
		crashMessage << "Server.lastTick: " << Server.lastTick << "\n";
		crashMessage << "Server.Tick: " << Server.Tick << "\n";
		crashMessage << "LastBulletTick: " << LastBulletTick << "\n";
		crashMessage << "ItemTick " << ItemTick << "\n";
		crashMessage << "BuildingTick: " << BuildingTick << "\n";
		crashMessage << "BulletTick: " << BulletTick << "\n";
		crashMessage << "RespawnTick: " << RespawnTick << "\n";
		crashMessage << "Top20Tick: " << Top20Tick << "\n";
		crashMessage << "CityTick: " << CityTick << "\n";

		Server.Log->logServerError(crashMessage.str());
	}

	// If we left the while loop, close the server
	cout << "Server::Close" << endl;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is not disconnected,
		if (Server.Player[i]->State > State_Disconnected) {

			// Disconnect the player
			Server.Player[i]->Clear();
		}
	}
	
	cout << endl << endl << endl << "The server has been safely shutdown.  Press any key to exit" << endl;

#ifdef WIN32
	getchar();
#endif

	return EXIT_SUCCESS;
}
