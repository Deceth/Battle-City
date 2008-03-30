#include "CServer.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CProcess::CProcess(CServer *Server) {
	this->p = Server;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CProcess::~CProcess() {
}

/***************************************************************
 * Function:	ProcessData
 *
 * @param TheDate
 # @param Index
 **************************************************************/
void CProcess::ProcessData(char *TheData, int Index) {

	// Switch on the type of packet
	switch((unsigned char)TheData[0]) {

		// Packet: cmVersion
		case cmVersion:
			this->ProcessVersion((sCMVersion *)&TheData[1], Index);
			break;

		// Packet: cmLogin
		case cmLogin:
			this->ProcessLogin((sCMLogin *)&TheData[1], Index);
			break;

		// Packet: cmNewAccount
		case cmNewAccount:
			this->ProcessNewAccount((sCMNewAccount *)&TheData[1], Index);
			break;

		// Packet: cmChatMessage
		case cmChatMessage:
			this->p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 0);
			break;

		// Packet: cmGlobal
		case cmGlobal:
			this->p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 1);
			break;

		// Packet: cmWalkie
		case cmWalkie:
			this->p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 2);
			break;

		// Packet: cmSetState
		case cmSetState:
			this->ProcessSetState(&TheData[1], Index);
			break;

		// Packet: cmJobApp
		case cmJobApp:
			this->ProcessJobApp(&TheData[1], Index);
			break;

		// Packet: cmJobCancel
		case cmJobCancel:
			this->ProcessCancelJob(&TheData[1], Index);
			break;

		// Packet: cmHireAccept
		case cmHireAccept:
			this->ProcessAccept(&TheData[1], Index);
			break;

		// Packet: cmHireDecline
		case cmHireDecline:
			this->ProcessDeny(Index);
			break;

		// Packet: cmBuild
		case cmBuild:
			this->ProcessBuild(&TheData[1], Index);
			break;

		// Packet: cmItemUp
		case cmItemUp:
			this->ProcessItemUp((sCMItem *)&TheData[1], Index);
			break;

		// Packet: cmItemDrop
		case cmItemDrop:
			this->ProcessItemDrop((sCMItem *)&TheData[1], Index);
			break;

		// Packet: cmHitObject
		case cmHitObject:
			this->ProcessHitObject((sCMItem *)&TheData[1], Index);
			break;

		// Packet: cmComms
		case cmComms:

			// If the player is hiring,
			if (this->p->City[this->p->Player[Index]->City]->hiring > -1) {
				TheData[0] = Index;
				this->p->Winsock->SendData(this->p->City[this->p->Player[Index]->City]->hiring, smComms, TheData, (int)strlen(&TheData[1]) + 1);
			}
			break;

		// Packet: cmFired
		case cmFired:

			// If the player is the one getting fired, leave
			if ( (this->p->Player[Index]->Mayor == 1) && (this->p->Player[Index]->City == this->p->Player[TheData[1]]->City) ) {
				this->p->Player[TheData[1]]->LeaveGame(true);
				this->p->Winsock->SendData(TheData[1], smFired, " ");
			}
			break;

		// Packet: cmUpdate
		case cmUpdate:
			this->ProcessUpdate((sCMUpdate *)&TheData[1], Index);
			break;

		// Packet: cmShoot
		case cmShoot:
			this->ProcessShot((sCMShot *)&TheData[1], Index);
			break;

		// Packet: cmDemolish
		case cmDemolish:
			this->ProcessDemolish((sCMDemolish *)&TheData[1], Index);
			break;

		// Packet: cmBan
		case cmBan:
			this->ProcessBan(&TheData[1], Index);
			break;

		// Packet: cmMedKit
		case cmMedKit:
			this->ProcessMedKit((int *)&TheData[1], Index);
			break;

		// Packet: cmCrash
		case cmCrash:
			this->ProcessCrash(&TheData[1], Index);
			break;

		// Packet: cmNextStep
		case cmNextStep:
			this->ProcessNextStep(TheData[1], Index);
			break;

		// Packet: cmSuccessor
		case cmSuccessor:
			this->ProcessSuccessor(TheData[1], Index);
			break;

		// Packet: cmAdmin
		case cmAdmin:
			this->ProcessAdmin((sCMAdmin *)&TheData[1], Index);
			break;

		// Packet: cmIsHiring
		case cmIsHiring:
			this->ProcessIsHiring(TheData[1], Index);
			break;

		// Packet: cmDeath
		case cmDeath:
			this->ProcessDeath(&TheData[1], Index);
			break;

		// Packet: cmTCPPing
		case cmTCPPing:
			this->p->Winsock->SendData(Index, smTCPPong, " ");
			break;

		// Packet: cmRequestSector
		case cmRequestSector:
			this->ProcessRequestSector((sCMSector *)&TheData[1], Index);
			break;

		// Packet: cmEditAccount
		case cmEditAccount:
			this->ProcessEditAccount((sCMLogin *)&TheData[1], Index);
			break;

		// Packet: cmAccountUpdate
		case cmAccountUpdate:
			this->ProcessUpdateAccount((sCMNewAccount *)&TheData[1], Index);
			break;

		// Packet: cmRecover
		case cmRecover:
			this->ProcessRecover(&TheData[1], Index);
			break;

		// Packet: cmRightClickCity
		case cmRightClickCity:
			this->ProcessRightClickCity(TheData[1], Index);
			break;

		// Packet: cmRequestInfo
		case cmRequestInfo:
			this->ProcessRequestInfo(Index);
			break;

		// Packet: cmChangeNews
		case cmChangeNews:

			// If the player is an admin, change the news
			if (this->p->Player[Index]->isAdmin()) {
				this->p->ChangeNews(&TheData[1]);
			}
			break;

		// Packet: cmSetMayor
		case cmSetMayor:

			// If the player is in game, in the city, and mayor,
			if ( (this->p->Player[TheData[1]]->State == State_Game)
				&&
				(this->p->Player[TheData[1]]->City == this->p->Player[Index]->City)
				&&
				(this->p->Player[Index]->Mayor == 1)
				) {

				// Un-mayor this player and mayor the other guy
				this->p->Player[TheData[1]]->setMayor(1);
				this->p->Player[Index]->setMayor(0);
			}
			break;

		// Packet: cmRefreshList
		case cmRefreshList:
			this->p->Send->SendCityList(Index);
			break;

		// Packet: cmMiniMap
		case cmMiniMap:
			this->p->Send->SendMiniMap(Index);
			break;

		// Packet: cmAdminEditRequest
		case cmAdminEditRequest:
			this->ProcessAdminEditRequest(Index, (sCMAdminEditRequest *)&TheData[1]);
			break;

		// Packet: cmAdminEdit
		case cmAdminEdit:
			this->ProcessAdminEdit(Index, (sCMAdminEdit *)&TheData[1]);
			break;

		// Packet: cmClickPlayer
		case cmClickPlayer:
			this->ProcessClickPlayer(Index, TheData[1]);
			break;

		// Packet: cmChangeTank
		case cmChangeTank:
			this->ProcessChangeTank(Index, TheData[1]);
			break;

		// Packet: cmWhisper
		case cmWhisper:
			this->p->Send->SendWhisper(Index, (sCMWhisper *)&TheData[1]);
			break;
	}
} 

/***************************************************************
 * Function:	ProcessBuild
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessBuild(char *TheData,int Index) {
	sCMBuild *data;
	sSMBuild bd;

	// If the player is Mayor, In Game, and not Dead
	if ( (p->Player[Index]->Mayor) && (p->Player[Index]->State == State_Game) && (p->Player[Index]->isDead == false) ) {
		data = (sCMBuild *)TheData;

		// If the player can build the requested building (or is an admin)
		if ( (p->City[p->Player[Index]->City]->canBuild[data->type - 1] == 1) || (p->Player[Index]->isAdmin()) ) {

			// Subtract the cost of the building,
			p->City[p->Player[Index]->City]->cash -= COST_BUILDING;

			// Create the building with the next ID, add it to the server linked list
			bd.City = p->Player[Index]->City;
			bd.count = 0;
			bd.type = data->type;
			bd.x = data->x;
			bd.y = data->y;
			p->Build->bldID++;
			bd.id = p->Build->bldID++;
			bd.pop = 0;
			p->Build->newBuilding(data->x,data->y,data->type,p->Player[Index]->City, bd.id);

			// HACK: If the ID is over 30000, cycle around to 1
			if (p->Build->bldID > 30000) {
				p->Build->bldID = 1;
			}

			// Tell everyone in the sector about the new building
			p->Send->SendSectorArea(data->x*48, data->y*48,(unsigned char)smNewBuilding,(char *)&bd,sizeof(bd));

			// If the building is not a House,
			if (! CBuilding::isHouse(data->type)) {

				// Set canBuild for the building type to "already has"
				p->City[p->Player[Index]->City]->setCanBuild((unsigned char)data->type - 1,2);

				// If the building is a Research,
				if (CBuilding::isResearch(data->type)) {

					// If research is not finished, set the research timer to 0
					// The CBuilding.cycle() Research timer will start the research when the building is populated
					if (p->City[p->Player[Index]->City]->research[(data->type - 3) / 2] != -1) {
						p->City[p->Player[Index]->City]->research[(data->type - 3) / 2] = 0;
					}
				}
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessJobApp
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessJobApp(char *TheData, int Index) {
	char packet[20];

	// If the city index is less than MAX_CITIES,
	if ((unsigned char)TheData[0] < MAX_CITIES) {

		// If the city has no mayor,
		if (p->City[(unsigned char)TheData[0]]->Mayor == -1) {

			// Add the player to the city as mayor
			p->Player[Index]->City = (unsigned char)TheData[0];
			p->Player[Index]->setMayor(1);
			p->Player[Index]->StartJoin(); 
			packet[0] = Index;
			packet[1] = 68;
			p->Send->SendToChat(smChatCommand, packet, 2); 
		} 

		// Else (city has a mayor)
		else {

			// If the player count is less than MAX_PLAYERS_PER_CITY
			if (p->City[(unsigned char)TheData[0]]->PlayerCount() < MAX_PLAYERS_PER_CITY) {
				
				// If the mayor is not in an interview,
				if (p->City[(unsigned char)TheData[0]]->hiring == -1) {

					// Set the player as the mayor's interviewee
					p->City[(unsigned char)TheData[0]]->hiring = Index;
					p->Player[Index]->State = State_Apply;
					packet[0] = Index;
					p->Winsock->SendData(p->City[(unsigned char)TheData[0]]->Mayor,smMayorHire,packet,1);
					p->Winsock->SendData(Index, smInterview, " ", 1);
					packet[0] = Index;
					packet[1] = 68;
					p->Send->SendToChat(smChatCommand, packet, 2); 
				} 

				// Else (mayor is in an interview), send busy signal
				else {
					p->Winsock->SendData(Index,smMayorInInterview," ",1);
				}
			}

			// Else (player count not less than MAX_PLAYERS_PER_CITY), send busy signal
			else {
				p->Winsock->SendData(Index,smMayorInInterview," ",1);
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessCancelJob
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessCancelJob(char *TheData, int Index) {

	// For each possible city,
	for (int j = 0; j < MAX_CITIES; j++) {

		// If that city is interviewing this player,
		if (p->City[j]->hiring == Index) {

			// Reset the interviewee and tell the mayor the interviewee left
			p->City[j]->hiring = -1;
			p->Winsock->SendData(p->City[j]->Mayor,smInterviewCancel," ");
		}
	}
}


/***************************************************************
 * Function:	ProcessAccept
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessAccept(char *TheData, int Index) {

	// If the player is mayor,
	if (p->Player[Index]->Mayor) {

		// If the player is hiring someone,
		if (p->City[p->Player[Index]->City]->hiring > -1) {

			// If the interviewee is connected,
			if (p->Player[p->City[p->Player[Index]->City]->hiring]->Socket) {

				// Add the interviewee
				p->Player[p->City[p->Player[Index]->City]->hiring]->City = p->Player[Index]->City;
				p->Player[p->City[p->Player[Index]->City]->hiring]->setMayor(0);
				p->Player[p->City[p->Player[Index]->City]->hiring]->StartJoin();
				p->Player[Index]->State = State_Game;
			}
			p->City[p->Player[Index]->City]->hiring = -1;
		}
	}
}

/***************************************************************
 * Function:	ProcessDeny
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessDeny(int Index) {

	// If the player is mayor,
	if (p->Player[Index]->Mayor) {

		// If the player is hiring someone,
		if (p->City[p->Player[Index]->City]->hiring > -1) {

			// If the interviewee is connected,
			if (p->Player[p->City[p->Player[Index]->City]->hiring]->Socket) {

				// Tell the interviewee to buzz off
				p->Winsock->SendData(p->City[p->Player[Index]->City]->hiring,smMayorDeclined," ",1);
			}
			p->City[p->Player[Index]->City]->hiring = -1;
		}
	}
}

/***************************************************************
 * Function:	ProcessUpdate
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessUpdate(sCMUpdate *data, int Index) {
	sSMUpdate update;
	sSMStateGame game;

	// If the player is dead, return
	if (p->Player[Index]->isDead == true) {
		return;
	}

	// If the player is within 300 x and 300 y of the update position, and the Movement timer is up,
	if (
		(abs(int(data->x - p->Player[Index]->X)) < 300)
		&&
		(abs(int(data->y - p->Player[Index]->Y)) < 300)
		&&
		((p->Player[Index]->lastMove + 3000) > p->Tick)
		) {

		// Reset the Movement timer
		p->Player[Index]->lastMove = p->Tick;

		// HACK: Cap data->x and data->y at 24500
		if (data->x > 24576) {
			data->x = 24500;
		}
		if (data->y > 24576) {
			data->y = 24500;
		}

		// Set the player's position to the location
		p->Player[Index]->X = update.x = data->x;
		p->Player[Index]->Y = update.y = data->y;

		// Send the update to the radar, minus the player
		update.turn = data->turn;
		update.id = (unsigned char)Index;
		update.dir = data->dir;
		update.move = data->move;
		p->Send->SendRadarNotIndex(Index,smUpdate,(char *)&update,sizeof(update));
	}

	// Else (player not within 300x300),
	else {

		// Warp the player
		// ???
		p->Player[Index]->lastMove = p->Tick;
		game.City = p->Player[Index]->City;
		game.x = (unsigned short)p->Player[Index]->X;
		game.y = (unsigned short)p->Player[Index]->Y;
		p->Winsock->SendData(Index, smWarp,(char *)&game,sizeof(game));
	}
}

/***************************************************************
 * Function:	ProcessItemUp
 *
 * @param User
 **************************************************************/
void CProcess::ProcessItemUp(sCMItem *data, int Index) {
	CItem *item;
	sSMItem msg;
	sCMItem itemPacket;
	
	// If the player is dead, return
	if (p->Player[Index]->isDead == true) {
		return;
	}

	item = p->Item->findItem(data->id);
	if (item) {

		// If the item is on the ground, and belongs to the player's city (or the player is an admin),
		if ((item->City == p->Player[Index]->City || p->Player[Index]->isAdmin()) && item->holder == -1) {
			
			// Pick up the item
			item->holder = Index;
			msg.id = (unsigned short)data->id;

			// Tell the player he picked it up
			itemPacket.id = item->id;
			itemPacket.active = data->active;
			itemPacket.type = item->type;
			p->Winsock->SendData(Index,smPickedUp, (char *)&itemPacket, sizeof(itemPacket));

			// Tell the sector the item is gone
			p->Send->SendSectorArea(item->x*48, item->y*48 ,smRemItem, (char *)&msg, sizeof(msg));
		}
	}
}

/***************************************************************
 * Function:	ProcessItemDrop
 *
 * @param User
 **************************************************************/
void CProcess::ProcessItemDrop(sCMItem *data, int Index) {
	CItem *item;
	sCMItem *itm;
	sCMItem itemPacket;

	// If the player is dead, return
	if (p->Player[Index]->isDead == true) {
		return;
	}

	
	itm = (sCMItem *)data;

	// If the item to drop is found,
	item = p->Item->findItem(itm->id);
	if (item) {

		// If it is held by this player,
		if (item->holder == Index) {

			// Tell the player to drop the item
			itemPacket.id = item->id;
			itemPacket.active = data->active;
			itemPacket.type = item->type;
			p->Winsock->SendData(Index,smDropped, (char *)&itemPacket, sizeof(itemPacket));

			// Activate and drop the item
			item->active = data->active;
			item->drop((int)((p->Player[Index]->X+24) / 48),(int)((p->Player[Index]->Y+24) / 48),Index);
		}
	}
}

/***************************************************************
 * Function:	ProcessVersion
 *
 * @param vers
 * @param Index
 **************************************************************/
void CProcess::ProcessVersion(sCMVersion *vers, int Index) {

	// If the versions don't match, send version check failure
	if (strcmp(vers->Version, VERSION) != 0) {

		// Send version check failure
		p->Winsock->SendData(Index, smError, "F");
		cout << "Invalid Version " << vers->Version << " from " << p->Player[Index]->IPAddress << "\n";
	}

	// Else (versions match),
	else {

		// If the IP is banned, send IP ban failure
		if (p->Account->CheckBan(vers->UniqID, p->Player[Index]->IPAddress) == 1) {
			p->Winsock->SendData(Index, smError, "G");
		}

		// Else (no IP ban), send success
		else  {
			p->Player[Index]->UniqID = vers->UniqID;
			p->Player[Index]->State = State_Verified;
			p->Player[Index]->id = Index;
		}
	}
}

/***************************************************************
 * Function:	ProcessLogin
 *
 * @param login
 * @param Index
 **************************************************************/
void CProcess::ProcessLogin(sCMLogin *login, int Index) {
	int CorrectPassword;
	int foundmatch;

	// If the player's state is Verified (past version check and ban check),
	if (p->Player[Index]->State == State_Verified)  {

		// If the account exists,
		if (p->Account->CheckAccount(login->User) == 1) {

			// If the password is correct,
			CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
			if (CorrectPassword == 2) {

				// If the user isn't banned,
				if (p->Account->CheckAccountBan(login->User) == 0) {
					foundmatch = 0;

					// HACK: FIX "- 1"
					// TODO: FIX "- 1"
					// For each possible player,
					for (int j = 0; j < (MAX_PLAYERS-1); j++) {
						
						// If the account is already in use, clear the player and set foundmatch=1
						if (p->PlatformCaseCompare(login->User, p->Player[j]->Name.c_str()) == 0) {
							p->Player[j]->Clear();
							foundmatch = 1;
						}

/*
						// If the CPU is already connected, clear the player and set foundmatch=2
						if (p->PlatformCaseCompare(p->Player[Index]->UniqID.c_str(), p->Player[j]->UniqID.c_str()) == 0 && Index != j) {
							p->Player[j]->Clear();
							foundmatch = 2;
						}
*/
					}

					// If the account is already logged in,
					if (foundmatch == 1) {

						// Send account already in use error
						cout << "Multilog from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "E");
					}

					// Else if the computer is already connected,
/*
					else if (foundmatch == 2) {

						// Send MultiCPU error
						cout << "MultiCPU log from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "H");
					}
*/

					// Else (no Multilog, no Multicpu), log the player in
					else {	
						p->Player[Index]->LoggedIn(login->User);
					}
				}

				// Else (ban check failed), send banned message
				else {
					cout << "Banned log from " << login->User << endl;
					p->Winsock->SendData(Index, smError, "G");
				} //Banned
			}

			// Else if incorrect password, send incorrect password message
			else if (CorrectPassword == 0) {
				p->Winsock->SendData(Index, smError, "B");
			}
		}

		// Else (account doesn't exist), send account doesn't exist message
		else {
			p->Winsock->SendData(Index, smError, "C");
		}
	}
}

/***************************************************************
 * Function:	ProcessNewAccount
 *
 * @param newaccount
 * @param Index
 **************************************************************/
void CProcess::ProcessNewAccount(sCMNewAccount *newaccount, int Index) {
	int CreateAccount = p->Account->NewAccount(newaccount->User, newaccount->Pass, newaccount->Email, newaccount->FullName, newaccount->Town, newaccount->State);

	// If the account was created, send success
	if (CreateAccount == 1) {
		p->Winsock->SendData(Index, smError, "A");
	}

	// Else if email address in use, send email in use
	else if (CreateAccount == -1) {
		p->Winsock->SendData(Index, smError, "K");
	}

	// Else if name in use, send name in use
	else if (CreateAccount == 0) {
		p->Winsock->SendData(Index, smError, "D");
	}
}

/***************************************************************
 * Function:	ProcessSetState
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessSetState(char *TheData, int Index) {
	char tmpString[5];

	// If the player is Verified,
	if (p->Player[Index]->State >= State_Verified) {
		memset(tmpString, 0, 5);
		tmpString[0] = (unsigned char)Index;

		switch(TheData[0]) {
			
			//State A/Chat
			case 65: 
			
			//State B/Daily Bullet
			case 66: 

			//State C/City List
			case 67: 
				if (p->Player[Index]->State == State_Game) {
					p->Player[Index]->LeaveGame(true);
				}
				p->Send->SendMeetingRoom(Index);
				break;
			
			//State D/Disconnected
			case 68: 
				strcpy(&tmpString[1], "D"); 
				p->Player[Index]->Clear(); 
				p->Send->SendToChat(smChatCommand, tmpString); 
				break;
		}
	}
}

/***************************************************************
 * Function:	ProcessShot
 *
 * @param User
 * @param Index
 **************************************************************/
void CProcess::ProcessShot(sCMShot *data, int Index) {
	sSMShot shotsfired;

	// If the Shot timer is up, or the player is an admin,
	if ( (p->Tick > p->Player[Index]->lastShot + 500) || (p->Player[Index]->isAdmin()) )	{

		// Reset the Shot timer
		p->Player[Index]->lastShot = p->Tick;

		// Create a new shot
		shotsfired.id = Index;
		shotsfired.type = data->type;
		shotsfired.x = data->x;
		shotsfired.y = data->y;
		shotsfired.dir = data->dir;
		p->Bullet->newBullet(data->x,data->y,data->type,data->dir, Index);

		// Tell everyone but the player about the new shot
		p->Send->SendRadarNotIndex(Index, smShoot, (char *)&shotsfired, sizeof(sSMShot));
	}

/*
	// Possible replacement for code above, once SHOT_TYPE_LASER and SHOT_TYPE_ROCKET are defined
	bool canShoot = false;

	if (p->Player[Index]->isAdmin()) {
		canShoot == true;
	}
	else if (SHOT_TYPE_LASER == data->type) {
		if( p->Player[Index]->lastShot + TIMER_SHOOT_LASER < p->Tick ) {
			canShoot = true;
		}
	}
	else if (SHOT_TYPE_ROCKET == data->type) {
		if( p->Player[Index]->lastShot + TIMER_SHOOT_ROCKET < p->Tick ) {
			canShoot = true;
		}
	}

	if (canShoot) {
		p->Player[Index]->lastShot = p->Tick;
		sSMShot shotsfired;
		shotsfired.id = Index;
		shotsfired.type = data->type;
		shotsfired.x = data->x;
		shotsfired.y = data->y;
		shotsfired.dir = data->dir;
		p->Bullet->newBullet(data->x,data->y,data->type,data->dir, Index);
		p->Send->SendRadarNotIndex(Index, smShoot, (char *)&shotsfired, sizeof(sSMShot));
	}
*/

}

/***************************************************************
 * Function:	ProcessDeath
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessDeath(char *TheData, int Index) {
	char packet[3];

	// If the player is in game,
	if (p->Player[Index]->State == State_Game) {

		// Add the death
		p->Account->AddDeath(Index);

		// Tell everyone else about the death,
		packet[0] = (unsigned char)Index;
		packet[1] = 0;
		packet[2] = TheData[0];
		p->Send->SendGameAllBut(-1, smDeath, packet, 3);
		cout << p->Player[Index]->Name << " has died" << endl;
		p->Player[Index]->isDead = true;
		p->Player[Index]->timeDeath = p->Tick;

		// If the player has more than 100 points,
		if (p->Player[Index]->Points > 100) {

			// Delete the player's items
			p->Item->deleteItemsByPlayer(Index);

			// Subtract two points
			p->Account->Sub2Points(Index);

			// If the player's not in your city,
			if (p->Player[Index]->City != TheData[0]) {

				// For each possible player,
				for (int j = 0; j < MAX_PLAYERS; j++) {

					// If the player is in that city and in game,
					if ((p->Player[j]->State == State_Game) && (p->Player[j]->City == TheData[0])) {

						// Give that player two points
						p->Account->AddPoints(j, 2);
					}
				}
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessMedKit
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessMedKit(int *data, int Index) {
	CItem *itm = p->Item->findItem(*data);

	// If the item is found, the player holds it, and it's a medkit,
	if (itm) {
		if (itm->type == 2 && itm->holder == Index) {

			// Use and delete the medkit
			p->Item->delItem(itm);
			p->Winsock->SendData(Index, smMedKit, " ");
		}
	}
}

/***************************************************************
 * Function:	ProcessDemolish
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessDemolish(sCMDemolish *data, int Index) {
	CBuilding *bld;

	// If the player is dead, return
	if (p->Player[Index]->isDead == true) {
		return;
	}

	// If the building is found, and it belongs to the player's city,
	bld = p->Build->findBuilding(data->id);
	if (bld) {
		if (bld->City == p->Player[Index]->City) {

			// Delete building
			p->Build->delBuilding(bld);
		}
	}
}

/***************************************************************
 * Function:	ProcessHitObject
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessHitObject(sCMItem *data, int Index) {
	CItem *item;

	// If the item is found, delete it
	item = p->Item->findItem(data->id);
	if (item) {
		p->Item->delItem(item);
	}
}

/***************************************************************
 * Function:	ProcessCrash
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessCrash(char *TheData,int Index) {
	std::string theerror;

	theerror.clear();
	theerror.append(TheData);
	p->Log->logClientError(theerror, Index);
}

/***************************************************************
 * Function:	ProcessNextStep
 *
 * @param TheStep
 * @param Index
 **************************************************************/
void CProcess::ProcessNextStep(char TheStep, int Index) {
	switch (TheStep) {

		// A Ready to start receiving data
		case 65: 
			p->Player[Index]->JoinGame();
			p->Winsock->SendData(Index, smNextStep, "B");
			break;

		//B Refreshed Area
		case 66: 
			p->Winsock->SendData(Index, smNextStep, "C");
			break;

		//C Done
		case 67:
			break;
	}
}

/***************************************************************
 * Function:	ProcessSuccessor
 *
 * @param Successor
 * @param Index
 **************************************************************/
void CProcess::ProcessSuccessor(char Successor, int Index) {

	// If the successor is in the same city as the player, and the player is Mayor,
	if (p->Player[Successor]->City == p->Player[Index]->City && p->Player[Index]->Mayor) {

		// Set the successor
		p->City[p->Player[Index]->City]->Successor = Successor;
	}
}

/***************************************************************
 * Function:	ProcessAdmin
 *
 * @param admin
 * @param Index
 **************************************************************/
void CProcess::ProcessAdmin(sCMAdmin *admin, int Index) {
	sSMAdmin sadmin;
	string LogAdminString;
	ostringstream convert;
	string queryString;
	int tmpID;
	CItem *itm;
	sCMItem PickUp;
	sSMItem msg;
	sSMBan ban;

	// If the player is an admin,
	if (p->Player[Index]->isAdmin()) {

		// Switch on the Admin command
		switch (admin->command) {

			// Command: KICK
			case 1:
				sadmin.adminIndex = Index;
				sadmin.command = 1;
				sadmin.id = admin->id;

				// If the player is in game, kick the player
				if (p->Player[admin->id]->State == State_Game) {
					LogAdminString = "Kick ";
					LogAdminString += p->Player[admin->id]->Name;
					p->Log->logAdmin(LogAdminString, Index);
					cout << "Kick::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
					p->Player[admin->id]->LeaveGame(true);
					p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
					p->Winsock->SendData(admin->id, smKicked, " ");
				}
				break;

			// Command: JOIN CITY
			case 2:
				
				// If the city is a valid city,
				if (admin->id >= 0 && admin->id < MAX_CITIES) {
					LogAdminString = "Join City ";		
					convert << admin->id;
					LogAdminString += convert.str();
					p->Log->logAdmin(LogAdminString, Index);
					p->Player[Index]->LeaveGame(true);
					p->Player[Index]->City = admin->id;

					// If the city has no mayor, set the admin to mayor
					if (p->City[admin->id]->Mayor == -1) {
						p->Player[Index]->setMayor(1);
					}
					p->Player[Index]->StartJoin();
				}
				break;

			// Command: WARP
			case 3:

				// Warp the player
				LogAdminString = "Warp ";
				LogAdminString += p->Player[admin->id]->Name;
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Warp::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
				p->Player[Index]->X = p->Player[admin->id]->X;
				p->Player[Index]->Y = p->Player[admin->id]->Y;
				break;

			// Command: SUMMON
			case 4:

				// Summon the player
				LogAdminString = "Summon ";
				LogAdminString += p->Player[admin->id]->Name;
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Summon::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
				p->Player[admin->id]->X = p->Player[Index]->X;
				p->Player[admin->id]->Y = p->Player[Index]->Y;
				break;

			// Command: BAN
			case 5:
				queryString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
				queryString += p->Player[admin->id]->Name;
				queryString += "', '";
				queryString += p->Player[admin->id]->IPAddress;
				queryString += "', '";
				queryString += p->Player[Index]->Name;
				queryString += "', '";
				queryString += p->Player[admin->id]->UniqID;
				queryString += "');";
				p->Database->Database.execDML(queryString.c_str());

				sadmin.adminIndex = Index;
				sadmin.command = 5;
				sadmin.id = admin->id;

				// If the player is in game, boot the player
				if (p->Player[admin->id]->State == State_Game) {
					LogAdminString = "Ban " + p->Player[admin->id]->Name;
					p->Log->logAdmin(LogAdminString, Index);
					cout << "Ban::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
					p->Player[admin->id]->LeaveGame(true);
					p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
					p->Winsock->SendData(admin->id, smKicked, " ");
				}
				break;

			// Command: SHUTDOWN
			case 6:
				p->Log->logAdmin("Shutdown", Index);
				cout << "Shutdown::" << p->Player[Index]->Name << endl;
				p->running = 0;
				break;

			// Command: SPAWN ITEM
			case 7:

				// Spawn the item
				LogAdminString = "Spawn Item ";
				convert << admin->id;
				LogAdminString += convert.str();
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Spawn Item::" << p->Player[Index]->Name << "::" << admin->id << endl;

				// HACK: cap itemID at 30000
				tmpID = p->Item->itmID++;
				if (p->Item->itmID > 30000) {
					p->Item->itmID = 1;
				}
				
				// Create the item, add it to the user
				itm = p->Item->newItem(0, 0, admin->id, p->Player[Index]->City, tmpID);
				itm->holder = Index;
				
				PickUp.id = itm->id;
				PickUp.active = itm->active;
				PickUp.type = itm->type;

				// Tell everone you created, picked up, and removed the item
				msg.id = itm->id;
				msg.x = 0;
				msg.y = 0;
				msg.City = p->Player[Index]->City;
				msg.active = 0;
				msg.type = (unsigned char)admin->id;

				p->Winsock->SendData(Index,smAddItem,(char *)&msg, sizeof(msg));
				p->Winsock->SendData(Index,smPickedUp, (char *)&PickUp, sizeof(PickUp));
				p->Winsock->SendData(Index,smRemItem, (char *)&msg, sizeof(msg));
				break;

			// Command: GET BANS
			case 8:

				// Get the bans
				p->Log->logAdmin("Request Bans", Index);
				queryString = "SELECT * FROM tBans";
				p->Database->Query = p->Database->Database.execQuery(queryString.c_str());

				// For each ban,
				while (!p->Database->Query.eof()) {

					// Tell the admin about the ban
					memset(&ban, 0, sizeof(ban));
					strcpy(ban.Account, p->Database->Query.getStringField("Account"));
					strcpy(ban.IPAddress, p->Database->Query.getStringField("IPAddress"));
					strcpy(ban.Reason, p->Database->Query.getStringField("Reason"));
					p->Winsock->SendData(Index, smBan, (char *)&ban, sizeof(ban));

					p->Database->Query.nextRow();
				}

				p->Database->Query.finalize();
				break;

			// Command: UNBAN
			case 9:

				// Get the bans
				p->Log->logAdmin("Unban Attempt", Index);
				queryString = "SELECT * FROM tBans";
				p->Database->Query = p->Database->Database.execQuery(queryString.c_str());

				// For each ban,
				for (int i = 0; i < admin->id; i++) {
					if (!p->Database->Query.eof()) {
						p->Database->Query.nextRow();
					}
					else {
						p->Database->Query.finalize();
						return;
					}
				}

				if (!p->Database->Query.eof()) {
					try {
						// Delete the ban
						queryString = "DELETE FROM tBans WHERE Account = '";
						queryString += p->Database->Query.getStringField("Account");
						queryString += "';";
						p->Database->Query.finalize();
						p->Log->logAdmin(queryString, Index);
						p->Database->Database.execDML(queryString.c_str());
					}
					catch (CppSQLite3Exception& e) {
						cerr << e.errorCode() << ":" << e.errorMessage() << endl;
						return;
					}
				}
				else {
					p->Database->Query.finalize();
					return;
				}
				break;

			// Command: REQUEST NEWS
			case 10: 
				p->Log->logAdmin("Request News", Index);
				p->Send->SendAdminNews(Index);
				break;

			// Command: UPDATE NEWS
			case 11:
				p->Log->logAdmin("Update News", Index);
				break;

		}
	}

	// Else (player is not admin),
	else {
		p->Log->logClientError("Attempted Admin Command", Index);
	}
}

/***************************************************************
 * Function:	ProcessBan
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessBan(char *TheData, int Index) {
	string DMLString;
	DMLString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
	DMLString += p->Player[Index]->Name;
	DMLString += "', '";
	DMLString += p->Player[Index]->IPAddress;
	DMLString += "', '";
	DMLString += TheData;
	DMLString += "', '";
	DMLString += p->Player[Index]->UniqID;
	DMLString += "');";
	p->Database->Database.execDML(DMLString.c_str());
	cout << "Ban::" << p->Player[Index]->Name << endl;
}

/***************************************************************
 * Function:	ProcessIsHiring
 *
 * @param NotHiring
 * @param Index
 **************************************************************/
void CProcess::ProcessIsHiring(char NotHiring, int Index) {

	// If the player is Mayor and in game, set notHiring
	if (p->Player[Index]->Mayor && p->Player[Index]->State == State_Game) {
		p->City[p->Player[Index]->City]->notHiring = NotHiring;
	}
}

/***************************************************************
 * Function:	ProcessRequestSector
 *
 * @param sector
 * @param Index
 **************************************************************/
void CProcess::ProcessRequestSector(sCMSector *sector, int Index) {
	p->Send->SendSector(Index, sector->x, sector->y);
}

/***************************************************************
 * Function:	ProcessEditAccount
 *
 * @param login
 * @param Index
 **************************************************************/
void CProcess::ProcessEditAccount(sCMLogin *login, int Index) {
	int CorrectPassword;
	int foundmatch = 0;

	// If the player is at Verified state,
	if (p->Player[Index]->State == State_Verified) {

		// If the account exists,
		if (p->Account->CheckAccount(login->User) == 1) {

			// If the password is correct,
			CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
			if (CorrectPassword == 2) {

				// If the player is not banned,
				if (p->Account->CheckAccountBan(login->User) == 0) {

					// For each possible player,
					// HACK: FIX "- 1"
					// TODO: FIX "- 1"
					for (int j = 0; j < (MAX_PLAYERS-1); j++) {

						// If the account is already in use, set foundmatch=1
						if (p->PlatformCaseCompare(login->User, p->Player[j]->Name.c_str()) == 0) {
							foundmatch = 1;
						}

						// If the CPU is already connected, set foundmatch=2
						if (p->PlatformCaseCompare(p->Player[Index]->UniqID.c_str(), p->Player[j]->UniqID.c_str()) == 0 && Index != j) {
							foundmatch = 2;
						}
					}

					// If the account is already logged in, send account already in use error
					if (foundmatch == 1) {
						cout << "Multilog from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "E");
					}

					// Else if the computer is already logged in, send computer already in use error
					else if (foundmatch == 2) {
						cout << "MultiCPU log from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "H");
					}

					// Else, send successful login and set state to Editing
					else {
						p->Player[Index]->Name = login->User;
						p->Account->SendAccountInformation(Index);
						p->Player[Index]->State = State_Editing;
					}
				}

				// Else (user is banned), send Banned error
				else {
					cout << "Banned log from " << login->User << endl; p->Winsock->SendData(Index, smError, "G");
				}
			}

			// Else if password is incorrect, send incorrect password error
			else if (CorrectPassword == 0) {
				p->Winsock->SendData(Index, smError, "B");
			}

			// Else (CorrectPassword == 1), allow edit
			// ???
			else  {
				p->Player[Index]->Name = login->User;
				p->Account->SendAccountInformation(Index);
				p->Player[Index]->State = State_Editing;
			}
		}

		// Else (account does not exist), sent account not found error
		else {
			p->Winsock->SendData(Index, smError, "C");
		}
	}
}

/***************************************************************
 * Function:	ProcessUpdateAccount
 *
 * @param updateaccount
 * @param Index
 **************************************************************/
void CProcess::ProcessUpdateAccount(sCMNewAccount *updateaccount, int Index) {
	int UpdateAccount;
	int CorrectPassword;

	// If the player is Editing,
	if (p->Player[Index]->State == State_Editing) {

		// Try to update the account
		UpdateAccount = p->Account->UpdateAccount(Index, updateaccount->Pass, updateaccount->Email, updateaccount->FullName, updateaccount->Town, updateaccount->State);

		// If the update succeeded,
		if (UpdateAccount == 1) {

			// Check the password
			CorrectPassword = p->Account->CheckPassword(updateaccount->User, updateaccount->Pass);

			// If the password is correct, log the player in
			if (CorrectPassword == 2) {
				p->Player[Index]->LoggedIn(p->Player[Index]->Name);
			}

			// If the password is wrong, send wrong password error
			else if (CorrectPassword == 0) {
				p->Winsock->SendData(Index, smError, "B"); 
			}
		}

		// Else, if the update failed, send Email already in use error
		else if (UpdateAccount == -1) {
			p->Winsock->SendData(Index, smError, "K");
		}
	}
}

/***************************************************************
 * Function:	ProcessRecover
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessRecover(char *TheData, int Index) {
	int i = p->Account->RecoverAccount(Index, TheData);
	
	// TODO: explain what this does

	// If RecoverAccount returns 0, send L
	if (i == 0) {
		p->Winsock->SendData(Index, smError, "L");
	}
	// Else, send M
	else {
		p->Winsock->SendData(Index, smError, "M");
	}
}

/***************************************************************
 * Function:	ProcessRequestInfo
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessRequestInfo(int Index) {
	int SearchX = (int)p->Player[Index]->X/48;
	int SearchY = (int)p->Player[Index]->Y/48;

	short int selectedCity = 255;

	int bestOrbValue = 0;
	int bestDistance = 255;

	int orbValue;
	int distance;

	// For each possible city,
	for (int i = 0; i < MAX_CITIES; i++) {

		// If the city isn't this player's city,
		if (i != p->Player[Index]->City) {

			// If the city is orbable,
			if (p->City[i]->isOrbable()) {

				// ??
				if (p->City[i]->x == 0 || p->City[i]->y == 0) {
					p->City[i]->x = (unsigned short)(512*48)-(32+(i % 8*MAX_CITIES)) * 48;
					p->City[i]->y = (unsigned short)(512*48)-(32+(i / 8*MAX_CITIES)) * 48; 
				}

				// Get the city's orb value and distance from the player's city
				orbValue = p->City[i]->getOrbValue();
				distance = (int)sqrt((float)((p->City[i]->x - SearchX)^2 + (p->City[i]->y - SearchY)^2));

				// If the city is worth more than the others found so far, select it
				if (orbValue > bestOrbValue) {
					selectedCity = i;
					bestDistance = distance;
					bestOrbValue = orbValue;
				}

				// Else, if the city is worth the same as the max but is closer, select it
				else if ( (orbValue==bestOrbValue) && (distance < bestDistance) ) {
					selectedCity = i;
					bestDistance = distance;
					bestOrbValue = orbValue;
				}
			}
		}
	}

	p->Send->SendInfoButton(Index, selectedCity);
}

/***************************************************************
 * Function:	ProcessRightClickCity
 *
 * @param City
 * @param Index
 **************************************************************/
void CProcess::ProcessRightClickCity(int City, int Index) {
	sSMRightClickCity cityinfo;

	// Send the city's building count and orb points
	cityinfo.BuildingCount = p->City[City]->currentBuildingCount;
	cityinfo.City = City;
	cityinfo.IsOrbable = p->City[City]->isOrbable();
	cityinfo.Orbs = p->City[City]->Orbs;
	cityinfo.OrbPoints = p->City[City]->getOrbValue();
	cityinfo.UptimeInMinutes = p->City[City]->getUptimeInMinutes();
	p->Winsock->SendData(Index, smRightClickCity, (char *)&cityinfo, sizeof(cityinfo));
}

/***************************************************************
 * Function:	ProcessAdminEdit
 *
 * @param Index
 * @param sCMAdminEdit
**************************************************************/
void CProcess::ProcessAdminEdit(int Index, sCMAdminEdit *adminedit) {
	string DMLString;
	std::ostringstream converter;
	sSMPoints pts;
	sSMPlayer player;

	// If the player is an admin,
	if (p->Player[Index]->isAdmin()) {

		// If the player is in a certain set of names,
		if (
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Weebo") == 0)
			||
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Vindkast") == 0)
		) {
			
			// If the account to edit exists,
			if (p->Account->CheckAccount(adminedit->User) == 1) {

				// Create the Update statement
				DMLString = "UPDATE tAccounts SET Deaths = ";
				converter.str("");
				converter << adminedit->Deaths;
				DMLString += converter.str();

				DMLString += ", Orbs = ";
				converter.str("");
				converter << adminedit->Orbs;
				DMLString += converter.str();

				DMLString += ", Assists = ";
				converter.str("");
				converter << adminedit->Assists;
				DMLString += converter.str();

				DMLString += ", Points = ";
				converter.str("");
				converter << adminedit->Points;
				DMLString += converter.str();

				DMLString += ", Fullname = '";
				DMLString += adminedit->FullName;

				DMLString += "', Town = '";
				DMLString += adminedit->Town;

				DMLString += "', State = '";
				DMLString += adminedit->State;

				DMLString += "', Password = '";
				DMLString += adminedit->Pass;

				DMLString += "', Email = '";
				DMLString += adminedit->Email;

				DMLString += "', Tank = ";
				converter.str("");
				converter << (int)adminedit->Tank;
				DMLString += converter.str();

				DMLString += ", Tank2 = ";
				converter.str("");
				converter  << (int)adminedit->Tank2;
				DMLString += converter.str();

				DMLString += ", Tank3 = ";
				converter.str("");
				converter  << (int)adminedit->Tank3;
				DMLString += converter.str();

				DMLString += ", Tank4 = ";
				converter.str("");
				converter  << (int)adminedit->Tank4;
				DMLString += converter.str();
				
				DMLString += ", Tank5 = ";
				converter.str("");
				converter  << (int)adminedit->Tank5;
				DMLString += converter.str();
								
				DMLString += ", Tank6 = ";
				converter.str("");
				converter  << (int)adminedit->Tank6;
				DMLString += converter.str();
								
				DMLString += ", Tank7 = ";
				converter.str("");
				converter  << (int)adminedit->Tank7;
				DMLString += converter.str();
								
				DMLString += ", Tank8 = ";
				converter.str("");
				converter  << (int)adminedit->Tank8;
				DMLString += converter.str();
								
				DMLString += ", Tank9 = ";
				std::ostringstream Tank9Convert;
				Tank9Convert << (int)adminedit->Tank9;
				DMLString += converter.str();

				DMLString += ", Red = ";
				converter.str("");
				converter  << (int)adminedit->Red;
				DMLString += converter.str();

				DMLString += ", Green = ";
				converter.str("");
				converter  << (int)adminedit->Green;
				DMLString += converter.str();

				DMLString += ", Blue = ";
				converter.str("");
				converter  << (int)adminedit->Blue;
				DMLString += converter.str();

				DMLString += ", Member = ";
				converter.str("");
				converter  << (int)adminedit->Member;
				DMLString += converter.str();

				DMLString += ", RentalCity = ";
				converter.str("");
				converter  << (int)adminedit->RentalCity;
				DMLString += converter.str();

				DMLString += ", IsAdmin = ";
				converter.str("");
				converter  << adminedit->playerType;
				DMLString += converter.str();

				DMLString += " WHERE lower(Account) = lower('";
				DMLString += adminedit->User;
				DMLString += "');";

				p->Log->logAdmin(DMLString, Index);

				p->Database->Database.execDML(DMLString.c_str());

				// For each possible player,
				// HACK: FIX "- 1"
				// TODO: FIX "- 1"
				for (int i = 0; i < MAX_PLAYERS-1; i++) {

					// If that player is the player we are editing,
					if (p->PlatformCaseCompare(adminedit->User, p->Player[i]->Name) == 0) {

						// Set the new information on the player
						p->Player[i]->Red = adminedit->Red;
						p->Player[i]->Green = adminedit->Green;
						p->Player[i]->Blue = adminedit->Blue;
						p->Player[i]->Member = adminedit->Member;
						p->Player[i]->playerType = adminedit->playerType;
						p->Player[i]->Tank = adminedit->Tank;
						p->Player[i]->Tank2 = adminedit->Tank2;
						p->Player[i]->Tank3 = adminedit->Tank3;
						p->Player[i]->Tank4 = adminedit->Tank4;
						p->Player[i]->Tank5 = adminedit->Tank5;
						p->Player[i]->Tank6 = adminedit->Tank6;
						p->Player[i]->Tank7 = adminedit->Tank7;
						p->Player[i]->Tank8 = adminedit->Tank8;
						p->Player[i]->Tank9 = adminedit->Tank9;
						p->Player[i]->Town = adminedit->Town;

						p->Player[i]->Points = adminedit->Points;
						p->Player[i]->Deaths = adminedit->Deaths;
						p->Player[i]->Assists = adminedit->Assists;
						p->Player[i]->Orbs = adminedit->Orbs;

						// Send the points to everyone but the player
						pts.Index = i;
						pts.Points = p->Player[i]->Points;
						pts.Deaths = p->Player[i]->Deaths;
						pts.Assists = p->Player[i]->Assists;
						pts.Orbs = p->Player[i]->Orbs;
						pts.MonthlyPoints = p->Player[i]->MonthlyPoints;
						p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));

						// Send the player info to everyone but the player
						player.Red = p->Player[i]->Red;
						player.Green = p->Player[i]->Green;
						player.Blue = p->Player[i]->Blue;
						player.Member = p->Player[i]->Member;
						player.Index = i;
						player.playerType = p->Player[i]->playerType;
						player.Tank = p->Player[i]->displayTank;
						strcpy(player.Name, p->Player[i]->Name.c_str());
						strcpy(player.Town, p->Player[i]->Town.c_str());
						p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
					}
				}
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessAdminEditRequest
 *
 * @param Index
 * @param admineditrequest
 **************************************************************/
void CProcess::ProcessAdminEditRequest(int Index, sCMAdminEditRequest *admineditrequest) {
	string QueryString;
	sCMAdminEdit newedit;

	// If the player is an admin,
	if (p->Player[Index]->isAdmin()) {

		// If the player is in a certain set of names,
		if (
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Weebo") == 0)
			||
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Vindkast") == 0)
		) {	

			// Try to select the account from the database
			try {
				QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('";
				QueryString += admineditrequest->User;
				QueryString += "');";
				p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

				// If the query returned an account,
				if (!p->Database->Query.eof()) {
					p->Log->logAdmin(QueryString, Index);

					// Copy the user information into the newedit struct
					strcpy(newedit.User, admineditrequest->User);
					strcpy(newedit.Pass, p->Database->Query.getStringField("Password"));
					strcpy(newedit.FullName, p->Database->Query.getStringField("Fullname"));
					strcpy(newedit.Town, p->Database->Query.getStringField("Town"));
					strcpy(newedit.State, p->Database->Query.getStringField("State"));
					strcpy(newedit.Email, p->Database->Query.getStringField("Email"));

					newedit.Points = p->Database->Query.getIntField("Points");
					newedit.Orbs = p->Database->Query.getIntField("Orbs");
					newedit.Deaths = p->Database->Query.getIntField("Deaths");
					newedit.Assists = p->Database->Query.getIntField("Assists");
					newedit.playerType = p->Database->Query.getIntField("IsAdmin");
					newedit.Member = p->Database->Query.getIntField("Member");
					newedit.Red = p->Database->Query.getIntField("Red");
					newedit.Green = p->Database->Query.getIntField("Green");
					newedit.Blue = p->Database->Query.getIntField("Blue");
					newedit.Tank = p->Database->Query.getIntField("Tank");
					newedit.Tank2 = p->Database->Query.getIntField("Tank2");
					newedit.Tank3 = p->Database->Query.getIntField("Tank3");
					newedit.Tank4 = p->Database->Query.getIntField("Tank4");
					newedit.Tank5 = p->Database->Query.getIntField("Tank5");
					newedit.Tank6 = p->Database->Query.getIntField("Tank6");
					newedit.Tank7 = p->Database->Query.getIntField("Tank7");
					newedit.Tank8 = p->Database->Query.getIntField("Tank8");
					newedit.Tank9 = p->Database->Query.getIntField("Tank9");
					newedit.RentalCity = p->Database->Query.getIntField("RentalCity");
	
					// Send the data back to the admin
					p->Winsock->SendData(Index, smAdminEdit, (char *)&newedit, sizeof(newedit));
				}

				p->Database->Query.finalize();
			}
			catch (CppSQLite3Exception& e) {
				cerr << e.errorCode() << ":" << e.errorMessage() << endl;
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessClickPlayer
 *
* @param Index
* @param Clicked
 **************************************************************/
void CProcess::ProcessClickPlayer(int Index, int Clicked) {
	sSMClickPlayer clickplayer;

	// Send the player info back to the clicker
	clickplayer.Index = Clicked;
	clickplayer.Orbs = p->Player[Clicked]->Orbs;
	clickplayer.Assists = p->Player[Clicked]->Assists;
	clickplayer.Deaths = p->Player[Clicked]->Deaths;
	p->Winsock->SendData(Index, smClickPlayer, (char *)&clickplayer, sizeof(clickplayer));
}

/***************************************************************
 * Function:	ProcessChangeTank
 *
 * @param Index
 * @param tankIndex
 **************************************************************/
void CProcess::ProcessChangeTank(int Index, int tankIndex) {
	unsigned char tank;
	sSMPlayer player;

	// If player wants tank index 0, set no-custom-tank
	if (tankIndex == 0) {
		tank = 0;
	}

	// Else, set tank to the requested tank index
	else if (tankIndex == 1) {
		tank = p->Player[Index]->Tank;
	}
	else if (tankIndex == 2) {
		tank = p->Player[Index]->Tank2;
	}
	else if (tankIndex == 3) {
		tank = p->Player[Index]->Tank3;
	}
	else if (tankIndex == 4) {
		tank = p->Player[Index]->Tank4;
	}
	else if (tankIndex == 5) {
		tank = p->Player[Index]->Tank5;
	}
	else if (tankIndex == 6) {
		tank = p->Player[Index]->Tank6;
	}
	else if (tankIndex == 7) {
		tank = p->Player[Index]->Tank7;
	}
	else if (tankIndex == 8) {
		tank = p->Player[Index]->Tank8;
	}
	else if (tankIndex == 9) {
		tank = p->Player[Index]->Tank9;
	}

	// If the player is already using this tank as the displayTank, return
	if (tank == p->Player[Index]->displayTank) {
		return;
	}

	// Else, change the player's displayTank, and notify all players
	p->Player[Index]->displayTank = tank;

	// Tell everyone about the tank change
	player.Red = p->Player[Index]->Red;
	player.Green = p->Player[Index]->Green;
	player.Blue = p->Player[Index]->Blue;
	player.Member = p->Player[Index]->Member;
	player.Index = Index;
	player.playerType = p->Player[Index]->playerType;
	player.Tank = p->Player[Index]->displayTank;
	strcpy(player.Name, p->Player[Index]->Name.c_str());
	strcpy(player.Town, p->Player[Index]->Town.c_str());
	p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
}
