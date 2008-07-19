#include "CInGame.h"

/***************************************************************
 * Constructor
 *
 **************************************************************/
CInGame::CInGame(CGame *game) {
	this->p = game;
	this->IsBuilding = 0;
	this->Cash = 0;
	this->ShowBuildMenu = 0;

	// Chat lines
	for (int i=0; i<MAX_CHAT_LINES; i++) { 
		this->chatLines[i].clear();
	}
	numChatLines = 0;
	this->ChatLine.clear();

	// Info lines
	for (int i=0; i<MAX_INFO_LINES; i++) { 
		this->infoLines[i].clear();
	}

	this->IsChatting = 0;
	this->ShowMap = 0;
	this->Successor = 0;
	this->HasLaser = 0;
	this->HasCloak = 0;
	this->HasRocket = 0;
	this->HasMedkit = 0;
	this->HasBomb = 0;
	this->HasMine = 0;
	this->HasOrb = 0;
	this->HasFlare = 0;
	this->HasDFG = 0;
	this->HasWall = 0;
	this->HasTurret = 0;
	this->HasSleeper = 0;
	this->HasPlasma = 0;	
	this->updateTick = 0;
	this->lastTick = 0;
	this->hospTick = 0;
	this->TCPPing = 0;
	memset(this->TCPPingValues, 0, sizeof(this->TCPPingValues));
	this->PingTick = 0;
	this->PingTimer = 0;
	this->Minimized = 0;
	this->timeDeath = 0;
	this->timeLastAttack = 0;

	for (int i = 0; i < 27; i++) {
		this->CanBuild[i] = 0;
	}

	memset(this->HasSector, 0, sizeof(this->HasSector));
	memset(this->RequestedSector, 0, sizeof(this->RequestedSector));

	this->minVisibleChatLine=0;
	this->numVisibleChatLines=8;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CInGame::~CInGame() {
}

/***************************************************************
 * Function:	Cycle
 *
 **************************************************************/
void CInGame::Cycle() {
	float curTick = this->p->Tick;

	// For each possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {

		// Run the Player cycle
		this->p->Player[j]->Cycle();
	}

	// Run the Bullet cycle
	this->p->Bullet->Cycle();
	this->p->Explode->Cycle();
	this->p->Item->Cycle();
	this->p->Build->Cycle();
	this->p->Inventory->Cycle();

	// Draw the game
	this->p->Draw->DrawGame();

	// If the player update timer is up, and this player is still alive,
	if ( (curTick > updateTick) && (this->p->Player[this->p->Winsock->MyIndex]->isDead == false)) {
		sCMUpdate packet;
		int me = this->p->Winsock->MyIndex;
		this->p->Player[me]->SectorX = (char)((this->p->Player[me]->X/48) / SectorSize);
		this->p->Player[me]->SectorY = (char)((this->p->Player[me]->Y/48) / SectorSize);
		packet.x = (int)this->p->Player[me]->X;
		packet.y = (int)this->p->Player[me]->Y;
		packet.dir = (unsigned char)this->p->Player[me]->Direction;
		packet.move = (unsigned char)(this->p->Player[me]->isMoving + 1);
		packet.turn = (unsigned char)(this->p->Player[me]->isTurning + 1);
		this->p->Winsock->SendData(cmUpdate, (char *)&packet, sizeof(packet));
		this->updateTick = curTick + 150;
	}

	// If the Ping timer is up,
	if (curTick > PingTick) {
		this->CheckRefVariables();
		this->p->Winsock->SendData(cmTCPPing, " ");
		this->PingTimer = this->p->Tick;
		this->PingTick = curTick + 10000;
	}

	// If the player is under attack,
	if (this->isUnderAttack) {

		// If the Under Attack timer is up,
		if (this->p->Tick > this->timeUnderAttack) {

			// Turn off Under Attack
			this->setIsUnderAttack(false);
		}
	}

	this->lastTick = this->p->Tick;
}

/***************************************************************
 * Function:	ReturnRank
 *
 * @param Points
 **************************************************************/
string CInGame::ReturnRank(int Points) {
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

/***************************************************************
 * Function:	PrintWhoData
 *
 **************************************************************/
void CInGame::PrintWhoData() {
	stringstream Converter;
	string Players;
	CPlayer* player;
	int PlayerCount = 0;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game,
		if (p->Player[i]->Name.length() > 0) {
			PlayerCount++;
			if (Players.length() != 0) {
				Players += ", ";
			}
			Players += player->Name;
		}
	}

	Converter << PlayerCount << " player" << (PlayerCount == 1 ? "" : "s") << " online: " << Players;
	this->setPlayerInfo(Converter.str());
}

/***************************************************************
 * Function:	AppendChat
 *
 * @param chatText
 * @param color
 **************************************************************/
void CInGame::AppendChat(string chatText, COLORREF color) {
	unsigned int maxLineLength = this->p->Draw->chatBarWidth;
	string lineSplitOnSpace;
	int lineLength;
	bool isMoreText = true;

	while (isMoreText) {

		// Get the line split on space with a max length of maxLineLength
		lineSplitOnSpace = CUtil::getLineSplitOnSpace(chatText, maxLineLength);
		lineLength = lineSplitOnSpace.length();

		// Add the line
		this->AppendChatLine(lineSplitOnSpace, color);

		// Remove the added text from the chatText
		if (chatText.length() > lineLength) {
			chatText = chatText.substr(lineLength, chatText.length());
		}
		else {
			isMoreText = false;
		}
	}
}

/***************************************************************
 * Function:	AppendChatLine
 *
 * @param chatText
 * @param color
 **************************************************************/
void CInGame::AppendChatLine(string chatText, COLORREF color) {
	// For every chat line (working backwards so we don't copy the same string all the way forward),
	for (int i=this->numChatLines-1; i>0; i--) { 

		// Set it to the line before it
		this->chatLines[i] = this->chatLines[i-1];
		this->chatColors[i] = this->chatColors[i-1];
	}

	// Add the new chat line (at index 0)
	this->chatLines[0] = chatText;
	this->chatColors[0] = color;
	if (this->numChatLines < MAX_CHAT_LINES) {
		this->numChatLines++;
	}

	// If there are "history" chat lines,
	if (this->numChatLines > this->numVisibleChatLines) {

		// Alter the newest "history" line
		this->chatLines[this->numVisibleChatLines] = "> " + this->chatLines[this->numVisibleChatLines];
	}
}

/***************************************************************
 * Function:	AppendInfo
 *
 * @param infoText
 **************************************************************/
void CInGame::AppendInfo(string infoText) {
	this->AppendChat(infoText, COLOR_SYSTEM);
}
/***************************************************************
 * Function:	setPlayerInfo
 *
 * @param infoText
 **************************************************************/
void CInGame::setPlayerInfo(string infoText) {
	this->setPlayerInfo(infoText, 0);
}
/***************************************************************
 * Function:	setPlayerInfo
 *
 * @param infoText
 * @param lineIndex
 **************************************************************/
void CInGame::setPlayerInfo(string infoText, int lineIndex) {
	unsigned int maxLineLength = this->p->Draw->chatBarWidth;
	bool isMoreText = true;
	int lineLength;
	string lineSplitOnSpace;

	// For each info line,
	for (int i=0; i<MAX_INFO_LINES; i++) {

		// Clear the line
		this->infoLines[i].clear();

		// If there is info text left to add,
		if (isMoreText) {

			// Get the longest possible line, split on space
			lineSplitOnSpace = CUtil::getLineSplitOnSpace(infoText, maxLineLength);
			lineLength = lineSplitOnSpace.length();

			// Add the info text
			this->infoLines[i] = lineSplitOnSpace;

			// Remove the added text from the infoText
			if (infoText.length() > lineLength) {
				infoText = infoText.substr(lineLength, infoText.length());
			}
			else {
				isMoreText = false;
			}
		}
	}
}

/***************************************************************
 * Function:	ClearOut
 *
 **************************************************************/
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
	HasCloak = 0;
	HasRocket = 0;
	HasMedkit = 0;
	HasBomb = 0;
	HasMine = 0;
	HasOrb = 0;
	HasFlare = 0;
	HasDFG = 0;
	HasWall = 0;
	HasTurret = 0;
	HasSleeper = 0;
	HasPlasma = 0;

	timeDeath = 0;
	timeLastAttack = 0;

	// Chat lines
	for (int i=0; i<MAX_CHAT_LINES; i++) { 
		this->chatLines[i].clear();
	}
	// Info lines
	for (int i=0; i<MAX_INFO_LINES; i++) { 
		this->infoLines[i].clear();
	}

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

/***************************************************************
 * Function:	CheckRefVariables
 *
 **************************************************************/
void CInGame::CheckRefVariables() {
	int me = this->p->Winsock->MyIndex;
	int HP = this->p->Player[me]->HP;
	int HP1 = (HP^2-31)*2;
	int HP2 = -(HP);

	if (this->p->Player[me]->refHP1 != HP1) {
		this->p->Winsock->SendData(cmBan, "refHP1gdmde");
		this->p->Process->ProcessKicked();
		return;
	}

	if (this->p->Player[me]->refHP2 != HP2) {
		this->p->Winsock->SendData(cmBan, "refHP2gdmde");
		this->p->Process->ProcessKicked();
		return;
	}

	this->CheckCheatTools();

	this->SendCheatCheck();
}

/***************************************************************
 * Function:	CheckCheatTools
 *
 **************************************************************/
void CInGame::CheckCheatTools() {
	HWND wnd = 0;
	wnd = FindWindow(0 , "WPE PRO");
	if (wnd > 0) {
		p->Winsock->SendData(cmBan, "wsckpkedtrpro");
		p->Process->ProcessKicked();
	}

	wnd = FindWindow(0 , "TSearch");
	if (wnd > 0) {
		p->Winsock->SendData(cmBan, "tsch");
		p->Process->ProcessKicked();
	}

	wnd = FindWindow(0 , "Cheat Engine 5.3");
	if (wnd > 0) {
		p->Winsock->SendData(cmBan, "Cheat Engine");
		p->Process->ProcessKicked();
	}

	wnd = FindWindow(0 , "Cheat Engine 5.4");
	if (wnd > 0) {
		p->Winsock->SendData(cmBan, "Cheat Engine");
		p->Process->ProcessKicked();
	}

}



/***************************************************************
 * Function:	SendCheatCheck
 *
 **************************************************************/
void CInGame::SendCheatCheck() {
	sCMCheatCheck cheatCheck;

	cheatCheck.costBuilding = COST_BUILDING;
	cheatCheck.damageLaser = DAMAGE_LASER;
	cheatCheck.damageMine = DAMAGE_MINE;
	cheatCheck.damageRocket = DAMAGE_ROCKET;
	cheatCheck.maxHealth = MAX_HEALTH;
	cheatCheck.movementBullet = MOVEMENT_SPEED_BULLET;
	cheatCheck.movementPlayer = MOVEMENT_SPEED_PLAYER;
	cheatCheck.timerCloak = TIMER_CLOAK;
	cheatCheck.timerDfg = TIMER_DFG;
	cheatCheck.timerLaser = TIMER_SHOOT_LASER;
	cheatCheck.timerRocket = TIMER_SHOOT_ROCKET;
	cheatCheck.timerRespawn = TIMER_RESPAWN;

	this->p->Winsock->SendData(cmCheatCheck, (char *)&cheatCheck, sizeof(cheatCheck));
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
	
	p->Draw->PanelLine2 = "Income:    ";
	p->Draw->PanelLine2 += p->Draw->CashFormat(p->InGame->Income);

	p->Draw->PanelLine3 = "Research:  -";
	p->Draw->PanelLine3 += p->Draw->CashFormat(p->InGame->Research);

	p->Draw->PanelLine4 = "Items:     -";
	p->Draw->PanelLine4 += p->Draw->CashFormat(p->InGame->Items);

	p->Draw->PanelLine5 = "Hospitals: -";
	p->Draw->PanelLine5 += p->Draw->CashFormat(p->InGame->Hospital);

	p->Draw->PanelLine6 = "Total:     ";
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

/***************************************************************
 * Function:	createBuilding
 *
 * @param type
 * @param x
 * @param y
 * @param isAutoBuild
 **************************************************************/
bool CInGame::createBuilding(unsigned char type, unsigned short x, unsigned short y, bool isAutoBuild) {
	sCMBuild buildingPacket;
	int collision;

	// If the player is not an admin, check for conditions...
	if (! this->p->Player[this->p->Winsock->MyIndex]->isAdmin()) {

		// If the city can't afford a building, return
		if (this->Cash < COST_BUILDING) {
			return false;
		}

		// If there is a collision, return
		collision = this->p->Collision->CheckBuildingCollision(x, y);
		if (collision) {
			return false;
		}
		
		// If the building is not in the city's build range, return
		if (! this->p->Build->inRange(false)) {
			return false;
		}

		// If the building is not on the map, return
		if ((x < 0) || (x > 510) || (y < 0) || (y > 510)) {
			return false;
		}
	}

	buildingPacket.type = type;
	buildingPacket.x = x;
	buildingPacket.y = y;
	buildingPacket.isAutoBuild = isAutoBuild;
	this->p->Winsock->SendData(cmBuild,(char *)&buildingPacket,sizeof(buildingPacket));

	return true;
}

/***************************************************************
 * Function:	isValidCityIndex
 *
 **************************************************************/
bool CInGame::isValidCityIndex(int city) {
	return ((city>-1) && (city<MAX_CITIES));
}