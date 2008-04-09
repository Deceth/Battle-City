#include "CInGame.h"

CInGame::CInGame(CGame *game)
{
	p = game;
	IsBuilding = 0;
	Cash = 0;
	ShowBuildMenu = 0;
	chatLine1.clear();
	chatLine2.clear();
	chatLine3.clear();
	chatLine4.clear();
	chatLine5.clear();
	chatLine6.clear();
	chatLine7.clear();
	chatLine8.clear();
	ChatLine.clear();
	IsChatting = 0;
	ShowMap = 0;
	Successor = 0;
	HasLaser = 0;
	HasRocket = 0;
	HasUpLink = 0;
	updateTick = 0;
	lastTick = 0;
	hospTick = 0;
	TCPPing = 0;
	memset(TCPPingValues, 0, sizeof(TCPPingValues));
	PingTick = 0;
	PingTimer = 0;
	Minimized = 0;
	timeDeath = 0;
	timeLastAttack = 0;

	for (int i = 0; i < 27; i++)
	{
		CanBuild[i] = 0;
	}

	memset(HasSector, 0, sizeof(HasSector));
	memset(RequestedSector, 0, sizeof(RequestedSector));

}

CInGame::~CInGame() {

}

void CInGame::Cycle() {
	float curTick = p->Tick;

	// For each possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {

		// Run the Player cycle
		p->Player[j]->Cycle();
	}

	// Run the Bullet cycle
	p->Bullet->Cycle();
	p->Explode->Cycle();
	p->Item->Cycle();
	p->Build->Cycle();
	p->Inventory->Cycle();

	// Draw the game
	p->Draw->DrawGame();

	// If the player update timer is up, and this player is still alive,
	if ( (curTick > updateTick) && (p->Player[p->Winsock->MyIndex]->isDead == false)) {
		sCMUpdate packet;
		int me = p->Winsock->MyIndex;
		p->Player[me]->SectorX = (char)((p->Player[me]->X/48) / SectorSize);
		p->Player[me]->SectorY = (char)((p->Player[me]->Y/48) / SectorSize);
		packet.x = (int)p->Player[me]->X;
		packet.y = (int)p->Player[me]->Y;
		packet.dir = (unsigned char)p->Player[me]->Direction;
		packet.move = (unsigned char)(p->Player[me]->isMoving + 1);
		packet.turn = (unsigned char)(p->Player[me]->isTurning + 1);
		p->Winsock->SendData(cmUpdate, (char *)&packet, sizeof(packet));
		updateTick = curTick + 150;
	}

	// If the Ping timer is up,
	if (curTick > PingTick) {
		this->CheckRefVariables();
		p->Winsock->SendData(cmTCPPing, " ");
		this->PingTimer = p->Tick;
		this->PingTick = curTick + 5000;
	}

	// If the player is under attack,
	if (this->isUnderAttack) {

		// If the Under Attack timer is up,
		if (p->Tick > this->timeUnderAttack) {

			// Turn off Under Attack
			this->setIsUnderAttack(false);
		}
	}

	this->lastTick = p->Tick;
}

string CInGame::ReturnRank(int Points)
{
	string rank;

	if (Points < 100) {
		rank = "Private";
	}
	else if (Points < 200) {
		rank = "Corporal";
	}
	else if (Points < 500) {
		rank = "Sergeant";
	}
	else if (Points < 1000) {
		rank = "Sergeant Major";
	}
	else if (Points < 2000) {
		rank = "Lieutenant";
	}
	else if (Points < 4000) {
		rank = "Captain";
	}
	else if (Points < 8000) {
		rank = "Major";
	}
	else if (Points < 16000) {
		rank = "Colonel";
	}
	else if (Points < 30000) {
		rank = "Brigadier";
	}
	else if (Points < 45000) {
		rank = "General";
	}
	else if (Points < 60000) {
		rank = "Baron";
	}
	else if (Points < 80000) {
		rank = "Earl";
	}
	else if (Points < 100000) {
		rank = "Count";
	}
	else if (Points < 125000) {
		rank = "Duke";
	}
	else if (Points < 150000) {
		rank = "Archduke";
	}
	else if (Points < 200000) {
		rank = "Grand Duke";
	}
	else if (Points < 250000) {
		rank = "Lord";
	}
	else if (Points < 300000) {
		rank = "Chancellor";
	}
	else if (Points < 350000) {
		rank = "Royaume";
	}
	else if (Points < 400000) {
		rank = "Emperor";
	}
	else if (Points < 500000) {
		rank = "Auror";
	}
	else {
		rank = "King";
	}

	return rank;
}

void CInGame::PrintWhoData()
{
		string Players;
		int PlayerCount = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (p->Player[i]->Name.length() > 0)
			{
				PlayerCount++;
				if (Players.length() == 0)
					Players += p->Player[i]->Name;
				else
				{
					Players += ", ";
					Players += p->Player[i]->Name;
				}

			}
		}

		ostringstream Converter;
		Converter << "There ";
		Converter << (PlayerCount == 1 ? "is " : "are ");
		Converter << PlayerCount;
		Converter << (PlayerCount == 1 ? " player" : " players");
		Converter << " currently online:  ";
		Converter << Players;
		AppendChat(Converter.str(), COLOR_SYSTEM);
}

void CInGame::AppendChat(string ChatText, COLORREF color)
{
	unsigned int MessageLength = this->p->Draw->chatBarWidth;
	while (ChatText.length() > MessageLength) {
		AppendLine(ChatText.substr(0, MessageLength), color);
		ChatText = ChatText.substr(MessageLength, ChatText.length());
	}

	AppendLine(ChatText, color);
}

void CInGame::AppendLine(string ChatText, COLORREF color)
{
	chatLine1 = chatLine2;
	chatLine2 = chatLine3;
	chatLine3 = chatLine4;
	chatLine4 = chatLine5;
	chatLine5 = chatLine6;
	chatLine6 = chatLine7;
	chatLine7 = chatLine8;
	chatLine8 = ChatText;
	chatColor1 = chatColor2;
	chatColor2 = chatColor3;
	chatColor3 = chatColor4;
	chatColor4 = chatColor5;
	chatColor5 = chatColor6;
	chatColor6 = chatColor7;
	chatColor7 = chatColor8;
	chatColor8 = color;	
}

void CInGame::ClearOut() {
	CBuilding *bld;

	p->Draw->ClearDrawing();

	lastTick = 0;
	updateTick = 0;

	ShowBuildMenu = 0;
	Successor = 0;
	p->Personnel->DenyApplicants = 0;
	IsBuilding = 0;
	Cash = 0;

	HasLaser = 0;
	HasRocket = 0;

	timeDeath = 0;
	timeLastAttack = 0;

	chatLine1.clear();
	chatLine2.clear();
	chatLine3.clear();
	chatLine4.clear();
	chatLine5.clear();
	ChatLine.clear();

	IsChatting = 0;
	BombsAreActivated = 0;

	p->Personnel->DenyApplicants = 0;

	for (int i = 0; i < 27; i++) {
		CanBuild[i] = 0;
	}

	// For each building,
	bld = this->p->Build->buildingListHead;
	while (bld) {

		// If the city is not a CC, delete it (which gets the next building)
		if (bld->isCC() == false) {
			bld = this->p->Build->delBuilding(bld);
		}

		// Else (didn't delete), get the next building
		else {
			bld = bld->next;
		}
	}

	// For each item,
	while (this->p->Item->itemListHead) {

		// Delete the item
		this->p->Item->delItem(this->p->Item->itemListHead);
	}

	// For each explosion,
	while (this->p->Explode->explosionListHead) {

		// Delete the explosion
		this->p->Explode->delExplosion(this->p->Explode->explosionListHead);
	}

	// For each bullet,
	while (this->p->Bullet->bulletListHead) {

		// Delete the bullet
		this->p->Bullet->delBullet(this->p->Bullet->bulletListHead);
	}

	// For each inventory item,
	while (this->p->Inventory->itemListHead) {

		// Delete the item
		this->p->Inventory->delItem(this->p->Inventory->itemListHead);
	}
	p->Inventory->SelectedItemType = 0;

	// For each possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {

		// Clear the player from the game
		p->Player[j]->InGameClear();
	}

	// Clear all sectors
	p->InGame->ClearAllSectors();
}

void CInGame::CheckRefVariables()
{
	int me = p->Winsock->MyIndex;
	int HP = p->Player[me]->HP;
	int HP1 = (HP^2-31)*2;
	int HP2 = -(HP);

	if (p->Player[me]->refHP1 != HP1)
	{
		p->Winsock->SendData(cmBan, "refHP1gdmde");
		p->Process->ProcessKicked();
		return;
	}

	if (p->Player[me]->refHP2 != HP2)
	{
		p->Winsock->SendData(cmBan, "refHP2gdmde");
		p->Process->ProcessKicked();
		return;
	}

	CheckCheatTools();
}

void CInGame::CheckCheatTools()
{
	HWND wnd = 0;
	wnd = FindWindow(0 , "WPE PRO");
	if (wnd > 0)
	{
		p->Winsock->SendData(cmBan, "wsckpkedtrpro");
		p->Process->ProcessKicked();
	}

	wnd = FindWindow(0 , "TSearch");
	if (wnd > 0)
	{
		p->Winsock->SendData(cmBan, "tsch");
		p->Process->ProcessKicked();
	}
}

/***************************************************************
 * Function:	RefreshArea
 *
 **************************************************************/
void CInGame::RefreshArea() {
	bool requestedSector = false;
	char me = p->Winsock->MyIndex;
	sCMSector sector;

	// For each sector in the nearby grid of 9 sectors,
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {

			// If the sector is on the map,
			if (
				((p->Player[me]->SectorX+i) >= 0)
				&&
				((p->Player[me]->SectorX+i) <= MaxSectors) 
				&&
				((p->Player[me]->SectorY+j) >= 0)
				&&
				((p->Player[me]->SectorY+j) <= MaxSectors)
			) {

				// If the player doesn't have the sector, and hasn't already requested it,
				if (
					(! this->HasSector[(p->Player[me]->SectorX+i)][(p->Player[me]->SectorY+j)])
					&&
					(! this->RequestedSector[(p->Player[me]->SectorX+i)][(p->Player[me]->SectorY+j)])
				) {

					// Get the bounds of the sector
					sector.x = p->Player[me]->SectorX + i;
					sector.y = p->Player[me]->SectorY + j;

					ClearSector(sector.x, sector.y);
					this->RequestedSector[sector.x][sector.y] = true;
					p->Winsock->SendData(cmRequestSector, (char *)&sector, sizeof(sector));
					requestedSector = true;
				}
			}
		}
	}

	// For each sector,
	for (int i = 0; i <= MaxSectors; i++) {
		for (int j = 0; j <= MaxSectors; j++) {

			// If the player has the sector,	
			if( this->HasSector[i][j] ) {

				// If the player is no longer near the sector, clear it
				if ((abs(i - p->Player[me]->SectorX) > 1) | (abs(j - p->Player[me]->SectorY) > 1)) {
					ClearSector(i, j);
				}
			}
		}
	}

	// If we requested a sector, and the minimap is open, 
	if ((requestedSector==true) && (p->InGame->ShowMap == 1)) {

		// Request the map
		p->Winsock->SendData(cmMiniMap, " ");
	}
}

/***************************************************************
 * Function:	ClearSector
 *
 * @param SectorX the X position of the sector
 * @param SectorY the Y position of the sector
 **************************************************************/
void CInGame::ClearSector(int SectorX, int SectorY) {
	CBuilding *bld;
	CItem *itm;

	// If the sector is on the map, and the minimap is not open,
	if (SectorX >= 0 && SectorY >= 0 && SectorX <= MaxSectors && SectorY <= MaxSectors && p->InGame->ShowMap == 0) {
		p->InGame->HasSector[SectorX][SectorY] = 0;
		p->InGame->RequestedSector[SectorX][SectorY] = false;

		// For each item,
		itm = p->Item->itemListHead;
		while (itm) {

			// If the item is in the sector,
			if ((itm->X / SectorSize) == SectorX && (itm->Y / SectorSize) == SectorY) {

				// Delete the item (from the client)
				itm = p->Item->delItem(itm);
			}

			// Else (item not in the sector), get the next item
			else {
				itm = itm->next;
			}
		}

		// For each building,
		bld = p->Build->buildingListHead;
		while (bld) {

			// If the building is in the sector (and not a CC),
			if ((bld->X / SectorSize) == SectorX && (bld->Y / SectorSize) == SectorY && bld->Type != 6) {

				// Delete the item (from the client)
				bld = p->Build->delBuilding(bld);
			}

			// Else (building is not in the sector),
			else {

				// Get the next building
				bld = bld->next;
			}
		}
	}
}

/***************************************************************
 * Function:	ClearAllSectors
 *
 **************************************************************/
void CInGame::ClearAllSectors() {
	CBuilding *bld;
	CItem *itm;

	memset(HasSector, 0, sizeof(HasSector));
	memset(RequestedSector, 0, sizeof(RequestedSector));

	// For each item,
	itm = p->Item->itemListHead;
	while (itm) {

		// Delete the item (from the client)
		itm = p->Item->delItem(itm);
	}

	// For each building,
	bld = p->Build->buildingListHead;
	while (bld) {

		// If the is not a CC,
		if (bld->isCC() == false) {

			// Delete the building (from the client)
			bld = p->Build->delBuilding(bld);
		}

		// Else (building is not in the sector),
		else {

			// Get the next building
			bld = bld->next;
		}
	}
}

/***************************************************************
 * Function:	PrintFinanceReport
 *
 **************************************************************/
void CInGame::PrintFinanceReport() {
	long netIncome = this->getGrossIncome();

	p->Draw->ClearPanel();

	p->Draw->PanelMode = modeFinance;
	p->Draw->PanelLine1 = "City Finance Report:";
	
	p->Draw->PanelLine2 = "Income: ";
	p->Draw->PanelLine2 += p->Draw->CashFormat(p->InGame->Income);

	p->Draw->PanelLine3 = "Research: -";
	p->Draw->PanelLine3 += p->Draw->CashFormat(p->InGame->Research);

	p->Draw->PanelLine4 = "Items: -";
	p->Draw->PanelLine4 += p->Draw->CashFormat(p->InGame->Items);

	p->Draw->PanelLine5 = "Hospitals: -";
	p->Draw->PanelLine5 += p->Draw->CashFormat(p->InGame->Hospital);

	p->Draw->PanelLine6 = "Total: ";
	if (netIncome < 0) {
		p->Draw->PanelLine6 += "-";
	}
	p->Draw->PanelLine6 += p->Draw->CashFormat(abs(netIncome));
}

/***************************************************************
 * Function:	getGrossIncome
 *
 **************************************************************/
int CInGame::getGrossIncome() {
	return this->Income - this->Hospital - this->Items - this->Research;
}

/***************************************************************
 * Function:	setIsUnderAttack
 *
 * @param isUnderAttack
 **************************************************************/
void CInGame::setIsUnderAttack(bool isUnderAttack) {

	// If isUnderAttack is true, set isUnderAttack true and timeUnderAttack
	if (isUnderAttack) {
		this->isUnderAttack = true;
		this->timeUnderAttack = this->p->Tick + TIMER_UNDER_ATTACK;
	}

	// Else (isUnderAttack is false), set isUnderAttack false and timeUnderAttack 0
	else {
		this->isUnderAttack = false;
		this->timeUnderAttack = 0;
	}

}
