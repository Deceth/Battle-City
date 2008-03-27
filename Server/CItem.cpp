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
	if (this->next) {
		next->prev = this->prev;
	}
	if (this->prev) {
		prev->next = this->next;
	}
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
	int buildingCount;


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
						buildingCount = p->Build->GetOrbBuildingCount(i);
						if (buildingCount >= ORBABLE_SIZE) {

							// Trigger the orb
							this->p->Log->logOrb(owner, i, buildingCount);
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
	this->items = 0;
	this->itmID = 1;
}

/***************************************************************
 * Destructor:	CItemList
 *
 **************************************************************/
CItemList::~CItemList() {
	while (this->items) {
		this->delItem(this->items);
	}
}

/***************************************************************
 * Function:	findItem
 *
 * @param id
 **************************************************************/
CItem *CItemList::findItem(unsigned short id) {
	CItem *itm = items;

	if (!itm) {
		return 0;
	}

	while (itm->prev) {
		itm = itm->prev;
	}

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
	CItem *itm = items;

	// If there are no other items,
	if (!itm) {

		// Create this item
		items = new CItem(x,y,type,City,id,p);
			
		// Increment the city's item counter for this item, return
		this->p->City[City]->itemC[type] += 1;
		return items;
	}

	// Else (there are other items)
	else {
		while(itm->next) {
			itm = itm->next;
		}
		
		// Set this item as next on the list
		itm->next = new CItem(x,y,type,City,id,p);
		itm->next->prev = itm;

		// Increment the city's item counter for this item, return
		p->City[City]->itemC[type] += 1;
		return itm->next;
	}

	return 0;
}

/***************************************************************
 * Function:	delItem
 *
 * @param del
 **************************************************************/
CItem *CItemList::delItem(CItem *del) {
	sSMItem packet;

	if (!items) {
		return 0;
	}

	if (del->prev) {
		items = del->prev;
	}
	else if (del->next) {
		items = del->next;
	}
	else {
		items = 0;
	}

	// Decrement the city's counter for this item  type
	p->City[del->City]->itemC[del->type] -= 1;


	packet.id = del->id;

	// If the item is not being held, tell the sector it is gone
	if (del->holder == -1) {
		p->Send->SendSectorArea(del->x*48, del->y*48, smRemItem,(char *)&packet,sizeof(packet));
	}
	// Else, tell only the holder it is gone
	else {
		p->Winsock->SendData(del->holder, smRemItem,(char *)&packet,sizeof(packet));
	}

	delete del;
	
	return items;
};

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CItemList::cycle() {
	CItem *itm = items;
	sSMExplode bomb;
	CBuilding *buildingToExplode;
	CItem *itemsToExplode;

	if (!itm) {
		return;
	}

	while (itm->prev) {
		itm = itm->prev;
	}

	while (itm) {

		// Item: BOMB (active)
		if (itm->type == 3 && itm->active == 1) {

			// If the Explode timer is up, and the item is not being held,
			if ( (p->Tick > itm->explodetick) && (itm->holder == -1)) {

				// Tell the sector about the explosion
				bomb.City = itm->City;
				bomb.x = (unsigned short)itm->x+1;
				bomb.y = (unsigned short)itm->y+1;
				itm = this->delItem(itm);
				p->Send->SendSectorArea(bomb.x*48, bomb.y*48, smExplosion, (char *)&bomb, sizeof(bomb));

				// Get a pointer to the buildingToExplode
				buildingToExplode = this->p->Build->buildings;
				if (buildingToExplode) {
					while (buildingToExplode->prev) {
						buildingToExplode = buildingToExplode->prev;
					}

					// For each buildingToExplode,
					while (buildingToExplode) {

						// If the buildingToExplode is adjacent to the bomb 
						if ( (abs(buildingToExplode->x - bomb.x) < 3) && (abs(buildingToExplode->y - bomb.y) < 3) && (buildingToExplode->City >= 0) ) {

							// Delete the building
							buildingToExplode = p->Build->delBuilding(buildingToExplode);
						}
						buildingToExplode = buildingToExplode->next;
					}
				}

				// Get a pointer to the itemsToExplode
				itemsToExplode = p->Item->items;
				if (itemsToExplode) {
					while (itemsToExplode->prev) {
						itemsToExplode = itemsToExplode->prev;
					}

					// For each itemToExplode,
					while (itemsToExplode) {

						// If the itemToExplode is adjacent to the bomb 
						if ( (abs((itemsToExplode->x + 1) - bomb.x) < 2) && (abs((itemsToExplode->y + 1) - bomb.y) < 2) && (itemsToExplode->holder == -1) ) {

							// Delete the itemToExplode
							itemsToExplode = delItem(itemsToExplode);
						}
						itemsToExplode = itemsToExplode->next;
					}
				}
			}
		}

		// Get the next item in the list
		if (itm) {
			itm = itm->next;
		}
	}
}

/***************************************************************
 * Function:	remItem
 *
 * @param x
 * @param y
 * @param owner
 **************************************************************/
CItem *CItemList::remItem(CItem *del) {
	
	// If there are no items, return
	if (!this->items) {
		return 0;
	}

	// Reset the neighboring items' next/prev pointers
	if (del->prev) {
		items = del->prev;
	}
	else if (del->next) {
		items = del->next;
	}
	else {
		items = 0;
	}

	// Delete the item
	delete del;
	
	return items;
};
