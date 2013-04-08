/*
===============================================================================

    Battle City - CItem file
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
 * Constructor:	CItem
 *
 * @param x
 * @param y
 * @param type
 * @param City
 * @param id
 * @param Server
 **************************************************************/
CItem::CItem(int x, int y, int type, int City, unsigned short id, CServer *Server) {
	this->p = Server;
	this->prev = 0;
	this->next = 0;
	this->x = x;
	this->y = y;
	this->City = City;
	this->type = type;
	this->id = id;
	this->explodetick = 0;
	this->holder = -1;
	this->active = 0;

	// Switch on item type
	switch (type) {

		// Item: WALL
		case 8:
			life = 40;
			break;

		// Item: PLASMA
		case 11:
			life = 40;
			break;

		// Item: TURRET
		case 9:
			life = 32;
			break;

		// Item: SLEEPER
		case 10:
			life = 16;
			break;
	}
}

/***************************************************************
 * Destructor:	CItem
 *
 **************************************************************/
CItem::~CItem() {
}

/***************************************************************
 * Function:	drop
 *
 * @param x
 * @param y
 * @param owner
 **************************************************************/
void CItem::drop(int x, int y, int owner) {	
	sSMItem msg;
	int deleted;
	int CityX;
	int CityY;
	int CalcX;
	int CalcY;

	// If this item is being held by the person dropping the item
	if (this->holder == owner) {

		// Clear out the holder
		holder = -1;

		// Item: BOMB (active)
		if (this->type == 3 && this->active == 1) {
			
			// Set the bomb timer
			this->explodetick = p->Tick + TIMER_BOMB;
		}

		// Item: ORB
		if (this->type == 5) {
			deleted = 0;
			active = 0;

			// For each possible city,
			for (int i = 0; i < MAX_CITIES; i++) {

				// If that city isn't this item's owners' city, 
				if (i != p->Player[owner]->City) {

					// Calculate the position of that city's CC
					CityX = (int)(512)-(32+(i % 8*MAX_CITIES));
					CityY = (int)(512)-(32+(i / 8*MAX_CITIES));

					// Calculate the offset between the orb's position and the city's CC's position
					CalcX = CityX - x;
					CalcY = CityY - y;

					// HACK: Fix for bottom row bug
					// If the city index is greater than 76, increment the X-position of the city's CC
					if (i > 76) {
						CalcX++;
					}
					
					// If the orb is on the CC,
					if ((CalcY == 2) && (CalcX >= 0) && (CalcX <= 2)) {

						// If the city is orbable,
						if (p->City[i]->isOrbable()) {

							// Make this item belong to the owner's city, in case of admin hijinx
							this->City = p->Player[owner]->City;

							// Trigger the orb
							this->p->Log->logOrb(owner, i, this->p->City[p->Player[owner]->City]->maxBuildingCount);
							this->p->City[p->Player[owner]->City]->didOrb(i, owner);
							this->p->City[i]->wasOrbed();
							this->p->Item->delItem(this);
							deleted = 1;
							break;
						}
					}
				}
			}

			// If the orb was not deleted as a result of being dropped,
			if (deleted == 0) {

				// Reset the item,
				holder = -1;
				this->City = p->Player[owner]->City;
				this->x = x; 
				this->y = y; 

				// Tell all players in the sector about the item
				msg.active = (unsigned char)this->active;
				msg.City = this->City;
				msg.id = this->id;
				msg.type = this->type;
				msg.x = this->x;
				msg.y = this->y;

				p->Send->SendSectorArea(this->x*48, this->y*48, smAddItem, (char *)&msg, sizeof(msg));
			}
		}

		// Item: OTHER
		else {
			// Reset the item,
			holder = -1;
			this->City = p->Player[owner]->City;
			this->x = x; 
			this->y = y; 

			// Tell all players in the sector about the item
			msg.active = (unsigned char)this->active;
			msg.City = this->City;
			msg.id = this->id;
			msg.type = this->type;
			msg.x = this->x;
			msg.y = this->y;

			p->Send->SendSectorArea(this->x*48, this->y*48 ,smAddItem, (char *)&msg, sizeof(msg));
		}
	}
}









/***************************************************************
 * Constructor:	CItemList
 *
 * @param Server
 **************************************************************/
CItemList::CItemList(CServer *Server) {
	this->p = Server;
	this->itmID = 1;
	this->itemListHead = 0;
}

/***************************************************************
 * Destructor:	CItemList
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

		// If the item matches the id param, return it
		if (itm->id == id) {
			return itm;
		}
		itm = itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	newItem
 *
 * @param x
 * @param y
 * @param type
 * @param City
 * @param id
 **************************************************************/
CItem *CItemList::newItem(int x, int y, int type, int City, unsigned short id) {
	CItem *itm = new CItem(x,y,type,City,id,p);

	// If there are other items,
	if (this->itemListHead) {

		// Tell the current head the new item is before it
		this->itemListHead->prev = itm;
		itm->next = this->itemListHead;
	}

	// Add the new item as the new head
	this->itemListHead = itm;

	// Increment the city's item counter for this item
	p->City[City]->itemC[type] += 1;

	// Return a pointer to the new item
	return itm;
}

/***************************************************************
 * Function:	delItem
 *
 * @param del
 **************************************************************/
CItem *CItemList::delItem(CItem *del) {
	sSMItem packet;
	packet.id = del->id;

	// Decrement the city's counter for this item  type
	p->City[del->City]->itemC[del->type] -= 1;

	// If the item is not being held, tell the sector it is gone
	if (del->holder == -1) {
		p->Send->SendSectorArea(del->x*48, del->y*48, smRemItem,(char *)&packet,sizeof(packet));
	}
	// Else, tell only the holder it is gone
	else {
		p->Winsock->SendData(del->holder, smRemItem,(char *)&packet,sizeof(packet));
	}

	// Remove the item
	return this->remItem(del);
};

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CItemList::cycle() {
	CItem *itm = this->itemListHead;
	sSMExplode bomb;
	CBuilding *buildingToExplode;
	CItem *itemsToExplode;
	bool alreadyHasNextItem;

	// For each item in the item list,
	while (itm) {
		alreadyHasNextItem = false;

		// Item: BOMB (active)
		if ( (itm->type == ITEM_TYPE_BOMB) && (itm->active == 1)) {

			// If the Explode timer is up, and the item is not being held,
			if ( (this->p->Tick > itm->explodetick) && (itm->holder == -1)) {

				// Tell the sector about the explosion
				bomb.City = itm->City;
				bomb.x = (unsigned short)itm->x+1;
				bomb.y = (unsigned short)itm->y+1;
				this->p->Send->SendSectorArea(bomb.x*48, bomb.y*48, smExplosion, (char *)&bomb, sizeof(bomb));

				// For each itemToExplode,
				itemsToExplode = this->itemListHead;
				while (itemsToExplode) {

					// If the itemToExplode isn't this item, and is adjacent to the bomb 
					if ( (itemsToExplode->id != itm->id) && (abs((itemsToExplode->x + 1) - bomb.x) < 2) && (abs((itemsToExplode->y + 1) - bomb.y) < 2) && (itemsToExplode->holder == -1) ) {

						// Delete the itemToExplode (note that delItem returns item->next)
						itemsToExplode = this->delItem(itemsToExplode);
					}
					// Else, get the next item
					else {
						itemsToExplode = itemsToExplode->next;
					}
				}

				// Delete the item (note that delItem returns item->next)
				itm = this->delItem(itm);
				alreadyHasNextItem = true;

				// For each buildingToExplode,
				buildingToExplode = this->p->Build->buildingListHead;
				while (buildingToExplode) {

					// If the buildingToExplode is adjacent to the bomb 
					if ( (abs(buildingToExplode->x - bomb.x) < 3) && (abs(buildingToExplode->y - bomb.y) < 3) && (buildingToExplode->City >= 0) ) {

						// Delete the building
						buildingToExplode = this->p->Build->delBuilding(buildingToExplode);
					}
					else {
						buildingToExplode = buildingToExplode->next;
					}
				}

				// If the item pointer got deleted (such as by bombing your own bombs), start over
				if (!itm || !itm->type || (itm->type < 0)) {
					itm = this->itemListHead;
				}
			}
		}

		// If not alreadyHasNextItem, get the next item in the list
		if (alreadyHasNextItem == false) {
			itm = itm->next;
		}
	}
}

/***************************************************************
 * Function:	deleteItemsByCity
 *
 * Note that this function uses remItem instead of delItem,
 * so it doesn't send out item-destroyed packets
 *
 * @param city
 * @param type
 **************************************************************/
void CItemList::deleteItemsByCity(int city) {
	CItem *itm = this->itemListHead;

	// For each item in the linked list,
	while (itm) {

		// If the item belongs to this city,
		if (itm->City == city) {

			// Delete the item and move the pointer to the next item in the linked list
			itm = this->remItem(itm);
		}
		// Else (item belongs to other city),
		else {

			// Move the pointer to the next item in the linked list
			itm = itm->next;
		}
	}
}

/***************************************************************
 * Function:	deleteItemsByFactory
 *
 * @param city
 * @param type
 **************************************************************/
void CItemList::deleteItemsByFactory(int city, int type) {
	CItem *itm = this->itemListHead;

	// For each item in the linked list,
	while (itm) {

		// If the item belongs to this city, and the item was made by this Factory
		if ((itm->City == city) && (itm->type == itemTypes[(type - 2) / 2 - 1])) {
			
			// Delete the item and move the pointer to the next item in the linked list
			itm = this->delItem(itm);
		}
		// Else (item belongs to other city, or item not made by this Factory),
		else {

			// Move the pointer to the next item in the linked list
			itm = itm->next;
		}
	}
}

/***************************************************************
 * Function:	deleteItemsByPlayer
 *
 * @param id
 **************************************************************/
void CItemList::deleteItemsByPlayer(int id) {
	CItem *itm = this->itemListHead;

	// For each item in the linked list,
	while (itm) {

		// If the item is being held by the player,
		if (itm->holder == id) {
			
			// Delete the item and move the pointer to the next item in the linked list
			itm = this->delItem(itm);
		}
		// Else (item belongs to other city, or item not made by this Factory),
		else {

			// Move the pointer to the next item in the linked list
			itm = itm->next;
		}
	}
}

/***************************************************************
 * Function:	remItem
 *
 * @param del
 **************************************************************/
CItem *CItemList::remItem(CItem *del) {
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
};
