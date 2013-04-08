/*
===============================================================================

    Battle City - CPlayer
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
#include "CPlayer.h"

/***************************************************************
 * Constructor:	CPlayer
 *
 **************************************************************/
CPlayer::CPlayer(CGame *game, int id) {
	this->p = game;

	this->ClearPlayer();

	this->id = id;
}

/***************************************************************
 * Destructor:	CPlayer
 *
 **************************************************************/
CPlayer::~CPlayer() {

}

/***************************************************************
 * Function:	ClearPlayer
 *
 **************************************************************/
void CPlayer::ClearPlayer() {
	this->playerType = 1;
	this->Points = 0;
	this->MonthlyPoints = 0;
	this->Orbs = 0;
	this->Deaths = 0;
	this->Assists = 0;

	this->Name.clear();
	this->NameString.clear();
	this->TownString.clear();
	this->Town.clear();

	this->InGameClear();
}

/***************************************************************
 * Function:	InGameClear
 *
 **************************************************************/
void CPlayer::InGameClear() {
	this->isMoving = 0;
	this->isDead = false;
	this->timeMove = 0;
	this->isTurning = 0;
	this->timeTurn = 0;
	this->Direction = 0;
	this->X = 0;
	this->Y = 0;
	this->SectorX = 0;
	this->SectorY = 0;

	this->City = 0;
	this->CityX = 0;
	this->CityY = 0;

	this->isCloaked = false;
	this->isMayor = 0;
	this->isShooting = 0;
	this->isFrozen = 0;
	this->isLagging = 0;
	this->isInGame = 0;
	this->timeFrozen = 0;

	this->lastUpdate = 0;
}

/***************************************************************
 * Function:	Cycle
 *
 **************************************************************/
void CPlayer::Cycle() {
	float MoveFactor;
	int me = p->Winsock->MyIndex;
	float velX = 0;
	float velY = 0;
	short HasSectorX;
	short HasSectorY;
	int movetimes = 0;
	float fDir;
	bool isOnDFG = false;
	bool isOnMine = false;

	// Set the player's move rate
	if (this->isAdmin()) {
		MoveFactor = MOVEMENT_SPEED_ADMIN;
	}
	else {
		MoveFactor = MOVEMENT_SPEED_PLAYER;
	}

	// If the player is in game,
	if (this->isInGame) {

		// If the player isn't me, and we haven't heard from the player in 15000, send the player to purgatory
		if ( (id != p->Winsock->MyIndex) && (p->Tick > (this->lastUpdate + 15000))) {
			this->X = 0;
			this->Y = 0;
		}

		// If the player isn't me, and we haven't heard from the player in 3000, set isLagging 1
		else if (id != p->Winsock->MyIndex && p->Tick > (this->lastUpdate + 3000)) {
			this->isLagging = 1;
			this->isMoving = 0;
		}

		// Else (not lagging), set isLagging 0
		else {
			this->isLagging = 0;
		}

		// If the player is frozen,
		if (this->isFrozen) {
			isMoving = 0;

			// If the Frozen timer is up, set isFrozen 0
			if (p->Tick > this->timeFrozen) {
				this->isFrozen = 0;
			}

			// Else (player is still frozen), return
			else {
				return;
			}
		}

		// If the player is turning,
		if (this->isTurning) {

			// If the Turn timer is up,
			if (p->Tick > this->timeTurn) {

				// Turn the player
				this->Direction += this->isTurning;

				// Prevent the Direction from going negative
				if (this->Direction < 0) {
					this->Direction = 31;
				}

				// Prevent the Direction from going above 31
				else if (this->Direction > 31)  {
					this->Direction = 0;
				}

				// Reset the Turn timer
				this->timeTurn = p->Tick + 50;
			}
		}

		// ???
		HasSectorX = short(X/48) / SectorSize;
		HasSectorY = short(Y/48) / SectorSize;

		// If the player is in a legitimate sector,
		if (
			(HasSectorX >= 0)
			&&
			(HasSectorX <= MaxSectors)
			&& 
			(HasSectorY >= 0)
			&& 
			(HasSectorY <= MaxSectors)
		) {

			// If the player is moving, this client has the sector loaded OR the player isn't me,
			if (isMoving && (p->InGame->HasSector[HasSectorX][HasSectorY] == 1 || id != p->Winsock->MyIndex)) {

				// Reverse order since trig goes counter-clockwise and bc isTurning goes clockwise
				fDir = (float)-this->Direction + 32; 
				
				// Update the player's X position
				velX = (float)((sin((float)(fDir)/16*3.14)*isMoving)) * (p->TimePassed * MoveFactor); //calculate their speed
				if (velX > 20) {
					velX = 20;
				}
				else if (velX < -20) {
					velX = -20;
				}
				X += velX;

				// Switch on collision check return value
				switch(p->Collision->CheckPlayerCollision(this->id)) {
					
					// Collision: NONE
					case 0:
						break;

					// Collision: BLOCKING
					case 2:

						// Relocate the player up to 1000 times
						do {
							if (movetimes > 1000) {
								break;
							}
							X += (float)((sin((float)(fDir+16)/16*3.14)*isMoving));
							movetimes += 1;
						}
						while (p->Collision->CheckPlayerCollision(this->id) == 2);
						break;

					// Collision: MINE
					case 101:
						isOnMine = true;
						break;

					// Collision: DFG
					case 103:
						isOnDFG = true;
						break;

					// Collision: LEFT MAP EDGE
					case 200: //Left map edge
						X = 0;
						break;

					// Collision: RIGHT MAP EDGE
					case 201:
						X = (512-1) * 48;
						break;

					default:
						break;
				}
				
				// Update the player's X position
				movetimes = 0;
				velY = (float)((cos((float)(fDir)/16*3.14)*isMoving)) * (p->TimePassed * MoveFactor);
				if (velY > 20) {
					velY = 20;
				}
				if (velY < -20) {
					velY = -20;
				}
				Y += velY;

				// Switch on collision check return value
				switch(p->Collision->CheckPlayerCollision(this->id)) {

					// Collision: NONE
					case 0:
						break;

					// Collision: BLOCKING
					case 2:

						// Relocate the player up to 1000 times
						do {
							if (movetimes > 1000) {
								break;
							}
							Y += (float)((cos((float)(fDir+16)/16*3.14)*isMoving));
							movetimes += 1;
						}
						while (p->Collision->CheckPlayerCollision(this->id) == 2);
						break;

					// Collision: MINE
					case 101:
						isOnMine = true;
						break;

					// Collision: DFG
					case 103:
						isOnDFG = true;
						break;

					// Collision: TOP MAP EDGE
					case 202:
						Y = 0;
						break;

					// Collision: BOTTOM MAP EDGE
					case 203:
						Y = (512-1)*48;
						break;

					default:
						break;
				}

				// If the player hit a mine, hit it
				if (isOnMine) {
					this->HitMine();
				}

				// If the player hit a DFG, hit it
				if (isOnDFG) {
					this->HitDFG();
				}

			}

			// Else (player not moving, and, client doesn't have sector OR player is me),
			else {

				// Switch on collision check return value
				switch(p->Collision->CheckPlayerCollision(this->id)) {

					// Collision: BLOCKING
					case 2:
						// Relocate the player up to 1000 times
						this->p->Player[id]->RelocatePlayer();
						break;
				}
			}
		}
	}

	// If the player is cloaked,
	if (this->isCloaked) {

		// If the Cloak timer is up, uncloak the player
		if (this->p->Tick > this->timeUncloak) {
			this->setCloak(false);
		}
	}
}

/***************************************************************
 * Function:	RelocatePlayer
 *
 **************************************************************/
void CPlayer::RelocatePlayer() {
	float fDir = 0;
	int tempdir = 0;
	int isMovingtimesforward = 0;
	int isMovingtimesback = 0;
	int isMovingtimesleft = 0;
	int isMovingtimesright = 0;

	int startX = (int)X;
	int startY = (int)Y;

	int finalXforward = 0;
	int finalYforward = 0;
	int finalXback = 0;
	int finalYback = 0;
	int finalXleft = 0;
	int finalYleft = 0;
	int finalXright = 0;
	int finalYright = 0;

	//Check closest tiles first
	int mYX = (int)X / 48;
	int mYY = (int)Y / 48;
	
	X = (float)(startX); 
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)startX;
	Y = (float)startY;

	//Up
	fDir = 0;

	do {
		if (isMovingtimesforward > 1000) {
			break;
		}
		Y += (float)(cos((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesforward += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXforward = (int)X;
	finalYforward = (int)Y;

	//Back
	fDir = 24;

	X = (float)startX;
	Y = (float)startY;

	do {
		if (isMovingtimesback > 1000) break;
		Y += (float)(cos((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesback += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXback = (int)X;
	finalYback = (int)Y;

	//Left
	tempdir = 36;
	if (tempdir < 0) tempdir = 47 - abs(tempdir);
	fDir = (float)-tempdir + 32;

	X = (float)startX;
	Y = (float)startY;

	do {
		if (isMovingtimesleft > 1000) break;
		X += (float)(sin((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesleft += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXleft = (int)X;
	finalYleft = (int)Y;

	//Right
	tempdir = 12;
	if (tempdir > 47) tempdir = tempdir - 47;
	fDir = (float)-tempdir + 32;

	X = (float)startX;
	Y = (float)startY;

	do {
		if (isMovingtimesright > 1000) break;
		X += (float)(sin((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesright += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXright = (int)X;
	finalYright = (int)Y;

	if ((isMovingtimesforward <= isMovingtimesback) && (isMovingtimesforward <= isMovingtimesleft) && (isMovingtimesforward <= isMovingtimesright)) {
		X = (float)finalXforward;
		Y = (float)finalYforward;
	}
	else if ((isMovingtimesback <= isMovingtimesforward) && (isMovingtimesback <= isMovingtimesleft) && (isMovingtimesback <= isMovingtimesright)) {
		X = (float)finalXback;
		Y = (float)finalYback;
	}
	else if ((isMovingtimesleft <= isMovingtimesforward) && (isMovingtimesleft <= isMovingtimesback) && (isMovingtimesleft <= isMovingtimesright)) {
		X = (float)finalXleft;
		Y = (float)finalYleft;
	}
	else if ((isMovingtimesright <= isMovingtimesforward) && (isMovingtimesright <= isMovingtimesback) && (isMovingtimesright <= isMovingtimesleft)) {
		X = (float)finalXright;
		Y = (float)finalYright;
	}
}

/***************************************************************
 * Function:	SetHP
 *
 **************************************************************/
void CPlayer::SetHP(int HP) {
	this->HP = HP;
	this->refHP1 = (HP^2-31)*2;
	this->refHP2 = -(HP);
}

/***************************************************************
 * Function:	HitMine
 *
 **************************************************************/
void CPlayer::HitMine() {
	CItem *item;
	sCMItem itemPacket;
	int me = p->Winsock->MyIndex;

	// If the player is dead, return
	if (p->Player[p->Winsock->MyIndex]->isDead == true) {
		return;
	}

	// If the current player is the one who hit the mine,
	if (me == this->id) {
		
		// Find the first mine under the player
		item = p->Item->findItembyLocationAndType((int)(p->Player[p->Winsock->MyIndex]->X + 24) / 48, (int)(p->Player[p->Winsock->MyIndex]->Y + 24) / 48, ITEM_TYPE_MINE, true);

		// If one was found,
		if (item) {

			// Tell everyone the item was hit
			itemPacket.id = item->id;
			itemPacket.active = item->active;
			itemPacket.type = item->Type;
			p->Winsock->SendData(cmHitObject,(char *)&itemPacket,sizeof(itemPacket));

			// Subtract DAMAGE_MINE from total HP when hitting a mine
			p->Player[me]->SetHP(p->Player[me]->HP - DAMAGE_MINE);

			// Explode, disable mine, play sound
			p->Explode->newExplosion(item->X * 48, item->Y * 48, 1);
			item->active = 0;
			p->Sound->PlayWav(sExplode, -1);

			// If player HP is now under 0, then destroy player tank
			if (p->Player[me]->HP <= 0) {
				char packet[2];
				packet[0] = item->City;
				packet[1] = 0;
				p->Player[p->Winsock->MyIndex]->isDead = true;
				p->InGame->timeDeath = p->Tick;
				p->Winsock->SendData(cmDeath, packet, 1);
				p->Sound->PlayWav(sDie, -1);
			}
		}
	}

	// Else (someone else hit the mine),
	else {

		// If I am in range, play a sound
		if ((abs(p->Player[this->id]->X - p->Player[me]->X) < 1000) && (abs(p->Player[this->id]->Y - p->Player[me]->Y) < 1000)) {
			p->Sound->Play3dSound(p->Sound->s_eXplode, 100, p->Player[this->id]->X, p->Player[this->id]->Y);
		}
	}
}

/***************************************************************
 * Function:	HitDFG
 *
 **************************************************************/
void CPlayer::HitDFG() {
	CItem *item;

	// If I am the player that hit the DFG,
	if (id == p->Winsock->MyIndex) {

		// Find the first DFG in my square,
		item = p->Item->findItembyLocationAndType((int)(p->Player[p->Winsock->MyIndex]->X + 24) / 48, (int)(p->Player[p->Winsock->MyIndex]->Y + 24) / 48, ITEM_TYPE_DFG, true);

		// If found,
		if (item) {

			// De-activate the DFG
			item->active = 0;

			// Freeze me until TIMER_DFG
			this->isFrozen = 1;
			this->timeFrozen = p->Tick + TIMER_DFG;
		}
	}
}

/***************************************************************
 * Function:	GenerateNameString
 *
 **************************************************************/
void CPlayer::GenerateNameString() {
	string newNameString = "";
	string newTownString = "";

	// If the player's not in game, abort
	if (this->isInGame == 0) {
		return;
	}

	// Add the rank (or Admin)
	if (this->isAdmin()) {
		newNameString += "Admin";
	}
	else {
		newNameString += this->p->InGame->ReturnRank(this->Points);
	}

	// Add a space
	newNameString += " ";
	newNameString += this->Name;

	// If the player is in a valid city, get the city to show
	if (CInGame::isValidCityIndex(this->City)) {
		newTownString += "(";
		if (this->isMayor) {
			newTownString += "Mayor of ";
		}
		newTownString += CityList[this->City];
		newTownString += ")";
	}

	this->NameString.clear();
	this->NameString += newNameString;

	this->TownString.clear();
	this->TownString += newTownString;

}

/***************************************************************
 * Function:	isAdmin
 *
 **************************************************************/
bool CPlayer::isAdmin() {
	return (this->playerType == 2);
}

/***************************************************************
 * Function:	setCloak
 *
 * @param isCloaked
 **************************************************************/
void CPlayer::setCloak(bool isCloaked) {
	if( isCloaked ) {
		this->isCloaked = true;
		this->timeUncloak = this->p->Tick + TIMER_CLOAK;
	}
	else {
		this->isCloaked = false;
		this->timeUncloak = 0;
	}
}

/***************************************************************
 * Function:	getTileX
 *
 **************************************************************/
int CPlayer::getTileX() {
	return (int) ((this->X+24) / 48);
}

/***************************************************************
 * Function:	getTileY
 *
 **************************************************************/
int CPlayer::getTileY() {
	return (int) ((this->Y+24) / 48);
}

/***************************************************************
 * Function:	setMayor
 *
 * @param set
 **************************************************************/
void CPlayer::setMayor(bool isMayor) {

	// Store whether the player is in-game or not
	this->isMayor = isMayor;

	// Re-generate the name string (to add/remove "Mayor of...")
	this->GenerateNameString();
}