/*
===============================================================================

    Battle City - CPlayer file
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
#include "CPlayer.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CPlayer::CPlayer(CServer *Server, int id) {
	this->p = Server;

	// Reset the player stats
	this->ResetPlayer();

	this->id = id;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CPlayer::~CPlayer() {
}

/***************************************************************
 * Function:	setMayor
 *
 * @param set
 **************************************************************/
void CPlayer::setMayor(bool isMayor) {
	this->setMayor(isMayor, true);
}
/***************************************************************
 * Function:	setMayor
 *
 * @param set
 * @param requireInGame
 **************************************************************/
void CPlayer::setMayor(bool isMayor, bool requireInGame) {
	sSMFinance finance;
	sSMMayorUpdate Mayorupdate;
	CCity* city;
	
	// Store whether the player is mayor or not
	this->Mayor = isMayor;

	// If requireInGame is false, or player is in game, 
	if ((!requireInGame) || this->isInGame()) {
		city = this->p->City[this->City];
	
		// If the player is being set mayor
		if (isMayor) {

			// Store this player as mayor of this city,
			city->Mayor = this->id;

			// If the player is hiring, send out MayorChanged messages
			if (city->hiring > -1) {
				this->p->Winsock->SendData(city->hiring,smMayorChanged," ");
			}

			// For each item,
			for (int j = 0; j <= 26; j++) {

				// Set the build tree for this city
				city->setCanBuild(j,city->canBuild[j], false);
			}

			// Request the city's finance report
			finance.Cash = city->cash;
			finance.Income = city->income;
			finance.Items = city->itemproduction;
			finance.Hospital = city->hospital;
			finance.Research = city->cashresearch;
			p->Winsock->SendData(id,smFinance,(char *)&finance,sizeof(finance));

			// Set isMayor = 1 on the packet
			Mayorupdate.IsMayor = 1;
		}

		// Else (player is being set to not-mayor),
		else {

			// If the city thinks the player is now its mayor, un-mayor it
			if (city->Mayor == this->id) {
				city->Mayor = -1;
			}


			// Set isMayor = 0 on the packet
			Mayorupdate.IsMayor = 0;
		}

		// Reset the mayor options
		city->Successor = -1;
		city->notHiring = 0;
		city->hiring = -1;

		// Tell everyone the mayor changed
		Mayorupdate.Index = id;
		p->Send->SendGameAllBut(-1, smMayorUpdate, (char *)&Mayorupdate, sizeof(Mayorupdate));
	}
}

/***************************************************************
 * Function:	Clear
 *
 **************************************************************/
void CPlayer::Clear() {
	this->Clear(true);
}
/***************************************************************
 * Function:	Clear
 *
 **************************************************************/
void CPlayer::Clear(bool triggerServerReset) {
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

	// If triggerServerReset, tell the server to try to reset
	if (triggerServerReset) {
		this->p->reset();
	}
}

/***************************************************************
 * Function:	ResetPlayer
 *
 **************************************************************/
void CPlayer::ResetPlayer() {

	// Reset the player stats
	this->City = -1;

	this->X = 0;
	this->Y = 0;
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
	this->RentalCity = -1;

	//this->id = 0;
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
	return 
		(this->State == State_Game)
		&&
		(CCity::isValidCityIndex(this->City));
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
	CCity* city = this->p->City[this->City];

	stategame.City = this->City;
	stategame.x = city->x;
	stategame.y = city->y;
	
	// Set the player on the CC
	this->X = stategame.x;
	this->Y = stategame.y;

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
	this->lastMove = this->p->Tick;
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
	char packet[2];
	bool foundSuccessor = false;
	CCity* city = this->p->City[this->City];
	CPlayer* playerSuccessor;

	// Delete the items the player is holding
	this->p->Item->deleteItemsByPlayer(this->id);

	// If transferMayor is true,
	// Handle transfering mayor to another player in the city.
	// This is optional (doesn't happen when orbed) to prevent conflicts that migth cause the Balkh bug
	if (transferMayor) {

		// If the player is mayor,
		if (this->Mayor && (city->Mayor==this->id)) {

			// If there is a successor,
			if (city->Successor > -1) {
				playerSuccessor = this->p->Player[city->Successor];

				// If that successor is in game, in this city, and not the mayor
				if (	
					(playerSuccessor->isInGame())
					&&
					(playerSuccessor->City == this->City)
					&&
					(playerSuccessor->id != this->id)
					) {

					// Set the successor to mayor
					playerSuccessor->setMayor(true);
					foundSuccessor = true;
				}
			}

			// If we failed to find a successor,
			if (! foundSuccessor) {

				// For each possible player,
				for (int j = 0; j < MAX_PLAYERS; j++) {
					playerSuccessor = this->p->Player[j];

					// If that player is in game, in this city, and not the mayor
					if (	
						(playerSuccessor->isInGame())
						&&
						(playerSuccessor->City == this->City)
						&&
						(playerSuccessor->id != this->id)
						) {

						// Set the successor to mayor
						playerSuccessor->setMayor(true);
						foundSuccessor = true;
						break;
					}
				}
			}

			// If we still didn't find a successor,
			if (! foundSuccessor) {

				// If the city is not orbable, destroy it
				if (city->isOrbable()==false) {
					city->destroy();
				}

				// Else (orbable), start the Destruct timer
				else {
					city->DestructTimer = this->p->Tick + TIMER_CITY_DESTRUCT;
					city->Mayor = -1;
					city->notHiring = 0;

					// Announce to everyone that the server is going to self destruct in TIMER_CITY_DESTRUCT seconds
					// TODO: implement
				}
			}
		}
	}

	// If the player is in game,
	if (this->isInGame()) {

		// Save the player's stats
		cout << "Left::" << Name << "::Saving...";
		this->p->Account->SaveStats(this->id);
		cout << " Done." << endl;

		// If showMessage, tell everyone you left
		if (showMessage) {
			packet[0] = (char)this->id;
			packet[1] = 69;
			this->p->Send->SendGameAllBut(this->id, smChatCommand, packet, 2); 
		}
	}

	// Reset this player
	this->State = State_Chat;
	this->X = 0;
	this->Y = 0;
	this->City = -1;
	this->isDead = false;
	this->timeDeath = 0;
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
	pts.Points = this->Points;
	pts.Deaths = this->Deaths;
	pts.Assists = this->Assists;
	pts.Orbs = this->Orbs;
	pts.MonthlyPoints = this->MonthlyPoints;
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

/***************************************************************
 * Function:	getTileX
 *
 **************************************************************/
int CPlayer::getTileX() {
	return (int) ((this->X+24) / 48);
}

/***************************************************************
 * Function:	getTileY
 *
 **************************************************************/
int CPlayer::getTileY() {
	return (int) ((this->Y+24) / 48);
}
