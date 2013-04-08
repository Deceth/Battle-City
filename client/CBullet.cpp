/*
===============================================================================

    Battle City - CBullet
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
* @param game
**************************************************************/
CBullet::CBullet(int x, int y, int type, int angle, unsigned short owner, CGame *game) {
	this->p = game;
	this->owner = owner;
	this->x = (float)x;
	this->y = (float)y;
	this->type = type;
	this->animation = 0;
	this->angle = angle;
	this->prev = 0;
	this->next = 0;
	
	// Owner: PLAYER
	if (this->type == 3) {
		this->turretId = 0;
	}

	// Owner: TURRET
	else if (this->type > 2)  {
		this->turretId = this->owner;
		this->type -= 3;
	}

	// Owner: PLAYER
	else {
		this->turretId = 0;
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
	
	// Type: Flare
	else if (this->type == 3) {
		this->life = 2500;
		this->damage = 5;
	}

	// Type: PLASMA
	else if (this->type >= 2) {
		this->life = 340;
		this->damage = 12;
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
* @param game
**************************************************************/
CBulletList::CBulletList(CGame *game) {
	this->p = game;
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
void CBulletList::Cycle() {
	float tick = p->Tick;
	CBullet *blt;
	Rect rp, rb;
	rb.w = 4;
	rb.h = 4;
	int me = p->Winsock->MyIndex;
	CBuilding *bld;
	CItem *itm;
	float fDir;
	float MoveY;
	float MoveX;
	char dmg = 0;
	char deathPacket[3];

	// For each bullet,
	blt = this->bulletListHead;
	while (blt) {

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
		
		if (MoveX > 20) {
			MoveX = 20;
		}
		if (MoveY > 20) {
			MoveY = 20;
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
		rb.X = (long)blt->x;
		rb.Y = (long)blt->y;
		rp.w = 48;
		rp.h = 48;


		// Bullet: ADMIN
		if (blt->turretId == 0 && p->Player[blt->owner]->isAdmin()) {

			// Switch on bullet type
			switch(blt->type) {

				// Type: 1
				case 1:
					 p->Explode->newExplosion((int)blt->x+24, (int)blt->y+24, 1);
					 break;

				// Type: 2
				case 2:
					 p->Explode->newExplosion((int)blt->x+72, (int)blt->y+72, 2);
					 break;

				// Type: 3
				case 3:
					 p->Explode->newExplosion((int)blt->x+2, (int)blt->y+2, 3);
					 break;
			}
		}

		/***************************************************************
		 * Players
		 **************************************************************/
		// If the bullet has life left,
		if (blt->life > 0) {

			// For each possible player,
			for (int pl = 0; pl < MAX_PLAYERS; pl++) {

				// If the player is in game, didn't shoot the bullet, and is alive,
				if ((p->Player[pl]->isInGame) && (pl != blt->owner) && (p->Player[pl]->isDead == false)) {

					// If the bullet hit the player,
					rp.X = (long)p->Player[pl]->X-48;
					rp.Y = (long)p->Player[pl]->Y-48;
					if (p->Collision->RectCollision(rp,rb)) {

						// Kill the bullet
						blt->life = -1;
						
						// If the player is me, and I'm not an admin,
						if ((pl == p->Winsock->MyIndex) && (p->Player[pl]->isAdmin() == false)) {

							// Subtract health, play hit wav
							p->Sound->PlayWav(sHit, 13);
							p->Player[pl]->SetHP(p->Player[pl]->HP - blt->damage);

							// If the bullet will kill me,
							if (p->Player[pl]->HP <= 0) {
//							if (p->Player[pl]->HP <= 0 || (blt->turretId == 0 && p->Player[blt->owner]->isAdmin())) {
								
								// Owner: PLAYER
								if (blt->turretId == 0) {

									// Set deathPacket[0] to the shooter's city
									deathPacket[0] = p->Player[blt->owner]->City;
								}

								// Owner: TURRET
								else {

									// Find the item, find its city, set deathPacket[0] to the turret's city
									itm = p->Item->findItem(blt->turretId);
									if (itm) {
										deathPacket[0] = itm->City;
									}
								}

								// Send the death packet
								deathPacket[1] = 0;
								p->Player[p->Winsock->MyIndex]->isDead = true;
								p->InGame->timeDeath = p->Tick;
								p->Winsock->SendData(cmDeath, deathPacket, 1);
								p->Sound->PlayWav(sDie, 5);
							}
						}
					}
				}
			}
		}

		/***************************************************************
		 * Items
		 **************************************************************/
		// If the bullet has life left,
		if (blt->life > 0) {

			// For each item,
			itm = p->Item->itemListHead;
			while (itm) {

				// Set the collision rectangle for the item
				rp.X = itm->X * 48 - 48;
				rp.Y = itm->Y * 48 - 48;

				// If the bullet hit the item other than its owner
				if (p->Collision->RectCollision(rp,rb) && itm->id != blt->turretId) {

					// Owner: TURRET
					if (blt->turretId > 0) {

						// If the item is active and type >= 8 (collision)
						if (itm->active && itm->Type >= 8) {

							// Kill the bullet
							blt->life = -1;

							// If I am within range, play a sound, break
							if ((abs(blt->x - p->Player[me]->X) < 1000) && (abs(blt->y - p->Player[me]->Y) < 1000)) {
								p->Explode->newExplosion((int)blt->x+24, (int)blt->y+24, 1);
								p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->x, blt->y);
							}
							break;
						}
					}

					// Owner: PLAYER
					else {

						// Kill the bullet
						blt->life = -1;

						// If I am within range, play a sound, break
						if ((abs(blt->x - p->Player[me]->X) < 1000) && (abs(blt->y - p->Player[me]->Y) < 1000)) {
							p->Explode->newExplosion((int)blt->x+24, (int)blt->y+24, 1);
							p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->x, blt->y);
						}
						break;
					}
				}

				// Get the next item
				itm = itm->next;
			}
		}

		/***************************************************************
		 * Buildings
		 **************************************************************/
		// For each building,
		bld = p->Build->buildingListHead;
		while (bld) {

			// Set up rectangle for collision measurement
			rp.X = (bld->X-3)*48;
			rp.w = 144;

			// If the building is a Factory or Hospital (anything with a bay?),
			if (bld->Type / 100 <= 2) {
				rp.Y = (bld->Y-2)*48;
				rp.h = 96;
			}
			else {
				rp.Y = (bld->Y-3)*48;
				rp.h = 144;
			}

			// If the bullet hits a building,
			if (p->Collision->RectCollision(rp,rb)) {

				// Kill the bullet
				blt->life = -1;

				// If I am within range, play a sound, break
				if ((abs(blt->x - p->Player[me]->X) < 1000) && (abs(blt->y - p->Player[me]->Y) < 1000)) {
					p->Explode->newExplosion((int)blt->x+24, (int)blt->y+24, 1);
					p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->x, blt->y);
				}
				break;
			}

			// Get the next building
			bld = bld->next;
		}



		/***************************************************************
		 * Rocks
		 **************************************************************/
		if (p->Map->map[(int)(blt->x+48)/48][(int)(blt->y+48)/48] == 2)  {

			// Kill the bullet
			blt->life = -1;

			// If I am within range, play a sound, break
			if ((abs(blt->x - p->Player[me]->X) < 1000) && (abs(blt->y - p->Player[me]->Y) < 1000)) {
				p->Explode->newExplosion((int)blt->x+24, (int)blt->y+24, 1);
				p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->x, blt->y);
			}
		}
		
		// If the bullet is now dead, delete the bullet
		if (blt->life < 0) { 
			blt = delBullet(blt);
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
}
