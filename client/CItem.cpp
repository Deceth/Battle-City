/*
===============================================================================

    Battle City - CItem
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
#include "CItem.h"

/***************************************************************
 * Constructor:	CItem
 *
 * @param x
 * @param y
 * @param type
 * @param City
 * @param id
 * @param action
 * @param Server
 **************************************************************/
CItem::CItem(int x, int y, int type, int City, unsigned short id, int active, CGame *game) {
	this->p = game;
	this->prev = 0;
	this->next = 0;
	this->Angle = 0;
	this->status = 0;
	this->dmg = 0;
	this->bulletDamage = 0;
	this->lastturn = 0;
	this->target = -1;
	this->bullet = 0;
	this->Life = 0;
	this->Animation = 0;
	this->Animationtick = 0;
	this->X = x;
	this->Y = y;
	this->Type = type;
	this->id = id;
	this->City = City;
	this->active = active;

	// Set startup delay, burn point, bullet damages
	if (this->Type == ITEM_TYPE_TURRET) {
		this->lastturn = this->p->Tick + 2000;
		this->dmg = 15;
		this->bulletDamage = 4;
	}
	else if (this->Type == ITEM_TYPE_SLEEPER) {
		this->lastturn = this->p->Tick + 2000;
		this->dmg = 15;
		this->bulletDamage = 4;
	}
	else if (this->Type == ITEM_TYPE_PLASMA) {
		this->lastturn = this->p->Tick + 2000;
		this->dmg = 30;
		this->bulletDamage = 5;
	}
}

/***************************************************************
 * Destructor:	CItem
 *
 **************************************************************/
CItem::~CItem() {
}

/***************************************************************
 * Function:	targetNearestEnemy
 *
 **************************************************************/
void CItem::targetNearestEnemy() {
	CPlayer* player;
	int itemX = this->X*48;
	int itemY = this->Y*48;
	int distance = 360;
	int d;
	
	this->target = -1;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game, isn't in the item's city, and isn't an admin,
		if (
			(player->isInGame)
			&&
			(this->City != player->City)
			&&
			(player->isDead == false)
			&&
			(player->isAdmin() == false)
		) {

			// Get the distance between the item and the player
			d = (int)sqrt((player->X-itemX)*(player->X-itemX) + (player->Y-itemY)*(player->Y-itemY));

			// If the player is closer to the turret than the previous target,
			if (d < distance) {

				// Target the player
				distance = d;
				this->target = (char)player->id;
			}
		}
	}

	// If the turret has a target,
	if (this->target != -1) {
		player = this->p->Player[this->target];

		// Get the firing angle
		this->Angle = (float)atan((float)(player->X / 48 - this->X) / (player->Y / 48 - this->Y));
		this->Angle = (float)(this->Angle * 180 / 3.14);

		// Adjust the angle by quadrant
		// ???
		
		// RIGHT
		if (player->X / 48 < this->X) {
			// DOWN
			if (player->Y / 48 < this->Y) {
				this->Angle = 180 - this->Angle;
			}
			// UP
			else {
				this->Angle = - this->Angle;
			}
		}
		// LEFT
		else {
			// DOWN
			if (player->Y / 48 < this->Y) {
				this->Angle = 180 - this->Angle;
			}
			// UP
			else {
				this->Angle = 360 - this->Angle;
			}
		}
	}
}

/***************************************************************
 * Function:	verifyBulletExists
 *
 **************************************************************/
void CItem::verifyBulletExists() {
	CBullet* blt = this->p->Bullet->bulletListHead;

	// If this item has a bullet attached,
	if (this->bullet) {

		// For each bullet,
		while (blt) {

			// If the bullet is the one that belongs to this turret, return
			if (blt->turretId == this->id) {
				return;
			}
			blt = blt->next;
		}

		// If we didn't find a bullet for this item, set its bullet value to 0
		this->bullet = 0;
	}

}







/***************************************************************
 * Class:		CItemList
 *
 **************************************************************/


/***************************************************************
 * Constructor:	CItemList
 *
 * @param game
 **************************************************************/
CItemList::CItemList(CGame *game) {
	this->p = game;
	this->itemListHead = 0;
}

/***************************************************************
 * Desnction:	CItemList
 *
 **************************************************************/
CItemList::~CItemList() {
	while (this->itemListHead) {
		this->delItem(this->itemListHead);
	}
}

/***************************************************************
 * Function:	findItem
 *
 * @param id
 **************************************************************/
CItem *CItemList::findItem(unsigned short id) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item matches the ID, return it
		if (itm->id == id) {
			return itm;
		}

		// Else, get the next item
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	findItembyLocation
 *
 * @param X
 * @param Y
 **************************************************************/
CItem *CItemList::findItembyLocation(int X, int Y) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item matches the X and Y, return it
		if (itm->X == X && itm->Y == Y) {
			return itm;
		}

		// Else, get the next item
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	findItembyLocationAndType
 *
 * @param X
 * @param Y
 * @param Type
 * @param activeOnly
 **************************************************************/
CItem *CItemList::findItembyLocationAndType(int X, int Y, int Type, bool activeOnly) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item matches the X and Y and Type, return it
		if ((itm->X == X) && (itm->Y == Y) && (itm->Type == Type)) {

			// If activeOnly is false, or the item is active, 
			if ( (!activeOnly) || (itm->active) ) {
				return itm;
			}
		}

		// Else, get the next item
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	newItem
 *
 * Note that this function is duplicated below.
 *
 * @param X
 * @param Y
 * @param Type
 * @param City
 * @param id
 * @param active
 **************************************************************/
CItem *CItemList::newItem(int X, int Y, int Type, int City, unsigned short id, int active) {
	CItem *itm = new CItem(X,Y,Type,City,id,active,p);

	// If there are other items,
	if (this->itemListHead) {

		// Tell the current head the new item is before it
		this->itemListHead->prev = itm;
		itm->next = this->itemListHead;
	}

	// Add the new item as the new head
	this->itemListHead = itm;

	// Return a pointer to the new item
	return itm;
}

/***************************************************************
 * Function:	Cycle
 *
 **************************************************************/
void CItemList::Cycle() {
	CItem *itm;
	int me = p->Winsock->MyIndex;
	CPlayer* playerMe = this->p->Player[p->Winsock->MyIndex];
	int itemX;
	int itemY;
	bool hasBullet = false;
	int tmpAngle;
	float fDir;
	int FlashY;
	int FlashX;

	// For each item,
	itm = this->itemListHead;
	while (itm) {
		itemX = itm->X*48;
		itemY = itm->Y*48;
		hasBullet = false;

		// If we're close enough to interact with the item,
		if ((abs(itm->X * 48-24 - playerMe->X) < 1000) && (abs(itm->Y * 48-24 - playerMe->Y) < 1000)) {

			// Item: TURRET (active)
			if (itm->Type > 8 && itm->active) {

				// If the item is BURNING, and the Animation timer is up,
				if (itm->Life == 1 && p->Tick > itm->Animationtick) {

					// Reset the animation timer
					itm->Animationtick = p->Tick + 50;

					// Cycle the animation
					if (itm->Animation == 2) {
						itm->Animation = 1;
					}
					else {
						itm->Animation = 2;
					}
				}

				// Reset the item's target
				itm->targetNearestEnemy();

				// If the turret's Turn counter is up,
				if (p->Tick > itm->lastturn) {

					// Reset the Turn counter
					itm->lastturn = p->Tick + 250;

					// Verify that its bullet still exists
					itm->verifyBulletExists();

					// If the turret has no bullet,
					if (! itm->bullet) {

						// If the turret has a target,
						if (itm->target != -1) {

							// Get an Angle value for the new bullet
							tmpAngle = (int)(itm->Angle/1.125);
							if (tmpAngle % 10 >= 5) {
								tmpAngle += 10;
							}
							tmpAngle = tmpAngle / 10;

							// Get direction and position values for the new bullet
							fDir = (float)-tmpAngle+32;
							FlashY = itm->Y * 48-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
							FlashX = itm->X * 48-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
							p->Explode->newExplosion(FlashX, FlashY, 3);
							itm->bullet = p->Bullet->newBullet(FlashX, FlashY, itm->bulletDamage, tmpAngle, itm->id);

							// Play a sound for the shot
							p->Sound->Play3dSound(p->Sound->s_bigturret, 100, (float)FlashX, (float)FlashY);
						}

						// Else (no target),
						else {
							if (itm->Type == ITEM_TYPE_SLEEPER) {
								itm->Animation = 0;
							}
						}
					}
				}
			}
			else {

				// Item: ORB
				if (itm->Type == ITEM_TYPE_ORB) {
					if (p->Tick > itm->Animationtick) {
						itm->Animationtick = p->Tick + 1000;
						itm->Animation++;
						if (itm->Animation > 2) {
							itm->Animation = 0;
						}
					}
				}
			}
		}

		// Get the next item
		itm = itm->next;
	}
}

/***************************************************************
 * Function:	delItem
 *
 * @param del
 **************************************************************/
CItem *CItemList::delItem(CItem *del) {
	CItem *returnItem = del->next;

	// If item has a next, tell that next to skip this over node
	if (del->next) {
		del->next->prev = del->prev;
	}

	// If item has a prev, tell that prev to skip this over node
	if (del->prev) {
		del->prev->next = del->next;
	}

	// Else (item has no prev), item is head, point head to next node
	else {
		this->itemListHead = del->next;
	}

	// Delete del
	delete del;
	
	// Return what was del->next
	return returnItem;
}

/***************************************************************
 * Function:	deleteItemsByCity
 *
 * @param theCity
 **************************************************************/
void CItemList::deleteItemsByCity(char theCity) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item belongs to the city, delete the item
		if (itm->City == theCity) {
			itm = this->delItem(itm);
		}

		// Else (item doesn't belong to city), get the next item
		else {
			itm = itm->next;
		}
	}
}








/***************************************************************
 * Class:		CInventory
 *
 **************************************************************/


/***************************************************************
 * Constructor:	CInventory
 *
 * @param game
 **************************************************************/
CInventory::CInventory(CGame *game) {
	this->p = game;
	this->itemListHead = 0;
	this->SelectedItemType = 0;
}
/***************************************************************
 * Destructor:	CInventory
 *
 **************************************************************/
CInventory::~CInventory() {
	while (this->itemListHead) {
		this->delItem(this->itemListHead);
	}
}

/***************************************************************
 * Function:	findItem
 *
 * @param id
 **************************************************************/
CItem *CInventory::findItem(unsigned short id) {
	CItem *itm = this->itemListHead;
	
	// For each item
	while (itm) {

		// If the item matches the ID, return
		if (itm->id == id) {
			return itm;
		}

		// Else, get next item
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	newItem
 *
 * Note that this function is duplicated above.
 *
 * @param X
 * @param Y
 * @param Type
 * @param City
 * @param id
 * @param active
 **************************************************************/
CItem *CInventory::newItem(int X, int Y, int Type, int City, unsigned short id, int active) {
	CItem *itm = new CItem(X,Y,Type,City,id,active,p);

	// If there are other items,
	if (this->itemListHead) {

		// Tell the current head the new item is before it
		this->itemListHead->prev = itm;
		itm->next = this->itemListHead;

		// If there is no selected item type, select this type
		if (!this->SelectedItemType) {
			SelectedItemType = itm->Type;
		}
	}

	// Else, select this type regardless
	else {
		SelectedItemType = itm->Type;
	}

	// Add the new item as the new head
	this->itemListHead = itm;

	// Return a pointer to the new item
	return itm;
}

/***************************************************************
 * Function:	delItem
 *
 * @param del
 **************************************************************/
CItem *CInventory::delItem(CItem *del) {
	CItem *returnItem = del->next;
	int i = 0;

	// If item has a next, tell that next to skip this over node
	if (del->next) {
		del->next->prev = del->prev;
	}

	// If item has a prev, tell that prev to skip this over node
	if (del->prev) {
		del->prev->next = del->next;
	}

	// Else (item has no prev), item is head, point head to next node
	else {
		this->itemListHead = del->next;
	}

	// Delete del
	delete del;
	
	// Return what was del->next
	return returnItem;
}

/***************************************************************
 * Function:	Cycle
 *
 * @param theCity
 **************************************************************/
void CInventory::Cycle() {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// Item: ORB
		if (itm->Type == ITEM_TYPE_ORB) {

			// If the Animation timer is up, reset the timer
			if (p->Tick > itm->Animationtick) {
				itm->Animationtick = p->Tick + 1000;
				itm->Animation++;
				if (itm->Animation > 2) {
					itm->Animation = 0;
				}
			}
		}

		// Get the next item
		itm = itm->next;
	}
}

/***************************************************************
 * Function:	ItemCheck
 *
 **************************************************************/
void CInventory::ItemCheck() {
	CItem *itm = this->itemListHead;
	bool hasCloak = false;
	bool hasRocket = false;
	bool hasMedkit = false;
	bool hasBomb = false;
	bool hasMine = false;
	bool hasOrb = false;
	bool hasFlare = false;
	bool hasDFG = false;
	bool hasWall = false;
	bool hasTurret = false;
	bool hasSleeper = false;
	bool hasPlasma = false;

	// For each item,
	while (itm) {

		switch (itm->Type) {

			// Item: CLOAK
			case ITEM_TYPE_CLOAK:
				hasCloak = true;
				break;

			// Item: ROCKET
			case ITEM_TYPE_ROCKET:
				hasRocket = true;
				break;

			// Item: MEDKIT
			case ITEM_TYPE_MEDKIT:
				hasMedkit = true;
				break;

			// Item: BOMB
			case ITEM_TYPE_BOMB:
				hasBomb = true;
				break;

			// Item: MINE
			case ITEM_TYPE_MINE:
				hasMine = true;
				break;

			// Item: ORB
			case ITEM_TYPE_ORB:
				hasOrb = true;
				break;

			// Item: FLARE
			case ITEM_TYPE_FLARE:
				hasFlare = true;
				break;

			// Item: DFG
			case ITEM_TYPE_DFG:
				hasDFG = true;
				break;

			// Item: WALL
			case ITEM_TYPE_WALL:
				hasWall = true;
				break;

			// Item: TURRET
			case ITEM_TYPE_TURRET:
				hasTurret = true;
				break;
			
			// Item: SLEEPER
			case ITEM_TYPE_SLEEPER:
				hasSleeper = true;
				break;

			// Item: PLASMA
			case ITEM_TYPE_PLASMA:
				hasPlasma = true;
				break;
		}

		// Get the next item
		itm = itm->next;
	}

	p->InGame->HasCloak = ( hasCloak ? 1 : 0 );
	p->InGame->HasRocket = ( hasRocket ? 1 : 0 );
	p->InGame->HasMedkit = ( hasMedkit ? 1 : 0 );
	p->InGame->HasBomb = ( hasBomb ? 1 : 0 );
	p->InGame->HasMine = ( hasMine ? 1 : 0 );
	p->InGame->HasOrb = ( hasOrb ? 1 : 0 );
	p->InGame->HasFlare = ( hasFlare ? 1 : 0 );
	p->InGame->HasDFG = ( hasDFG ? 1 : 0 );
	p->InGame->HasWall = ( hasWall ? 1 : 0 );
	p->InGame->HasTurret = ( hasTurret ? 1 : 0 );
	p->InGame->HasSleeper = ( hasSleeper ? 1 : 0 );
	p->InGame->HasPlasma = ( hasPlasma ? 1 : 0 );
}

/***************************************************************
 * Function:	findItembyType
 *
 * @param Type
 **************************************************************/
CItem *CInventory::findItembyType(int Type) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item matches the Type, return it
		if (itm->Type == Type) {
			return itm;
		}

		// Else, get the next item
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	Drop
 *
 **************************************************************/
void CInventory::Drop() {
	int me = p->Winsock->MyIndex;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;
	sCMItem ii;
	CItem *itm;

	// If the player is not frozen,
	if (p->Player[p->Winsock->MyIndex]->isFrozen == 0) {

		// If an item is selected,
		itm = p->Inventory->findItembyType(p->Inventory->SelectedItemType);
		if (itm) {
			ii.id = itm->id;

			// Item: in range required
			if (itm->Type >= 8 || itm->Type == ITEM_TYPE_MINE) {

				// If the player is in range,
				if (p->Build->inRange(true)) {

					// Tell everyone about the item drop
					itm->active = 1;
					ii.active = itm->active;
					ii.type = itm->Type;
					p->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
				}
			}

			// Else (no range check required)
			else {

				// Item: DFG?
				if (itm->Type == ITEM_TYPE_DFG) {
					itm->active = 1;
				}

				// Item: BOMB
				else if (itm->Type == ITEM_TYPE_BOMB) {
					itm->active = p->InGame->BombsAreActivated;
				}

				// Tell everyone about the item drop
				ii.active = itm->active;
				ii.type = itm->Type;
				p->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
			}
		}
	}
}

/***************************************************************
 * Function:	triggerItem
 *
 * @param type
 **************************************************************/
void CInventory::triggerItem(int type) {
	CItem *itm;
	CPlayer* player = this->p->Player[this->p->Winsock->MyIndex];

	// If the current player is alive,
	if (!player->isDead)  {

		// If the item was found,
		itm = p->Inventory->findItembyType(type);
		if (itm) {

			// Item: MEDKIT
			if (type == ITEM_TYPE_MEDKIT) {

				// Refill health, play sound, tell server to delete item (and check for med), delete item
				player->SetHP(MAX_HEALTH);
				this->p->Sound->PlayWav(sClick, -1);
				this->p->Winsock->SendData(cmMedKit, (char *)&itm->id, sizeof(itm->id));
				this->p->Inventory->delItem(itm);
			}

			// Item: CLOAK
			else if (type == ITEM_TYPE_CLOAK) {
				this->p->Winsock->SendData(cmCloak, (char *)&itm->id, sizeof(itm->id));
			}

			// Item: BOMB
			else if (type == ITEM_TYPE_BOMB) {
				// Activate the bomb stack
				p->InGame->BombsAreActivated = 1 - p->InGame->BombsAreActivated;
			}
		}
	}
}

