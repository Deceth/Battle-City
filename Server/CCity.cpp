#include "CCity.h"

/***************************************************************
 * Constructor
 *
 **************************************************************/
CCity::CCity(CServer *Server) {
	p = Server;
	notHiring = 0;
	Orbs = 0;
	hiring = -1;
	destroy();
}


/***************************************************************
 * Destructor
 *
 **************************************************************/
CCity::~CCity() {
}

/***************************************************************
 * Function:	destroy
 *
 **************************************************************/
void CCity::destroy() {
	int i;
	CBuilding *bld;
	CItem *itm;

	// Rset active to 1
	this->active = 1;

	// Remove all the buildings
	bld = p->Build->buildings;
	if (bld) {
		while (bld->prev) {
			bld = bld->prev;
		}

		while (bld) {
			if (bld->City == id) {
				bld = p->Build->remBuilding(bld);
			}
			else {
				if (bld) {
					bld = bld->next;
				}
			}
		}
	}

	// Reset the mayor
	this->Mayor = -1;

	// If the city had hiring on, tell any applicants that the mayor changed
	if (hiring > -1) {
		p->Winsock->SendData(hiring, smMayorChanged, " ");
	}

	// Turn off hiring
	this->hiring = -1;

	// Reset the build tree
	for (i = 0; i <= 26; i++) {
		this->canBuild[i] = 0;
	}

	// DO NOT enable Hospital
	//canBuild[0] = 1;
	
	// Enable House, Laser Research, Turret Research
	this->canBuild[1] = 1;
	this->canBuild[2] = 1;
	this->canBuild[4] = 1;

	// Reset the orb counter to 0
	this->Orbs = 0;

	// Reset all money values to default
	this->cash = MONEY_STARTING_VALUE;
	this->income = 0;
	this->itemproduction = 0;
	this->cashresearch = 0;
	this->hospital = 0;

	// Reset successor to -1
	this->Successor = -1;

	// Reset the destruct timer to 0
	this->DestructTimer = 0;

	// Destroy all the city's items
	itm = p->Item->items;
	if (itm) {
		while (itm->prev) {
			itm = itm->prev;
		}

		while (itm) {
			if (itm->City == id) {
				itm = p->Item->remItem(itm);
			}
			else if (itm) {
				itm = itm->next;
			}
		}
	}

	// Reset all research to not-finished
	for (i = 0; i < 20; i++)
		research[i] = 0;

	// Reset all item counters to 0
	for (i = 0; i < 12; i++)
		itemC[i] = 0;

	// Tell all players the city was destroyed
	p->Send->SendGameAllBut(-1,smDestroyCity,(char *)&id, sizeof(id));
}

/***************************************************************
 * Function:	setCanBuild
 *
 * @param i
 * @param can
 **************************************************************/
void CCity::setCanBuild(int i, int can) {
	char packet[4];
	int iCan = 0;

	// Store whether the city can build the building in canBuild
	this->canBuild[i] = can;

	// If the player can build this building (+1), set iCan to 1
	if (can) {
		iCan = 1;
	}

	// If the player can't build, set iCan to 0
	if (can == 2) {
		iCan = 0;
	}

	// If this city's mayor is in-game, tell the mayor about the build-tree change
	if (this->p->Player[this->Mayor]->State == State_Game) {
		packet[0] = i+1;
		packet[1] = iCan;
		packet[2] = 0;
		this->p->Winsock->SendData(Mayor,(unsigned char)smCanBuild,packet);
	}
}

/***************************************************************
 * Function:	cycle
 *
 **************************************************************/
void CCity::cycle() {
	int random_int;
	sSMFinance finance;

	// If the city has no mayor,
	if (this->Mayor == -1) {
		
		// If the destruct timer is set,
		if (this->DestructTimer > 0) {

			// If the Destruct timer is up, destroy the city
			if (this->p->Tick > this->DestructTimer) {
				this->destroy();
			}
		}

		// If no mayor, return
		return;
	}

	// ???
	if (this->x == 0 || this->y == 0) {
		this->x = (unsigned short)(512*48)-(32+(id % 8*MAX_CITIES)) * 48;
		this->y = (unsigned short)(512*48)-(32+(id / 8*MAX_CITIES)) * 48; 
	}
	
	// if the Money timer is up,
	if (this->p->Tick > this->moneyCycle) {

		// Cap money at MONEY_MAX_VALUE
		if (this->cash > MONEY_MAX_VALUE) {
			this->cash = MONEY_MAX_VALUE;
		}
		else if (this->cash < 0) {
			this->cash = 0;

			// Randomly give back cash
			srand ( time(NULL) );
			random_int = rand()%4 +1;
			if (random_int == 1) {
				this->cash = COST_UPKEEP_HOSPITAL;
			}
		}

		// Reset the money timer
		this->moneyCycle = this->p->Tick + 7000;
		
		finance.Cash = this->cash;
		finance.Income = this->income;
		finance.Items = this->itemproduction;
		finance.Hospital = this->hospital;
		finance.Research = this->cashresearch;
		this->p->Winsock->SendData(Mayor,smFinance,(char *)&finance,sizeof(finance));

		this->income = 0;
		this->itemproduction = 0;
		this->cashresearch = 0;
		this->hospital = 0;
	}
}

/***************************************************************
 * Function:	wasOrbed
 *
 **************************************************************/
void CCity::wasOrbed() {

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in the city and in game,
		if (this->p->Player[i]->City == id && this->p->Player[i]->State == State_Game) {

			// Boot the player
			cout << "Orbed::" << this->p->Player[i]->Name << endl;
			this->p->Player[i]->LeaveGame(0);
		}
	}

	// Destroy this city
	
	this->destroy();
}

/***************************************************************
 * Function:	didOrb
 *
 * @param City
 * @param index
 **************************************************************/
void CCity::didOrb(int City, int index) {
	int pointsgiven;
	sSMOrbedCity orbed;

	// Get the point value of the orbed city
	pointsgiven = p->Build->GetOrbPointCount(City);

	// Tell each player about the orb
	orbed.points = (unsigned int)pointsgiven;
	orbed.OrberCity = id;
	orbed.City = City;
	orbed.OrberCityPoints = p->Build->GetOrbPointCount(id);
	p->Send->SendAllBut(-1,smOrbed,(char *)&orbed,sizeof(orbed));

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in the city and in game,
		if (p->Player[i]->City == id && p->Player[i]->State == State_Game) {

			// If the player was the orber, add an orb
			if (i == index) {
				p->Account->AddOrb(i);
			}

			// Else (not the orber), add an assist
			else {
				p->Account->AddAssist(i);
			}

			// Add the points for the orb
			p->Account->AddPoints(i, pointsgiven);
		}
	}

	// Add one to the city's orb total
	this->Orbs++;
}

/***************************************************************
 * Function:	PlayerCount
 *
 * @param i
 * @param can
 **************************************************************/
int CCity::PlayerCount() {
	int count = 0;

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in the city and in game,
		if (p->Player[i]->State == State_Game && p->Player[i]->City == id) {

			// Increment the player count
			count++;
		}
	}	

	return count;
}

