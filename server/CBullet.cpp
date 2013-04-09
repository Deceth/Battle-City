/*
===============================================================================

    Battle City - CBullet file
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
#include "CBullet.h"

/***************************************************************
* Constructor:	CBullet
*
* @param x
* @param y
* @param type
* @param angle
* @param owner
* @param Server
**************************************************************/
CBullet::CBullet(int x, int y, int type, int angle, unsigned short owner, CServer *Server) {
	this->p = Server;
	this->owner = owner;
	this->x = (float)x;
	this->y = (float)y;
	this->type = type;
	this->damage = 0;
	this->anim = 0;
	this->angle = angle;
	this->prev = 0;
	this->next = 0;

	// Owner: PLAYER
	if (this->type == 3) {
		this->turid = 0;
	}
	
	// Owner: TURRET
	else if (this->type > 2)  {
		this->turid = this->owner;
		this->type -= 3;
	}

	// Owner: PLAYER
	else {
		this->turid = 0;
	}

	/***************************************************************
	* Life, Damage
	**************************************************************/

	// Type: LASER
	if (this->type == 0) {
		this->life = 260;
		this->damage = 5;
	}

	// Type: GREEN, ZOOK, SLEEPER
	else if (this->type == 1) {
		this->life = 340;
		this->damage = 8;
	}

	// Type: PLASMA
	else if (this->type == 2) {
		this->life = 340;
		this->damage = 12;
	}

	// Type: Flare
	else if (this->type == 3) {
		this->life = 2500;
		this->damage = 5;
	}
}

/***************************************************************
* Destructor: CBullet
*
**************************************************************/
CBullet::~CBullet() {
}




/***************************************************************
* Constructor:	CBulletList
*
**************************************************************/
CBulletList::CBulletList(CServer *server) {
	this->p = server;
	this->bulletListHead = 0;
}

/***************************************************************
* Destructor:	CBulletList
*
**************************************************************/
CBulletList::~CBulletList() {
	while (this->bulletListHead) {
		this->delBullet(this->bulletListHead);
	}
}

/***************************************************************
 * Function:	newBullet
 *
 * @param x
 * @param y
 * @param type
 * @param angle
 * @param owner
 **************************************************************/
CBullet *CBulletList::newBullet(int x, int y, int type, int angle, int owner) {
	CBullet *blt = new CBullet(x, y, type, angle, owner, p);

	// If there are other bullets,
	if (this->bulletListHead) {

		// Tell the current head the new bullet is before it
		this->bulletListHead->prev = blt;
		blt->next = this->bulletListHead;
	}

	// Add the new bullet as the new head
	this->bulletListHead = blt;

	// Return a pointer to the new bullet
	return blt;
}

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CBulletList::cycle() {
	Rect rp;
	Rect rb;
	CItem *itm;
	CBullet *blt;
	CBuilding *bld;
	bool alreadyHasNextItem;
	bool alreadyHasNextBuilding;
	float fDir;
	float MoveY;
	float MoveX;
	sSMItemLife item;

	rb.w = 4;
	rb.h = 4;

	// For each bullet,
	blt = this->bulletListHead;
	while (blt)	{

		// Calculate direction and movement values
		fDir = (float)-blt->angle+32;
		if (blt->type == 3) {
			MoveY = (float)(cos((float)(fDir)/16*3.14)) * (p->TimePassed * MOVEMENT_SPEED_FLARE);
			MoveX = (float)(sin((float)(fDir)/16*3.14)) * (p->TimePassed * MOVEMENT_SPEED_FLARE);
		}
		else {
			MoveY = (float)(cos((float)(fDir)/16*3.14)) * (p->TimePassed * MOVEMENT_SPEED_BULLET);
	 		MoveX = (float)(sin((float)(fDir)/16*3.14)) * (p->TimePassed * MOVEMENT_SPEED_BULLET);
		}

		blt->y += MoveY;
		blt->x += MoveX;

		// Calculate "bullet life" based on time
		blt->life -= (p->TimePassed * MOVEMENT_SPEED_BULLET);

		// If the bullet is off the screen, kill it
		if ((blt->x < 0) || (blt->x > 24576)) {
			blt->x = 0;
			blt->life = -1;
		}
		if ((blt->y < 0) || (blt->y > 24576))	{
			blt->y = 0;
			blt->life = -1;
		}
		// Set rectangles for collision measurements
		rb.x = (long)blt->x;
		rb.y = (long)blt->y;
		rp.w = 48;
		rp.h = 48;

		/***************************************************************
		 * Players
		 **************************************************************/
		// If the bullet has life left,
		if (blt->life > 0) {

			// For each possible player,
			for (int pl = 0; pl < MAX_PLAYERS; pl++) {

				// If the player is in game, didn't shoot the bullet, and is alive,
				if (p->Player[pl]->isInGame() && (pl != blt->owner) && (blt->life > 0) && (p->Player[pl]->isDead == false)) {

					// If the bullet hit he player,
					rp.x = (long)p->Player[pl]->X-48;
					rp.y = (long)p->Player[pl]->Y-48;
					if (p->Collision->checkCollision(rp,rb)) {

						// We could implement server-side health management here
						// That would allow for all players in a sector to know all players' health,
						// And give us the ability to show hit-player-explosions, or even health bars.

						// But, it would mean players would occassionally take damage even when they weren't hit on their screen
						// Lag would be be less of an issue when hitting other people,
						// But would become a big issue when you yourself get hit

						// Kill the bullet and break
						blt->life = -1;
						break;
					}
				}
			}
		}

		/***************************************************************
		 * Items
		 **************************************************************/
		// If the bullet has life left,
		if (blt->life > 0) {

			// For each item in the item list,
			itm = p->Item->itemListHead;
			while (itm) {

				// Reset booleans that track whether we already moved to the next item in the linked list by deleting an item
				alreadyHasNextItem = false;
				alreadyHasNextBuilding = false;

				// Set the collision rectangle for the item
				rp.x = itm->x * 48 - 48;
				rp.y = itm->y * 48 - 48;

				// If the bullet hit the item,
				if (p->Collision->checkCollision(rp,rb) && itm->holder == -1) {

					// If the type >= 8 (collision)
					if (itm->type >= 8) {

						// Damage the item
						itm->life -= blt->damage;

						// If the item has no life,
						if (itm->life < 0) {

							// Delete the item
							itm = p->Item->delItem(itm);
							alreadyHasNextItem = true;
						}
						
						// Else (item has life),
						else {

							// Switch on item type
							switch (itm->type) {

								// Item: 8
								case ITEM_TYPE_WALL:

								// Item: 11
								case ITEM_TYPE_PLASMA:
									if (itm->life < 21) {

										// Burn
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;

								// Item: 10
								case ITEM_TYPE_SLEEPER:
									if (itm->life < 17) {

										// Burn
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;

								// Item: 9
								case ITEM_TYPE_TURRET:
									if (itm->life < 9) {

										// Burn
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;
							}
						}

						// Kill the bullet, break
						blt->life = -1;
						break;
					}

					// Else (type < 8)
					else {

						// Kill the bullet
						blt->life = -1;

						// Kill the item
						itm = p->Item->delItem(itm);
						alreadyHasNextItem = true;
						break;
					}
				}

				// If we don't already have the next item, get the next item
				if (alreadyHasNextItem==false) {
					itm = itm->next;
				}
			}
		}

		/***************************************************************
		 * Buildings
		 **************************************************************/
		// If the bullet has life left,
		if (blt->life > 0) {

			// For each building,
			bld = this->p->Build->buildingListHead;
			while (bld) {
				alreadyHasNextBuilding = false;

				// Set up rectangle for collision measurement
				rp.x = (bld->x-3)*48;
				rp.w = 144;

				// If the building is a Factory or Hospital (anything with a bay?),
				if (bld->isFactory() || bld->isHospital() || bld->isCC()) {
					rp.y = (bld->y-2)*48;
					rp.h = 96;
				}
				else {
					rp.y = (bld->y-3)*48;
					rp.h = 144;
				}

				// If the bullet hits a building,
				if (p->Collision->checkCollision(rp,rb)) {

					// Kill the bullet
					blt->life = -1;

					// If that building is not a CC and has 0 (or less) life,
					if ((! bld->isCC()) && (bld->pop <= 0)) {

						// Destroy the building
						bld = p->Build->delBuilding(bld);
						alreadyHasNextBuilding = true;
					}

					// Break
					break;
				}

				// If we haven't already moved to the next building by deleting a building, get the next building
				if (alreadyHasNextBuilding == false) {
					bld = bld->next;
				}
			}
		}

		/***************************************************************
		 * Rocks
		 **************************************************************/
		// If the bullet still has life,
		if (blt->life > 0) {
			if (p->Map->map[(int)(blt->x+48)/48][(int)(blt->y+48)/48] == 2) {
				blt->life = -1;
			}
		}
		
		// If the bullet is now dead, delete the bullet
		if (blt->life < 0) {
			blt = this->delBullet(blt);
		}

		// Else (didn't delete the bullet), get the next bullet
		else {
			blt = blt->next;
		}
	}
}

/***************************************************************
 * Function:	delBullet
 *
 **************************************************************/
CBullet *CBulletList::delBullet(CBullet *del) {
	CBullet *returnBullet = del->next;

	// If bullet has a next, tell that next to skip this over node
	if (del->next) {
		del->next->prev = del->prev;
	}

	// If bullet has a prev, tell that prev to skip this over node
	if (del->prev) {
		del->prev->next = del->next;
	}
	// Else (bullet has no prev), bullet is head, point head to next node
	else {
		this->bulletListHead = del->next;
	}
	
	// Delete the bullet
	delete del;
	
	// Return what was del->next
	return returnBullet;
};
