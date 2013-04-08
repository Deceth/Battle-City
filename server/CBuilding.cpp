/*
===============================================================================

    Battle City - CBuilding file
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
 * @param x
 * @param y
 * @param type
 * @param City
 * @param id
 * @param Server
 **************************************************************/
CBuilding::CBuilding(int x, int y, int type, int City, unsigned short id, CServer *Server) {
	this->p = Server;
	this->prev = 0;
	this->next = 0;
	this->x = x;
	this->y = y;
	this->City = City;
	this->type = type;
	this->id = id;

	this->pop = 0;
	this->AttachedID = 0;
	this->AttachedID2 = 0;
	this->AttachedPop = 0;
	this->AttachedPop2 = 0;

	this->ProduceTick = 0;
	this->CashTick = 0;
	this->PopulationTick = 0;
	this->MoneyTick = 0;
	this->ResearchTick = 0;
}

/***************************************************************
 * Destroyer:	CBuilding
 *
 **************************************************************/
CBuilding::~CBuilding() {
}

/***************************************************************
 * Function:	hasMaxPopulation
 *
 * @return Returns true if this building is at its max population
 **************************************************************/
bool CBuilding::hasMaxPopulation() {
	if (this->isHouse()) {
		return this->pop == POPULATION_MAX_HOUSE;
	}
	else {
		return this->pop == POPULATION_MAX_NON_HOUSE;
	}
}

/***************************************************************
 * Function:	isCC
 *
 * @return Returns true if this building is a CC
 **************************************************************/
bool CBuilding::isCC() {
	return this->isCC(this->type);
}
/***************************************************************
 * Function:	isCC
 *
 * @param buildingType
 * @return Returns true if the buildingType is a CC
 **************************************************************/
bool CBuilding::isCC(int buildingType) {
	return (buildingType == 0);
}

/***************************************************************
 * Function:	isFactory
 *
 * @return Returns true if this building is a Factory
 **************************************************************/
bool CBuilding::isFactory() {
	return this->isFactory(this->type);
}
/***************************************************************
 * Function:	isFactory
 *
 * @param buildingType
 * @return Returns true if the buildingType is a Factory
 **************************************************************/
bool CBuilding::isFactory(int buildingType) {
	return
		((buildingType % 2)==0)
		&&
		(buildingType > 2);
}

/***************************************************************
 * Function:	isHospital
 *
 * @return Returns true if this building is a Hospital
 **************************************************************/
bool CBuilding::isHospital() {
	return this->isHospital(this->type);
}
/***************************************************************
 * Function:	isHospital
 *
 * @param buildingType
 * @return Returns true if the buildingType is a House
 **************************************************************/
bool CBuilding::isHospital(int buildingType) {
	return (buildingType == 1);
}

/***************************************************************
 * Function:	isHouse
 *
 * @return Returns true if this building is a House
 **************************************************************/
bool CBuilding::isHouse() {
	return this->isHouse(this->type);
}
/***************************************************************
 * Function:	isHouse
 *
 * @param buildingType
 * @return Returns true if the buildingType is a House
 **************************************************************/
bool CBuilding::isHouse(int buildingType) {
	return (buildingType == 2);
}

/***************************************************************
 * Function:	isResearch
 *
 * @return Returns true if this building is a Research
 **************************************************************/
bool CBuilding::isResearch() {
	return this->isResearch(this->type);
}
/***************************************************************
 * Function:	isResearch
 *
 * @param buildingType
 * @return Returns true if the buildingType is a Research
 **************************************************************/
bool CBuilding::isResearch(int buildingType) {
	return
		((buildingType % 2)==1)
		&&
		(buildingType > 2);
}

















/***************************************************************
 * Constructor:	CBuildingList
 *
 **************************************************************/
CBuildingList::CBuildingList(CServer *Server) {
	this->p = Server;
	this->buildingListHead = 0;
	this->bldID = 1;
}

/***************************************************************
 * Destroyer:	CBuildingList
 *
 **************************************************************/
CBuildingList::~CBuildingList() {
	while (this->buildingListHead) {
		this->remBuilding(this->buildingListHead);
	}
}

/***************************************************************
 * Function:	findBuilding
 *
 * @param id
 **************************************************************/
CBuilding *CBuildingList::findBuilding(unsigned short id) {
	CBuilding *bld = this->buildingListHead;

	// For each building,
	while (bld) {

		// If the building is the one you're looking for, return a pointer to it
		if (bld->id == id) {
			return bld;
		}

		// Else (building didn't match yet), load up the next building
		bld = bld->next;
	}

	// If the building was not found, return 0
	return 0;
}

/***************************************************************
 * Function:	newBuilding
 *
 * @param x
 * @param y
 * @param type
 * @param City
 * @param id
 **************************************************************/
CBuilding *CBuildingList::newBuilding(int x, int y, int type, int City, unsigned short id) {
	CBuilding *bld = new CBuilding(x, y, type, City, id, this->p);

	// If there are other buildings,,
	if (this->buildingListHead) {

		// Tell the current head the new building is before it
		this->buildingListHead->prev = bld;
		bld->next = this->buildingListHead;
	}

	// Add the new building as the new head
	this->buildingListHead = bld;

	// Tell the city to add a building
	this->p->City[City]->addBuilding(bld->type);

	// Return a pointer to the new building
	return bld;
}

/***************************************************************
 * Function:	deleteBuildingsByCity
 *
 * Note that this function uses remItem instead of delItem,
 * so it doesn't send out item-destroyed packets
 *
 * @param city
 * @param type
 **************************************************************/
void CBuildingList::deleteBuildingsByCity(int city) {
	CBuilding *bld = this->buildingListHead;

	// For each building in the linked list,
	while (bld) {

		// If the building belongs to this city, and building isn't a CC,
		if ((bld->City == city) && (bld->isCC() == false)) {

			// Delete the building and move the pointer to the next item in the linked list
			bld = this->remBuilding(bld);
		}
		// Else (building belongs to other city),
		else {

			// Move the pointer to the next building in the linked list
			bld = bld->next;
		}
	}
}

/***************************************************************
 * Function:	delBuilding
 *
 * @param buildingToDelete
 **************************************************************/
CBuilding *CBuildingList::delBuilding(CBuilding *buildingToDelete) {
	CBuilding *otherBuilding;

	/************************************************
	 * Handle items and build-tree
	 ************************************************/
	// Building: FACTORY
	if (buildingToDelete->isFactory()) {
		this->p->Item->deleteItemsByFactory(buildingToDelete->City, buildingToDelete->type);
	}

	// Building: RESEARCH
	else if (buildingToDelete->isResearch()) {
	/*
		// Tell the city it can NOT build this Research's Factory (setCanBuild does its own index++)
		p->City[buildingToDelete->City]->setCanBuild((unsigned char)buildingToDelete->type, 0, true);

		// Tell the city is needs to re-research this Research
		p->City[buildingToDelete->City]->research[(buildingToDelete->type - 3) / 2] = 0;

		// If this building is a Med Research (type 9), set canBuild on Hospital (type 1... setCanBuild does type++)
		if (buildingToDelete->type == 9) {
			p->City[buildingToDelete->City]->setCanBuild(0, 0, true);
		}
	*/
	}

	// Building: ALL
	// Tell the city it can now build this building type MINUS ONE (setCanBuild does its own index++)
	this->p->City[buildingToDelete->City]->setCanBuild(buildingToDelete->type-1, 1, true);


	/************************************************
	 * Handle building-house relationships
	 ************************************************/
	// Building: HOUSE
	if (buildingToDelete->isHouse()) {

		// If the House has a building in slot 1,
		if (buildingToDelete->AttachedID > 0) {

			// Try to find the building in slot 1
			otherBuilding = this->p->Build->findBuilding(buildingToDelete->AttachedID);

			// If the building was found,
			if (otherBuilding) {

				// Detach the building from this House
				otherBuilding->AttachedID = 0;
				otherBuilding->pop = 0;

				// Tell everyone in the sector about the population change
				sSMPop pop;
				pop.id = otherBuilding->id;
				pop.pop = otherBuilding->pop / 8;
				this->p->Send->SendSectorArea(otherBuilding->x*48,otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}

		// If the House has a building in slot 2,
		if (buildingToDelete->AttachedID2 > 0) {

			// Try to find the building in slot 2
			otherBuilding = p->Build->findBuilding(buildingToDelete->AttachedID2);

			// If the building was found,
			if (otherBuilding) {

				// Detach the building from the House
				otherBuilding->AttachedID = 0;
				otherBuilding->pop = 0;
				
				// Tell everyone in the sector about the population change
				sSMPop pop;
				pop.id = otherBuilding->id;
				pop.pop = otherBuilding->pop / 8;
				this->p->Send->SendSectorArea(otherBuilding->x*48,otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}
	}

	// Building: NOT HOUSE
	else {

		// If the building is attached to a House,
		if (buildingToDelete->AttachedID > 0) {

			// Try to find the House attached to the building
			otherBuilding = p->Build->findBuilding(buildingToDelete->AttachedID);

			// If the House was found,
			if (otherBuilding) {

				// If the House has this building attached in slot 1,
				if (otherBuilding->AttachedID == buildingToDelete->id) {
					
					// Detach the building from the House's slot 1
					otherBuilding->AttachedPop = 0;
					otherBuilding->AttachedID = 0;
					otherBuilding->pop = otherBuilding->AttachedPop2;

					// Tell everyone in the sector about the population change
					sSMPop pop;
					pop.id = otherBuilding->id;
					pop.pop = otherBuilding->pop / 16;
					this->p->Send->SendSectorArea(otherBuilding->x*48,otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}

				// Else if the House has this building attached in slot 2,
				else if (otherBuilding->AttachedID2 == buildingToDelete->id) {

					// Detach the building from the House's slot 2
					otherBuilding->AttachedPop2 = 0;
					otherBuilding->AttachedID2 = 0;
					otherBuilding->pop = otherBuilding->AttachedPop;

					// Tell everyone in the sector about the population change
					sSMPop pop;
					pop.id = otherBuilding->id;
					pop.pop = otherBuilding->pop / 16;
					this->p->Send->SendSectorArea(otherBuilding->x*48,otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}
			}
		}
	}

	// Tell everyone in the sector that the building was removed
	sSMBuild bv;
	bv.id = buildingToDelete->id;
	bv.City = buildingToDelete->City;
	bv.x = 1;
	this->p->Send->SendSectorArea(buildingToDelete->x*48, buildingToDelete->y*48,smRemBuilding,(char *)&bv,sizeof(bv));

	// Tell the team is it under attack
	this->p->Send->SendTeam(buildingToDelete->City, smUnderAttack,(char *)&bv,sizeof(bv));

	// Remove the building
	return this->remBuilding(buildingToDelete);
}

/***************************************************************
 * Function:	remBuilding
 *
 * @param del
 **************************************************************/
CBuilding *CBuildingList::remBuilding(CBuilding *del) {
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
	
	// Tell the city it is subtracting a building
	this->p->City[del->City]->subtractBuilding(del->type);

	// Delete the building
	delete del;
	
	// Return what was del->next
	return returnBuilding;
}

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CBuildingList::cycle() {
	CBuilding *bld = this->buildingListHead;
	CBuilding *otherBuilding;
	sSMItem item;
	sSMItemCount c;
	sSMPop pop;

	// For each building,
	while (bld) {

		/************************************************
		 * Population timer
		 ************************************************/
		// If the population timer is up,
		if (this->p->Tick > bld->PopulationTick) {

			// Reset the population timer
			bld->PopulationTick = this->p->Tick + 250;

			// If the building doesn't have full population and is not a House
			if ((! bld->hasMaxPopulation()) && (! bld->isHouse())) {
				
				// If the building is attached to a House,
				if (bld->AttachedID > 0) {

					// Add 5 population to the building, and cap it at POPULATION_MAX_NON_HOUSE
					if ((bld->pop + 5) > POPULATION_MAX_NON_HOUSE) {
						bld->pop = POPULATION_MAX_NON_HOUSE;
					}
					else {
						bld->pop += 5;
					}

					// Try to find the House attached to this building
					otherBuilding = this->p->Build->findBuilding(bld->AttachedID);

					// If the House was found,
					if (otherBuilding) {

						// If the building is attached to the House as AttachedId,
						if (bld->id == otherBuilding->AttachedID) {
							// Set the building's pop on the House
							otherBuilding->AttachedPop = bld->pop;
							otherBuilding->pop = otherBuilding->AttachedPop + otherBuilding->AttachedPop2;

							// Send the updated House population to the sector
							pop.id = otherBuilding->id;
							pop.pop = otherBuilding->pop / 16;
							p->Send->SendSectorArea(otherBuilding->x*48, otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
						}
						// Else if the building is attached to the House as AttachedId2,
						else if (bld->id == otherBuilding->AttachedID2) {
							// Set the building's pop on the House
							otherBuilding->AttachedPop2 = bld->pop;
							otherBuilding->pop = otherBuilding->AttachedPop + otherBuilding->AttachedPop2;

							// Send the updated House population to the sector
							pop.id = otherBuilding->id;
							pop.pop = otherBuilding->pop / 16;
							p->Send->SendSectorArea(otherBuilding->x*48, otherBuilding->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
						}
					}

					// Send the updated building population to the sector
					pop.id = bld->id;
					pop.pop = bld->pop / 8;
					this->p->Send->SendSectorArea(bld->x*48, bld->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}

				// Else (building is not attached to a house)
				else {

					// For each building,
					otherBuilding = this->buildingListHead;
					while (otherBuilding) {

						// If the ListBuilding belongs to the same city as this building
						if (otherBuilding->City == bld->City) {

							// If the ListBuilding is a House
							if (otherBuilding->type == 2) {

								// If House has an opening in slot 1,
								if (otherBuilding->AttachedID == 0) {

									// Attach the building to the House in slot 1, break out of the while loop
									otherBuilding->AttachedPop = bld->pop;
									otherBuilding->AttachedID = bld->id;
									otherBuilding->pop = otherBuilding->AttachedPop + otherBuilding->AttachedPop2;
									bld->AttachedID = otherBuilding->id;
									break;
								}

								// Else, if House has an opening in slot 2,
								else if (otherBuilding->AttachedID2 == 0) {

									// Attach the building to the House in slot 2, break out of the while loop
									otherBuilding->AttachedPop2 = bld->pop;
									otherBuilding->AttachedID2 = bld->id;
									otherBuilding->pop = otherBuilding->AttachedPop + otherBuilding->AttachedPop2;
									bld->AttachedID = otherBuilding->id;
									break;
								}
							}
						}

						// Move the pointer to the next building in the linked list
						otherBuilding = otherBuilding->next;
					}
				}
			}
		}
		
		/************************************************
		 * Money timer
		 ************************************************/
		// If the money timer is up,
		if (this->p->Tick > bld->MoneyTick) {

			// Reset the money timer,
			bld->MoneyTick = p->Tick + 7000;

			// Building: HOUSE
			if (bld->isHouse()) {

				// Add COST_INCOME_POPULATION cash per population
				this->p->City[bld->City]->cash += bld->pop * COST_INCOME_POPULATION;
				this->p->City[bld->City]->income += bld->pop * COST_INCOME_POPULATION;
			}

			// Building: RESEARCH (full population)
			else if (bld->isResearch() && bld->hasMaxPopulation()) {

				// Subtract research upkeep
				this->p->City[bld->City]->cash -= COST_ITEM;
				this->p->City[bld->City]->cashresearch += COST_ITEM;
			}

			// Building: HOSPITAL
			else if (bld->isHospital()) {

				// Subtract hospital upkeep
				this->p->City[bld->City]->cash -= COST_UPKEEP_HOSPITAL;
				this->p->City[bld->City]->hospital += COST_UPKEEP_HOSPITAL;
			}
		}

		/************************************************
		 * Research timer
		 ************************************************/
		// If the research timer is up,
		if (this->p->Tick > bld->ResearchTick) {

			// Increase the research timer
			bld->ResearchTick = p->Tick + 1000;

			// If the building is a Research,
			if ( bld->isResearch() ) {

				// If research is in progress,
				if (this->p->City[bld->City]->research[(bld->type - 3) / 2] > 0) {

					// If the population isn't full, abort research
					if (! bld->hasMaxPopulation()) {
						this->p->City[bld->City]->research[(bld->type - 3) / 2] = 0;
					}

					// Else if the research is now complete,
					else if (this->p->City[bld->City]->research[(bld->type - 3) / 2] < p->Tick) {

						// Tell the Research to stop researching
						this->p->City[bld->City]->research[(bld->type - 3) / 2] = -1;

						// Set canBuild to true for this Research's Factory (setCanBuild does it's own type++)
						this->p->City[bld->City]->setCanBuild(bld->type, 1, false);
						
						// If this building is a Med Research (type 9), set canBuild on Hospital (type is really 1... setCanBuild does type++)
						if (bld->type == 9) {
							this->p->City[bld->City]->setCanBuild(0, 1, false);
						}

						// For each building in the build tree,
						for (int l = 0;l < 12; l++) {

							// If the building is below this Research in the build tree,
							if (buildTree[l] == (bld->type - 3) / 2) {

								// Set canBuild true on that building
								this->p->City[bld->City]->setCanBuild((unsigned char)l * 2 + 2, 1, false);
							}
						}
					}
				}

				// Else if research has not yet started,
				else if (this->p->City[bld->City]->research[(bld->type - 3) / 2] == 0) {
					
					// If the population is full, start research
					if (bld->hasMaxPopulation()) {
						this->p->City[bld->City]->research[(bld->type - 3) / 2] = this->p->Tick + TIMER_RESEARCH;
					}
				}
			}
		}

		/************************************************
		 * Item production timer
		 ************************************************/
		// If the item production timer is up,
		if (this->p->Tick > bld->ProduceTick) {

			// If the building is a functioning Factory (full population),
			if (bld->isFactory() && bld->hasMaxPopulation()) {

				// Get the type of item produced by this Factory
				item.type = itemTypes[(bld->type - 2) / 2 - 1];

				// If the city's itemCount is less than this item's item limit, and the city can build the item,
				if ((this->p->City[bld->City]->itemC[item.type] < maxItems[item.type]) && (this->p->City[bld->City]->canBuild[bld->type - 2] == 2) ) {

					// TODO:  add check whether research has full population

					// If the city can afford a new item,
					if (this->p->City[bld->City]->cash >= COST_ITEM) {

						// If the building has yet been set to produce,
						// TODO: reset bld->ProduceTick when its Research is destroyed
						if (bld->ProduceTick == 0) {
							
							// Prepare the building for production by setting the count of items left to produce as "max - itemCount"
							c.count = maxItems[item.type] - (unsigned char)this->p->City[bld->City]->itemC[item.type];
							c.id = bld->id;
							
							// Send the sector and reset the production timer, WITHOUT BUILDING AN ITEM
							this->p->Send->SendSectorArea(bld->x*48, bld->y*48,smItemCount,(char *)&c,sizeof(c));
							bld->ProduceTick = p->Tick + 7000;
						}

						// If the building has already been set to produce,
						else {

							// Transfer the price of an item into the city's production budget
							this->p->City[bld->City]->cash -= COST_ITEM;
							this->p->City[bld->City]->itemproduction += COST_ITEM;
							
							// Create an item on the factory bay with the next available item ID
							item.x = bld->x - 1;
							item.y = bld->y - 2;
							item.City = bld->City;
							item.active = 0;
							item.id = p->Item->itmID++;

							// HACK: if more than 30000 items, cycle back to 1
							if (this->p->Item->itmID > 30000) {
								this->p->Item->itmID = 1;
							}
							// Create the item, tell everyone in the sector about the new item
							this->p->Item->newItem(item.x,item.y,item.type,item.City,item.id);
							this->p->Send->SendSectorArea(item.x*48, item.y*48,smAddItem,(char *)&item, sizeof(item));

							// Set the city's itemCount for the item type, tell everyone in the sector about the new itemCount
							c.count = maxItems[item.type] - (unsigned char)this->p->City[bld->City]->itemC[item.type];
							c.id = bld->id;
							this->p->Send->SendSectorArea(item.x*48, item.y*48,smItemCount,(char *)&c,sizeof(c));

							// If the Factory can't make any more items, set ProduceTick to 0
							if (c.count == 0) {
								bld->ProduceTick = 0;
							}
							// Else (Factory can make more items), reset the item production counter
							else {
								bld->ProduceTick = this->p->Tick + 7000;
							}
						}
					}
				}
			}
		}

		// Move the pointer to the next building in the list
		bld = bld->next;
	}
}
