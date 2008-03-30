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
	this->lastturn = 0;
	this->target = -1;
	this->bullet = 0;
	this->Life = 0;
	this->Animation = 0;
	this->Animationtick = 0;

	if (Type > 8) {
		lastturn = p->Tick + 2000;
		if (Type == 9 || Type == 10) {
			dmg = 15;
		}
		else {
			dmg = 30;
		}
	}
	this->X = x;
	this->Y = y;
	this->Type = type;
	this->id = id;
	this->City = City;
	this->active = active;
}

/***************************************************************
 * Destructor:	CItem
 *
 **************************************************************/
CItem::~CItem() {
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
 **************************************************************/
CItem *CItemList::findItembyLocationAndType(int X, int Y, int Type) {
	CItem *itm = this->itemListHead;

	// For each item,
	while (itm) {

		// If the item matches the X and Y and Type, return it
		if (itm->X == X && itm->Y == Y && itm->Type == Type) {
			return itm;
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
	CBullet *blt;
	int me = p->Winsock->MyIndex;

	// For each item,
	itm = this->itemListHead;
	while (itm) {

		if (itm->Type > 8 && itm->active) {

			if (itm->Life == 1 && p->Tick > itm->Animationtick) {

				itm->Animationtick = p->Tick + 50;
				if (itm->Animation == 2) {
					itm->Animation = 1;
				}
				else {
					itm->Animation = 2;
				}
			}

			itm->target = -1;
			
			// 1024
			//int distance = 455;

			// 800
			int distance = 360;

			int d;
			for (int i = 0; i < MAX_PLAYERS; i++) {

				if (p->Player[i]->isInGame && (itm->City != p->Player[i]->City) && (p->Player[i]->isAdmin() == false)) {

					if (p->Player[i]->isDead == true) {

					}
					else {

						d = (int)sqrt((p->Player[i]->X-itm->X*48)*(p->Player[i]->X-itm->X*48)+(p->Player[i]->Y-itm->Y*48)*(p->Player[i]->Y-itm->Y*48));

						// distance from turret < distance
						if (d < distance) {
							distance = d; // distance from turret
							itm->target = (char)p->Player[i]->id;
						}
					}
				}
			}

			if (itm->target != -1) {
				itm->Angle = (float)atan((float)(p->Player[itm->target]->X / 48 - itm->X) / (p->Player[itm->target]->Y / 48 - itm->Y));
				itm->Angle = (float)(itm->Angle * 180 / 3.14);

				// find what quad

				// player == right
				if (p->Player[itm->target]->X / 48 < itm->X) {

					// player == down
					if (p->Player[itm->target]->Y / 48 < itm->Y) {
						itm->Angle = 180 - itm->Angle;
					}
					else {
						itm->Angle = - itm->Angle;
					}
				}
				else {
					// player == down
					if (p->Player[itm->target]->Y / 48 < itm->Y) {
						itm->Angle = 180 - itm->Angle;
					}
					else {
						itm->Angle = 360 - itm->Angle;
					}
				}

				if (p->Tick > itm->lastturn) {

					if (!itm->bullet) {

						int tmpAngle = 0;
						tmpAngle = (int)(itm->Angle/1.125);

						if (tmpAngle % 10 >= 5) {
							tmpAngle += 10;
						}

						tmpAngle = tmpAngle / 10;
						int tmpdmg = 0;
						if (itm->Type == 11) tmpdmg = 5; else tmpdmg = 4;
						float fDir = (float)-tmpAngle+32;
						int FlashY = itm->Y * 48-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
						int FlashX = itm->X * 48-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
						p->Explode->newExplosion(FlashX, FlashY, 3);
						itm->bullet = p->Bullet->newBullet(FlashX, FlashY, tmpdmg, tmpAngle, itm->id);
						if ((abs(itm->X * 48-24 - p->Player[me]->X) < 1000) && (abs(itm->Y * 48-24 - p->Player[me]->Y) < 1000))
						itm->lastturn = p->Tick + 250;
						p->Sound->Play3dSound(p->Sound->s_bigturret, 100, (float)FlashX, (float)FlashY);
					}

					else {
						int foundbullet = 0;

						// For each bullet,
						blt = this->p->Bullet->bulletListHead;
						while (blt) {

							// If the bullet belongs to this turret,
							if (blt->turretId == itm->id) {

								// Save foundbullet=1, break
								foundbullet = 1;
								break;
							}
							blt = blt->next;
						}

						// If we didn't find a bullet for this item, set its bullet value to 0
						if (foundbullet == 0) {
							itm->bullet = 0;
						}
					}
				}
			}
			else {
				if (itm->Type == 10) {
					itm->Animation = 0;
				}
			}
		}
		else {

			// Item: ORB
			if (itm->Type == 5) {
				if (p->Tick > itm->Animationtick) {
					itm->Animationtick = p->Tick + 1000;
					itm->Animation++;
					if (itm->Animation > 2) {
						itm->Animation = 0;
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
		if (itm->Type == 5) {

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
	bool hasLaser = false;
	bool hasRocket = false;
	bool hasUpLink = false;

	// For each item,
	while (itm) {

		switch (itm->Type) {

			// Item: LASER
			case 0:
				hasLaser = true;
				break;

			// Item: ROCKET
			case 1:
				hasRocket = true;
				break;

			// Item: UPLINK
			case 6:
				hasUpLink = true;
				break;
		}

		// Get the next item
		itm = itm->next;
	}

	p->InGame->HasLaser = ( hasLaser ? 1 : 0 );
	p->InGame->HasRocket = ( hasRocket ? 1 : 0 );
	p->InGame->HasUpLink = ( hasUpLink ? 1 : 0 );
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
			if (itm->Type >= 8 || itm->Type == 4) {

				// If the player is in range,
				if (p->Build->inRange() == 1) {

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
				if (itm->Type == 7) {
					itm->active = 1;
				}

				// Item: BOMB
				else if (itm->Type == 3) {
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
