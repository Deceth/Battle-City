/*
===============================================================================

    Battle City - CExplode
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
#include "CExplode.h"

/***************************************************************
* Constructor:	CExplosion
*
* @param x
* @param y
* @param type
* @param game
**************************************************************/
CExplosion::CExplosion(int x, int y, int type, CGame *game) {
	this->p = game;
	this->x = x;
	this->y = y;
	this->type = type;
	this->animation = 0;
	this->tick = 0;
	this->prev = 0;
	this->next = 0;
}

/***************************************************************
* Destructor:	CExplosion
*
**************************************************************/
CExplosion::~CExplosion() {
}







/***************************************************************
* Class:		CExplosionList
*
**************************************************************/


/***************************************************************
* Constructor:	CExplosion
*
* @param game
**************************************************************/
CExplosionList::CExplosionList(CGame *game) {
	this->p = game;
	this->explosionListHead = 0;
}

/***************************************************************
* Destructor:	CExplosionList
*
**************************************************************/
CExplosionList::~CExplosionList() {
	while (this->explosionListHead) {
		this->delExplosion(this->explosionListHead);
	}
}


/***************************************************************
* Function:		newExplosion
*
* @param x
* @param y
* @param type
**************************************************************/
CExplosion *CExplosionList::newExplosion(int x, int y, int type) {
	CExplosion *xpl = new CExplosion(x, y, type, p);

	// If there are other explosions,
	if (this->explosionListHead) {

		// Tell the current head the new explosion is before it
		this->explosionListHead->prev = xpl;
		xpl->next = this->explosionListHead;
	}

	// Add the new explosion as the new head
	this->explosionListHead = xpl;

	// Return a pointer to the new explosion
	return xpl;
}

/***************************************************************
 * Function:	delExplosion
 *
 **************************************************************/
CExplosion *CExplosionList::delExplosion(CExplosion *del) {
	CExplosion *returnExplosion = del->next;

	// If explosion has a next, tell that next to skip this over node
	if (del->next) {
		del->next->prev = del->prev;
	}

	// If explosion has a prev, tell that prev to skip this over node
	if (del->prev) {
		del->prev->next = del->next;
	}
	// Else (explosion has no prev), explosion is head, point head to next node
	else {
		this->explosionListHead = del->next;
	}
	
	// Delete the explosion
	delete del;
	
	// Return what was del->next
	return returnExplosion;
}

/***************************************************************
* Function:		Cycle
*
**************************************************************/
void CExplosionList::Cycle() {
	CExplosion *xpl;
	bool alreadyHasNextExplosion;

	// For each explosion,
	xpl = this->explosionListHead;
	while (xpl) {
		alreadyHasNextExplosion = false;

		// If the Explosion timer is up,
		if (p->Tick > xpl->tick) {
			
			// Cycle the animation, reset the Explosion timer
			xpl->animation++;
			xpl->tick = p->Tick+20;

			// Switch on the explosion type
			switch(xpl->type) {

				// Type: 1
				case 1:

				// Type: 2
				case 2:				
					if (xpl->animation == 10) {
						xpl = this->delExplosion(xpl);
						alreadyHasNextExplosion = true;
					}
					break;

				// Type: 3
				case 3:
					if (xpl->animation == 3) {
						xpl = this->delExplosion(xpl);
						alreadyHasNextExplosion = true;
					}
					break;
			}
		}

		// If we don't already have the next explosion, get the next explosion
		if (alreadyHasNextExplosion == false) {
			xpl = xpl->next;
		}
	}
}
