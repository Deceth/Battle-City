/*
===============================================================================

    Battle City - CBuilding
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
#include "CBuilding.h"

/***************************************************************
 * Constructor:	CBuilding
 *
 * @param X
 * @param Y
 * @param Type
 * @param City
 * @param id
 * @param Game
 **************************************************************/
CBuilding::CBuilding(unsigned short X, unsigned short Y, int Type, unsigned char City, unsigned short id, CGame *Game) {
	this->p = Game;
	this->City = City;
	this->X = X;
	this->Y = Y;
	this->pop = 0;
	this->Animation = rand()%6;
	this->timeAnimation = 0;
	this->Smoke = 0;
	this->timeSmoke = 0;
	this->id = id;
	this->ItemsLeft = 0;

	this->prev = 0;
	this->next = 0;

	this->rawType = Type;
	if( Type > -1 ) {
		this->Type = buildingTypes[Type - 1];
	}
	else {
		this->Type = 6;
	}
}

/***************************************************************
 * Destructor:	CBuilding
 *
 **************************************************************/
CBuilding::~CBuilding() {
}

/***************************************************************
 * Function:	isCC
 *
 * @return Returns true if this building is a CC
 **************************************************************/
bool CBuilding::isCC() {
	return this->isCC(this->Type);
}
/***************************************************************
 * Function:	isCC
 *
 * @param buildingType
 * @return Returns true if the buildingType is a CC
 **************************************************************/
bool CBuilding::isCC(int buildingType) {
	return (buildingType / 100 == 0);
}

/***************************************************************
 * Function:	isFactory
 *
 * @return Returns true if this building is a Factory
 **************************************************************/
bool CBuilding::isFactory() {
	return this->isFactory(this->Type);
}
/***************************************************************
 * Function:	isFactory
 *
 * @param buildingType
 * @return Returns true if the buildingType is a Factory
 **************************************************************/
bool CBuilding::isFactory(int buildingType) {
	return ((buildingType / 100)==1);
}

/***************************************************************
 * Function:	isHospital
 *
 * @return Returns true if this building is a Hospital
 **************************************************************/
bool CBuilding::isHospital() {
	return this->isHospital(this->Type);
}
/***************************************************************
 * Function:	isHospital
 *
 * @param buildingType
 * @return Returns true if the buildingType is a House
 **************************************************************/
bool CBuilding::isHospital(int buildingType) {
	return ((buildingType / 100)==2);
}

/***************************************************************
 * Function:	isHouse
 *
 * @return Returns true if this building is a House
 **************************************************************/
bool CBuilding::isHouse() {
	return this->isHouse(this->Type);
}
/***************************************************************
 * Function:	isHouse
 *
 * @param buildingType
 * @return Returns true if the buildingType is a House
 **************************************************************/
bool CBuilding::isHouse(int buildingType) {
	return ((buildingType / 100)==3);
}

/***************************************************************
 * Function:	isResearch
 *
 * @return Returns true if this building is a Research
 **************************************************************/
bool CBuilding::isResearch() {
	return this->isResearch(this->Type);
}
/***************************************************************
 * Function:	isResearch
 *
 * @param buildingType
 * @return Returns true if the buildingType is a Research
 **************************************************************/
bool CBuilding::isResearch(int buildingType) {
	return ((buildingType / 100)==4);
}











/***************************************************************
 * Constructor:	CBuildingList
 *
 * @param game
 **************************************************************/
CBuildingList::CBuildingList(CGame *game) {
	this->p = game;
	this->buildingListHead = 0;
}

/***************************************************************
 * Destructor:	CBuildingList
 *
 **************************************************************/
CBuildingList::~CBuildingList() {
	while (this->buildingListHead)
		this->delBuilding(this->buildingListHead);
}

/***************************************************************
 * Constructor:	findBuilding
 *
 * @param id
 **************************************************************/
CBuilding *CBuildingList::findBuilding(unsigned short id) {
	CBuilding *bld = this->buildingListHead;

	// For each building,
	while (bld) {

		// If the building matches the id, return it
		if (bld->id == id) {
			return bld;
		}

		// Else, get the next building
		bld = bld->next;
	}

	// Return 0
	return 0;
}

/***************************************************************
 * Constructor:	findBuildingbyLocation
 *
 * @param X
 * @param Y
 **************************************************************/
CBuilding *CBuildingList::findBuildingbyLocation(int X, int Y) {
	CBuilding *bld = this->buildingListHead;

	// For each building,
	while (bld) {

		// If the building matches the X and Y, return it
		if (bld->X == X && bld->Y == Y) {
			return bld;
		}

		// Else, get the next building
		bld = bld->next;
	}

	// Return 0
	return 0;
}

/***************************************************************
 * Constructor:	newBuilding
 *
 * @param X
 * @param Y
 * @param City
 * @param Type
 * @param id
 **************************************************************/
CBuilding *CBuildingList::newBuilding(unsigned short X, unsigned short Y, unsigned char City, int Type, unsigned short id) {
	CBuilding *bld;

	// If the id is over 0,
	if (id > 0) {

		// If the id is already in use, return 0
		bld = this->findBuilding(id);
		if (bld) {
			return 0;
		}
	}

	// Create the building
	bld = new CBuilding(X, Y, Type, City, id, this->p);

	// If there are other buildings,
	if (this->buildingListHead) {

		// Tell the current head the new building is before it
		this->buildingListHead->prev = bld;
		bld->next = this->buildingListHead;
	}

	// Add the new building as the new head
	this->buildingListHead = bld;

	// Return a pointer to the new building
	return bld;
}

/***************************************************************
 * Constructor:	Cycle
 *
 **************************************************************/
void CBuildingList::Cycle() {
	int me = p->Winsock->MyIndex;
	float curTick = p->Tick;
	CBuilding *bld = this->buildingListHead;

	// For each building,
	while (bld) {

		// If the Animation Timer is up,
		if (curTick > bld->timeAnimation) {

			// Cycle the Animation
			bld->Animation++;
			if (bld->Animation > 5) {
				bld->Animation = 0;
			}

			// Reset the Animation timer
			bld->timeAnimation = curTick + 500;
		}

		// If the building is smoking,
		if (bld->Smoke > 0) {

			// If the Smoke timer is up,
			if (curTick > bld->timeSmoke) {

				// Cycle the smoke
				bld->Smoke++; 
				if (bld->Smoke > 5) {
					if (bld->ItemsLeft > 0)  {
						bld->Smoke = 1;
					}
					else  {
						bld->Smoke = 0;
					}
				}

				// Reset the Smoke timer
				bld->timeSmoke = curTick + 500;
			}
		}

		// Else if building isn't smoking, but building now has items left,
		else if (bld->ItemsLeft > 0)  {

			// Set Smoking on
			bld->Smoke = 1;
		}

		// Get the next building
		bld = bld->next;
	}
}

/***************************************************************
 * Constructor:	delBuilding
 *
 * @param del
 **************************************************************/
CBuilding *CBuildingList::delBuilding(CBuilding *del) {
	CBuilding *returnBuilding = del->next;

	// If building has a next, tell that next to skip this over node
	if (del->next) {
		del->next->prev = del->prev;
	}

	// If building has a prev, tell that prev to skip this over node
	if (del->prev) {
		del->prev->next = del->next;
	}
	// Else (building has no prev), building is head, point head to next node
	else {
		this->buildingListHead = del->next;
	}
	
	// Delete the building
	delete del;
	
	// Return what was del->next
	return returnBuilding;
}

/***************************************************************
 * Function:	deleteBuildingsByCity
 *
 * @param city
 **************************************************************/
void CBuildingList::deleteBuildingsByCity(int city) {
	CBuilding *bld = this->buildingListHead;

	// For each building in the linked list,
	while (bld) {

		// If the building belongs to this city, and building isn't a CC,
		if ((bld->City == city) && (bld->isCC() == false)) {

			// Delete the building and move the pointer to the next item in the linked list
			bld = this->delBuilding(bld);
		}
		// Else (building belongs to other city),
		else {

			// Move the pointer to the next building in the linked list
			bld = bld->next;
		}
	}
}

/***************************************************************
 * Function:	inRange
 *
 **************************************************************/
bool CBuildingList::inRange(bool withBuildingProximity) {
	CBuilding *bld;
	int city = p->Player[p->Winsock->MyIndex]->City;
	int playerTileX = p->Player[p->Winsock->MyIndex]->getTileX();
	int playerTileY = p->Player[p->Winsock->MyIndex]->getTileY();
	int buildingTileX;
	int buildingTileY;

	// If the player is an admin, return 1
	if (p->Player[p->Winsock->MyIndex]->isAdmin()) {
		return true;
	}

	// Else, normal player,

	// For each building,
	bld = this->buildingListHead;
	while (bld) {

		// If the building belongs to the builder's city,
		if (bld->City == city) {

			// Get the center tile of the building
			buildingTileX = bld->X-1;
			buildingTileY = bld->Y-1;

			// If withBuildingProximity is true,
				if (withBuildingProximity) {

				// If we're near the building, return true
				if ( (abs(buildingTileX - playerTileX) <= DISTANCE_MAX_FROM_BUILDING) && (abs(buildingTileY - playerTileY) <= DISTANCE_MAX_FROM_BUILDING)) {
					return true;
				}
			}

			// if the building is the CC,
			if (bld->isCC()) {

				//If the building's location is close enough to the player's location, return 1
				if ( (abs(buildingTileX - playerTileX) < DISTANCE_MAX_FROM_CC) && (abs(buildingTileY - playerTileY) < DISTANCE_MAX_FROM_CC)) {
					return true;
				}
			}
		}

		// Get the next building
		bld = bld->next;
	}

	// If no building was found in range, return 0
	this->p->InGame->AppendInfo("You cannot build or drop defensive items this far away from your City Center!");

	return 0;
}

