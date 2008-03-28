#include "CProcess.h"
#include "NetMessages.h"

void *ClassPointer;

CProcess::CProcess(CGame *game) {
	p = game;
	ClassPointer = this;
}

CProcess::~CProcess() {

}

int CProcess::ProcessData(char *TheData)
{
#ifndef _DEBUG
	try {
#endif
	switch((unsigned char)TheData[0])
	{
		case smLoginCorrect:
			ProcessLogin((unsigned char)TheData[1], (unsigned char)TheData[2]);
			break;
		case smAppendNews:
			if (p->State == STATE_GAME)
				p->Admin->SetNews(&TheData[1]);
			else
				p->Meeting->AppendNews(&TheData[1]);
			break;
		case smChatMessage:
			ProcessChatMessage(TheData, 0);
			break;
		case smChatCommand:
			ProcessChatCommand(TheData[1], TheData[2]);
			break;
		case smEditAccount:
			ProcessEditAccount(&TheData[1]);
			break;
		case smPlayerData:
			ProcessPlayerData((sSMPlayer *)&TheData[1]);
			break;
		case smMayorHire:
			ProcessMayorHire(TheData[1]);
			break;
		case smAddRemCity:
			p->Meeting->AddCity((unsigned char)TheData[1],(unsigned char)TheData[2], (unsigned char)TheData[3]);
			break;
		case smStateGame:
			ProcessEnterGame((sSMStateGame *)&TheData[1]);
			break;
		case smJoinData:
			ProcessJoinData((sSMJoinData *)&TheData[1]);
			break;
		case smError:
			ProcessError(TheData[1]);
			break;
		case smNewBuilding:
			ProcessNewBuilding((sSMBuild *)&TheData[1],1);
			break;
		case smPlaceBuilding:
			ProcessNewBuilding((sSMBuild *)&TheData[1],0);
			break;
		case smUpdate:	
			ProcessUpdate((sSMUpdate *)&TheData[1]);
			break;
		case smCanBuild:
			ProcessCanBuild(TheData);
			break;
		case smFinance:
			ProcessFinance((sSMFinance *)&TheData[1]);
			break;
		case smInterview:
			ProcessInterview();
			break;
		case smComms:
			ProcessComms(TheData);
			break;
		case smMayorDeclined:
			ProcessRejected();
			break;
		case smMayorInInterview:
			ProcessMayorInInterview();
			break;
		case smInterviewCancel:
			ProcessInterviewCancel();
			break;
		case smAddItem:
			ProcessAddItem((sSMItem *) &TheData[1]);
			break;
		case smRemItem:
			ProcessRemoveItem((sSMItem *) &TheData[1]);
			break;
		case smItemCount:
			ProcessItemCount((sSMItemCount *)&TheData[1]);
			break;
		case smPickedUp:
			ProcessPickUp((sCMItem *) &TheData[1]);
			break;
		case smDropped:
			ProcessDropped((sCMItem *) &TheData[1]);
			break;
		case smUpdatePop:
			ProcessPop((sSMPop *)&TheData[1]);
			break;
		case smDeath:
			ProcessDeath(TheData[1], TheData[2], TheData[3]);
			break;
		case smDestroyCity:
			ProcessDestroyCity((unsigned char)TheData[1]);
			break;
		case smRemBuilding:
			ProcessRemoveBuilding((sSMBuild *)&TheData[1]);
			break;
		case smOrbed:
			ProcessOrbed((sSMOrbedCity *)&TheData[1]);
			break;
		case smGlobal:
			ProcessGlobal(TheData);
			break;
		case smShoot:
			ProcessShot((sSMShot *)&TheData[1]);
			break;
		case smTCPPong:
			ProcessPong();
			break;
		case smPointsUpdate:
			ProcessPointsUpdate((sSMPoints *)&TheData[1]);
			break;
		case smMedKit:
			ProcessMedKit();
			break;
		case smExplosion:
			ProcessExplosion((sSMExplode *)&TheData[1]);
			break;
		case smWarp:
			ProcessWarp((sSMStateGame *)&TheData[1]);
			break;
		case smNextStep:
			if (TheData[1] == 65)
			{
				p->Dialog->StartDialog = 0;
				if (p->Meeting->hWnd) EndDialog(p->Meeting->hWnd, 1);
				if (p->Interview->hWnd) EndDialog(p->Interview->hWnd, 1);
				p->State = STATE_JOINING;
			}
			if (TheData[1] == 66)
			{
				p->InGame->ClearAllSectors();
				p->InGame->RefreshArea();
			}
			if (TheData[1] == 67)
			{
				p->State = STATE_GAME;
			}
			p->Winsock->SendData(cmNextStep, &TheData[1], 1);
			break;
		case smMayorUpdate:
			ProcessMayorUpdate((sSMMayorUpdate *)&TheData[1]);
			break;
		case smItemLife:
			ProcessItemLife((sSMItemLife *)&TheData[1]);
			break;
		case smFired:
			ProcessFired();
			break;
		case smAdmin:
			ProcessAdmin((sSMAdmin *)&TheData[1]);
			break;
		case smKicked:
			ProcessKicked();
			break;
		case smSectorSent:
			ProcessSectorSent((sCMSector *)&TheData[1]);
			break;
		case smInfoButton:
			ProcessInfoButton((sSMInfoButton *)&TheData[1]);
			break;
		case smClearPlayer:
			ProcessClearPlayer(TheData[1]);
			break;
		case smMayorChanged:
			ProcessMayorChanged();
			break;
		case smRightClickCity:
			ProcessRightClickCity((sSMRightClickCity *)&TheData[1]);
			break;
		case smBan:
			p->Admin->AddBan((sSMBan *)&TheData[1]);
			break;
		case smRespawn:
			ProcessRespawn(TheData[1]);
			break;
		case smAdminEdit:
			ProcessAdminEdit((sCMAdminEdit *)&TheData[1]);
			break;
		case smClickPlayer:
			ProcessClickPlayer((sSMClickPlayer *)&TheData[1]);
			break;
		case smWhisper:
			ProcessWhisper(TheData);
			break;
		default:
			std::ostringstream thing;
			int i = TheData[0];
			thing << i;
			p->Engine->logerror("Unhandled Data Packet " + thing.str());
			return 1;
			break;
	}
	return 0;

#ifndef _DEBUG
	}
	catch (...) {
	std::ostringstream thing;
	int i = TheData[0];
	thing << i;
	p->Engine->logerror("ProcessData " + thing.str());
	return 1;
	}
#endif
}

void CProcess::ProcessLogin(unsigned char Index, unsigned char isAdmin)
{
#ifndef _DEBUG
	try {
#endif
	p->Player[Index]->ClearPlayer();
	p->Winsock->MyIndex = Index;
	p->Player[Index]->isAdmin = isAdmin;
	p->Player[Index]->Name.clear();
	p->Player[Index]->Name = p->Login->user;
	p->Meeting->ShowMeetingDlg();
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessLogin"); p->Engine->logerror("ProcessLogin");}
#endif
}

void CProcess::ProcessClearPlayer(unsigned char Index)
{
	p->Player[Index]->ClearPlayer();
	if (p->Admin->IsOpen) p->Admin->DrawPlayerList();
}

void CProcess::ProcessError(int error)
{
#ifndef _DEBUG
	try {
#endif
	switch(error)
	{
		case 65: //Message A
			{MessageBox(p->hWnd, "Account created succesfully!", "BattleCity", 0); p->Login->ShowLoginDlg(); break;}
		case 66: //Message B
			{MessageBox(p->hWnd, "Incorrect Password", "BattleCity", 0); p->Login->ShowLoginDlg(); break;}
		case 67: //Message C
			{MessageBox(p->hWnd, "Account does not exist", "BattleCity", 0); p->Login->ShowLoginDlg(); break;}
		case 68: //Message D
			{MessageBox(p->hWnd, "User name is already in use!", "BattleCity", 0); p->NewAccount->ShowNewDlg(); break;}
		case 69: //Message E
			{MessageBox(p->hWnd, "Account is currently in use", "BattleCity", 0); p->Login->ShowLoginDlg(); break;}
		case 70: //Message F
			{MessageBox(p->hWnd, "Please download the latest version of BattleCity at http://battlecity.looble.com", 0, 0); SendMessage(p->hWnd, WM_CLOSE, 0, 0); break;}
		case 71: //Message G
			{MessageBox(p->hWnd, "Banned", 0, 0); SendMessage(p->hWnd, WM_CLOSE, 0, 0); break;}
		case 72: //Message H
			{MessageBox(p->hWnd, "Only one client per computer", 0, 0); SendMessage(p->hWnd, WM_CLOSE, 0, 0); break;}
		case 74: //Message J
			{MessageBox(p->hWnd, "Invalid verification code.", 0, 0); SendMessage(p->hWnd, WM_CLOSE, 0, 0); break;}
		case 75: //Message K
			if (p->NewAccount->Editing == 1)
				{MessageBox(p->hWnd, "Email is already in use!", "BattleCity", 0); p->NewAccount->ShowEditDlg();}
				else {MessageBox(p->hWnd, "Email is already in use!", "BattleCity", 0); p->NewAccount->ShowNewDlg();}
			break;
		case 76: //Message L
			{MessageBox(p->hWnd, "Email address not found.  Please try again!", 0, 0); p->Recover->ShowRecoverDlg(); break;}
		case 77: //Message M
			{MessageBox(p->hWnd, "Your account information has been sent to your email address!", 0, 0); p->Login->ShowLoginDlg(); break;}
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessError"); p->Engine->logerror("ProcessError");}
#endif
}

void CProcess::ProcessEvent(int Event)
{
#ifndef _DEBUG
	try {
#endif
	switch (Event)
	{
		case 1: //Error connecting
			p->running = 0;
			Sleep(100);
			MessageBox(p->hWnd, "Unable to connect to the BattleCity server.  Make sure you are properly connected to the internet, and if you are, the server is likely down for maintenance or being updated.  Please try again later.", "BattleCity", 0);
			SendMessage(p->hWnd, WM_CLOSE, 0, 0);
			break;
		case 2: //Connected
			p->Login->ShowLoginDlg();
			p->Send->SendVersion();
			break;
		case 3: //Disconnected
			if (p->running == 0) return;
			p->running = 0;
			Sleep(100);
 			MessageBox(p->hWnd, "You have been disconnected from the BattleCity server.  Please try again later.", "BattleCity", 0);
			SendMessage(p->hWnd, WM_CLOSE, 0, 0);
			break;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessEvent"); p->Engine->logerror("ProcessEvent");}
#endif
}

void CProcess::ProcessChatMessage(char *TheData, char global)
{
#ifndef _DEBUG
	try {
#endif
	string tmpstring;
	int Index = (unsigned char)TheData[1];
	tmpstring = p->Player[Index]->Name + ": " + &TheData[2];
	switch (p->State)
	{
		case STATE_GAME:
			if (p->Player[Index]->isAdmin == 2)
			{
				p->InGame->AppendChat(tmpstring, RGB(255, 165, 0));
			}
			else
			{
				if (p->Player[Index]->isDead == true)
				{
					p->InGame->AppendChat(tmpstring, RGB(135, 135, 135));
				}
				else
				{
					if (p->Player[Index]->City == p->Player[p->Winsock->MyIndex]->City)
					{
						p->InGame->AppendChat(tmpstring, RGB(0, 255, 0));
					}
					else
					{
						p->InGame->AppendChat(tmpstring, RGB(255, 0, 0));
					}
				}
			}
			break;
		case STATE_MEETING:
			p->Meeting->AppendData(tmpstring);
			break;
		default:

			break;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessChatMessage"); p->Engine->logerror("ProcessChatMessage");}
#endif
}

void CProcess::ProcessChatCommand(int Index, int message)
{
#ifndef _DEBUG
	try {
#endif
	string tmpString;
	if (p->Player[Index]->Name.length() > 0)
	{
		tmpString = p->Player[Index]->Name;
		switch(message)
		{
			case 65: //Message A
				{p->Meeting->AddPlayer(Index); break;}
			case 66: //Message B
			case 67: //Message C 
			case 68: //Message D
				{p->Meeting->ClearPlayer(Index); break;}
			case 69: //Message E
				{tmpString.append(" has left"); p->Player[Index]->InGameClear(); if (p->Admin->IsOpen) p->Admin->DrawPlayerList(); break;}
		}
		switch(p->State)
		{
			case STATE_GAME:
				p->InGame->AppendChat(&tmpString[0], RGB(255, 215, 0));
				break;
			default:
				break;
		}
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessChatCommand"); p->Engine->logerror("ProcessChatCommand");}
#endif
}

void CProcess::ProcessEnterGame(sSMStateGame *game)
{
#ifndef _DEBUG
	try {
#endif
	int me = p->Winsock->MyIndex;

	p->Draw->resetPlayerOffset();
	p->Player[me]->X = (float)game->x;
	p->Player[me]->Y = (float)game->y;
	p->Player[me]->SectorX = (game->x/48) / SectorSize;
	p->Player[me]->SectorY = (game->y/48) / SectorSize;
	p->Player[me]->CityX = game->x;
	p->Player[me]->CityY = game->y;
	p->Player[me]->City = game->City;
	p->Player[me]->SetHP(40);
	p->Player[me]->isInGame = 1;
	p->Player[me]->GenerateNameString();

	if (p->Player[me]->isMayor)
	{
		p->InGame->NewbieTip = "You are Mayor of ";
		p->InGame->NewbieTip += CityList[p->Player[p->Winsock->MyIndex]->City];
		p->InGame->NewbieTip += ".  Right-click anywhere on the screen to start building!";
	}
	else
	{
		p->InGame->NewbieTip = "You have joined ";
		p->InGame->NewbieTip += CityList[p->Player[p->Winsock->MyIndex]->City];
		p->InGame->NewbieTip += ".  Press 'u' on top of an item to pick it up, and 'd' to drop it!";
	}

	p->InGame->AppendChat("Full instructions at:", RGB(255, 215, 0));
	p->InGame->AppendChat("http://battlecity.looble.com/", RGB(255, 215, 0));
	p->InGame->PrintWhoData();
	p->InGame->AppendChat(p->Player[me]->Name + " has just joined!", RGB(255, 215, 0));

	p->Dialog->StartDialog = 0;
	if (p->Meeting->hWnd) EndDialog(p->Meeting->hWnd, 1);
	if (p->Interview->hWnd) EndDialog(p->Interview->hWnd, 1);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessWarp"); p->Engine->logerror("ProcessWarp");}
#endif
}

void CProcess::ProcessNewBuilding(sSMBuild *data, int sound)
{
#ifndef _DEBUG
	try {
#endif
	p->Build->newBuilding(data->x,data->y,data->City,buildingTypes[(unsigned char)data->type - 1], data->id);
	CBuilding *bld = p->Build->findBuilding(data->id);
	if (bld) bld->pop = data->pop;

	if (sound && p->State == STATE_GAME)
	{
		int me = p->Winsock->MyIndex;
		if (data->x * 48 < p->Player[me]->X + 1000 &&
			data->x * 48 + 1000 > p->Player[me]->X &&
			data->y * 48 < p->Player[me]->Y + 1000 &&
			data->y * 48 + 1000 > p->Player[me]->Y)
				p->Sound->Play3dSound(p->Sound->s_build, 100, (float)data->x*48, (float)data->y*48);
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessNewBuilding"); p->Engine->logerror("ProcessNewBuilding");}
#endif
}

void CProcess::ProcessUpdate(sSMUpdate *data)
{
#ifndef _DEBUG
	try {
#endif
	p->Player[data->id]->isDead = false;
	p->Player[data->id]->Y = data->y;
	p->Player[data->id]->X = data->x;
	p->Player[data->id]->isMoving = data->move - 1;
	p->Player[data->id]->isTurning = data->turn - 1;
	p->Player[data->id]->Direction = data->dir;
	p->Player[data->id]->lastUpdate = p->Tick;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessUpdate"); p->Engine->logerror("ProcessUpdate");}
#endif
}

void CProcess::ProcessPlayerData(sSMPlayer *player)
{
#ifndef _DEBUG
	try {
#endif
	int i = player->Index;
	p->Player[i]->isAdmin = player->isAdmin;
	p->Player[i]->Red = player->Red;
	p->Player[i]->Green = player->Green;
	p->Player[i]->Blue = player->Blue;
	p->Player[i]->Member = player->Member;
	p->Player[i]->Tank = player->Tank;

	if (player->Red == 255 && player->Green == 255 && player->Blue == 255)
	{
		p->Player[i]->RainbowName = true;
	}
	
	p->Player[i]->Name = player->Name;
	p->Player[i]->Town = player->Town;
	p->Player[i]->GenerateNameString();
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessPlayerData"); p->Engine->logerror("ProcessPlayerData");}
#endif
}

void CProcess::ProcessCanBuild(char TheData[255])
{
#ifndef _DEBUG
	try {
#endif
	if (TheData[2])
	{
		p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 1;
	}
	else
		p->InGame->CanBuild[(unsigned char)TheData[1]-1] = 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessCanBuild"); p->Engine->logerror("ProcessCanBuild");}
#endif
}

void CProcess::ProcessComms(char TheData[255])
{
#ifndef _DEBUG
	try {
#endif
	string tmpString;
	if (p->Player[(unsigned char)TheData[1]]->Name.length() > 0)
		{
			switch (TheData[2])
			{
				case 124: // |
					tmpString = "The mayor has arrived - Start talking.";
					break;
				default:
					tmpString = p->Player[(unsigned char)TheData[1]]->Name + ": ";
					tmpString.append(&TheData[2]);
					break;
			}
			if (p->State == STATE_GAME) p->Personnel->AppendData(tmpString);
			else  p->Interview->AppendData(tmpString);
		}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessComms"); p->Engine->logerror("ProcessComms");}
#endif
}

void CProcess::ProcessRejected()
{
#ifndef _DEBUG
	try {
#endif
	MessageBox(p->hWnd, "The mayor did not hire you.  Try a different city.", "BattleCity", 0);
	p->Interview->Chat.clear();
	p->Meeting->ShowMeetingDlg();
	EndDialog(p->Interview->hWnd, 1);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessRejected"); p->Engine->logerror("ProcessRejected");}
#endif
}

void CProcess::ProcessInterview()
{
#ifndef _DEBUG
	try {
#endif
	p->Interview->Chat.clear();
	p->Interview->ShowInterviewDlg();
	p->Interview->AppendData("Please wait here while I notify the Mayor of your arrival");
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessInterview"); p->Engine->logerror("ProcessInterview");}
#endif
}

void CProcess::ProcessMayorHire(int Index)
{
#ifndef _DEBUG
	try {
#endif
	string tmpString;
	tmpString.clear();
	if (p->Personnel->DenyApplicants == 0) 
	{
		if (p->Player[Index]->Name.length() > NULL)
		{
			tmpString = p->InGame->ReturnRank(p->Player[Index]->Points) + " ";
			tmpString += p->Player[Index]->Name;
			tmpString += " of ";
			tmpString += p->Player[Index]->Town;
			tmpString += + " is looking for a job";
			p->Personnel->ApplicantWaiting = 1;
			p->InGame->AppendChat("Note:  Another player is applying for a position in your city.", RGB(255, 215, 0));
			p->Personnel->AppendData(tmpString);
			p->Sound->PlayWav(sBuzz,-1);
		}
	}
	else
	{
		char packet[2];
		packet[0] = (char)p->Personnel->DenyApplicants;
		packet[1] = 0;
		p->Winsock->SendData(cmIsHiring, packet, 1);
		p->Winsock->SendData(cmHireDecline, " ", 1);
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessMayorHire"); p->Engine->logerror("ProcessMayorHire");}
#endif
}

void CProcess::ProcessAddItem(sSMItem *item)
{
#ifndef _DEBUG
	try {
#endif
	p->Item->newItem(item->x, item->y, item->type, item->City, item->id, item->active);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessAddItem"); p->Engine->logerror("ProcessAddItem");}
#endif
} 

void CProcess::ProcessRemoveItem(sSMItem *item)
{
#ifndef _DEBUG
	try {
#endif
	CItem *item2;
	item2 = p->Item->findItem(item->id);
	if (item2)
		p->Item->delItem(p->Item->findItem(item->id));
	else
	{
		item2 = p->Inventory->findItem(item->id);
		if (item2)
		{
			p->Inventory->delItem(p->Inventory->findItem(item->id));
			p->Inventory->ItemCheck();
		}
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessRemoveItem"); p->Engine->logerror("ProcessRemoveItem");}
#endif
}

void CProcess::ProcessItemCount(sSMItemCount *count)
{
#ifndef _DEBUG
	try {
#endif
	CBuilding *bld = p->Build->findBuilding(count->id);

	if (bld) bld->ItemsLeft = count->count;
	return;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessItemCount"); p->Engine->logerror("ProcessItemCount");}
#endif
}

void CProcess::ProcessJoinData(sSMJoinData *join)
{
#ifndef _DEBUG
	try {
#endif
	string tmpString;
	int i = join->id;
	p->Player[i]->isMayor = join->Mayor;
	p->Player[i]->City = join->City;
	p->Player[i]->isInGame = 1;
	p->Player[i]->X = 0;
	p->Player[i]->Y = 0;
	p->Player[i]->GenerateNameString();
	if (p->Player[i]->Name.length() > 0 && p->State == STATE_GAME)
	{
		tmpString = p->Player[i]->Name + " has just joined";
		p->InGame->AppendChat(tmpString, RGB(255, 215, 0));
	}
	if (p->Admin->IsOpen) p->Admin->DrawPlayerList();
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessJoinData"); p->Engine->logerror("ProcessJoinData");}
#endif
}

void CProcess::ProcessPickUp(sCMItem *item)
{
#ifndef _DEBUG
	try {
#endif
	CItem *item2;
	item2 = p->Item->findItem(item->id);

	if (item2)
	{
		CItem *itm = p->Inventory->newItem(item2->X, item2->Y, item2->Type, item2->City, item->id, item->active);
		p->Inventory->ItemCheck();
		p->Inventory->SelectedItemType = itm->Type;
		if (p->Options->newbietips)
		{
			switch (item2->Type)
			{
			case 0: //Laser
				p->InGame->NewbieTip = "Press shift to shoot your laser!";
				break;
			case 1: //Rocket
				p->InGame->NewbieTip = "Cougars are stronger than lasers. They can only be fired when your tank is stationary.";
				break;
			case 2: //MedKit
				p->InGame->NewbieTip = "A Medkit restores health to full! Press 'h' to use a Medkit.";
				break;
			case 3: //Bomb
				p->InGame->NewbieTip = "Bombs can be used to destroy buildings! Press 'b' to drop an activated bomb.";
				break;
			case 4: //Mines
				p->InGame->NewbieTip = "If an enemy hits your mine, they will loose a large amount of health and most likely be destroyed!";
				break;
			case 5: //Orb
				p->InGame->NewbieTip = "The Orb is used to destroy enemy cities! Drop it on your opponents Command Center to earn points and gain rank!";
				break;
			case 6: //Walkie Talkie
				p->InGame->NewbieTip = "Walkies are required in order to communicate with teamates who are outside of radar range.";
				break;
			case 7: //DFG
				p->InGame->NewbieTip = "If an enemy hits your DFG, they will be unable to move for 7 seconds! Place them in unexpected areas.";
				break;
			case 8: //Wall
				p->InGame->NewbieTip = "Walls are used to block access to important areas. Try putting them on your Command Center to help protect against Orbers!";
				break;
			case 9: //Normal Turret
				p->InGame->NewbieTip = "Turrets automatically shoot at any enemy that comes within their range. Place them around your city!";
				break;
			case 10: //Sleeping Turret
				p->InGame->NewbieTip = "Sleepers are similar to turrets and plasma cannons, except they remain invisible to enemies until they begin shooting!";
				break;
			case 11: //Plasma Turret
				p->InGame->NewbieTip = "Plasma Cannons automatically shoot at any enemy that come within their range. Place them around your city!";
				break;
			}
		}
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessPickUp"); p->Engine->logerror("ProcessPickUp");}
#endif
}

void CProcess::ProcessDropped(sCMItem *item)
{
#ifndef _DEBUG
	try {
#endif
	CItem *item2;
	item2 = p->Inventory->findItem(item->id);
	if (item2) 
	{
		p->Inventory->delItem(item2);
		p->Inventory->ItemCheck();
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessDropped"); p->Engine->logerror("ProcessDropped");}
#endif
}

void CProcess::ProcessPop(sSMPop *pop)
{
#ifndef _DEBUG
	try {
#endif
	CBuilding *bld = p->Build->findBuilding(pop->id);
	if (bld) bld->pop = pop->pop;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessPop"); p->Engine->logerror("ProcessPop");}
#endif
}

void CProcess::ProcessDeath(int Index, char deathType, char City)
{
#ifndef _DEBUG
	try {
#endif
	string tmpString;
    srand((unsigned int)p->Tick);
    int random_integer = rand()%13;
	int me = p->Winsock->MyIndex;

	p->Player[Index]->isDead = true;
	p->Player[Index]->isMoving = false;
	p->Player[Index]->isTurning = false;

	if (Index != p->Winsock->MyIndex)
	{
		if ((abs(p->Player[Index]->X - p->Player[me]->X) < 1000) && (abs(p->Player[Index]->Y - p->Player[me]->Y) < 1000))
		{
			p->Explode->newExplosion((int)p->Player[Index]->X, (int)p->Player[Index]->Y, 1);
			p->Sound->Play3dSound(p->Sound->s_eXplode, 100, p->Player[Index]->X, p->Player[Index]->Y);
			p->Sound->Play3dSound(p->Sound->s_die, 100, p->Player[Index]->X, p->Player[Index]->Y);
		}
	}
	else
	{
		p->Sound->StopWav(12);
	}

	switch (random_integer)
	{
		case 0:
			tmpString = p->Player[Index]->Name + " no longer exists as a single entity";
			break;
		case 1:
			tmpString = p->Player[Index]->Name + " just had a TNT experience";
			break;
		case 2:
			tmpString = p->Player[Index]->Name + " has shattered into many pieces";
			break;
		case 3:
			tmpString = p->Player[Index]->Name + " has been blown up";
			break;
		case 4:
			tmpString = p->Player[Index]->Name + " is no more";
			break;
		case 5:
			tmpString = p->Player[Index]->Name + " is worm food";
			break;
		case 6:
			tmpString = p->Player[Index]->Name + " just ran out of luck";
			break;
		case 7:
			tmpString = "Alas poor " + p->Player[Index]->Name + " we knew him well";
			break;
		case 8:
			tmpString = "A funeral service will shortly be held for " + p->Player[Index]->Name;
			break;
		case 9:
			tmpString = p->Player[Index]->Name + " has bitten the dust";
			break;
		case 10:
			tmpString = p->Player[Index]->Name + " has gone to meet his maker";
			break;
		case 11:
			tmpString = p->Player[Index]->Name + " is dead meat";
			break;
		case 12:
			tmpString = p->Player[Index]->Name + " was useless at this game anyway!";
			break;
	}

	if (p->Player[p->Winsock->MyIndex]->City == City)
		tmpString += " (2)";

	p->InGame->AppendChat(tmpString, RGB(255, 215, 0));
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessDeath"); p->Engine->logerror("ProcessDeath");}
#endif
}

void CProcess::ProcessShot(sSMShot *shotsfired)
{
#ifndef _DEBUG
	try {
#endif
	int me = p->Winsock->MyIndex;
	if (p->Player[shotsfired->id]->isAdmin == 2)
	{
		float fDir = (float)-p->Player[shotsfired->id]->Direction+32;
		int FlashY = (int)p->Player[shotsfired->id]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		int FlashX = (int)p->Player[shotsfired->id]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		p->Explode->newExplosion(FlashX, FlashY, 3);
		p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(p->Player[shotsfired->id]->X - p->Player[me]->X) < 1000) && (abs(p->Player[shotsfired->id]->Y - p->Player[me]->Y) < 1000))
			p->Sound->Play3dSound(p->Sound->s_turret, 100, p->Player[shotsfired->id]->X, p->Player[shotsfired->id]->Y);
	}
	else
	{
		float fDir = (float)-p->Player[shotsfired->id]->Direction+32;
		int FlashY = (int)p->Player[shotsfired->id]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
		int FlashX = (int)p->Player[shotsfired->id]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
		p->Explode->newExplosion(FlashX, FlashY, 3);
		p->Bullet->newBullet(shotsfired->x, shotsfired->y, shotsfired->type, shotsfired->dir, shotsfired->id);

		if ((abs(p->Player[shotsfired->id]->X - p->Player[me]->X) < 1000) && (abs(p->Player[shotsfired->id]->Y - p->Player[me]->Y) < 1000))
		{
			switch(shotsfired->type)
			{
				case 0:
					p->Sound->Play3dSound(p->Sound->s_tanklaser, 100, p->Player[shotsfired->id]->X, p->Player[shotsfired->id]->Y);
					break;
				case 1:
					p->Sound->Play3dSound(p->Sound->s_bigturret, 100, p->Player[shotsfired->id]->X, p->Player[shotsfired->id]->Y);
					break;
			}
		}
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessShot"); p->Engine->logerror("ProcessShot");}
#endif
}

void CProcess::ProcessDestroyCity(char City)
{
#ifndef _DEBUG
	try {
#endif
	p->Item->DestroyCity(City);
	p->Build->DestroyCity(City);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessDestroyCity"); p->Engine->logerror("ProcessDestroyCity");}
#endif
}

void CProcess::ProcessRemoveBuilding(sSMBuild *build)
{
#ifndef _DEBUG
	try {
#endif
	CBuilding *bld = p->Build->findBuilding(build->id);

	if (bld)
	{
		if (build->x == 1)
		{
			p->Explode->newExplosion(bld->X * 48, bld->Y * 48, 2);
		}

		if (bld->X * 48 < p->Player[p->Winsock->MyIndex]->X + 400 &&
			bld->X * 48 + 400 > p->Player[p->Winsock->MyIndex]->X &&
			bld->Y * 48 < p->Player[p->Winsock->MyIndex]->Y + 400 &&
			bld->Y * 48 + 400 > p->Player[p->Winsock->MyIndex]->Y)
				p->Sound->Play3dSound(p->Sound->s_demolish, 100, (float)bld->X*48, (float)bld->Y*48);

		p->Build->delBuilding(bld);
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessRemoveBuilding"); p->Engine->logerror("ProcessRemoveBuilding");}
#endif
}
void CProcess::ProcessOrbed(sSMOrbedCity *orbed)
{
#ifndef _DEBUG
	try {
#endif
	if (p->Player[p->Winsock->MyIndex]->City == orbed->City)
	{
		p->State = STATE_MEETING;
		p->Sound->PlayWav(sDie, -1);
		MessageBox(p->hWnd, "Your city has been destroyed by the power of an Orb! This usually means that you were not defending your city properly. You have been warned!", "BattleCity", 0);
		p->Meeting->ShowMeetingDlg();
	}
	else
	{
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
		for (int i = 0; i < MaxPlayers; i++)
		{
			if (p->Player[i]->City == orbed->City)
			{
				p->Player[i]->isInGame = false;
				p->Player[i]->X = 0;
				p->Player[i]->Y = 0;
				p->Player[i]->isDead = false;
			}
		}
		p->InGame->AppendChat(msg.c_str(), RGB(0, 0, 255));

		if (orbed->OrberCity == p->Player[p->Winsock->MyIndex]->City)
			p->Engine->ThreadMessage(msg.c_str());
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessOrbed"); p->Engine->logerror("ProcessOrbed");}
#endif
}

void CProcess::ProcessPointsUpdate(sSMPoints *pts)
{
#ifndef _DEBUG
	try {
#endif
	p->Player[pts->Index]->Points = pts->Points;
	p->Player[pts->Index]->Deaths = pts->Deaths;
	p->Player[pts->Index]->Orbs = pts->Orbs;
	p->Player[pts->Index]->Assists = pts->Assists;
	p->Player[pts->Index]->MonthlyPoints = pts->MonthlyPoints;
	p->Player[pts->Index]->GenerateNameString();
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessPointsUpdate"); p->Engine->logerror("ProcessPointsUpdate");}
#endif
}

void CProcess::ProcessMedKit()
{
#ifndef _DEBUG
	try {
#endif
	p->Player[p->Winsock->MyIndex]->SetHP(40);
	p->Sound->PlayWav(sClick, -1);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessMedKit"); p->Engine->logerror("ProcessMedKit");}
#endif
}

void CProcess::ProcessExplosion(sSMExplode *bomb)
{
#ifndef _DEBUG
	try {
#endif
	int me = p->Winsock->MyIndex;
	p->Explode->newExplosion(bomb->x*48, bomb->y*48, 2);
	if ((abs(bomb->x - (p->Player[me]->X / 48) - 1) <= 1) && (abs(bomb->y - (p->Player[me]->Y / 48) - 1) <= 1))
	{
		if (p->Player[me]->isAdmin != 2 && p->Player[p->Winsock->MyIndex]->isDead == false)
		{
			char packet[3];
			packet[0] = (char)bomb->City;
			packet[1] = 0;
			p->Player[p->Winsock->MyIndex]->isDead = true;
			p->InGame->timeDeath = p->Tick;
			p->Winsock->SendData(cmDeath, packet, 1);
			p->Sound->PlayWav(sDie, -1);
		}
	}
	if ((abs(bomb->x*48 - p->Player[me]->X) < 1000) && (abs(bomb->y*48 - p->Player[me]->Y) < 1000))
		p->Sound->Play3dSound(p->Sound->s_eXplode, 100, (float)bomb->x*48, (float)bomb->y*48);
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessExplosion"); p->Engine->logerror("ProcessExplosion");}
#endif
}

void CProcess::ProcessWarp(sSMStateGame *game)
{
#ifndef _DEBUG
	try {
#endif
	int me = p->Winsock->MyIndex;
	p->Player[me]->X = (float)game->x;
	p->Player[me]->Y = (float)game->y;
	p->Player[me]->City = game->City;
	p->Player[me]->SectorX = (game->x/48) / SectorSize;
	p->Player[me]->SectorY = (game->y/48) / SectorSize;
	p->InGame->RefreshArea();
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessWarp"); p->Engine->logerror("ProcessWarp");}
#endif
}

void CProcess::ProcessItemLife(sSMItemLife *itm)
{
#ifndef _DEBUG
	try {
#endif

		CItem *sdf = p->Item->findItem(itm->id);
		if (sdf)
		{
			sdf->Life = itm->life;
			sdf->Animation = 1;
			sdf->Animationtick = 0;
		}

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessItemLife"); p->Engine->logerror("ProcessItemLife");}
#endif
}

void CProcess::ProcessInterviewCancel()
{
#ifndef _DEBUG
	try {
#endif

	p->Personnel->AppendData("The applicant has left.");
	p->Personnel->ApplicantWaiting = 0;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessItemLife"); p->Engine->logerror("ProcessItemLife");}
#endif
}

void CProcess::ProcessMayorInInterview()
{
#ifndef _DEBUG
	try {
#endif

	MessageBox(p->hWnd, "That mayor is already in an interview.  Try a different city.", "BattleCity", 0);
	p->Meeting->ShowMeetingDlg();

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessItemLife"); p->Engine->logerror("ProcessItemLife");}
#endif
}

void CProcess::ProcessMayorUpdate(sSMMayorUpdate *pts)
{
#ifndef _DEBUG
	try {
#endif

	p->Player[pts->Index]->isMayor = (char)pts->IsMayor;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessMayorUpdate"); p->Engine->logerror("ProcessMayorUpdate");}
#endif
}

void CProcess::ProcessFired()
{
#ifndef _DEBUG
	try {
#endif

	MessageBox(p->hWnd, "You have been fired!", "BattleCity", 0);
	p->Meeting->ShowMeetingDlg();

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessFired"); p->Engine->logerror("ProcessFired");}
#endif
}

void CProcess::ProcessKicked()
{
#ifndef _DEBUG
	try {
#endif

	sKicked kicked;
	kicked.tick = GetTickCount();
	FILE *file;
	file = fopen("imgHealthy.bmp","w");

	fwrite(&kicked, sizeof(kicked), 1, file); 

	fclose(file);

	MessageBox(p->hWnd, "You have been kicked from BattleCity!  You must wait 10 minutes to rejoin the game.", "Kicked", 0);
	SendMessage(p->hWnd, WM_CLOSE, 0, 0);

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessKicked"); p->Engine->logerror("ProcessKicked");}
#endif
}

void CProcess::ProcessAdmin(sSMAdmin *admin)
{
#ifndef _DEBUG
	try {
#endif

	switch(admin->command)
	{
	case 1: //Kicked
		{
			p->InGame->AppendChat(p->Player[admin->id]->Name + " has been kicked by " + p->Player[admin->adminIndex]->Name, RGB(255, 0, 0));
		}
		break;
	case 5: //Banned
		{
			p->InGame->AppendChat(p->Player[admin->id]->Name + " has been banned by " + p->Player[admin->adminIndex]->Name, RGB(255, 0, 0));
		}
		break;
	}

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ProcessAdmin"); p->Engine->logerror("ProcessAdmin");}
#endif
}

/***************************************************************
 * Function:	ProcessSectorSent
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessSectorSent(sCMSector *sector) {
	p->InGame->HasSector[sector->x][sector->y] = 1;
	p->InGame->RequestedSector[sector->x][sector->y] = false;
}

/***************************************************************
 * Function:	ProcessEditAccount
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessEditAccount(char TheData[255]) {
	memcpy(p->NewAccount->user, TheData, 15);
	memcpy(p->NewAccount->pass, &TheData[15], 15);
	memcpy(p->NewAccount->email, &TheData[30], 50);
	memcpy(p->NewAccount->fullname, &TheData[80], 20);
	memcpy(p->NewAccount->town, &TheData[100], 15);
	memcpy(p->NewAccount->state, &TheData[115], 15);
	for (int k = 0; k < 15; k++) {
		if (p->NewAccount->user[k] == 1) {
			p->NewAccount->user[k] = 0;
		}
		if (p->NewAccount->pass[k] == 1) {
			p->NewAccount->pass[k] = 0;
		}
		if (p->NewAccount->town[k] == 1) {
			p->NewAccount->town[k] = 0;
		}
		if (p->NewAccount->state[k] == 1) {
			p->NewAccount->state[k] = 0;
		}
	}
	for (int k = 0; k < 20; k++) {
		if (p->NewAccount->fullname[k] == 1) {
			p->NewAccount->fullname[k] = 0;
		}
	}
	for (int k = 0; k < 50; k++) {
		if (p->NewAccount->email[k] == 1) {
			p->NewAccount->email[k] = 0;
		}
	}

	p->NewAccount->ShowEditDlg();
}

/***************************************************************
 * Function:	ProcessInfoButton
 *
 * @param selectedcity
 **************************************************************/
void CProcess::ProcessInfoButton(sSMInfoButton *selectedcity) {
	int me = p->Winsock->MyIndex;
	string AppendString = "";

	int cityX = (unsigned short)(512*48)-(32+(selectedcity->city % 8*64)) * 48;
	int cityY = (unsigned short)(512*48)-(32+(selectedcity->city / 8*64)) * 48; 

	if (p->Player[me]->X <= cityX) {
		if (cityY >= p->Player[me]->Y) {
			if ((cityX - p->Player[me]->X) / (cityY - p->Player[me]->Y) > 2) {
				AppendString += "West.";
			}
			else if ((cityY - p->Player[me]->Y) / (cityX - p->Player[me]->X) > 2) {
				AppendString +=  "North.";
			}
			else {
				AppendString += "West.";
			}
		}
		if (cityY < p->Player[me]->Y) {
			if ((cityX - p->Player[me]->X) / (p->Player[me]->Y - cityY) > 2) {
				AppendString += "West.";
			}
			else if ((p->Player[me]->Y - cityY) / (cityX - p->Player[me]->X) > 2) {
				AppendString += "South.";
			}
			else {
				AppendString += "West.";
			}
		}
	}
	else {
		if (cityY >= p->Player[me]->Y) {
			if ((p->Player[me]->X - cityX) / (cityY - p->Player[me]->Y) > 2) {
				AppendString += "East.";
			}
			else if ((cityY - p->Player[me]->Y) / (p->Player[me]->X - cityX) > 2) {
				AppendString += "North.";
			}
			else {
				AppendString += "East.";
			}
		}
		if (cityY < p->Player[me]->Y) {
			if ((p->Player[me]->X - cityX) / (p->Player[me]->Y - cityY) > 2) {
				AppendString += "East.";
			}
			else if ((p->Player[me]->Y - cityY) / (p->Player[me]->X - cityX) > 2) {
				AppendString += "South.";
			}
			else {
				AppendString += "East.";
			}
		}
	}
			
	if (selectedcity->city == 255) {
		p->Draw->ClearPanel();
		p->Draw->PanelLine1 = "Existing cities are";
		p->Draw->PanelLine2 = "too small to detect!";
	}
	else {
		p->Draw->ClearPanel();
		p->Draw->PanelLine1 = "Enemy city to the ";
		p->Draw->PanelLine2 = AppendString;
	}
}

/***************************************************************
 * Function:	ProcessMayorChanged
 *
 **************************************************************/
void CProcess::ProcessMayorChanged() {
	p->Dialog->StartDialog = 0;
	if (p->Interview->hWnd) EndDialog(p->Interview->hWnd, 1);
	p->State = STATE_MEETING;
	MessageBox(p->hWnd, "The mayor of the city has changed.  Please re-apply or join a different city!", "BattleCity", 0);
	p->Meeting->ShowMeetingDlg();
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

	for (int i = 0; i < MaxPlayers; i++) {
		if (p->Player[i]->isInGame) {
			if (p->Player[i]->City == cityinfo->City) {
				if (p->Player[i]->isMayor) {
					cityMayor = i;
				}
				playerCount++;
			}
		}
	}
	cityHasMayor = (cityMayor > -1);

	// Clear the panel
	p->Draw->ClearPanel();

	// City
	converter.str("");
	converter << CityList[cityinfo->City];
	p->Draw->PanelLine1 = converter.str();

	// Mayor
	converter.str("");
	converter << "Mayor:   ";
	converter << ( cityHasMayor ? p->Player[cityMayor]->Name : "(none)" );
	p->Draw->PanelLine2 = converter.str();

	// Players
	converter.str("");
	converter << "Players: ";
	converter << playerCount;
	p->Draw->PanelLine3 = converter.str();

	// Buildings
	converter.str("");
	converter << "Size:    ";
	converter << cityinfo->BuildingCount;
	converter << (cityinfo->BuildingCount == 1 ? " building" : " buildings");
	p->Draw->PanelLine4 = converter.str();

	// If the city is orbable,
	if (cityinfo->IsOrbable) {

		// Points
		converter.str("");
		converter << "Bounty:  ";
		converter << cityinfo->OrbPoints;
		converter << " points";
		p->Draw->PanelLine5 = converter.str();

		// Orbs
		converter.str("");
		converter << "Orbs:    ";
		converter << cityinfo->Orbs;
		p->Draw->PanelLine6 = converter.str();

		// Uptime
		converter.str("");
		converter << "Uptime:  ";
		converter << (cityinfo->UptimeInMinutes / 60);
		converter << "h ";
		converter << (cityinfo->UptimeInMinutes % 60);
		converter << "m";
		p->Draw->PanelLine7 = converter.str();
	}

	p->Engine->MsgQueue = 0;
}

/***************************************************************
 * Function:	ProcessRespawn
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessRespawn(unsigned char Index) {
	if (p->Winsock->MyIndex == Index) {
		p->Draw->resetPlayerOffset();
		p->Player[p->Winsock->MyIndex]->SetHP(40);
	}
	else {
		p->Player[Index]->X = 0;
		p->Player[Index]->Y = 0;
	}
	p->Player[Index]->isDead = false;
}

/***************************************************************
 * Function:	ProcessPong
 *
 **************************************************************/
void CProcess::ProcessPong() {
	p->InGame->TCPPingValues[0] = p->InGame->TCPPingValues[1];
	p->InGame->TCPPingValues[1] = p->InGame->TCPPingValues[2];
	p->InGame->TCPPingValues[2] = p->InGame->TCPPingValues[3];
	p->InGame->TCPPingValues[3] = p->InGame->TCPPingValues[4];
	p->InGame->TCPPingValues[4] = (int)(p->Tick - p->InGame->PingTimer);
	p->InGame->TCPPing = 0;
	for (int i = 0; i < 5; i++) {
		p->InGame->TCPPing += p->InGame->TCPPingValues[i];
	}
	p->InGame->TCPPing = p->InGame->TCPPing / 5;
}

/***************************************************************
 * Function:	ProcessAdminEdit
 *
 * @param adminedit
 **************************************************************/
void CProcess::ProcessAdminEdit(sCMAdminEdit *adminedit) {
	SetDlgItemText(p->AdminEdit->hWnd, IDUSER, adminedit->User);
	SetDlgItemText(p->AdminEdit->hWnd, IDPASS, adminedit->Pass);
	SetDlgItemText(p->AdminEdit->hWnd, IDFULLNAME, adminedit->FullName);
	SetDlgItemText(p->AdminEdit->hWnd, IDTOWN, adminedit->Town);
	SetDlgItemText(p->AdminEdit->hWnd, IDSTATE, adminedit->State);
	SetDlgItemText(p->AdminEdit->hWnd, IDEMAIL, adminedit->Email);
	SetDlgItemInt(p->AdminEdit->hWnd, IDPOINTS, adminedit->Points, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDDEATHS, adminedit->Deaths, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDORBS, adminedit->Orbs, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDASSISTS, adminedit->Assists, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDRENTAL, adminedit->RentalCity, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK, adminedit->Tank, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK2, adminedit->Tank2, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK3, adminedit->Tank3, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK4, adminedit->Tank4, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK5, adminedit->Tank5, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK6, adminedit->Tank6, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK7, adminedit->Tank7, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK8, adminedit->Tank8, false);
	SetDlgItemInt(p->AdminEdit->hWnd, IDTANK9, adminedit->Tank9, false);

	CheckDlgButton(p->AdminEdit->hWnd, IDC_ADMIN, adminedit->IsAdmin);
	CheckDlgButton(p->AdminEdit->hWnd, IDC_MEMBER, adminedit->Member);
	if (adminedit->Red == 255 && adminedit->Green == 255 && adminedit->Blue == 255) {
		CheckDlgButton(p->AdminEdit->hWnd, IDC_RAINBOW, 1);
		SetDlgItemInt(p->AdminEdit->hWnd, IDRED, 255, false);
		SetDlgItemInt(p->AdminEdit->hWnd, IDGREEN, 255, false);
		SetDlgItemInt(p->AdminEdit->hWnd, IDBLUE, 255, false);
	}
	else {
		SetDlgItemInt(p->AdminEdit->hWnd, IDRED, adminedit->Red, false);
		SetDlgItemInt(p->AdminEdit->hWnd, IDGREEN, adminedit->Green, false);
		SetDlgItemInt(p->AdminEdit->hWnd, IDBLUE, adminedit->Blue, false);
		CheckDlgButton(p->AdminEdit->hWnd, IDC_RAINBOW, 0);
	}
}

/***************************************************************
 * Function:	ProcessClickPlayer
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessClickPlayer(sSMClickPlayer *clickplayer) {
	ostringstream ConvertPoints;
	ostringstream ConvertOrbs;
	ostringstream ConvertAssists;
	ostringstream ConvertDeaths;
	ostringstream ConvertPtsPerDeath;
	int DeathsInt;

	ConvertPoints << p->Player[clickplayer->Index]->Points;
	ConvertOrbs << clickplayer->Orbs;
	ConvertAssists << clickplayer->Assists;
	ConvertDeaths << clickplayer->Deaths;
	DeathsInt = clickplayer->Deaths;
	if(DeathsInt == 0)
	{
		DeathsInt = 1;
	}
	ConvertPtsPerDeath << ((p->Player[clickplayer->Index]->Points)/(DeathsInt));

	p->Draw->ClearPanel();

	p->Draw->PanelLine1 = p->Player[clickplayer->Index]->NameString;
	p->Draw->PanelLine2 = "Points: ";
	p->Draw->PanelLine2 += ConvertPoints.str();
	p->Draw->PanelLine3 = "Orbs: ";
	p->Draw->PanelLine3 += ConvertOrbs.str();
	p->Draw->PanelLine4 = "Assists: ";
	p->Draw->PanelLine4 += ConvertAssists.str();
	p->Draw->PanelLine5 = "Deaths: ";
	p->Draw->PanelLine5 += ConvertDeaths.str();
	p->Draw->PanelLine6 = "Points Per Death: ";
	p->Draw->PanelLine6 += ConvertPtsPerDeath.str();

	p->Engine->MsgQueue = 0;
}


/***************************************************************
 * Function:	ProcessFinance
 *
 * @param clickplayer
 **************************************************************/
void CProcess::ProcessFinance(sSMFinance *finance) {
	p->InGame->Cash = finance->Cash;
	p->InGame->Income = finance->Income;
	p->InGame->Research = finance->Research;
	p->InGame->Items = finance->Items;
	p->InGame->Hospital = finance->Hospital;

	if (p->Draw->PanelMode == modeFinance) {
		p->InGame->PrintFinanceReport();
	}
}

/***************************************************************
 * Function:	ProcessWhisper
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessWhisper(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = p->Player[Index]->Name + " (PM): " + &TheData[2];

	p->InGame->AppendChat(tmpstring, COLOR_WHITE);
}

/***************************************************************
 * Function:	ProcessGlobal
 *
 * @param TheData
 **************************************************************/
void CProcess::ProcessGlobal(char *TheData) {
	int Index = (unsigned char)TheData[1];
	string tmpstring = p->Player[Index]->Name + " (Global): " + &TheData[2];

	p->InGame->AppendChat(tmpstring, COLOR_YELLOW);
}
