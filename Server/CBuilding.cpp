#include "CBuilding.h"

CBuilding *CBuildingList::findBuilding(unsigned short id) {
	CBuilding *bld = buildings;

	// If there are no buildings, return 0
	if (!bld) {
		return 0;
	}

	// Move to the start of the building linked list
	while (bld->prev) {
		bld = bld->prev;
	}

	// While there are buildings in the linked list,
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

int CBuildingList::GetOrbPointCount(int theCity) {
	int pointsgiven = 0;
	int count = p->Build->GetOrbBuildingCount(theCity);

	// Get the base point value from the city's building count

	// SIZE: 0-10
	if (count < 11) {
		pointsgiven = 0;
	}
	// SIZE: 11-15
	else if (count < 16) {
		pointsgiven = 20;
	}
	// SIZE: 16-20
	else if (count < 21) {
		pointsgiven = 30;
	}
	// SIZE: 21+
	else {
		pointsgiven = 50;
	}

	// Add 5 points for each of the city's Orbs
	pointsgiven += (p->City[theCity]->Orbs * 5);

	return pointsgiven;
}

CBuilding *CBuildingList::newBuilding(int x, int y, int type, int City, unsigned short id) {
	CBuilding *bld = buildings;

	// If there are no buildings in the building linked list,
	if (!bld) {

		// Set the linked list to point to the new building
		buildings = new CBuilding(x, y, type, City, id, p);

		// Return a pointer to the new building (happens to also be a pointer to the linked list)
		return buildings;
	}
	
	// Else (there are buildings),
	else {

		// Move to the end of the linked list
		while(bld->next) {
			bld = bld->next;
		}

		// Add the new building at the end of the list
		bld->next = new CBuilding(x, y, type, City, id, p);

		// Tell the new building about the building before it
		bld->next->prev = bld;

		// Return a pointer to the new building
		return bld->next;
	}

	// TODO: remove unreachable return
	return 0;
}

CBuilding *CBuildingList::delBuilding(CBuilding *del) {
	CItem *item;
	
	// If there are no buildings, return 0
	if (!buildings) {
		return 0;
	}

	/************************************************
	 * Handle items and build-tree
	 ************************************************/
	// If the building is a Factory (type%2==0, type>2)
	if ((del->type % 2) == 0 && del->type > 2) {

		// If there are items in the item linked list,
		if (p->Item->items) {

			// Move to the start of the linked list
			item = p->Item->items;
			while (item->prev) {
				item = item->prev;
			}
			
			// For each item in the linked list,
			while (item) {
				// If the item belongs to this city, and the item was made by this Factory
				if ((item->City == del->City) && (item->type == itemTypes[(del->type - 2) / 2 - 1])) {
					
					// Delete the item and move the pointer to the next item in the linked list
					item = p->Item->delItem(item);
				}
				// Else (item belongs to other city, or item not made by this Factory),
				else {

					// Move the pointer to the next item in the linked list
					item = item->next;
				}
			}
		}
		
		// Tell the city it can now build this building type MINUS ONE (because setCanBuild adds one for some reason)
		p->City[del->City]->setCanBuild(del->type-1, 1);
	}

	// Else (building is not a Factory),
	else  {
		// Tell the city it can now build this building type MINUS ONE (because setCanBuild adds one for some reason)
		p->City[del->City]->setCanBuild(del->type-1, 1);
	}


	/************************************************
	 * Handle building-house relationships
	 ************************************************/
	// If the building is a House,
	if (del->type == 2) {

		// If the House has a building in slot 1,
		if (del->AttachedID > 0) {

			// Try to find the building in slot 1
			// TODO: remove instantiation of object inside conditional
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID);

			// If the building was found,
			if (bld2) {

				// Detach the building from this House
				bld2->AttachedID = 0;
				bld2->pop = 0;

				// Tell everyone in the sector about the population change
				sSMPop pop;
				pop.id = bld2->id;
				pop.pop = bld2->pop / 8;
				p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}

		// If the House has a building in slot 2,
		if (del->AttachedID2 > 0) {

			// Try to find the building in slot 2
			// TODO: remove instantiation of object inside conditional
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID2);

			// If the building was found,
			if (bld2) {

				// Detach the building from the House
				bld2->AttachedID = 0;
				bld2->pop = 0;
				
				// Tell everyone in the sector about the population change
				sSMPop pop;
				pop.id = bld2->id;
				pop.pop = bld2->pop / 8;
				p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}
	}

	// Else (building is not a house)
	else {

		// If the building is attached to a House,
		if (del->AttachedID > 0) {

			// Try to find the House attached to the building
			// TODO: remove instantiation of object inside conditional
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID);

			// If the House was found,
			if (bld2) {

				// If the House has this building attached in slot 1,
				if (bld2->AttachedID == del->id) {
					
					// Detach the building from the House's slot 1
					bld2->AttachedPop = 0;
					bld2->AttachedID = 0;
					bld2->pop = bld2->AttachedPop2;

					// Tell everyone in the sector about the population change
					sSMPop pop;
					pop.id = bld2->id;
					pop.pop = bld2->pop / 16;
					p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}

				// Else if the House has this building attached in slot 2,
				else if (bld2->AttachedID2 == del->id) {

					// Detach the building from the House's slot 2
					bld2->AttachedPop2 = 0;
					bld2->AttachedID2 = 0;
					bld2->pop = bld2->AttachedPop;

					// Tell everyone in the sector about the population change
					sSMPop pop;
					pop.id = bld2->id;
					pop.pop = bld2->pop / 16;
					p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}
			}
		}
	}

	// Tell everyone in the sector that the building was removed
	sSMBuild bv;
	bv.id = del->id;
	bv.x = 1;
	p->Send->SendSectorArea(del->x*48, del->y*48,smRemBuilding,(char *)&bv,sizeof(bv));

	// If the building had a building before it, point the buildings list pointer at that building
	if (del->prev) {
		buildings = del->prev;
	}
	// Else, if the building had a building after it, point the buildings list pointer at that building
	else if (del->next) {
		buildings = del->next;
	}
	// Else, no buildings exist, point buildings at 0
	else {
		buildings = 0;
	}
	
	// Delete the building
	delete del;
	
	// Return the linked list pointer
	return buildings;
}


CBuilding::CBuilding(int x, int y, int type, int City, unsigned short id, CServer *Server) {
	p = Server;
	prev = 0;
	next = 0;
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

CBuilding::~CBuilding() {
	// Tell the surrounding buildings to bypass this building in the list
	if (next) {
		next->prev = prev;
	}
	if (prev) {
		prev->next = next;
	}
}

int CBuildingList::GetBuildingCount(int theCity) {
	CBuilding *bld = buildings;
	int count = 1;

	// If there are no buildings, return 0
	if (!bld) {
		return 0;
	}

	// Move bld to the head of the buildings linked list
	while (bld->prev) {
		bld = bld->prev;
	}

	// For each building,
	while (bld) {

		// If the building belongs to this city, increment count
		if (bld->City == theCity) {
			count++;
		}

		// Get the next building
		bld = bld->next;
	}

	// Return count
	return count;
}

int CBuildingList::GetOrbBuildingCount(int theCity) {
	CBuilding *bld = buildings;
	int count = 1;
	int foundorbfactory = 0;

	// If there are no buildings, return 0
	if (!bld) {
		return 0;
	}

	// Move bld to the head of the buildings linked list
	while (bld->prev) {
		bld = bld->prev;
	}

	// For each building,
	while (bld) {

		// If the building belongs to this city, increment count
		if (bld->City == theCity) {
			count++;

			// If the building is an Orb Factory (type 16), set foundorbfactory true
			if (bld->type == 16) {
				foundorbfactory = 1;
			}
		}

		// Get the next building
		bld = bld->next;
	}

	// If less than 11 buildings were found, but one was an orb factory, return 11
	if (count < 11 && foundorbfactory == 1) {
		return 11;
	}
	// Else (no orb fac or more than 11 buildings), return building count
	else {
		return count;
	}
}

CBuilding *CBuildingList::remBuilding(CBuilding *del) {

	// If there are no buildings, return 0
	if (!buildings) {
		return 0;
	}

	// If there's a building before this one, point the linked list pointer at that building
	if (del->prev) {
		buildings = del->prev;
	}
	// Else, if there's a building after this one, point the linked list pointer at that building
	else if (del->next)
		buildings = del->next;
	// Else, point the linked list pointer at 0
	else {
		buildings = 0;
	}

	// Delete the building
	delete del;
	
	// Return the linked list pointer
	return buildings;
}

void CBuildingList::cycle() {
	sSMItem item;
	sSMItemCount c;
	sSMPop pop;

	// Get a pointer to the list of all buildings
	CBuilding *bld = p->Build->buildings;

	// If there are buildings to process, move to the first building in the linked list
	if (bld) {
		while (bld->prev) {
			bld = bld->prev;
		}
	}

	// While there are buildings to process,
	while (bld) {

		// If the population timer is up,
		if (p->Tick > bld->PopulationTick) {

			// Reset the population timer
			bld->PopulationTick = p->Tick + 250;

			// If the building doesn't have full population and is not a House
			if (bld->pop != 50 && bld->type != 2) {
				
				// If the building is attached to a House,
				if (bld->AttachedID > 0) {

					// Add 5 population to the building, and cap it at 50
					if ((bld->pop + 5) > 50) {
						bld->pop = 50;
					}
					else {
						bld->pop += 5;
					}

					// Try to find the House attached to this building
					// TODO: remove instantiation of object inside loop
					CBuilding *bld2 = p->Build->findBuilding(bld->AttachedID);

					// If the House was found,
					if (bld2) {

						// If the building is attached to the House as AttachedId,
						if (bld->id == bld2->AttachedID) {
							// Set the building's pop on the House
							bld2->AttachedPop = bld->pop;
							bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;

							// Send the updated House population to the sector
							pop.id = bld2->id;
							pop.pop = bld2->pop / 16;
							p->Send->SendSectorArea(bld2->x*48, bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
						}
						// Else if the building is attached to the House as AttachedId2,
						else if (bld->id == bld2->AttachedID2) {
							// Set the building's pop on the House
							bld2->AttachedPop2 = bld->pop;
							bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;

							// Send the updated House population to the sector
							pop.id = bld2->id;
							pop.pop = bld2->pop / 16;
							p->Send->SendSectorArea(bld2->x*48, bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
						}
					}

					// Send the updated building population to the sector
					pop.id = bld->id;
					pop.pop = bld->pop / 8;
					p->Send->SendSectorArea(bld->x*48, bld->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}

				// Else (building is not attached to a house)
				else {
					// Get the building list
					// TODO: remove instantiation of object inside loop
					CBuilding *bld2 = p->Build->buildings;

					// If any buildings exist,
					if (bld2) {

						// Move to the head of the building linked list
						while (bld2->prev) {
							bld2 = bld2->prev;
						}

						// While there are buildings "ListBuildings" in the linked list,
						while (bld2) {

							// If the ListBuilding belongs to the same city as this building
							if (bld2->City == bld->City) {

								// If the ListBuilding is a House
								if (bld2->type == 2) {

									// If House has an opening in slot 1,
									if (bld2->AttachedID == 0) {

										// Attach the building to the House in slot 1, break out of the while loop
										bld2->AttachedPop = bld->pop;
										bld2->AttachedID = bld->id;
										bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
										bld->AttachedID = bld2->id;
										break;
									}

									// Else, if House has an opening in slot 2,
									else if (bld2->AttachedID2 == 0) {

										// Attach the building to the House in slot 2, break out of the while loop
										bld2->AttachedPop2 = bld->pop;
										bld2->AttachedID2 = bld->id;
										bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
										bld->AttachedID = bld2->id;
										break;
									}
								}
							}

							// Move the pointer to the next building in the linked list
							bld2 = bld2->next;
						}
					}
				}
			}
		}
		
		// If the money timer is up,
		if (p->Tick > bld->MoneyTick) {

			// Reset the money timer,
			bld->MoneyTick = p->Tick + 7000;

			// Building: HOUSE
			if (bld->type == 2) {

				// Add 10000 cash per population
				p->City[bld->City]->cash += bld->pop * 10000;
				p->City[bld->City]->income += bld->pop * 10000;
			}

			// Building: RESEARCH (full population)
			else if (bld->type % 2 && bld->type > 2 && bld->pop == 50) {

				// Subtract research upkeep
				p->City[bld->City]->cash -= 750000;
				p->City[bld->City]->cashresearch += 750000;
			}

			// Building: HOSPITAL
			else if (bld->type == 1) {

				// Subtract hospital upkeep
				p->City[bld->City]->cash -= 2000000;
				p->City[bld->City]->hospital += 2000000;
			}
		}

		// If the research timer is up,
		if (p->Tick > bld->ResearchTick) {

			// Increase the research timer
			bld->ResearchTick = p->Tick + 1000;

			// If the building is a functioning Research (full population, type%2==1, type>2)
			if (bld->pop == 50 && bld->type % 2 && bld->type > 2) {

				// If the parent Research is researching,
				if (p->City[bld->City]->research[(bld->type - 3) / 2] > 0) {

					// If the parent Research's research is complete,
					if (p->City[bld->City]->research[(bld->type - 3) / 2] < p->Tick) {

						// Tell the parent Research to stop researching
						p->City[bld->City]->research[(bld->type - 3) / 2] = -1;

						// Set canBuild to true for this building
						p->City[bld->City]->setCanBuild(bld->type, 1);
						
						// If this building is a Med Research (type 9), set canBuild on Hospital (type 0)
						if (bld->type == 9) {
							p->City[bld->City]->setCanBuild(0, 1);
						}

						// For each building in the build tree,
						for (int l = 0;l < 12; l++) {

							// If the building is this building's parent Research,
							if (buildTree[l] == (bld->type - 3) / 2) {

								// Set canBuild true on the parent Research's associated Factory
								p->City[bld->City]->setCanBuild((unsigned char)l * 2 + 2, 1);
							}
						}
					}
				}
			}
		}

		// If the item production timer is up,
		if (p->Tick > bld->ProduceTick) {

			// If the building is a functioning Factory (full population, type%2==0, type>2),
			if ((bld->type % 2) == 0 && bld->type > 2 && bld->pop == 50) {

				// Get the type of item produced by this Factory
				item.type = itemTypes[(bld->type - 2) / 2 - 1];

				// If the city's itemCount is less than this item's item limit, and the associated Research exists,
				// TODO:  add check of (building[bld->type-2].pop==50)
				if (p->City[bld->City]->itemC[item.type] < maxItems[item.type] && p->City[bld->City]->canBuild[bld->type - 2] == 2) {

					// If the city can afford a new item,
					if (p->City[bld->City]->cash > 750000) {

						// If the building has yet been set to produce,
						// TODO: reset bld->ProduceTick when its Research is destroyed
						if (bld->ProduceTick == 0) {
							
							// Prepare the building for production by setting the count of items left to produce as "max - itemCount"
							c.count = maxItems[item.type] - (unsigned char)p->City[bld->City]->itemC[item.type];
							c.id = bld->id;
							
							// Send the sector and reset the production timer, WITHOUT BUILDING AN ITEM
							p->Send->SendSectorArea(bld->x*48, bld->y*48,smItemCount,(char *)&c,sizeof(c));
							bld->ProduceTick = p->Tick + 7000;
						}

						// If the building has already been set to produce,
						else {

							// If the item production timer is up,
							// TODO: remove this if statement, the condition is checked above!
							if (p->Tick > bld->ProduceTick) {

								// Transfer the price of an item into the city's production budget
								p->City[bld->City]->cash -= 750000;
								p->City[bld->City]->itemproduction += 750000;
								
								// Create an item on the factory bay with the next available item ID
								item.x = bld->x - 1;
								item.y = bld->y - 2;
								item.City = bld->City;
								item.active = 0;
								item.id = p->Item->itmID++;

								// HACK: if more than 30000 items, cycle back to 1
								if (p->Item->itmID > 30000) {
									p->Item->itmID = 1;
								}
								// Create the item, tell everyone in the sector about the new item
								p->Item->newItem(item.x,item.y,item.type,item.City,item.id);
								p->Send->SendSectorArea(item.x*48, item.y*48,smAddItem,(char *)&item, sizeof(item));

								// Set the city's itemCount for the item type, tell everyone in the sector about the new itemCount
								c.count = maxItems[item.type] - (unsigned char)p->City[bld->City]->itemC[item.type];
								c.id = bld->id;
								p->Send->SendSectorArea(item.x*48, item.y*48,smItemCount,(char *)&c,sizeof(c));

								// If the Factory can't make any more items, set ProduceTick to 0
								if (c.count == 0) {
									bld->ProduceTick = 0;
								}
								// Else (Factory can make more items), reset the item production counter
								else
									bld->ProduceTick = p->Tick + 7000;
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
