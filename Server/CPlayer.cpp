#include "CPlayer.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CPlayer::CPlayer(CServer *Server) {
	this->p = Server;

	// Reset the player stats
	this->ResetPlayer();
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CPlayer::~CPlayer() {
}

/***************************************************************
 * Function:	CPlayer
 *
 * @param set
 **************************************************************/
void CPlayer::setMayor(int set) {
	sSMFinance finance;
	sSMMayorUpdate Mayorupdate;

	// Store whether the player is in-game or not
	this->Mayor = set;

	// Set the player to in-game
	// ???
	this->State = State_Game;

	// If the player is now mayor,
	if (set) {

		// ???
		this->p->City[this->City]->id = this->City;

		// Store this player as mayor of this city,
		this->p->City[this->City]->Mayor = this->id;

		// If the player is hiring, send out MayorChanged messages
		if (this->p->City[this->City]->hiring > -1) {
			this->p->Winsock->SendData(this->p->City[this->City]->hiring,smMayorChanged," ");
		}

		// Reset the mayor options
		this->p->City[this->City]->Successor = -1;
		this->p->City[this->City]->notHiring = 0;
		this->p->City[this->City]->hiring = -1;

		// For each item,
		for (int j = 0; j <= 26; j++) {

			// Set the build tree for this city
			this->p->City[this->City]->setCanBuild(j,this->p->City[City]->canBuild[j]);
		}

		// Request the city's finance report
		finance.Cash = p->City[this->City]->cash;
		finance.Income = p->City[this->City]->income;
		finance.Items = p->City[this->City]->itemproduction;
		finance.Hospital = p->City[this->City]->hospital;
		finance.Research = p->City[this->City]->cashresearch;
		p->Winsock->SendData(id,smFinance,(char *)&finance,sizeof(finance));

		// Set isMayor = 1 on the packet
		Mayorupdate.IsMayor = 1;
	}

	// Else (player is now not mayor),
	else {
		// Set isMayor = 0 on the packet
		Mayorupdate.IsMayor = 0;
	}

	// Tell everyone the mayor changed
	Mayorupdate.Index = id;
	p->Send->SendGameAllBut(-1, smMayorUpdate, (char *)&Mayorupdate, sizeof(Mayorupdate));
}

/***************************************************************
 * Function:	Clear
 *
 **************************************************************/
void CPlayer::Clear() {
	char packet[1];

	// If the player is in game, boot the player
	if (this->p->Player[this->id]->State == State_Game) {
		this->p->Player[this->id]->LeaveGame(true);
	}

	// Tell everyone to clear the player
	packet[0] = id;
	this->p->Send->SendAllBut(-1, smClearPlayer, packet, 1);

	// Reset the player stats
	this->ResetPlayer();
}

/***************************************************************
 * Function:	ResetPlayer
 *
 **************************************************************/
void CPlayer::ResetPlayer() {

	// Reset the player stats
	this->X = 0;
	this->Y = 0;
	this->City = 0;
	this->Name.clear();
	this->Town.clear();
	this->UniqID.clear();
	this->IPAddress.clear();
	this->Points = 0;
	this->Orbs = 0;
	this->Assists = 0;
	this->Deaths = 0;
	this->MonthlyPoints = 0;
	this->Mayor = 0;
	this->State = State_Disconnected;
	shutdown(this->Socket, 2);
	memset(this->Buffer, 0, 2048);
	this->BufferLength = 0;

	this->playerType = 1;
	this->displayTank = 0;
	this->Tank = 0;
	this->Tank2 = 0;
	this->Tank3 = 0;
	this->Tank4 = 0;
	this->Tank5 = 0;
	this->Tank6 = 0;
	this->Tank7 = 0;
	this->Tank8 = 0;
	this->Tank9 = 0;
	this->Red = 0;
	this->Green = 0;
	this->Blue = 0;
	this->Member = 0;
	this->RentalCity = 0;

	this->id = 0;
	this->lastMove = 0;
	this->lastShot = 0;
	this->isDead = false;
	this->timeDeath = 0;
}

/***************************************************************
 * Function:	FindApplyMayor
 *
 **************************************************************/
int CPlayer::FindApplyMayor() {
	
	// For each possible city
	for (int j = 0; j < MAX_CITIES; j++) {

		// If the city is hiring... equals the player?
		if (this->p->City[j]->hiring == this->id) {

			// Return the Mayor
			return this->p->City[j]->Mayor;
		}
	}
	return -1;
}


/***************************************************************
 * Function:	isAdmin
 *
 * Returns true if the player is an admin
 **************************************************************/
bool CPlayer::isAdmin() {
	return (this->playerType == 2);
}

/***************************************************************
 * Function:	isInApply
 *
 * Returns true if the player's state is Apply
 **************************************************************/
bool CPlayer::isInApply() {
	return (this->State == State_Apply);
}

/***************************************************************
 * Function:	isInChat
 *
 * Returns true if the player's state is Chat
 **************************************************************/
bool CPlayer::isInChat() {
	return (this->State == State_Chat);
}

/***************************************************************
 * Function:	isConnected
 *
 * Returns true if the player's state is connected
 **************************************************************/
bool CPlayer::isConnected() {
	return (this->State != State_Disconnected);
}

/***************************************************************
 * Function:	isInGame
 *
 * Returns true if the player's state is Game
 **************************************************************/
bool CPlayer::isInGame() {
	return (this->State == State_Game);
}

/***************************************************************
 * Function:	isInGameApplyOrChat
 *
 * Returns true if the player's state is Chat, Game, or Apply
 **************************************************************/
bool CPlayer::isInGameApplyOrChat() {
	return this->isInApply() || this->isInChat() || this->isInGame();
}





//--------------------------- Actions ---------------------------\\



/***************************************************************
 * Function:	JoinGame
 *
 **************************************************************/
void CPlayer::JoinGame() {
	sSMStateGame stategame;
	sSMJoinData join;

	// Get the location of the CC
	this->p->City[City]->y = (int)(512*48)-(34+(City / 8*MAX_CITIES)) * 48; 
	this->p->City[City]->x = (int)(512*48)-(33+(City % 8*MAX_CITIES)) * 48;
	stategame.City = this->City;
	stategame.x = this->p->City[this->City]->x;
	stategame.y = this->p->City[this->City]->y;
	
	// Set the player on the CC
	this->p->Player[this->id]->X = stategame.x;
	this->p->Player[this->id]->Y = stategame.y;

	// Set the player's stats
	this->p->Account->GetStats(this->id);
	this->MonthlyPoints = this->p->Account->GetMonthlyTop20(id);

	// Send the data to the player
	this->p->Winsock->SendData(this->id,smStateGame,(char *)&stategame,sizeof(stategame));

	// Tell everyone but the player that the player joined
	join.id = this->id;
	join.Mayor = this->Mayor;
	join.City = this->City;
	this->p->Send->SendGameAllBut(this->id,smJoinData,(char *)&join,sizeof(join));
	this->p->Player[this->id]->lastMove = this->p->Tick;
	cout << "Join::" << Name.c_str() << endl;
}

/***************************************************************
 * Function:	StartJoin
 *
 **************************************************************/
void CPlayer::StartJoin() {

	// Set state to In Game
	this->State = State_Game;
	this->p->Send->SendGameData(this->id);
}

/***************************************************************
 * Function:	LeaveGame
 *
 * @param showMessage
 **************************************************************/
void CPlayer::LeaveGame(bool showMessage) {
	this->LeaveGame(showMessage, true);
}
/***************************************************************
 * Function:	LeaveGame
 *
 * @param showMessage
 * @param transferMayor
 **************************************************************/
void CPlayer::LeaveGame(bool showMessage, bool transferMayor) {
	int failed = 0;
	char packet[2];

	// Delete the items the player is holding
	this->p->Item->deleteItemsByPlayer(this->id);

	// If transferMayor is true,
	// Handle transfering mayor to another player in the city.
	// This is optional (doesn't happen when orbed) to prevent conflicts that migth cause the Balkh bug
	if (transferMayor) {

		// If the player is mayor,
		if (this->Mayor) {

			// If there is a successor,
			if (this->p->City[this->City]->Successor > -1) {

				// If that successor is in game, in this city, and not the mayor
				if (	
					(this->p->Player[this->p->City[this->City]->Successor]->isInGame())
					&&
					(this->p->Player[this->p->City[this->City]->Successor]->City == this->City)
					&&
					(this->p->City[this->City]->Successor != this->id)
					) {

					// Set the successor to mayor
					this->p->Player[this->p->City[this->City]->Successor]->setMayor(1);
				}

				// Else (successor not in game), set failed = 1
				else {
					failed = 1;
				}
			}

			// Else (no successor), set failed = 1
			else {
				failed = 1;
			}

			// If we failed to find a successor,
			if (failed == 1) {

				// For each possible player,
				for (int j = 0; j < MAX_PLAYERS; j++) {

					// If that player is in game, in this city, and not the mayor
					if ( (this->p->Player[j]->isInGame()) && (this->p->Player[j]->City == this->City) && (this->id != j) ) {

						// Make the player the new mayor
						this->p->Player[j]->setMayor(1);
						break;
					}
				}

				// If this player is still the mayor,
				if (this->p->City[this->City]->Mayor == id) {

					// If the city is not orbable, destroy it
					if (this->p->City[this->City]->isOrbable()==false) {
						this->p->City[this->City]->destroy();
					}

					// Else (orbable), start the Destruct timer
					else {
						this->p->City[this->City]->DestructTimer = this->p->Tick + TIMER_CITY_DESTRUCT;
						this->p->City[this->City]->Mayor = -1;
						this->p->City[this->City]->notHiring = 0;
					}
				}
			}
		}
	}

	// Reset this player
	this->p->Player[this->id]->X = 0;
	this->p->Player[this->id]->Y = 0;
	this->p->Player[this->id]->City = 0;
	this->p->Player[this->id]->isDead = false;
	this->p->Player[this->id]->timeDeath = 0;

	// If the player is in game,
	if (this->isInGame()) {

		// Save the player's stats
		this->p->Account->SaveStats(this->id);

		// If showMessage, tell everyone you left
		if (showMessage) {
			packet[0] = (char)this->id;
			packet[1] = 69;
			this->p->Send->SendGameAllBut(this->id, smChatCommand, packet, 2); 
			cout << "Left::" << Name << endl;
		}
	}

	// Set the state to Chat
	this->p->Player[this->id]->State = State_Chat;
}

/***************************************************************
 * Function:	LoggedIn
 *
 * @param User
 **************************************************************/
void CPlayer::LoggedIn(string User) {
	char TempString[20];
	sSMPlayer player;
	sSMPoints pts;

	// Set name, login data
	this->Name = User;
	this->p->Account->GetLoginData(this->id);

	// Set state to Chat (reset later?)
	this->State = State_Chat;

	cout << "LoggedIn::" << User << "\n";

	// HACK: Increment playerType
	if (this->playerType == 1) {
		this->playerType = 2;
	}
	else {
		this->playerType = 1;
	}

	// Tell everyone you logged in
	memset(&TempString, 0, sizeof(TempString));
	TempString[0] = (unsigned char)id;
	TempString[1] = (unsigned char)playerType;
	this->p->Winsock->SendData(id, 1, TempString, 2); //Log in succesful

	// Send everyone your player data
	strcpy(player.Name, this->Name.c_str());
	strcpy(player.Town, this->Town.c_str());
	player.Index = this->id;
	player.playerType = this->playerType;
	player.Red = this->Red;
	player.Green = this->Green;
	player.Blue = this->Blue;
	player.Member = this->Member;
	player.Tank = this->displayTank;
	this->p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
	this->p->Send->SendCurrentPlayers(this->id);

	// Send everyone your stats
	this->p->Account->GetStats(this->id);
	pts.Index = this->id;
	pts.Points = this->p->Player[this->id]->Points;
	pts.Deaths = this->p->Player[this->id]->Deaths;
	pts.Assists = this->p->Player[this->id]->Assists;
	pts.Orbs = this->p->Player[this->id]->Orbs;
	pts.MonthlyPoints = this->p->Player[this->id]->MonthlyPoints;
	this->p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));

	// Set state to Verified (set above?)
	State = State_Verified;
	this->p->Log->logAccount("Login ::  " + User + " :: " + this->IPAddress + " :: " + this->UniqID);
}

/***************************************************************
 * Function:	setCloak
 *
 * @param isCloaked
 **************************************************************/
void CPlayer::setCloak(bool isCloaked) {
	if( isCloaked ) {
		this->isCloaked = true;
		this->timeUncloak = this->p->Tick + TIMER_CLOAK;
	}
	else {
		this->isCloaked = false;
		this->timeUncloak = 0;
	}
}