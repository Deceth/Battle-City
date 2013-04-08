/*
===============================================================================

    Battle City - CCity file
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
#include "CCity.h"

/***************************************************************
 * Constructor
 *
 **************************************************************/
CCity::CCity(CServer *Server, int id) {
	this->p = Server;
	this->id = id;

	this->resetToDefault();
}


/***************************************************************
 * Destructor
 *
 **************************************************************/
CCity::~CCity() {
}

/***************************************************************
 * Function:	resetToDefault
 *
 **************************************************************/
void CCity::resetToDefault() {

	// Reset active to 1
	this->active = 1;

	// Reset the mayor, stats, hiring info, etc.  
	this->Mayor = -1;
	this->Successor = -1;
	this->Orbs = 0;
	this->currentBuildingCount = 1;
	this->startTime = 0;
	this->notHiring = 0;
	this->maxBuildingCount = 1;
	this->hadBombFactory = false;
	this->hadOrbFactory = false;

	// If the mayor had an applicant, tell any applicants that the mayor changed
	if (hiring > -1) {
		p->Winsock->SendData(hiring, smMayorChanged, " ");
	}
	this->hiring = -1;

	// Reset the build tree
	for (int i = 0; i <= 26; i++) {
		this->canBuild[i] = 0;
	}

	// DO NOT enable Hospital
	//canBuild[0] = 1;
	
	// Enable House, Laser Research, Turret Research
	this->canBuild[1] = 1;
	this->canBuild[2] = 1;
	this->canBuild[4] = 1;

	// Reset all money values to default
	this->cash = MONEY_STARTING_VALUE;
	this->income = 0;
	this->itemproduction = 0;
	this->cashresearch = 0;
	this->hospital = 0;

	// Reset timers to 0
	this->DestructTimer = 0;
	this->moneyTick = 0;
	this->cityVerificationTick = 0;

	// Destroy all the city's items
	this->p->Item->deleteItemsByCity(this->id);

	// Destroy all the city's buildings
	this->p->Build->deleteBuildingsByCity(this->id);

	// Reset all research to not-finished
	for (int i = 0; i < 20; i++) {
		research[i] = 0;
	}

	// Reset all item counters to 0
	for (int i = 0; i < 12; i++) {
		itemC[i] = 0;
	}
}

/***************************************************************
 * Function:	destroy
 *
 **************************************************************/
void CCity::destroy() {

	// Reset the city to default
	this->resetToDefault();

	// Tell all players the city was destroyed
	this->p->Send->SendGameAllBut(-1,smDestroyCity,(char *)&this->id, sizeof(this->id));
}

/***************************************************************
 * Function:	setCanBuild
 *
 * @param i
 * @param can
 **************************************************************/
void CCity::setCanBuild(int i, int can, bool allowReset) {
	char packet[4];
	int iCan = 0;
	int canBuildSize = sizeof(this->canBuild) / sizeof(int);

	// If i is an invalid canBuild index, abort
	if (i >= canBuildSize) {
		return;
	}

	// canBuild[x] = 0: cannot be built
	// canBuild[x] = 1: can be built
	// canBuild[x] = 2: already has

	// If we're trying to reset canBuild from 2 ("already has") to 1 ("can build"),
	// But allowReset parameter is false, override canBuild back to 2 .
	if ((can==1) && (this->canBuild[i] == 2) && (allowReset==false)) {
		can = 2;
	}

	// Store whether the city can build the building in canBuild
	this->canBuild[i] = can;

	// If the player can build this building (+1), set iCan to 1
	if (can) {
		iCan = 1;
	}

	// If the player can't build, set iCan to 0
	if (can == 2) {
		iCan = 0;
	}

	// If this city's mayor is in-game, tell the mayor about the build-tree change
	if (this->p->Player[this->Mayor]->isInGame()) {
		packet[0] = i+1;
		packet[1] = iCan;
		packet[2] = 0;
		this->p->Winsock->SendData(Mayor,(unsigned char)smCanBuild,packet);
	}
}

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CCity::cycle() {
	int random_int;
	sSMFinance finance;
	
	// If the city has no mayor,
	if (this->Mayor == -1) {
		
		// If the destruct timer is set,
		if (this->DestructTimer > 0) {

			// If the Destruct timer is up, destroy the city
			if (this->p->Tick > this->DestructTimer) {
				this->destroy();
			}
		}

		// If no mayor, return
		return;
	}

	// if the Money timer is up,
	if (this->p->Tick > this->moneyTick) {

		// Cap money at MONEY_MAX_VALUE
		if (this->cash > MONEY_MAX_VALUE) {
			this->cash = MONEY_MAX_VALUE;
		}
		else if (this->cash < 0) {
			this->cash = 0;

			// Randomly give back cash
			srand ( time(NULL) );
			random_int = rand()%4 +1;
			if (random_int == 1) {
				this->cash = COST_UPKEEP_HOSPITAL;
			}
		}

		// Reset the money timer
		this->moneyTick = this->p->Tick + 7000;
		
		finance.Cash = this->cash;
		finance.Income = this->income;
		finance.Items = this->itemproduction;
		finance.Hospital = this->hospital;
		finance.Research = this->cashresearch;
		this->p->Winsock->SendData(Mayor,smFinance,(char *)&finance,sizeof(finance));

		this->income = 0;
		this->itemproduction = 0;
		this->cashresearch = 0;
		this->hospital = 0;
	}

	// if the City Verification timer is up,
	if (this->p->Tick > this->cityVerificationTick) {
		
		if (this->p->Player[this->Mayor]->isInGame() == false) {
			stringstream ss;

			ss << "------------------------------------" << endl;
			ss << "City has a mayor who is not in game!" << endl;
			ss << "------------------------------------" << endl;
			ss << "City: " << this->id << endl;
			ss << "Mayor: " << this->Mayor << endl;
			ss << "Successor: " << this->Successor << endl;
			ss << "------------------------------------" << endl;

			this->p->Log->logServerError(ss.str());

			this->Mayor = -1;
		}

		// Reset the timer
		this->cityVerificationTick = this->p->Tick + 10000;
	}
}

/***************************************************************
 * Function:	wasOrbed
 *
 **************************************************************/
void CCity::wasOrbed() {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, and is in this city,
		if ((this->p->Player[i]->isInGame()) && (this->p->Player[i]->City == id)) {

			// Boot the player, passing 
			cout << "Orbed::" << this->p->Player[i]->Name << endl;
			this->p->Player[i]->LeaveGame(false, false);
		}
	}

	// Destroy this city
	this->destroy();
}

/***************************************************************
 * Function:	didOrb
 *
 * @param City
 * @param index
 **************************************************************/
void CCity::didOrb(int City, int index) {
	int pointsGiven;
	int orberCityPoints;
	sSMOrbedCity orbed;

	// Add one to the city's orb total
	this->Orbs++;

	// Get the value of the orbed city and the orber's city
	pointsGiven = p->City[City]->getOrbValue();
	orberCityPoints = p->City[id]->getOrbValue();

	// Tell each player about the orb
	orbed.points = pointsGiven;
	orbed.OrberCity = id;
	orbed.City = City;
	orbed.OrberCityPoints = orberCityPoints;
	p->Send->SendAllBut(-1,smOrbed,(char *)&orbed,sizeof(orbed));

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, and is in the city,
		if (p->Player[i]->isInGame() && (p->Player[i]->City == id)) {

			// If the player was the orber, add an orb
			if (i == index) {
				p->Account->AddOrb(i);
			}

			// Else (not the orber), add an assist
			else {
				p->Account->AddAssist(i);
			}

			// Add the points for the orb
			p->Account->AddPoints(i, pointsGiven);
		}
	}

}

/***************************************************************
 * Function:	PlayerCount
 *
 * @param i
 * @param can
 **************************************************************/
int CCity::PlayerCount() {
	int count = 0;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in the city and in game,
		if (p->Player[i]->isInGame() && p->Player[i]->City == this->id) {

			// Increment the player count
			count++;
		}
	}	

	return count;
}

/***************************************************************
 * Function:	addBuilding
 **************************************************************/
void CCity::addBuilding(int type) {

	// Increment the current building count
	this->currentBuildingCount++;

	// If the current building count is greater than the max building count,
	if (this->currentBuildingCount > this->maxBuildingCount ) {

		// Set the max building count to the current building count
		this->maxBuildingCount = this->currentBuildingCount;
	}

	// If Bomb Factory (18), set hadBombFactory
	if (type == 18) {
		this->hadBombFactory = true;
	}

	// If Orb Factory (16), set hadOrbFactory
	else if (type == 16) {
		this->hadOrbFactory = true;
	}

	// If the building is now orbable, but startTime isn't set, set it
	if (this->isOrbable() && (this->startTime==0)) {

		// If p is initialized, set the startTime
		if (p) {
			this->startTime = this->p->Tick;
		}

		// Tell the city it is now orbable
		//this->p->Send->SendTeam(this->id, smNowOrbable, " ", 1);
	}
}

/***************************************************************
 * Function:	subtractBuilding
 **************************************************************/
void CCity::subtractBuilding(int type) {

	// Decrement the current building count
	if (this->currentBuildingCount > 1) {
		this->currentBuildingCount--;
	}
}

/***************************************************************
 * Function:	getOrbValue
 *
 **************************************************************/
int CCity::getOrbValue() {
	int pointsgiven = 0;

	// Get the base point value from the city's max building count

	// SIZE: ORBABLE_SIZE+10+
	if (this->maxBuildingCount >= ORBABLE_SIZE+10) {
		pointsgiven = 50;
	}
	// SIZE: ORBABLE_SIZE+5+
	else if (this->maxBuildingCount >= ORBABLE_SIZE+5) {
		pointsgiven = 40;
	}
	// SIZE: ORBABLE_SIZE+
	else if (this->maxBuildingCount >= ORBABLE_SIZE) {
		pointsgiven = 30;
	}
	// Orb Factory
	else if (this->hadOrbFactory) {
		pointsgiven = 20;
	}
	// Bom Factory
	else if (this->hadBombFactory) {
		pointsgiven = 10;
	}
	else {
		pointsgiven = 0;
	}

	// Add 5 points for each of the city's Orbs
	pointsgiven += (this->Orbs * 5);

	return pointsgiven;
}

/***************************************************************
 * Function:	isOrbable
 *
 **************************************************************/
bool CCity::isOrbable() {
	return
		this->hadBombFactory ||
		this->hadOrbFactory ||
		(this->maxBuildingCount >= ORBABLE_SIZE);
}

/***************************************************************
 * Function:	getUptime
 *
 **************************************************************/
int CCity::getUptime() {
	return (this->p->Tick - this->startTime);
}

/***************************************************************
 * Function:	getUptimeInSeconds
 *
 **************************************************************/
int CCity::getUptimeInSeconds() {
	return this->getUptime() / 1000;
}

/***************************************************************
 * Function:	getUptimeInMinutes
 *
 **************************************************************/
int CCity::getUptimeInMinutes() {
	return this->getUptimeInSeconds() / 60;
}

/***************************************************************
 * Function:	isValidCityIndex
 *
 **************************************************************/
bool CCity::isValidCityIndex(int city) {
	return ((city>-1) && (city<MAX_CITIES));
}
