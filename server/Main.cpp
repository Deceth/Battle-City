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

#include "CServer.h"
#include <exception> 

/// <summary>
/// Construct CServer object
/// </summary>
CServer Server;

/// <summary>
/// Command-line application
/// </summary>
/// <param name="argc">Argument Count</param>
/// <param name="argv">Argument Vector</param>
/// <returns></returns>
int main(int argc, char *argv[]) {
    //  Last bullet time interval
	float LastBulletTick = 0;
    //  Item time interval
	float ItemTick = 0;
    //  Building time interval
	float BuildingTick = 0;
    //  Bullet time interval
	float BulletTick = 0;
    //  Respawn time interval
	float RespawnTick = 0;
    //  Top 20 time interval
	float Top20Tick = 0;
    //  City time interval
	float CityTick = 0;
    //  Indicate whether server information has been printed
	bool printServerInfo = false;
    //  Store crash message
	stringstream crashMessage;
    //  Initialize the CServer object
	Server.Init();
    //  Try launching the Server
	try {
        //  Loop while Server is running
		while (Server.running == 1) {
            //  Update last time interval with current time interval
			Server.lastTick = Server.Tick;
            //  Refresh current time interval
			Server.Tick = Server.Timer->GetTime();
            //  Perform winsock cycle for network connectivity
			Server.Winsock->Cycle();
            //  Verify current time interval is greater than item time interval
			if (Server.Tick > ItemTick) {
                //  Perform item cycle
				Server.Item->cycle();
                //  Refresh item time interval
				ItemTick = Server.Tick + 100;
			}
            //  Verify current time interval is greater than building time interval
			if (Server.Tick > BuildingTick) {
                //  Perform build cycle
				Server.Build->cycle();
                //  Refresh build time internval
				BuildingTick = BuildingTick + 50;
			}
            //  Verify current time interval is greater than bullet time interval
			if (Server.Tick > BulletTick) {
                //  Calculate time lapsed between current time interval and last bullet time interval
				Server.TimePassed = Server.Tick - LastBulletTick;
                //  Perform bullet cycle
				Server.Bullet->cycle();
                //  Update last bullet time interval with curren time interval
				LastBulletTick = Server.Tick;
                //  Refresh bullet time interval
				BulletTick = Server.Tick + 12;
			}
            //  Verify current time interval is greater than top 20 time interval
			if (Server.Tick > Top20Tick) {
                //  Perform top 20 generation
				Server.Account->GenerateTop20();
                //  Refresh top 20 time interval
				Top20Tick = Server.Tick + 300000;
			}
            //  Verify current time interval is greater than city time interval
			if (Server.Tick > CityTick) {
                //  Loop through all cities
				for (int i = 0; i < MAX_CITIES; i++) {
                    //  Perform cycle on each city
					Server.City[i]->cycle();
				}
                //  Refresh city time interval
				CityTick = Server.Tick + 1000;
			}
            //  Verify current time interval is greater than respawn time interval
			if (Server.Tick > RespawnTick) {
                //  Perform player respawn
				Server.respawnPlayers();
                //  Refresh respawn time interval
				RespawnTick = Server.Tick + 1000;
			}
            //  Perform a sleep
			Server.PlatformSleep(1);
		}
	} catch (std::exception& e) {
        //  Upon a standard exception the exception
        //  message will be stored in crashMessage variable
		crashMessage << endl << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "SERVER CRASH: EXCEPTION CAUGHT" << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "Exception details:" << endl;
		crashMessage << e.what() << endl;
		crashMessage << "------------------------------" << endl;
        //  Indiciate printed server information as true
		printServerInfo = true;
	} catch (...) {
        //  Upon any other exception, report unknown crash
		crashMessage << endl << endl;
		crashMessage << "------------------------------" << endl;
		crashMessage << "SERVER CRASH: no exception :(" << endl;
		crashMessage << "------------------------------" << endl;
        //  Indiciate printed server information as true
		printServerInfo = true;
	}
	//  Upon verifying printServerInfo as true append time interval information
    //  to the crashMessage variable then save variable to log
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
        //  Append crashMessage to the log file
		Server.Log->logServerError(crashMessage.str());
	}
    //  Print to the console
	cout << "Server::Close" << endl;
    //  Loop through available players
	for (int i = 0; i < MAX_PLAYERS; i++) {
        //  Verify player is not disconnected
		if (Server.Player[i]->State > State_Disconnected) {
            //  Perform player disconnect
			Server.Player[i]->Clear();
		}
	}
	//  Print to console that the server has safely shutdown
	cout << endl << endl << endl << "The server has been safely shutdown.  Press any key to exit" << endl;
    //  Wait for user to press any key
#ifdef WIN32
	getchar();
#endif
    //  Perform application exit
	return EXIT_SUCCESS;
}
