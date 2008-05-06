#include "CProcess.h"
#include "NetMessages.h"

void *ClassPointer;

/***************************************************************
 * Constructor
 *
 * @param game
 **************************************************************/
CProcess::CProcess(CGame *game) {
	this->p = game;
	ClassPointer = this;
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
 * @param TheData
 **************************************************************/
int CProcess::ProcessData(char *TheData) {

	// Switch on the type of packet
	switch((unsigned char)TheData[0]) {

		// Packet: smLoginCorrect
		case smLoginCorrect:
			this->ProcessLogin((unsigned char)TheData[1], (unsigned char)TheData[2]);
			break;

		// Packet: smAppendNews
		case smAppendNews:
			if (this->p->State == STATE_GAME)
				this->p->Admin->SetNews(&TheData[1]);
			else
				this->p->Meeting->AppendNews(&TheData[1]);
			break;

		// Packet: smChatMessage
		case smChatMessage:
			this->ProcessChatMessage(TheData, 0);
			break;

		// Packet: smChatCommand
		case smChatCommand:
			this->ProcessChatCommand(TheData[1], TheData[2]);
			break;
		
		// Packet: smEditAccount
		case smEditAccount:
			this->ProcessEditAccount(&TheData[1]);
			break;

		// Packet: smPlayerData
		case smPlayerData:
			this->ProcessPlayerData((sSMPlayer *)&TheData[1]);
			break;

		// Packet: smMayorHire
		case smMayorHire:
			this->ProcessMayorHire(TheData[1]);
			break;

		// Packet: smAddRemCity
		case smAddRemCity:
			this->p->Meeting->AddCity((unsigned char)TheData[1],(unsigned char)TheData[2], (unsigned char)TheData[3]);
			break;

		// Packet: smStateGame
		case smStateGame:
			this->ProcessEnterGame((sSMStateGame *)&TheData[1]);
			break;

		// Packet: smJoinData
		case smJoinData:
			this->ProcessJoinData((sSMJoinData *)&TheData[1]);
			break;

		// Packet: smError
		case smError:
			this->ProcessError(TheData[1]);
			break;

		// Packet: smNewBuilding
		case smNewBuilding:
			this->ProcessNewBuilding((sSMBuild *)&TheData[1],1);
			break;

		// Packet: smPlaceBuilding
		case smPlaceBuilding:
			this->ProcessNewBuilding((sSMBuild *)&TheData[1],0);
			break;

		// Packet: smUpdate
		case smUpdate:	
			this->ProcessUpdate((sSMUpdate *)&TheData[1]);
			break;

		// Packet: smCanBuild
		case smCanBuild:
			this->ProcessCanBuild(TheData);
			break;

		// Packet: smFinance
		case smFinance:
			this->ProcessFinance((sSMFinance *)&TheData[1]);
			break;

		// Packet: smInterview
		case smInterview:
			this->ProcessInterview();
			break;

		// Packet: smComms
		case smComms:
			this->ProcessComms(TheData);
			break;

		// Packet: smMayorDeclined
		case smMayorDeclined:
			this->ProcessRejected();
			break;

		// Packet: smMayorInInterview
		case smMayorInInterview:
			this->ProcessMayorInInterview();
			break;

		// Packet: smInterviewCancel
		case smInterviewCancel:
			this->ProcessInterviewCancel();
			break;

		// Packet: smAddItem
		case smAddItem:
			this->ProcessAddItem((sSMItem *) &TheData[1]);
			break;

		// Packet: smRemItem
		case smRemItem:
			this->ProcessRemoveItem((sSMItem *) &TheData[1]);
			break;

		// Packet: smItemCount
		case smItemCount:
			this->ProcessItemCount((sSMItemCount *)&TheData[1]);
			break;

		// Packet: smPickedUp
		case smPickedUp:
			this->ProcessPickUp((sCMItem *) &TheData[1]);
			break;

		// Packet: smDropped
		case smDropped:
			this->ProcessDropped((sCMItem *) &TheData[1]);
			break;

		// Packet: smUpdatePop
		case smUpdatePop:
			this->ProcessPop((sSMPop *)&TheData[1]);
			break;

		// Packet: smDeath
		case smDeath:
			this->ProcessDeath(TheData[1], TheData[2], TheData[3]);
			break;

		// Packet: smDestroyCity
		case smDestroyCity:
			this->ProcessDestroyCity((unsigned char)TheData[1]);
			break;

		// Packet: smRemBuilding
		case smRemBuilding:
			this->ProcessRemoveBuilding((sSMBuild *)&TheData[1]);
			break;

		// Packet: smUnderAttack
		case smUnderAttack:
			this->ProcessUnderAttack();
			break;

		// Packet: smOrbed
		case smOrbed:
			this->ProcessOrbed((sSMOrbedCity *)&TheData[1]);
			break;

		// Packet: smGlobal
		case smGlobal:
			this->ProcessGlobal(TheData);
			break;

		// Packet: smShoot
		case smShoot:
			this->ProcessShot((sSMShot *)&TheData[1]);
			break;

		// Packet: smTCPPong
		case smTCPPong:
			this->ProcessPong();
			break;

		// Packet: smPointsUpdate
		case smPointsUpdate:
			this->ProcessPointsUpdate((sSMPoints *)&TheData[1]);
			break;

		// Packet: smMedKit
		case smMedKit:
			this->ProcessMedKit();
			break;

		// Packet: smCloak
		case smCloak:
			this->ProcessCloak(TheData[1]);
			break;

		// Packet: smExplosion
		case smExplosion:
			this->ProcessExplosion((sSMExplode *)&TheData[1]);
			break;

		// Packet: smWarp
		case smWarp:
			this->ProcessWarp((sSMStateGame *)&TheData[1]);
			break;

		// Packet: smNextStep
		case smNextStep:
			if (TheData[1] == 65) {
				this->p->Dialog->StartDialog = 0;
				if (this->p->Meeting->hWnd) {
					EndDialog(this->p->Meeting->hWnd, 1);
				}
				if (this->p->Interview->hWnd) {
					EndDialog(this->p->Interview->hWnd, 1);
				}
				this->p->State = STATE_JOINING;
			}
			if (TheData[1] == 66) {
				this->p->InGame->ClearAllSectors();
				this->p->InGame->RefreshArea();
			}
			if (TheData[1] == 67) {
				this->p->State = STATE_GAME;
			}
			this->p->Winsock->SendData(cmNextStep, &TheData[1], 1);
			break;
		
		// Packet: smMayorUpdate
		case smMayorUpdate:
			this->ProcessMayorUpdate((sSMMayorUpdate *)&TheData[1]);
			break;

		// Packet: smItemLife
		case smItemLife:
			this->ProcessItemLife((sSMItemLife *)&TheData[1]);
			break;

		// Packet: smFired
		case smFired:
			this->ProcessFired();
			break;

		// Packet: smAdmin
		case smAdmin:
			this->ProcessAdmin((sSMAdmin *)&TheData[1]);
			break;

		// Packet: smKicked
		case smKicked:
			this->ProcessKicked();
			break;

		// Packet: smSectorSent
		case smSectorSent:
			this->ProcessSectorSent((sCMSector *)&TheData[1]);
			break;

		// Packet: smInfoButton
		case smInfoButton:
			this->ProcessInfoButton((sSMInfoButton *)&TheData[1]);
			break;

		// Packet: smClearPlayer
		case smClearPlayer:
			this->ProcessClearPlayer(TheData[1]);
			break;

		// Packet: smMayorChanged
		case smMayorChanged:
			this->ProcessMayorChanged();
			break;

		// Packet: smRightClickCity
		case smRightClickCity:
			this->ProcessRightClickCity((sSMRightClickCity *)&TheData[1]);
			break;

		// Packet: smBan
		case smBan:
			this->p->Admin->AddBan((sSMBan *)&TheData[1]);
			break;

		// Packet: smRespawn
		case smRespawn:
			this->ProcessRespawn(TheData[1]);
			break;

		// Packet: smAdminEdit
		case smAdminEdit:
			this->ProcessAdminEdit((sCMAdminEdit *)&TheData[1]);
			break;

		// Packet: smClickPlayer
		case smClickPlayer:
			this->ProcessClickPlayer((sSMClickPlayer *)&TheData[1]);
			break;

		// Packet: smWhisper
		case smWhisper:
			this->ProcessWhisper(TheData);
			break;

		// Packet: smAutoBuild
		case smAutoBuild:
			this->ProcessAutoBuild((sSMAutoBuild*)&TheData[1]);
			break;

		// Packet: smNowOrbable
		case smNowOrbable:
			this->ProcessNowOrbable();
			break;

		// Packet: unknown!
		default:
			std::ostringstream thing;
			int i = TheData[0];
			thing << i;
			this->p->Engine->logerror("Unhandled Data Packet " + thing.str());
			return 1;
			break;
	}

	return 0;
}

/***************************************************************
 * Function:	ProcessLogin
 *
 * @param Index
 * @param playerType
 **************************************************************/
void CProcess::ProcessLogin(unsigned char Index, unsigned char playerType) {
	CPlayer* player = this->p->Player[Index];

	this->p->Winsock->MyIndex = Index;

	player->ClearPlayer();
	player->playerType = playerType;
	player->Name.clear();
	player->Name = this->p->Login->user;
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessClearPlayer
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessClearPlayer(unsigned char Index) {
	this->p->Player[Index]->ClearPlayer();
	if (this->p->Admin->IsOpen) {
		this->p->Admin->DrawPlayerList();
	}
	this->p->InGame->PrintWhoData();
}

/***************************************************************
 * Function:	ProcessError
 *
 * @param error
 **************************************************************/
void CProcess::ProcessError(int error) {
	
	switch(error) {
		case 65: //Message A
			{MessageBox(this->p->hWnd, "Account created succesfully!", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 66: //Message B
			{MessageBox(this->p->hWnd, "Incorrect Password", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 67: //Message C
			{MessageBox(this->p->hWnd, "Account does not exist", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 68: //Message D
			{MessageBox(this->p->hWnd, "User name is already in use!", "BattleCity", 0); this->p->NewAccount->ShowNewDlg(); break;}
		case 69: //Message E
			{MessageBox(this->p->hWnd, "Account is currently in use", "BattleCity", 0); this->p->Login->ShowLoginDlg(); break;}
		case 70: //Message F
			{MessageBox(this->p->hWnd, "Please download the latest version of BattleCity at http://battlecity.looble.com", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 71: //Message G
			{MessageBox(this->p->hWnd, "Banned", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 72: //Message H
			{MessageBox(this->p->hWnd, "Only one client per computer", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 74: //Message J
			{MessageBox(this->p->hWnd, "Invalid verification code.", 0, 0); SendMessage(this->p->hWnd, WM_CLOSE, 0, 0); break;}
		case 75: //Message K
			if (this->p->NewAccount->Editing == 1)
				{MessageBox(this->p->hWnd, "Email is already in use!", "BattleCity", 0); this->p->NewAccount->ShowEditDlg();}
				else {MessageBox(this->p->hWnd, "Email is already in use!", "BattleCity", 0); this->p->NewAccount->ShowNewDlg();}
			break;
		case 76: //Message L
			{MessageBox(this->p->hWnd, "Email address not found.  Please try again!", 0, 0); this->p->Recover->ShowRecoverDlg(); break;}
		case 77: //Message M
			{MessageBox(this->p->hWnd, "Your account information has been sent to your email address!", 0, 0); this->p->Login->ShowLoginDlg(); break;}
	}
}

/***************************************************************
 * Function:	ProcessEvent
 *
 * @param Event
 **************************************************************/
void CProcess::ProcessEvent(int Event) {

	switch (Event) {
		case 1: //Error connecting
			this->p->running = 0;
			Sleep(100);
			MessageBox(this->p->hWnd, "Unable to connect to the BattleCity server.  Make sure you are properly connected to the internet, and if you are, the server is likely down for maintenance or being updated.  Please try again later.", "BattleCity", 0);
			SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
			break;
		case 2: //Connected
			this->p->Login->ShowLoginDlg();
			this->p->Send->SendVersion();
			break;
		case 3: //Disconnected
			if (this->p->running == 0) return;
			this->p->running = 0;
			Sleep(100);
 			MessageBox(this->p->hWnd, "You have been disconnected from the BattleCity server.  Please try again later.", "BattleCity", 0);
			SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
			break;
	}
}

/***************************************************************
 * Function:	ProcessChatMessage
 *
 * @param TheData
 * @param global
 **************************************************************/
void CProcess::ProcessChatMessage(char *TheData, char global) {
	string tmpstring;
	int Index = (unsigned char)TheData[1];
	tmpstring = this->p->Player[Index]->Name + ": " + &TheData[2];
	switch (this->p->State) {
		case STATE_GAME:
			if (this->p->Player[Index]->isAdmin()) {
				this->p->InGame->AppendChat(tmpstring, COLOR_ADMIN_CHAT);
			}
			else {

				if (this->p->Player[Index]->isDead == true) {
					this->p->InGame->AppendChat(tmpstring, COLOR_DEAD_CHAT);
				}
				else {
					if (this->p->Player[Index]->City == this->p->Player[this->p->Winsock->MyIndex]->City) {
						this->p->InGame->AppendChat(tmpstring, COLOR_TEAM_CHAT);
					}
					else {
						this->p->InGame->AppendChat(tmpstring, COLOR_ENEMY_CHAT);
					}
				}
			}
			break;
		case STATE_MEETING:
			this->p->Meeting->AppendData(tmpstring);
			break;
		default:

			break;
	}
}

/***************************************************************
 * Function:	ProcessChatCommand
 *
 * @param Index
 * @param message
 **************************************************************/
void CProcess::ProcessChatCommand(int Index, int message) {
	string tmpString;
	if (this->p->Player[Index]->Name.length() > 0) {
		tmpString = this->p->Player[Index]->Name;
		switch(message) {
			case 65: //Message A
				this->p->Meeting->AddPlayer(Index);
				this->p->InGame->PrintWhoData();
				break;
			case 66: //Message B
			case 67: //Message C 
			case 68: //Message D
				this->p->Meeting->ClearPlayer(Index);
				this->p->InGame->PrintWhoData();
				break;
			case 69: //Message E
				tmpString.append(" has left");
				this->p->Player[Index]->InGameClear();
				this->p->InGame->PrintWhoData();
				if (this->p->Admin->IsOpen) {
					this->p->Admin->DrawPlayerList();
				}
				break;
		}
		switch(this->p->State) {
			case STATE_GAME:
				this->p->InGame->AppendChat(&tmpString[0], COLOR_SYSTEM);
				break;
			default:
				break;
		}
	}
}

/***************************************************************
 * Function:	ProcessEnterGame
 *
 * @param game
 **************************************************************/
void CProcess::ProcessEnterGame(sSMStateGame *game) {
	int me = this->p->Winsock->MyIndex;
	string newbieTip;

	this->p->Draw->resetPlayerOffset();
	this->p->Player[me]->X = (float)game->x;
	this->p->Player[me]->Y = (float)game->y;
	this->p->Player[me]->SectorX = (game->x/48) / SectorSize;
	this->p->Player[me]->SectorY = (game->y/48) / SectorSize;
	this->p->Player[me]->CityX = game->x;
	this->p->Player[me]->CityY = game->y;
	this->p->Player[me]->City = game->City;
	this->p->Player[me]->SetHP(MAX_HEALTH);
	this->p->Player[me]->isInGame = 1;
	this->p->Player[me]->GenerateNameString();

	this->p->InGame->AppendInfo("Press HELP for instructions, or visit: http://battlecity.looble.com/");

	// If newbie tips are on,
	if (this->p->Options->newbietips) {

		newbieTip = "Newbie Tip: You joined ";
		newbieTip += CityList[this->p->Player[this->p->Winsock->MyIndex]->City];

		if (this->p->Player[me]->isMayor) {
			newbieTip += " as the Mayor.  Right-click anywhere on the screen to start building!";
		}
		else {
			newbieTip += " as a Commando.  Press Shift to shoot!";
		}

		this->p->InGame->AppendInfo(newbieTip);
	}

	this->p->InGame->PrintWhoData();
	this->p->InGame->AppendChat(this->p->Player[me]->Name + " has just joined!", COLOR_SYSTEM);

	this->p->Dialog->StartDialog = 0;
	
	if (this->p->Meeting->hWnd) {
		EndDialog(this->p->Meeting->hWnd, 1);
	}
	if (this->p->Interview->hWnd) {
		EndDialog(this->p->Interview->hWnd, 1);
	}
}

/***************************************************************
 * Function:	ProcessNewBuilding
 *
 * @param data
 * @param sound
 **************************************************************/
void CProcess::ProcessNewBuilding(sSMBuild *data, int sound) {
	this->p->Build->newBuilding(data->x, data->y, data->City, data->type, data->id);
	CBuilding *bld = this->p->Build->findBuilding(data->id);
	if (bld) {
		bld->pop = data->pop;
	}

	if (sound && this->p->State == STATE_GAME) {
		int me = this->p->Winsock->MyIndex;
		if (data->x * 48 < this->p->Player[me]->X + 1000 &&
			data->x * 48 + 1000 > this->p->Player[me]->X &&
			data->y * 48 < this->p->Player[me]->Y + 1000 &&
			data->y * 48 + 1000 > this->p->Player[me]->Y) {
				this->p->Sound->Play3dSound(this->p->Sound->s_build, 100, (float)data->x*48, (float)data->y*48);
		}
	}
}

/***************************************************************
 * Function:	ProcessUpdate
 *
 * @param data
 **************************************************************/
void CProcess::ProcessUpdate(sSMUpdate *data) {
	CPlayer* player = this->p->Player[data->id];

	player->isDead = false;
	player->Y = data->y;
	player->X = data->x;
	player->isMoving = data->move - 1;
	player->isTurning = data->turn - 1;
	player->Direction = data->dir;
	player->lastUpdate = this->p->Tick;
}

/***************************************************************
 * Function:	ProcessPlayerData
 *
 * @param player
 **************************************************************/
void CProcess::ProcessPlayerData(sSMPlayer *playerPacket) {
	CPlayer* player = this->p->Player[playerPacket->Index];

	player->playerType = playerPacket->playerType;
	player->Red = playerPacket->Red;
	player->Green = playerPacket->Green;
	player->Blue = playerPacket->Blue;
	player->Member = playerPacket->Member;
	player->Tank = playerPacket->Tank;

	if (playerPacket->Red == 255 && playerPacket->Green == 255 && playerPacket->Blue == 255) {
		player->RainbowName = true;
	}
	
	player->Name = playerPacket->Name;
	player->Town = playerPacket->Town;
	player->GenerateNameString();
}

/***************************************************************
 * Function:	ProcessCanBuild
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessCanBuild(char TheData[255]) {
	if (TheData[2]) {
		this->p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 1;
	}
	else {
		this->p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 0;
	}
}

/***************************************************************
 * Function:	ProcessComms
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessComms(char TheData[255]) {
	string tmpString;
	if (this->p->Player[(unsigned char)TheData[1]]->Name.length() > 0) {
		switch (TheData[2]) {
			case 124: // |
				tmpString = "The mayor has arrived - Start talking.";
				break;
			default:
				tmpString = this->p->Player[(unsigned char)TheData[1]]->Name + ": ";
				tmpString.append(&TheData[2]);
				break;
		}
		if (this->p->State == STATE_GAME) {
			this->p->Personnel->AppendData(tmpString);
		}
		else  {
			this->p->Interview->AppendData(tmpString);
		}
	}
}

/***************************************************************
 * Function:	ProcessRejected
 *
 **************************************************************/
void CProcess::ProcessRejected() {
	MessageBox(this->p->hWnd, "The mayor did not hire you.  Try a different city.", "BattleCity", 0);
	this->p->Interview->Chat.clear();
	this->p->Meeting->ShowMeetingDlg();
	EndDialog(this->p->Interview->hWnd, 1);
}

/***************************************************************
 * Function:	ProcessInterview
 *
 **************************************************************/
void CProcess::ProcessInterview() {
	this->p->Interview->Chat.clear();
	this->p->Interview->ShowInterviewDlg();
	this->p->Interview->AppendData("Please wait here while I notify the Mayor of your arrival");
}

/***************************************************************
 * Function:	ProcessMayorHire
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessMayorHire(int Index) {
	string tmpString;
	tmpString.clear();

	if (this->p->Personnel->DenyApplicants == 0)  {

		if (this->p->Player[Index]->Name.length() > NULL) {
			tmpString = this->p->InGame->ReturnRank(this->p->Player[Index]->Points) + " ";
			tmpString += this->p->Player[Index]->Name;
			tmpString += " of ";
			tmpString += this->p->Player[Index]->Town;
			tmpString += + " is looking for a job";
			this->p->Personnel->ApplicantWaiting = 1;
			this->p->InGame->AppendInfo("Note:  Another player is applying for a position in your city.");
			this->p->Personnel->AppendData(tmpString);
			this->p->Sound->PlayWav(sBuzz,-1);
		}
	}

	else {
		char packet[2];
		packet[0] = (char)this->p->Personnel->DenyApplicants;
		packet[1] = 0;
		this->p->Winsock->SendData(cmIsHiring, packet, 1);
		this->p->Winsock->SendData(cmHireDecline, " ", 1);
	}
}

/***************************************************************
 * Function:	ProcessAddItem
 *
 * @param item
 **************************************************************/
void CProcess::ProcessAddItem(sSMItem *item) {
	this->p->Item->newItem(item->x, item->y, item->type, item->City, item->id, item->active);
} 

/***************************************************************
 * Function:	ProcessRemoveItem
 *
 * @param item
 **************************************************************/
void CProcess::ProcessRemoveItem(sSMItem *item) {
	CItem *item2;
	item2 = this->p->Item->findItem(item->id);
	
	if (item2) {
		this->p->Item->delItem(this->p->Item->findItem(item->id));
	}
	else {
		item2 = this->p->Inventory->findItem(item->id);
		if (item2) {
			this->p->Inventory->delItem(item2);
			this->p->Inventory->ItemCheck();
		}
	}
}

/***************************************************************
 * Function:	ProcessItemCount
 *
 * @param count
 **************************************************************/
void CProcess::ProcessItemCount(sSMItemCount *count) {
	CBuilding *bld = this->p->Build->findBuilding(count->id);

	if (bld) bld->ItemsLeft = count->count;
	return;
}

/***************************************************************
 * Function:	ProcessJoinData
 *
 * @param join
 **************************************************************/
void CProcess::ProcessJoinData(sSMJoinData *join) {
	string tmpString;
	CPlayer* player = this->p->Player[join->id];

	player->City = join->City;
	player->isInGame = 1;
	player->X = 0;
	player->Y = 0;
	player->setMayor(join->Mayor != 0);
	player->GenerateNameString();

	if (player->Name.length() > 0 && this->p->State == STATE_GAME) {
		tmpString = player->Name + " has just joined";
		this->p->InGame->AppendChat(tmpString, COLOR_SYSTEM);
	}

	if (this->p->Admin->IsOpen) {
		this->p->Admin->DrawPlayerList();
	}

	this->p->InGame->PrintWhoData();
}

/***************************************************************
 * Function:	ProcessPickUp
 *
 * @param item
 **************************************************************/
void CProcess::ProcessPickUp(sCMItem *item) {
	CItem *item2;
	item2 = this->p->Item->findItem(item->id);

	if (item2) {
		CItem *itm = this->p->Inventory->newItem(item2->X, item2->Y, item2->Type, item2->City, item->id, item->active);
		this->p->Inventory->ItemCheck();
		this->p->Inventory->SelectedItemType = itm->Type;

		if (this->p->Options->newbietips) {
			switch (item2->Type) {

				case ITEM_TYPE_CLOAK:
					this->p->InGame->AppendInfo(NEWBIE_TIP_CLOAK);
					break;

				case ITEM_TYPE_ROCKET:
					this->p->InGame->AppendInfo(NEWBIE_TIP_ROCKET);
					break;

				case ITEM_TYPE_MEDKIT:
					this->p->InGame->AppendInfo(NEWBIE_TIP_MEDKIT);
					break;

				case ITEM_TYPE_BOMB:
					this->p->InGame->AppendInfo(NEWBIE_TIP_BOMB);
					break;

				case ITEM_TYPE_MINE:
					this->p->InGame->AppendInfo(NEWBIE_TIP_MINE);
					break;

				case ITEM_TYPE_ORB:
					this->p->InGame->AppendInfo(NEWBIE_TIP_ORB);
					break;

				case ITEM_TYPE_WALKIE:
					this->p->InGame->AppendInfo(NEWBIE_TIP_WALKIE);
					break;

				case ITEM_TYPE_DFG:
					this->p->InGame->AppendInfo(NEWBIE_TIP_DFG);
					break;

				case ITEM_TYPE_WALL:
					this->p->InGame->AppendInfo(NEWBIE_TIP_WALL);
					break;

				case ITEM_TYPE_TURRET:
					this->p->InGame->AppendInfo(NEWBIE_TIP_TURRET);
					break;

				case ITEM_TYPE_SLEEPER:
					this->p->InGame->AppendInfo(NEWBIE_TIP_SLEEPER);
					break;

				case ITEM_TYPE_PLASMA:
					this->p->InGame->AppendInfo(NEWBIE_TIP_PLASMA);
					break;
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessDropped
 *
 * @param item
 **************************************************************/
void CProcess::ProcessDropped(sCMItem *item) {
	CItem *item2;
	item2 = this->p->Inventory->findItem(item->id);
	
	if (item2) {
		this->p->Inventory->delItem(item2);
		this->p->Inventory->ItemCheck();
	}
}

/***************************************************************
 * Function:	ProcessPop
 *
 * @param pop
 **************************************************************/
void CProcess::ProcessPop(sSMPop *pop) {
	CBuilding *bld = this->p->Build->findBuilding(pop->id);
	if (bld) {
		bld->pop = pop->pop;
	}
}

/***************************************************************
 * Function:	ProcessDeath
 *
 * @param Index
 * @param deathType
 * @param City
 **************************************************************/
void CProcess::ProcessDeath(int Index, char deathType, char City) {
	string tmpString;
    srand((unsigned int)this->p->Tick);
    int random_integer = rand()%13;
	int me = this->p->Winsock->MyIndex;
	CPlayer* player = this->p->Player[Index];
	CPlayer* playerMe = this->p->Player[me];
	string cityName = CityList[City];

	player->isDead = true;
	player->isMoving = false;
	player->isTurning = false;

	// If this player is not the player who died,
	if (Index != me) {

		// If the dead player is in sound range,
		if ((abs(player->X - playerMe->X) < 1000) && (abs(player->Y - playerMe->Y) < 1000)) {

			// Add an explosion and play explosion and death sounds
			this->p->Explode->newExplosion((int)player->X, (int)player->Y, 1);
			this->p->Sound->Play3dSound(this->p->Sound->s_eXplode, 100, player->X, player->Y);
			this->p->Sound->Play3dSound(this->p->Sound->s_die, 100, player->X, player->Y);
		}
	}

	// Else (I died), stop tank sound
	else {
		this->p->Sound->StopWav(12);
	}

	// Choose a random death message
	switch (random_integer) {
		case 0:
			tmpString = player->Name + " no longer exists as a single entity";
			break;
		case 1:
			tmpString = player->Name + " just had a TNT experience";
			break;
		case 2:
			tmpString = player->Name + " has shattered into many pieces";
			break;
		case 3:
			tmpString = player->Name + " has been blown up";
			break;
		case 4:
			tmpString = player->Name + " is no more";
			break;
		case 5:
			tmpString = player->Name + " is worm food";
			break;
		case 6:
			tmpString = player->Name + " just ran out of luck";
			break;
		case 7:
			tmpString = "Alas poor " + player->Name + " we knew him well";
			break;
		case 8:
			tmpString = "A funeral service will shortly be held for " + player->Name;
			break;
		case 9:
			tmpString = player->Name + " has bitten the dust";
			break;
		case 10:
			tmpString = player->Name + " has gone to meet his maker";
			break;
		case 11:
			tmpString = player->Name + " is dead meat";
			break;
		case 12:
			tmpString = player->Name + " was useless at this game anyway!";
			break;
	}

	// If the player was killed by his own city, show "(Friendly Fire!)"
	if (player->City == City) {
		tmpString += " (Friendly Fire!)";
	}

	// Else, show "(" + cityName + ")"
	else {
		tmpString += " (" + cityName + ")";
	}

	// Add the death message to the chat
	this->p->InGame->AppendChat(tmpString, COLOR_SYSTEM);
}

/***************************************************************
 * Function:	ProcessShot
 *
 * @param shotsfired
 **************************************************************/
void CProcess::ProcessShot(sSMShot *shotsfired) {
	int me = this->p->Winsock->MyIndex;
	CPlayer* playerShooter = this->p->Player[shotsfired->id];
	CPlayer* playerMe = this->p->Player[me];

	float fDir;
	int FlashY;
	int FlashX; 

	if (playerShooter->isAdmin()) {
		fDir = (float)-playerShooter->Direction+32;
		FlashY = (int)playerShooter->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		FlashX = (int)playerShooter->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		this->p->Explode->newExplosion(FlashX, FlashY, 3);
		this->p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(playerShooter->X - this->p->Player[me]->X) < 1000) && (abs(playerShooter->Y - playerMe->Y) < 1000)) {
			this->p->Sound->Play3dSound(this->p->Sound->s_turret, 100, playerShooter->X, playerShooter->Y);
		}
	}

	else {
		fDir = (float)-playerShooter->Direction+32;
		FlashY = (int)playerShooter->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		FlashX = (int)playerShooter->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		this->p->Explode->newExplosion(FlashX, FlashY, 3);
		this->p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(playerShooter->X - playerMe->X) < 1000) && (abs(playerShooter->Y - playerMe->Y) < 1000)) {

			switch(shotsfired->type) {

				case 0:
					this->p->Sound->Play3dSound(this->p->Sound->s_tanklaser, 100, playerShooter->X, this->p->Player[shotsfired->id]->Y);
					break;
				case 1:
					this->p->Sound->Play3dSound(this->p->Sound->s_bigturret, 100, this->p->Player[shotsfired->id]->X, this->p->Player[shotsfired->id]->Y);
					break;
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessDestroyCity
 *
 * @param City
 **************************************************************/
void CProcess::ProcessDestroyCity(char City) {
	this->p->Item->deleteItemsByCity(City);
	this->p->Build->deleteBuildingsByCity(City);
}

/***************************************************************
 * Function:	ProcessRemoveBuilding
 *
 * @param build
 **************************************************************/
void CProcess::ProcessRemoveBuilding(sSMBuild *build) {
	CBuilding *bld = this->p->Build->findBuilding(build->id);
	CPlayer* playerMe = this->p->Player[this->p->Winsock->MyIndex];

	// If the building was found,
	if (bld) {

		// ???
		if (build->x == 1) {
			this->p->Explode->newExplosion(bld->X * 48, bld->Y * 48, 2);
		}

		// If the building is within sound range,
		if (bld->X * 48 < playerMe->X + 400 &&
			bld->X * 48 + 400 > playerMe->X &&
			bld->Y * 48 < playerMe->Y + 400 &&
			bld->Y * 48 + 400 > playerMe->Y) {

				// Play the Demolish sound
				this->p->Sound->Play3dSound(this->p->Sound->s_demolish, 100, (float)bld->X*48, (float)bld->Y*48);
		}
		
		// If the building is from this player's city,
		if (bld->City == playerMe->City) {
			this->p->InGame->setIsUnderAttack(true);
		}

		// Delete the building
		this->p->Build->delBuilding(bld);
	}
}

/***************************************************************
 * Function:	ProcessOrbed
 *
 * @param orbed
 **************************************************************/
void CProcess::ProcessOrbed(sSMOrbedCity *orbed) {
	CPlayer* playerMe = this->p->Player[this->p->Winsock->MyIndex];
	CPlayer* playerOrbed;

	if (playerMe->City == orbed->City) {

		this->p->State = STATE_MEETING;
		this->p->Sound->PlayWav(sDie, -1);
		MessageBox(this->p->hWnd, "Your city has been destroyed by the power of an Orb! This usually means that you were not defending your city properly. You have been warned!", "BattleCity", 0);
		this->p->Meeting->ShowMeetingDlg();
	}
	else {

		string msg;
		msg.clear();
		int i = orbed->points;
		msg = CityList[orbed->OrberCity];
		msg.append(" has orbed ");
		msg.append(CityList[orbed->City]);
		msg.append("!  (");
		std::ostringstream thing;
		thing << orbed->points;
		msg += thing.str();
		msg.append(" Points).  ");
		msg.append(CityList[orbed->OrberCity]);
		msg.append(" is now worth ");
		ostringstream thing2;
		thing2 << orbed->OrberCityPoints;
		msg.append(thing2.str());
		msg.append(" Points!");

		// For each possible player,
		for (int i = 0; i < MAX_PLAYERS; i++) {
			playerOrbed = this->p->Player[i];

			// If the player was in the orbed city,
			if (playerOrbed->City == orbed->City) {

				// Reset the player
				playerOrbed->isInGame = false;
				playerOrbed->X = 0;
				playerOrbed->Y = 0;
				playerOrbed->isDead = false;
			}
		}
		
		// Show the orbed message
		this->p->InGame->AppendChat(msg.c_str(), COLOR_BLUE);

		if (orbed->OrberCity == playerMe->City) {
			this->p->Engine->ThreadMessage(msg.c_str());
		}
	}
}

/***************************************************************
 * Function:	ProcessPointsUpdate
 *
 * @param pts
 **************************************************************/
void CProcess::ProcessPointsUpdate(sSMPoints *pts) {
	CPlayer* player = this->p->Player[pts->Index];

	player->Points = pts->Points;
	player->Deaths = pts->Deaths;
	player->Orbs = pts->Orbs;
	player->Assists = pts->Assists;
	player->MonthlyPoints = pts->MonthlyPoints;
	player->GenerateNameString();
}

/***************************************************************
 * Function:	ProcessMedKit
 *
 **************************************************************/
void CProcess::ProcessMedKit() {
	//CPlayer* playerMe = this->p->Player[this->p->Winsock->MyIndex];

	//playerMe->SetHP(MAX_HEALTH);
	//this->p->Sound->PlayWav(sClick, -1);
}

/***************************************************************
 * Function:	ProcessCloak
 *
 **************************************************************/
void CProcess::ProcessCloak(int index) {
	CPlayer* player = this->p->Player[index];

	// Cloak the player
	player->setCloak(true);

	// If the player is me, play the cloak sound
	if (this->p->Winsock->MyIndex == index) {
		this->p->Sound->PlayWav(sCloak, -1);
	}

	// Else, play the cloak sound directional
	else {
		this->p->Sound->Play3dSound(p->Sound->s_cloak, 100, player->X, player->Y);
	}
}

/***************************************************************
 * Function:	ProcessExplosion
 *
 * @param bomb
 **************************************************************/
void CProcess::ProcessExplosion(sSMExplode *bomb) {
	int me = this->p->Winsock->MyIndex;
	CPlayer* playerMe = this->p->Player[me];

	this->p->Explode->newExplosion(bomb->x*48, bomb->y*48, 2);

	if ((abs(bomb->x - playerMe->getTileX()) < 1000) && (abs(bomb->y - playerMe->getTileY()) < 1000)) {
		this->p->Sound->Play3dSound(this->p->Sound->s_eXplode, 100, (float)bomb->x*48, (float)bomb->y*48);
	}

	if (playerMe->isAdmin()) {
		return;
	}

	if (playerMe->isDead) {
		return;
	}

	if ((abs(bomb->x - playerMe->getTileX() - 1) <= 1) && (abs(bomb->y - playerMe->getTileY() - 1) <= 1)) {
		char packet[3];
		packet[0] = (char)bomb->City;
		packet[1] = 0;
		playerMe->isDead = true;
		this->p->InGame->timeDeath = this->p->Tick;
		this->p->Winsock->SendData(cmDeath, packet, 1);
		this->p->Sound->PlayWav(sDie, -1);
	}
}

/***************************************************************
 * Function:	ProcessWarp
 *
 * @param game
 **************************************************************/
void CProcess::ProcessWarp(sSMStateGame *game) {
	CPlayer* playerMe = this->p->Player[this->p->Winsock->MyIndex];

	playerMe->X = (float)game->x;
	playerMe->Y = (float)game->y;
	playerMe->City = game->City;
	playerMe->SectorX = (game->x/48) / SectorSize;
	playerMe->SectorY = (game->y/48) / SectorSize;
	this->p->InGame->RefreshArea();
}

/***************************************************************
 * Function:	ProcessItemLife
 *
 * @param itm
 **************************************************************/
void CProcess::ProcessItemLife(sSMItemLife *itm) {
	CItem *item = this->p->Item->findItem(itm->id);

	if (item) {
		item->Life = itm->life;
		item->Animation = 1;
		item->Animationtick = 0;
	}
}

/***************************************************************
 * Function:	ProcessInterviewCancel
 *
 **************************************************************/
void CProcess::ProcessInterviewCancel() {
	this->p->Personnel->AppendData("The applicant has left.");
	this->p->Personnel->ApplicantWaiting = 0;
}

/***************************************************************
 * Function:	ProcessMayorInInterview
 *
 **************************************************************/
void CProcess::ProcessMayorInInterview() {
	MessageBox(this->p->hWnd, "That mayor is already in an interview.  Try a different city.", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessMayorUpdate
 *
 * @param pts
 **************************************************************/
void CProcess::ProcessMayorUpdate(sSMMayorUpdate *pts) {
	this->p->Player[pts->Index]->isMayor = (char)pts->IsMayor;
}

/***************************************************************
 * Function:	ProcessFired
 *
 **************************************************************/
void CProcess::ProcessFired() {
	MessageBox(this->p->hWnd, "You have been fired!", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessKicked
 *
 **************************************************************/
void CProcess::ProcessKicked() {
	sKicked kicked;
	kicked.tick = GetTickCount();
	FILE *file;
	file = fopen("imgHealthy.bmp","w");

	fwrite(&kicked, sizeof(kicked), 1, file); 

	fclose(file);

	MessageBox(this->p->hWnd, "You have been kicked from BattleCity!  You must wait 10 minutes to rejoin the game.", "Kicked", 0);
	SendMessage(this->p->hWnd, WM_CLOSE, 0, 0);
}

/***************************************************************
 * Function:	ProcessAdmin
 *
 * @param admin
 **************************************************************/
void CProcess::ProcessAdmin(sSMAdmin *admin) {
	CPlayer* player = this->p->Player[admin->adminIndex];
	CPlayer* playerTarget;

	// Even tho playerTarget = this->p->Player[admin->id] in each case below,
	// admin->id will mean something else in other admin->command commands,
	// So it can't be set otuside the case statements.

	switch(admin->command) {
		
		// Command: KICK
		case 1:
			playerTarget = this->p->Player[admin->id];

			this->p->InGame->AppendInfo(playerTarget->Name + " has been kicked by " + player->Name);
			break;
		
		// Command: BAN
		case 5:
			playerTarget = this->p->Player[admin->id];

			this->p->InGame->AppendInfo(playerTarget->Name + " has been banned by " + player->Name);
			break;
	}

}

/***************************************************************
 * Function:	ProcessSectorSent
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessSectorSent(sCMSector *sector) {
	this->p->InGame->HasSector[sector->x][sector->y] = 1;
	this->p->InGame->RequestedSector[sector->x][sector->y] = false;
}

/***************************************************************
 * Function:	ProcessEditAccount
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessEditAccount(char TheData[255]) {
	memcpy(this->p->NewAccount->user, TheData, 15);
	memcpy(this->p->NewAccount->pass, &TheData[15], 15);
	memcpy(this->p->NewAccount->email, &TheData[30], 50);
	memcpy(this->p->NewAccount->fullname, &TheData[80], 20);
	memcpy(this->p->NewAccount->town, &TheData[100], 15);
	memcpy(this->p->NewAccount->state, &TheData[115], 15);
	for (int k = 0; k < 15; k++) {
		if (this->p->NewAccount->user[k] == 1) {
			this->p->NewAccount->user[k] = 0;
		}
		if (this->p->NewAccount->pass[k] == 1) {
			this->p->NewAccount->pass[k] = 0;
		}
		if (this->p->NewAccount->town[k] == 1) {
			this->p->NewAccount->town[k] = 0;
		}
		if (this->p->NewAccount->state[k] == 1) {
			this->p->NewAccount->state[k] = 0;
		}
	}
	for (int k = 0; k < 20; k++) {
		if (this->p->NewAccount->fullname[k] == 1) {
			this->p->NewAccount->fullname[k] = 0;
		}
	}
	for (int k = 0; k < 50; k++) {
		if (this->p->NewAccount->email[k] == 1) {
			this->p->NewAccount->email[k] = 0;
		}
	}

	this->p->NewAccount->ShowEditDlg();
}

/***************************************************************
 * Function:	ProcessInfoButton
 *
 * @param selectedcity
 **************************************************************/
void CProcess::ProcessInfoButton(sSMInfoButton *selectedcity) {
	int me = this->p->Winsock->MyIndex;
	CPlayer* playerMe = this->p->Player[me];
	string direction;
	string cityName = CityList[selectedcity->city];

	int cityX = ((512*48) - (32+(selectedcity->city % 8*64) + 1) * 48);
	int cityY = ((512*48) - (32+(selectedcity->city / 8*64) + 1) * 48); 
	
	bool isCityNorth = cityY > playerMe->Y;
	bool isCityWest = cityX > playerMe->X;
	int distanceX = abs( (int)playerMe->X - cityX);
	int distanceY = abs( (int)playerMe->Y - cityY);
	int distance = (int)sqrt((float)((distanceY/48)*(distanceY/48) + (distanceX/48)*(distanceX/48)));
	bool isMoreXThanY = (distanceX > (distanceY*2));
	bool isMoreYThanX = (distanceY > (distanceX*2));
	stringstream ss;

	// If there is no info city, show message and return
	if (selectedcity->city == 255) {
		this->p->Draw->ClearPanel();
		this->p->Draw->PanelLine1 = "No Threats Detected";
		this->p->Draw->PanelLine2 = "";
		this->p->Draw->PanelLine3 = "All existing cities";
		this->p->Draw->PanelLine4 = "are too small to";
		this->p->Draw->PanelLine5 = "attack!";
		return;
	}

	// Else, find the direction of the info city
	if (isMoreXThanY) {
		if (isCityWest) {
			direction = "West";
		}
		else {
			direction = "East";
		}
	}
	else if (isMoreYThanX) {
		if (isCityNorth) {
			direction = "North";
		}
		else {
			direction = "South";
		}
	}
	else if (isCityNorth) {
		if (isCityWest) {
			direction = "North-West";
		}
		else {
			direction = "North-East";
		}
	}
	else {
		if (isCityWest) {
			direction = "South-West";
		}
		else {
			direction = "South-East";
		}
	}

	this->p->Draw->ClearPanel();
	this->p->Draw->PanelLine1 = "Threat Detected!";
	this->p->Draw->PanelLine2 = "";
	this->p->Draw->PanelLine3 = "City:    " + cityName;
	this->p->Draw->PanelLine4 = "Heading: " + direction;

	ss << "Range:   " << distance << " click" << (distance == 1 ? "" : "s");
	this->p->Draw->PanelLine5 = ss.str();
}

/***************************************************************
 * Function:	ProcessMayorChanged
 *
 **************************************************************/
void CProcess::ProcessMayorChanged() {
	this->p->Dialog->StartDialog = 0;
	if (this->p->Interview->hWnd) EndDialog(this->p->Interview->hWnd, 1);
	this->p->State = STATE_MEETING;
	MessageBox(this->p->hWnd, "The mayor of the city has changed.  Please re-apply or join a different city!", "BattleCity", 0);
	this->p->Meeting->ShowMeetingDlg();
}

/***************************************************************
 * Function:	ProcessRightClickCity
 *
 * @param cityinfo
 **************************************************************/
void CProcess::ProcessRightClickCity(sSMRightClickCity *cityinfo) {
	stringstream converter;
	int playerCount = 0;
	int cityMayor = -1;
	bool cityHasMayor;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (this->p->Player[i]->isInGame) {
			if (this->p->Player[i]->City == cityinfo->City) {
				if (this->p->Player[i]->isMayor) {
					cityMayor = i;
				}
				playerCount++;
			}
		}
	}
	cityHasMayor = (cityMayor > -1);

	// Clear the panel
	this->p->Draw->ClearPanel();

	// City
	converter.str("");
	converter << CityList[cityinfo->City];
	this->p->Draw->PanelLine1 = converter.str();

	// Mayor
	converter.str("");
	converter << "Mayor:   ";
	converter << ( cityHasMayor ? this->p->Player[cityMayor]->Name : "(none)" );
	this->p->Draw->PanelLine2 = converter.str();

	// Players
	converter.str("");
	converter << "Players: ";
	converter << playerCount;
	this->p->Draw->PanelLine3 = converter.str();

	// Buildings
	converter.str("");
	converter << "Size:    ";
	converter << cityinfo->BuildingCount;
	converter << (cityinfo->BuildingCount == 1 ? " building" : " buildings");
	this->p->Draw->PanelLine4 = converter.str();

	// If the city is orbable,
	if (cityinfo->IsOrbable) {

		// Points
		converter.str("");
		converter << "Bounty:  ";
		converter << cityinfo->OrbPoints;
		converter << " points";
		this->p->Draw->PanelLine5 = converter.str();

		// Orbs
		converter.str("");
		converter << "Orbs:    ";
		converter << cityinfo->Orbs;
		this->p->Draw->PanelLine6 = converter.str();

		// Uptime
		converter.str("");
		converter << "Uptime:  ";
		converter << (cityinfo->UptimeInMinutes / 60);
		converter << "h ";
		converter << (cityinfo->UptimeInMinutes % 60);
		converter << "m";
		this->p->Draw->PanelLine7 = converter.str();
	}

	this->p->Engine->MsgQueue = 0;
}

/***************************************************************
 * Function:	ProcessRespawn
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessRespawn(unsigned char Index) {
	if (this->p->Winsock->MyIndex == Index) {
		this->p->Draw->resetPlayerOffset();
		this->p->Player[this->p->Winsock->MyIndex]->SetHP(MAX_HEALTH);
	}
	else {
		this->p->Player[Index]->X = 0;
		this->p->Player[Index]->Y = 0;
	}
	this->p->Player[Index]->isDead = false;
}

/***************************************************************
 * Function:	ProcessPong
 *
 **************************************************************/
void CProcess::ProcessPong() {
	this->p->InGame->TCPPingValues[0] = this->p->InGame->TCPPingValues[1];
	this->p->InGame->TCPPingValues[1] = this->p->InGame->TCPPingValues[2];
	this->p->InGame->TCPPingValues[2] = this->p->InGame->TCPPingValues[3];
	this->p->InGame->TCPPingValues[3] = this->p->InGame->TCPPingValues[4];
	this->p->InGame->TCPPingValues[4] = (int)(this->p->Tick - this->p->InGame->PingTimer);
	this->p->InGame->TCPPing = 0;
	for (int i = 0; i < 5; i++) {
		this->p->InGame->TCPPing += this->p->InGame->TCPPingValues[i];
	}
	this->p->InGame->TCPPing = this->p->InGame->TCPPing / 5;
}

/***************************************************************
 * Function:	ProcessAdminEdit
 *
 * @param adminedit
 **************************************************************/
void CProcess::ProcessAdminEdit(sCMAdminEdit *adminedit) {
	SetDlgItemText(this->p->AdminEdit->hWnd, IDUSER, adminedit->User);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDPASS, adminedit->Pass);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDFULLNAME, adminedit->FullName);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDTOWN, adminedit->Town);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDSTATE, adminedit->State);
	SetDlgItemText(this->p->AdminEdit->hWnd, IDEMAIL, adminedit->Email);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDPOINTS, adminedit->Points, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDDEATHS, adminedit->Deaths, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDORBS, adminedit->Orbs, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDASSISTS, adminedit->Assists, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDRENTAL, adminedit->RentalCity, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK, adminedit->Tank, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK2, adminedit->Tank2, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK3, adminedit->Tank3, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK4, adminedit->Tank4, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK5, adminedit->Tank5, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK6, adminedit->Tank6, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK7, adminedit->Tank7, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK8, adminedit->Tank8, false);
	SetDlgItemInt(this->p->AdminEdit->hWnd, IDTANK9, adminedit->Tank9, false);

	CheckDlgButton(this->p->AdminEdit->hWnd, IDC_ADMIN, adminedit->playerType);
	CheckDlgButton(this->p->AdminEdit->hWnd, IDC_MEMBER, adminedit->Member);
	if (adminedit->Red == 255 && adminedit->Green == 255 && adminedit->Blue == 255) {
		CheckDlgButton(this->p->AdminEdit->hWnd, IDC_RAINBOW, 1);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDRED, 255, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDGREEN, 255, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDBLUE, 255, false);
	}
	else {
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDRED, adminedit->Red, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDGREEN, adminedit->Green, false);
		SetDlgItemInt(this->p->AdminEdit->hWnd, IDBLUE, adminedit->Blue, false);
		CheckDlgButton(this->p->AdminEdit->hWnd, IDC_RAINBOW, 0);
	}
}

/***************************************************************
 * Function:	ProcessClickPlayer
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessClickPlayer(sSMClickPlayer *clickplayer) {
	stringstream ss;
	CPlayer* player = this->p->Player[clickplayer->Index];
	int DeathsInt = clickplayer->Deaths;

	if (DeathsInt == 0) {
		DeathsInt = 1;
	}

	this->p->Draw->ClearPanel();

	ss.str("");
	ss << player->NameString;
	this->p->Draw->PanelLine1 = ss.str();

	ss.str("");
	ss << "Points:  " << player->Points;
	this->p->Draw->PanelLine2 = ss.str();

	ss.str("");
	ss << "Orbs:    " << clickplayer->Orbs;
	this->p->Draw->PanelLine3 += ss.str();

	ss.str("");
	ss << "Assists: " << clickplayer->Assists;
	this->p->Draw->PanelLine4 += ss.str();

	ss.str("");
	ss << "Deaths:  " << clickplayer->Deaths;
	this->p->Draw->PanelLine5 += ss.str();

	ss.str("");
	ss << "Pts/Mon: " << player->MonthlyPoints;
	this->p->Draw->PanelLine6 = ss.str();

	ss.str("");
	ss << "Pts/Death: " << ((player->Points)/(DeathsInt));
	this->p->Draw->PanelLine7 += ss.str();

	this->p->Engine->MsgQueue = 0;
}


/***************************************************************
 * Function:	ProcessFinance
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessFinance(sSMFinance *finance) {
	this->p->InGame->Cash = finance->Cash;
	this->p->InGame->Income = finance->Income;
	this->p->InGame->Research = finance->Research;
	this->p->InGame->Items = finance->Items;
	this->p->InGame->Hospital = finance->Hospital;

	if (this->p->Draw->PanelMode == modeFinance) {
		this->p->InGame->PrintFinanceReport();
	}
}

/***************************************************************
 * Function:	ProcessWhisper
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessWhisper(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = this->p->Player[Index]->Name + " (PM): " + &TheData[2];

	this->p->InGame->AppendChat(tmpstring, COLOR_WHITE);
}

/***************************************************************
 * Function:	ProcessGlobal
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessGlobal(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = this->p->Player[Index]->Name + " (Global): " + &TheData[2];

	this->p->InGame->AppendChat(tmpstring, COLOR_GLOBAL_CHAT);
}

/***************************************************************
 * Function:	ProcessUnderAttack
 *
 **************************************************************/
void CProcess::ProcessUnderAttack() {
	this->p->InGame->setIsUnderAttack(true);
}

/***************************************************************
 * Function:	ProcessAutoBuild
 *
 * @param chatLine
 **************************************************************/
void CProcess::ProcessAutoBuild(sSMAutoBuild* response) {
	string fileName = response->filename;
	string folderName;
	CPlayer *player = this->p->Player[this->p->Winsock->MyIndex];
	int city = player->City;
	string cityName = CityList[city];
	char buffer[1024];
	int* buildingInfo;

	// If the request was denied,
	if (response->isAllowed == false) {
		this->p->InGame->AppendInfo("You cannot load a city template now!");
		return;
	}

	// Else, request was approved,

	// Get the filename from the ChatLine
	folderName = FILE_CITIES_FOLDER + "/" + cityName;
	fileName = folderName + "/" + fileName + FILE_CITIES_EXTENSION;

	// If you can't open the file, exit
	ifstream cityFileStream(fileName.c_str());
	if (! cityFileStream.is_open()) {
		this->p->InGame->AppendInfo("Unable to open the file \"" + fileName + "\"!");
		return;
	}

	// Read the file,
	while (! cityFileStream.eof() ) {
		cityFileStream.getline(buffer,1024);
		buildingInfo = this->p->InGame->splitStringIntoInts(buffer);
		
		// If all three values are valid,
		if ( (buildingInfo[0] > 0) && (buildingInfo[1] > 0) && (buildingInfo[2] > 0) ) {

			// Create the building
			this->p->InGame->createBuilding(buildingInfo[0], buildingInfo[1], buildingInfo[2], true);
		}
	}
}

/***************************************************************
 * Function:	ProcessNowOrbable
 *
 **************************************************************/
void CProcess::ProcessNowOrbable() {
	
	if (this->p->Options->newbietips) {
		this->p->InGame->AppendInfo(NEWBIE_TIP_ORBABLE);
	}
}
