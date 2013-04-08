#include "CGame.h"

/***************************************************************
 * Constructor
 *
 * @param game
 **************************************************************/
CDrawing::CDrawing(CGame *game) {
	p = game;
	this->ClearDrawing();
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CDrawing::~CDrawing() {

}

/***************************************************************
 * Function:	DrawGame
 *
 **************************************************************/
void CDrawing::DrawGame() {
	this->DrawMap();
	this->DrawBuildings();
	this->DrawBullets();
	this->DrawItems();
	this->DrawPlayers();
	this->DrawPanel();
	this->DrawTank();
	this->DrawExplosions();
	this->DrawRadar();
	this->DrawChat();
	this->DrawInfo();
	this->DrawInventory();

	// If the player is Mayor,
	if (p->Player[p->Winsock->MyIndex]->isMayor) {

		// If the build menu is on, draw the build menu
		if (p->InGame->ShowBuildMenu) {
			this->DrawBuildMenu();
		}

		// If the player has a building or demolish on the cursor, draw the building/demolish
		if (p->InGame->IsBuilding != 0) {
			this->DrawBuildPlacement();
		}
	}

	// If the minimap is on, draw the minimap
	if (p->InGame->ShowMap == 1) {
		this->DrawMiniMap();
	}
}

/***************************************************************
 * Function:	ClearDrawing
 *
 **************************************************************/
void CDrawing::ClearDrawing() {

	// Set the map width and height based on resolution
	this->MaxMapX = p->ResolutionX - 200;
	this->MaxMapY = p->ResolutionY;

	// 1024
	if (p->Options->resolution1024) {
		this->defaultPlayerOffsetX = (this->MaxMapX / 2) - 24;
		this->defaultPlayerOffsetY = (this->MaxMapY / 2) - 24;
		this->groundTilesToDraw = 7;
		this->tilesToDraw = 18;
		this->demoCursorOffsetX = 4;
		this->demoCursorOffsetY = -12;
		this->buildingCursorOffsetX = this->demoCursorOffsetX - 48;
		this->buildingCursorOffsetY = this->demoCursorOffsetY - 48 - 12;
		this->minimapOffsetX = 30;
		this->minimapOffsetY = 25;
		this->clickOffsetX = 9;
		this->clickOffsetY = 9;
		this->chatBarWidth = 103;
		//this->chatBarWidth = 51;
	}
	// 800
	else {
		this->defaultPlayerOffsetX = (this->MaxMapX / 2) - 24;
		this->defaultPlayerOffsetY = (this->MaxMapY / 2) - 24;
		this->groundTilesToDraw = 6;
		this->tilesToDraw = 14;
		this->demoCursorOffsetX = -12;
		this->demoCursorOffsetY = 4;
		this->buildingCursorOffsetX = this->demoCursorOffsetX - 48;
		this->buildingCursorOffsetY = this->demoCursorOffsetY - 48 - 16;
		this->minimapOffsetX = 7;
		this->minimapOffsetY = 7;
		this->clickOffsetX = 7;
		this->clickOffsetY = 7;
		this->chatBarWidth = 75;
		//this->chatBarWidth = 37;
	}

	arrowTick = 0;
	lastArrow = 0;

	BuildMenuX = 0;
	BuildMenuY = 0;
}

/***************************************************************
 * Function:	DrawMap
 *
 **************************************************************/
void CDrawing::DrawMap() {
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int goffX = (int) (p->Player[me]->X + this->PlayerOffsetX) % 128;
	int goffY = (int) (p->Player[me]->Y + this->PlayerOffsetY) % 128;
	int myX = (int)p->Player[me]->X / 48;
	int myY = (int)p->Player[me]->Y / 48;
	int i = 0;
	int j = 0;
	int tileX;
	int tileY;

	// Draw the ground tiles (groundTilesToDraw+1 total)
	for (i = -this->groundTilesToDraw; i < this->groundTilesToDraw; i++) {
		for (j = -this->groundTilesToDraw; j < this->groundTilesToDraw; j++) {
			p->DDraw->Draw(p->DDraw->imgGround, 128*i+goffX, 128*j+goffY, 144, 144, 0, 0, MaxMapX, MaxMapY);
		}
	}

	// Draw landscape and CC tiles on top of the ground (tilesToDraw^2 total)
	for (i = -this->tilesToDraw; i<this->tilesToDraw; i++) {
		for (j = -this->tilesToDraw; j<this->tilesToDraw; j++) {
			tileX = myX + i;
			tileY = myY + j;

			// If the tile is on the map,
			if (tileX > 0 && tileY > 0 && tileX <= 512 && tileY <= 512) {

				// If the map square is Lava, draw Lava
				if (p->Map->map[tileX][tileY] == MAP_SQUARE_LAVA) {
					p->DDraw->Draw(p->DDraw->imgLava, (myX-tileX)*48+this->PlayerOffsetX+offX, (myY-tileY)*48+this->PlayerOffsetY+offY, 48, 48, p->Map->tiles[tileX][tileY], 0, MaxMapX, MaxMapY);
				}
				// Else if the map square is Rock, draw Rock
				else if (p->Map->map[tileX][tileY] == MAP_SQUARE_ROCK) {
					p->DDraw->Draw(p->DDraw->imgRocks, (myX-tileX)*48+this->PlayerOffsetX+offX, (myY-tileY)*48+this->PlayerOffsetY+offY, 48, 48, p->Map->tiles[tileX][tileY], 0, MaxMapX, MaxMapY);	
				}

				// If the player does not have the sector loaded,
				if (p->InGame->HasSector[(tileX / SectorSize)][(tileY / SectorSize)] == 0) {
					p->DDraw->Draw(p->DDraw->imgLoading, (myX-tileX)*48+this->PlayerOffsetX+offX, (myY-tileY)*48+this->PlayerOffsetY+offY, 48, 48, 0, 0, MaxMapX, MaxMapY);
				}
			}

			// Else (tile is off the map), paint it black
			else {
				p->DDraw->DFillRect((myX-tileX)*48+this->PlayerOffsetX+offX, (myY-tileY)*48+this->PlayerOffsetY+offY, 48, 48);
			}
		}
	}
}

/***************************************************************
 * Function:	DrawPanel
 *
 **************************************************************/
void CDrawing::DrawPanel() {

	// Draw the top half of the interface
	p->DDraw->Draw(p->DDraw->imgInterface, MaxMapX, 0, 200, 430);

	// Draw the bottom half of the interface
	p->DDraw->Draw(p->DDraw->imgInterfaceBottom, MaxMapX, 430, 200, 170);

	//If the player is Mayor,
	if (p->Player[p->Winsock->MyIndex]->isMayor) {

		// Draw the money box
		p->DDraw->Draw(p->DDraw->imgMoneyBox, MaxMapX+2, 224, 120, 19);

		// If the city is losing money, draw moneyDown and the total in red
		if (p->InGame->getGrossIncome() < 0) {
			p->DDraw->Draw(p->DDraw->imgMoneyDown, MaxMapX+8, 225, 10, 14);
			p->DDraw->DTextOut(MaxMapX+21, 226, CashFormat(p->InGame->Cash), COLOR_RED);
		}
		// Else (city is gaining money), draw moneyUp and the total in green
		else {
			p->DDraw->Draw(p->DDraw->imgMoneyUp, MaxMapX+8, 226, 10, 14);
			p->DDraw->DTextOut(MaxMapX+21, 226, CashFormat(p->InGame->Cash), COLOR_GREEN);
		}

		// If the mayor has an applicant,
		if (p->Personnel->ApplicantWaiting) {

			// Make the Staff button flash on a timer
			if (p->Personnel->ApplicantAnim == 1) {
				p->DDraw->Draw(p->DDraw->imgBtnStaff, MaxMapX + 147, 269, 42, 19);
			}
			if (p->Tick > p->Personnel->ApplicantTick) {
				p->Personnel->ApplicantAnim = 1 - p->Personnel->ApplicantAnim;
				p->Personnel->ApplicantTick = p->Tick + 500;
			}
		}
	}

	// Draw the Arrow
	this->DrawArrow();

	// Draw the Health,
	this->DrawHealth();

	// If the player is an admin,
	if (p->Player[p->Winsock->MyIndex]->isAdmin()) {

		// Draw the staff button as the Admin button
		p->DDraw->Draw(p->DDraw->imgBtnStaff, MaxMapX + 110, 460, 42, 19);
	}

	// Display Panel-Text lines 1 through 7
	p->DDraw->DTextOut(MaxMapX + 12, 465, PanelLine1, COLOR_PANEL_HEAD);
	p->DDraw->DTextOut(MaxMapX + 12, 480, PanelLine2, COLOR_PANEL_BODY);
	p->DDraw->DTextOut(MaxMapX + 12, 495, PanelLine3, COLOR_PANEL_BODY);
	p->DDraw->DTextOut(MaxMapX + 12, 510, PanelLine4, COLOR_PANEL_BODY);
	p->DDraw->DTextOut(MaxMapX + 12, 525, PanelLine5, COLOR_PANEL_BODY);
	p->DDraw->DTextOut(MaxMapX + 12, 540, PanelLine6, COLOR_PANEL_BODY);
	p->DDraw->DTextOut(MaxMapX + 12, 555, PanelLine7, COLOR_PANEL_BODY);
}

/***************************************************************
 * Function:	DrawTank
 *
 **************************************************************/
void CDrawing::DrawTank() {
	int me = p->Winsock->MyIndex;
	int tankImage;

	// If the player is dead,
	if (p->Player[me]->isDead == true) {
		std::ostringstream convert;
		int timeleft = (int)(((int)(p->InGame->timeDeath + TIMER_RESPAWN) - p->Tick) / 1000);
		if (timeleft < 0) {
			timeleft = 0;
		}
		convert << timeleft;

		// Draw dead message and respawn timer
		p->DDraw->DTextOut(220, 270, "You have been blown up!", COLOR_WHITE);
		p->DDraw->DTextOut(220, 285, "You will respawn in: ", COLOR_WHITE);
		p->DDraw->DTextOut(387, 285, convert.str(), COLOR_WHITE);
		return;
	}

	// Draw the tank
	tankImage = this->getPlayerTankImage(p->Player[me], p->Player[me]);
	p->DDraw->Draw(p->DDraw->imgTanks, this->PlayerOffsetX, this->PlayerOffsetY, 48, 48, (p->Player[me]->Direction / 2) * 48, tankImage * 48);

	// If Names are on,
	if (p->Options->names) {

		// Draw the name
		this->drawPlayerName(p->Player[me], p->Player[me]);
	}
}
 
/***************************************************************
 * Function:	DrawBuildings
 *
 **************************************************************/
void CDrawing::DrawBuildings() {
	int me = p->Winsock->MyIndex;
	int myOffsetX = this->getOffsetX(p->Player[me]->X);
	int myOffsetY = this->getOffsetY(p->Player[me]->Y);
	int offX = myOffsetX % 48;
	int offY = myOffsetY % 48;
	int myX = myOffsetX / 48;
	int myY = myOffsetY / 48;
	float curTick = p->Tick;
	CBuilding *bld;

	int tileX;
	int tileY;
	int buildingType;
	int buildingSubType;
	int by;
	int bX;

	int moffX;
	int moffY;
	int popX;
	int popY;
	int imgX;
	int firstno;
	int secondno;

	// For each building,
	bld = p->Build->buildingListHead;
	while (bld) {

		// If this building is within sight,
		if (this->isTileInSight(myOffsetX, myOffsetY, bld->X, bld->Y)) {

			// Draw the building
			tileX = (myX - bld->X) * 48;
			tileY = (myY - bld->Y) * 48;
			buildingType = bld->Type / 100;
			buildingSubType = bld->Type - buildingType*100;
			by = 144*buildingType;
			bX = (bld->Animation/2)*144;
			this->p->DDraw->Draw(this->p->DDraw->imgBuildings, tileX+offX, tileY+offY, 144, 144, bX, by, MaxMapX, MaxMapY);

			moffX = 0;
			moffY = 0;
			popX = bld->pop * 48;
			popY = 0;
			imgX = buildingSubType * 32;
			firstno = bld->ItemsLeft / 10;
			secondno = bld->ItemsLeft - firstno*10;

			switch (buildingType) {

				// Building: CC
				case 0:
					popY = 48;
					this->p->DDraw->Draw(this->p->DDraw->imgPopulation, tileX+offX+96, tileY+offY+49, 48, 48, popX, popY, MaxMapX, MaxMapY);
					break;

				// Building: House
				case 2:
					this->p->DDraw->Draw(this->p->DDraw->imgPopulation, tileX+offX+96, tileY+offY+33, 48, 48, popX, popY, MaxMapX, MaxMapY);
					break;

				// Building: Hospital
				case 3:
					this->p->DDraw->Draw(this->p->DDraw->imgPopulation, tileX+offX+92, tileY+offY+92, 48, 48, popX, popY, MaxMapX, MaxMapY);
					break;

				// Building: Factory
				case 1:
					moffX = 56;
					moffY = 52;
					this->p->DDraw->Draw(this->p->DDraw->imgBlackNumbers, tileX+offX+56, tileY+offY+84, 16, 16, 16*firstno, 0, MaxMapX, MaxMapY);
					this->p->DDraw->Draw(this->p->DDraw->imgBlackNumbers, tileX+offX+72, tileY+offY+84, 16, 16, 16*secondno, 0, MaxMapX, MaxMapY);
					this->p->DDraw->Draw(this->p->DDraw->imgPopulation, tileX+offX+96, tileY+offY+48, 48, 48, popX, popY, MaxMapX, MaxMapY);

					if (bld->Smoke > 0) {
						this->p->DDraw->Draw(this->p->DDraw->imgSmoke, tileX+offX+6, tileY+offY-15, 180, 60, 0, (bld->Smoke-1)*60, MaxMapX, MaxMapY);
					}
					this->p->DDraw->Draw(this->p->DDraw->imgItems, tileX+offX+moffX, tileY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY);
					break;

				// Building: Research
				case 4:
					moffX = 14;
					moffY = 98;
					this->p->DDraw->Draw(this->p->DDraw->imgPopulation, tileX+offX+96, tileY+offY+90, 48, 48, popX, popY, MaxMapX, MaxMapY);
					this->p->DDraw->Draw(this->p->DDraw->imgItems, tileX+offX+moffX, tileY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY);
					break;
			}
		}

		// Get the next building
		bld = bld->next;
	}
}

/***************************************************************
 * Function:	DrawPlayers
 *
 **************************************************************/
void CDrawing::DrawPlayers() {
	int me = p->Winsock->MyIndex;
	int tankimg = 0;
	int myOffsetX = this->getOffsetX(p->Player[me]->X);
	int myOffsetY = this->getOffsetY(p->Player[me]->Y);
	int relativeX;
	int relativeY;
	int nameWidth;
	int centerPoint;
	string nameString;
	string townString;
	int lagWidth;
	int townWidth;

	// For each player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, isn't this player, and is in sight,
		if ((p->Player[i]->isInGame) && (me != i) && this->isPositionInSight(myOffsetX, myOffsetY, p->Player[i]->X, p->Player[i]->Y)) {
		
			// Get the player's position relative to my position
			relativeX = myOffsetX - p->Player[i]->X;
			relativeY = myOffsetY - p->Player[i]->Y;

			// Get the player's tank image
			tankimg = this->getPlayerTankImage(p->Player[i], p->Player[me]);

			// If tankimg != -1, draw the tank
			if (tankimg != -1) {
				p->DDraw->Draw(p->DDraw->imgTanks, relativeX, relativeY, 48, 48, (p->Player[i]->Direction / 2) * 48, tankimg * 48, MaxMapX, MaxMapY);
			}

			// If Names are on,
			if (p->Options->names) {

				// Draw the name
				this->drawPlayerName(p->Player[i], p->Player[me]);
			}
		}
	}
}

/***************************************************************
 * Function:	DrawBuildMenu
 *
 **************************************************************/
void CDrawing::DrawBuildMenu() {
	int minY = 0;

	// For each item,
	for (int i = 0; i < 26; i++) {

		// If the player can build the item, or the player is an admin,
		if (p->InGame->CanBuild[i] || p->Player[p->Winsock->MyIndex]->isAdmin()) {

			// Increment the height of the build menu
			minY += 16;
		}
	}

	// If the build menu needs more space than BuildMenuY, set BuildMenuY
	// This causes the build menu to not overflow past the top of the screen
	if (minY > this->BuildMenuY) {
		this->BuildMenuY = minY;
	}
	int DrawY = this->BuildMenuY;


	// Draw the background box
	p->DDraw->DFillRect(BuildMenuX, DrawY-minY, 180, minY+16);

	// Draw Demolish
	p->DDraw->Draw(p->DDraw->imgBuildIcons, BuildMenuX - 16, DrawY, 16, 16, 13*16);
	p->DDraw->DTextOut(BuildMenuX, DrawY, "Demolish", COLOR_YELLOW);
	DrawY -= 16;

	// For each item,
	for (int i = 0; i < 26; i++) {

		// If the player can build the item, or the player is an admin,
		if (p->InGame->CanBuild[i] || p->Player[p->Winsock->MyIndex]->isAdmin()) { 

			// Draw the item
			p->DDraw->Draw(p->DDraw->imgBuildIcons, BuildMenuX - 16, DrawY, 16, 16, buildButton[i]*16);
			p->DDraw->DTextOut(BuildMenuX, DrawY, buildNames[i], COLOR_YELLOW);
			DrawY -= 16;
		}
	}
	
}

/***************************************************************
 * Function:	DrawBuildPlacement
 *
 **************************************************************/
void CDrawing::DrawBuildPlacement() {
	
	// If the player has a Building or Demolish on the cursor,
	if (p->InGame->IsBuilding != 0) {
		int me = p->Winsock->MyIndex;
		int offX = (int)p->Player[me]->X % 48;
		int offY = (int)p->Player[me]->Y % 48;
		int tX = (p->Input->LastMouseX / 48) * 48 + this->buildingCursorOffsetX;
		int tY = (p->Input->LastMouseY / 48) * 48 + this->buildingCursorOffsetY;
		int buildingType = buildingTypes[p->InGame->IsBuilding-1] / 100;
		int buildingSubType = buildingTypes[p->InGame->IsBuilding-1] - buildingType*100;
		int moffX = 0;
		int moffY = 0; 

		// If the player has a Building on the cursor,
		if (p->InGame->IsBuilding > 0) {
			tX = (p->Input->LastMouseX / 48) * 48 + this->buildingCursorOffsetX;
			tY = (p->Input->LastMouseY / 48) * 48 + this->buildingCursorOffsetY;

			p->DDraw->Draw(p->DDraw->imgBuildings, tX+offX, tY+offY, 144, 144, 0, (buildingTypes[p->InGame->IsBuilding-1]/100)*144, MaxMapX, MaxMapY);
			int imgX = buildingSubType * 32;

			// Factory
			if (buildingType == 1) {
				moffX = 56;
				moffY = 52;
				p->DDraw->Draw(p->DDraw->imgItems, tX+offX+moffX, tY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY); 
			}
			else if(buildingType == 4) {
				moffX = 14;
				moffY = 98;
				p->DDraw->Draw(p->DDraw->imgItems, tX+offX+moffX, tY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY); 
			}
		}

		// Else (player has Demolish on the cursor),
		else {
			tX = (p->Input->LastMouseX / 48) * 48 + this->demoCursorOffsetX;
			tY = (p->Input->LastMouseY / 48) * 48 + this->demoCursorOffsetY;

			p->DDraw->Draw(p->DDraw->imgDemolish, tX+offX+moffX, tY+offY+moffY, 32, 32, 0, 0, MaxMapX, MaxMapY); 
		}
	}
}

/***************************************************************
 * Function:	DrawArrow
 *
 **************************************************************/
void CDrawing::DrawArrow() {
	int me = this->p->Winsock->MyIndex;
	int difX;
	int difY;

	// If the Arrow timer is up,
	if (this->p->Tick > this->arrowTick) {

		// Reset the Arrow timer
		this->arrowTick = this->p->Tick + 100;

		// If the player is under attack, and the Arrow Flash timer is up,
		if ( (this->p->InGame->isUnderAttack) && (this->p->Tick > this->arrowFlashTick) ) {

			// Toggle flashArrow
			this->flashArrow = !this->flashArrow;

			// Reset the Flash Arrow timer
			this->arrowFlashTick = this->p->Tick + 500;
		}

		// Compare player's X coords to city's X coords
		difX = (int)this->p->Player[me]->CityX - (int)p->Player[me]->X;
		if (difX == 0) {
			difX = 1;
		}

		// Compare player's Y coords to city's Y coords
		difY = (int)this->p->Player[me]->CityY - (int)this->p->Player[me]->Y;
		if (difY == 0) {
			difY = 1;
		}

		// West
		if (this->p->Player[me]->X <= this->p->Player[me]->CityX) {

			// North West
			if (this->p->Player[me]->CityY >= this->p->Player[me]->Y)  {

				if ((difX) / (difY) > 2) {
					lastArrow = 160;
				}
				else if ((difY) / (difX) > 2) {
					lastArrow = 80;
				}
				else {
					lastArrow = 120;
				}
			}

			// South West
			if (p->Player[me]->CityY < p->Player[me]->Y) {
				if ((difX) / (difY) > 2) {
					lastArrow = 160;
				}
				else if ((difY) / (difX) > 2) {
					lastArrow = 240;
				}
				else {
					lastArrow = 200;
				}
			}
		}
		
		// East
		else  {

			// North East
			if (p->Player[me]->CityY >= p->Player[me]->Y) {
				if ((difX) / (difY) > 2) {
					lastArrow = 0;
				}
				else if ((difY) / (difX) > 2) {
					lastArrow = 80;
				}
				else {
					lastArrow = 40;
				}
			}

			// South East
			if (p->Player[me]->CityY < p->Player[me]->Y) {
				if ((difX) / (difY) > 2) {
					lastArrow = 0;
				}
				else if ((difY) / (difX) > 2) {
					lastArrow = 240;
				}
				else {
					lastArrow = 280;
				}
			}
		}
	}
	
	// If the player is under attack, draw red arrows
	if (p->InGame->isUnderAttack && this->flashArrow) {
		p->DDraw->Draw(p->DDraw->imgArrowsRed, MaxMapX + 5, 160, 40, 40, lastArrow);
	}

	// Else (not under attack), draw normal arrows
	else {
		p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, lastArrow);
	}
}

/***************************************************************
 * Function:	DrawHealth
 *
 **************************************************************/
void CDrawing::DrawHealth()
{
	float percent = (float(p->Player[p->Winsock->MyIndex]->HP) / float(40));
	p->DDraw->Draw(p->DDraw->imgHealth, (MaxMapX + 137), 250-int(percent * 87), 38, int(percent * 87));
}

/***************************************************************
 * Function:	DrawItems
 *
 **************************************************************/
void CDrawing::DrawItems() {
	int me = p->Winsock->MyIndex;
	int myOffsetX = p->Player[me]->X + this->PlayerOffsetX;
	int myOffsetY = p->Player[me]->Y + this->PlayerOffsetY;
	int offX = myOffsetX % 48;
	int offY = myOffsetY % 48;
	int myX = myOffsetX / 48;
	int myY = myOffsetY / 48;
	int tileX;
	int tileY;
	int orientation;
	CItem *itm;

	// For each item,
	itm = p->Item->itemListHead;
	while (itm) {

		// If the player has the item's sector,
		if (p->InGame->HasSector[itm->X / SectorSize][itm->Y / SectorSize]) {

			// If the item is in sight,
			if (this->isTileInSight(myOffsetX, myOffsetY, itm->X, itm->Y)) {

				tileX = ((myX - itm->X) * 48)+offX;
				tileY = ((myY - itm->Y) * 48)+10+offY;

				// Switch on Item Type
				switch (itm->Type) {

					// Type: BOMB
					case ITEM_TYPE_BOMB:
						if (itm->active) 
							p->DDraw->Draw(p->DDraw->imgItems, tileX, tileY, 48, 48, 144, 91, MaxMapX, MaxMapY);
						else 
							p->DDraw->Draw(p->DDraw->imgItems, tileX, tileY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
						break;

					// Type: ORB
					case ITEM_TYPE_ORB:
						p->DDraw->Draw(p->DDraw->imgItems, tileX, tileY, 48, 48, itm->Type*48, 42 + itm->Animation * 48, MaxMapX, MaxMapY);
						break;

					// Type: MINE
					case ITEM_TYPE_MINE:

					// Type: DFG
					case ITEM_TYPE_DFG:
						if (itm->active && (itm->City != p->Player[me]->City) && (p->Player[me]->isAdmin() == false)) {
							//Don't draw
						}
						else {
							p->DDraw->Draw(p->DDraw->imgItems, tileX, tileY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
						}
						break;

					// Type: SLEEPER
					case ITEM_TYPE_SLEEPER:
						if ((itm->target > -1) || (itm->City == p->Player[me]->City) || p->Player[p->Winsock->MyIndex]->isAdmin()) {
							p->DDraw->Draw(p->DDraw->imgTurretBase, tileX, tileY-10, 48, 48, itm->Animation * 48, (itm->Type-9)*48, MaxMapX, MaxMapY);
							orientation = (int)(itm->Angle/22.5);
							p->DDraw->Draw(p->DDraw->imgTurretHead, tileX, tileY-10, 48, 48, orientation*48, (itm->Type-9)*48, MaxMapX, MaxMapY);
						}
						break;

					case ITEM_TYPE_CLOAK:

					case ITEM_TYPE_ROCKET:

					case ITEM_TYPE_MEDKIT:

					case ITEM_TYPE_FLARE:

					case ITEM_TYPE_WALL:
						p->DDraw->Draw(p->DDraw->imgItems, tileX, tileY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
						break;

					case ITEM_TYPE_TURRET:

					case ITEM_TYPE_PLASMA:
						p->DDraw->Draw(p->DDraw->imgTurretBase, tileX, tileY-10, 48, 48, itm->Animation * 48, (itm->Type-9)*48, MaxMapX, MaxMapY);
						orientation = (int)(itm->Angle/22.5);
						p->DDraw->Draw(p->DDraw->imgTurretHead, tileX, tileY-10, 48, 48, orientation*48, (itm->Type-9)*48, MaxMapX, MaxMapY);
						break;

				}
			}
		}

		// Get the next item
		itm = itm->next;
	}

}

/***************************************************************
 * Function:	DrawChat
 *
 **************************************************************/
void CDrawing::DrawChat() {
	int Y;

	// Draw info lines
	Y = 0;
	p->DDraw->DTextOut(0, Y, p->InGame->playerInfoLine1, p->InGame->playerInfoColor1);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->playerInfoLine2, p->InGame->playerInfoColor2);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->playerInfoLine3, p->InGame->playerInfoColor3);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->playerInfoLine4, p->InGame->playerInfoColor4);

	// If mouse is over chat,
	Y = MaxMapY - 126;
	if (p->Input->MouseOverChat) {
		p->DDraw->DFillRect(0, Y, MaxMapX, 126);
	}

	// Draw chat lines
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine1, p->InGame->chatColor1);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine2, p->InGame->chatColor2);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine3, p->InGame->chatColor3);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine4, p->InGame->chatColor4);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine5, p->InGame->chatColor5);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine6, p->InGame->chatColor6);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine7, p->InGame->chatColor7);
	Y += 14;
	p->DDraw->DTextOut(0, Y, p->InGame->chatLine8, p->InGame->chatColor8);
	Y += 14;

	// If chatting, fill just the chat bar with black
	if (p->InGame->IsChatting == 1) {
		p->DDraw->DFillRect(0, Y, MaxMapX, 14);
		p->DDraw->DTextOut(0, Y, p->InGame->ChatLine, COLOR_YELLOW);
	}
}

/***************************************************************
 * Function:	DrawInfo
 *
 **************************************************************/
void CDrawing::DrawInfo() {
}

/***************************************************************
 * Function:	DrawInventory
 *
 **************************************************************/
void CDrawing::DrawInventory() {
	CItem *itm;
	int DrawX;
	int DrawY;
	ostringstream stringConverter;
	int OrbAnimation = 0;
	int ItemCount[12];
	memset(ItemCount, 0, sizeof(ItemCount));

	// For each item,
	itm = p->Inventory->itemListHead;
	while (itm) {

		// Type: ORB
		if (itm->Type == 5) {

			// Set orb Animation
			OrbAnimation = itm->Animation;
		}
		ItemCount[itm->Type]++;

		// Get the next item
		itm = itm->next;
	}

	// Laser
	if (ItemCount[0] > 0) {
		DrawX = (MaxMapX + 7);
		DrawY = 267;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 0) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 0, 0);

		// If more than one, draw the number
		if (ItemCount[0] > 1) {
			stringConverter << ItemCount[0];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Rocket
	if (ItemCount[1] > 0) {
		DrawX = (MaxMapX + 42);
		DrawY = 267;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 1) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 1*32, 0);

		// If more than one, draw the number
		if (ItemCount[1] > 1) {
			stringConverter << ItemCount[1];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Medkit
	if (ItemCount[2] > 0) {
		DrawX = (MaxMapX + 77);
		DrawY = 267;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 2) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 2*32, 0);

		// If more than one, draw the number
		if (ItemCount[2] > 1) {
			stringConverter << ItemCount[2];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Bomb
	if (ItemCount[3] > 0) {
		DrawX = (MaxMapX + 7);
		DrawY = 302;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 3) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}

		// If activated, draw active image
		if (p->InGame->BombsAreActivated) {
			p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 152, 89);
		}
		// Else (not activated), draw normal image
		else {
			p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 3*32, 0);
		}

		// If more than one, draw the number
		if (ItemCount[3] > 1) {
			stringConverter << ItemCount[3];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Mine
	if (ItemCount[4] > 0) {
		DrawX = (MaxMapX + 42);
		DrawY = 302;
		
		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 4) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 4*32, 0);

		// If more than one, draw the number
		if (ItemCount[4] > 1) {
			stringConverter << ItemCount[4];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Orb
	if (ItemCount[5] > 0) {
		DrawX = (MaxMapX + 77);
		DrawY = 302;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 5) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 250, 41 + (OrbAnimation * 32) + (OrbAnimation * 16));

		// If more than one, draw the number
		if (ItemCount[5] > 1) {
			stringConverter << ItemCount[5];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Flare
	if (ItemCount[6] > 0) {
		DrawX = (MaxMapX + 7);
		DrawY = 337;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 6) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 6*32, 0);

		// If more than one, draw the number
		if (ItemCount[6] > 1) {
			stringConverter << ItemCount[6];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// DFG
	if (ItemCount[7] > 0) {
		DrawX = (MaxMapX + 42);
		DrawY = 337;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 7) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 7*32, 0);

		// If more than one, draw the number
		if (ItemCount[7] > 1) {
			stringConverter << ItemCount[7];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Wall
	if (ItemCount[8] > 0) {
		DrawX = (MaxMapX + 77);
		DrawY = 337;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 8) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 8*32, 0);

		// If more than one, draw the number
		if (ItemCount[8] > 1) {
			stringConverter << ItemCount[8];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Turret
	if (ItemCount[9] > 0) {
		DrawX = (MaxMapX + 7);
		DrawY = 372;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 9) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 9*32, 0);

		// If more than one, draw the number
		if (ItemCount[9] > 1) {
			stringConverter << ItemCount[9];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Sleeper
	if (ItemCount[10] > 0) {
		DrawX = (MaxMapX + 42);
		DrawY = 372;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 10) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 10*32, 0);

		// If more than one, draw the number
		if (ItemCount[10] > 1) {
			stringConverter << ItemCount[10];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}

	// Plasma
	if (ItemCount[11] > 0) {
		DrawX = (MaxMapX + 77);
		DrawY = 372;

		// If selected, draw selection image under item
		if (p->Inventory->SelectedItemType == 11) {
			p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		}
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 11*32, 0);

		// If more than one, draw the number
		if (ItemCount[11] > 1) {
			stringConverter << ItemCount[11];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, stringConverter.str(), COLOR_YELLOW);
			stringConverter.str("");
		}
	}
}

/***************************************************************
 * Function:	DrawBullets
 *
 **************************************************************/
void CDrawing::DrawBullets() {
	CBullet *blt;
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int tmpX;
	int tmpY;
	int dX;
	int dY;

	// For each bullet,
	blt = p->Bullet->bulletListHead;
	while (blt) {
		tmpX = (int)p->Player[me]->X - (int)blt->x + this->PlayerOffsetX;
		tmpY = (int)p->Player[me]->Y - (int)blt->y + this->PlayerOffsetY;

		// If the bullet is in sight,
		if (tmpX > 0 && tmpY > 0 && tmpX < 900 && tmpY < 900) {
			dX = blt->animation * 8;
			dY = dY = blt->type * 8;

			// Cycle through the bullet Animation
			blt->animation++;
			if (blt->animation > 3) {
				blt->animation = 0;
			}

			// Draw the bullet
			p->DDraw->Draw(p->DDraw->imgBullets, tmpX, tmpY, 8, 8, dX, dY, MaxMapX, MaxMapY);
		}

		// Get the next bullet
		blt = blt->next;
	}
}

/***************************************************************
 * Function:	DrawRadar
 *
 **************************************************************/
void CDrawing::DrawRadar() {
	int me = p->Winsock->MyIndex;
	int RadarSrcX = 0;
	int RadarX = 0;
	int RadarY = 0;
	int ratio = 24;

	// For each player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, and is not cloaked,
		if ( (p->Player[i]->isInGame) && (p->Player[i]->isCloaked == false) ) {

			// If the player is in range,
			if (
				(p->Player[i]->X >= p->Player[me]->X - RadarSize)
				&&
				(p->Player[i]->X <= p->Player[me]->X + RadarSize)
				&&
				(p->Player[i]->Y >= p->Player[me]->Y - RadarSize)
				&& 
				(p->Player[i]->Y <= p->Player[me]->Y + RadarSize)
				&&
				(p->Player[i]->X != 0)
				&&
				(p->Player[i]->Y != 0)
			) {

				RadarX = (int)((MaxMapX+100)-(p->Player[i]->X - p->Player[me]->X+70)/ratio);
				RadarY = (int)(80-(p->Player[i]->Y - p->Player[me]->Y+69)/ratio);
				
				// If the player is in my city, set color to 3
				if (p->Player[i]->City == p->Player[me]->City) {
					RadarSrcX = 3;
				}

				// Else if player is an admin, set color to 1
				else if (p->Player[i]->isAdmin()) {
					RadarSrcX = 1;
				}
				// Else (player is enemy), set color to 2
				else {
					RadarSrcX = 2;
				}

				// ???
				if (RadarX >= (MaxMapX+28) && RadarX <= (MaxMapX+166) && RadarY >= 8 && RadarY <= 146) {

					// If player is alive, draw the player
					if (p->Player[i]->isDead == false) {
						p->DDraw->Draw(p->DDraw->imgRadarColors, RadarX, RadarY, 2, 2, RadarSrcX * 2);
					}

					// Else (player is dead), ???
					else {
						p->DDraw->Draw(p->DDraw->imgMiniMapColors, RadarX, RadarY, 2, 2, 15);
					}
				}
			}
		}
	}
}

/***************************************************************
 * Function:	DrawExplosions
 *
 **************************************************************/
void CDrawing::DrawExplosions() {
	int me = p->Winsock->MyIndex;
	CExplosion *xpl;
	int tX;
	int tY;
		
	// For each explosion,
	xpl = this->p->Explode->explosionListHead;
	while (xpl) {

		tX = (int)(p->Player[me]->X - xpl->x + this->PlayerOffsetX);
		tY = (int)(p->Player[me]->Y - xpl->y + this->PlayerOffsetY);

		switch(xpl->type) {

			//Small explosion
			case 1: 
				p->DDraw->Draw(p->DDraw->imgSExplosion, tX, tY, 48, 48, xpl->animation * 48, 0, MaxMapX, MaxMapY);
				break;

			//Large explosion
			case 2: 
				p->DDraw->Draw(p->DDraw->imgLExplosion, tX, tY, 144, 144, 0, xpl->animation * 144, MaxMapX, MaxMapY);
				break;

			//Muzzle flash
			case 3: 
				p->DDraw->Draw(p->DDraw->imgMuzzleFlash, tX, tY, 12, 12, xpl->animation * 12, 0, MaxMapX, MaxMapY);
				break;
		}

		// Get the next explosion
		xpl = xpl->next;
	}
}

/***************************************************************
 * Function:	DrawMiniMap
 *
 **************************************************************/
void CDrawing::DrawMiniMap() {
	CBuilding *bld;
	int me = p->Winsock->MyIndex;
	int myX = (int)p->Player[me]->X / 48;
	int myY = (int)p->Player[me]->Y / 48;
	int tX;
	int tY;
	int X;
	int Y;

	int radius = 48;
	int tilesize = 5;

	Rect build_rect = {0,0,3*tilesize,3*tilesize};
	Rect plr_rect = {242,242,tilesize,tilesize};
	Rect terrain_rect = {0,0,tilesize,tilesize};
	char DrawX = 0;

	p->DDraw->DFillRect(10, 10, 460, 460);

	/************************************************
	 * Terrain
	 ************************************************/
	// For each square within {radius} of my position
	for (Y = myY-radius; Y <= myY+radius; Y++) {
		for (X = myX-radius; X <= myX+radius; X++) {

			// If the square is on the map,
			if (X > 0 && Y > 0 && X < 512 && Y < 512) {

				// Square: LAVA
				if (p->Map->map[X][Y] == MAP_SQUARE_LAVA) {
					DrawX = 0;
				}

				// Square: LAVA
				else if (p->Map->map[X][Y] == MAP_SQUARE_ROCK) {
					DrawX = 1;
				}

				// Square: OTHER
				else {
					DrawX = -1;
				}

				// If LAVA or ROCK,
				if (DrawX != -1) {
					terrain_rect.X = this->PlayerOffsetX+((myX-X-this->minimapOffsetX)*48)/(tilesize*2);
					terrain_rect.Y = this->PlayerOffsetY+((myY-Y-this->minimapOffsetY)*48)/(tilesize*2);

					// Check some constraints on the position of the square... ???
					if (terrain_rect.X > 10 && terrain_rect.X < 464 && terrain_rect.Y > 10 && terrain_rect.Y < 464) {

						// Draw the color on the mini map
						p->DDraw->Draw(p->DDraw->imgMiniMapColors, terrain_rect.X, terrain_rect.Y, terrain_rect.w, terrain_rect.h, DrawX*15);
					}
				}
			}
		}
	}

	/************************************************
	 * Buildings
	 ************************************************/
	// For each building,
	bld = p->Build->buildingListHead;
	while (bld) {

		tX = (myX - bld->X - this->minimapOffsetX) * 48;
		tY = (myY - bld->Y - this->minimapOffsetY) * 48;
		build_rect.X = this->PlayerOffsetX + (tX) / (2*tilesize);
		build_rect.Y = this->PlayerOffsetY + (tY) / (2*tilesize);

		if (build_rect.X > 10 && build_rect.X < 464 && build_rect.Y > 10 && build_rect.Y < 464) {

			// If the building is my CC, draw 2
			if ((bld->City == this->p->Player[me]->City) && (bld->isCC())) {
				DrawX = 2; 
			}
			// Else (not my CC), draw 3
			else {
				DrawX = 3;
			}
			this->p->DDraw->Draw(this->p->DDraw->imgMiniMapColors, build_rect.X, build_rect.Y, build_rect.w, build_rect.h, DrawX*15);
		}

		// Get the next building
		bld = bld->next;
	}

	// Draw me
	DrawX = 2;
	this->p->DDraw->Draw(this->p->DDraw->imgMiniMapColors, plr_rect.X, plr_rect.Y, plr_rect.w, plr_rect.h, DrawX*15);
}

/***************************************************************
 * Function:	CashFormat
 *
 **************************************************************/
string CDrawing::CashFormat(long Number) {
	char strCash[20];
	char output[20];
	memset(output, 0, 20);
	itoa(Number, strCash, 10);
	strcpy(output, "$");

	switch (strlen(strCash)) {

		case 4:
			memcpy(&output[1], strCash, 1);
			memcpy(&output[2], ",", 1);
			memcpy(&output[3], &strCash[2], 3);
			break;

		case 5:
			memcpy(&output[1], strCash, 2);
			memcpy(&output[3], ",", 1);
			memcpy(&output[4], &strCash[2], 3);
			break;

		case 6:
			memcpy(&output[1], strCash, 3);
			memcpy(&output[4], ",", 1);
			memcpy(&output[5], &strCash[3], 3);
			break;

		case 7:
			memcpy(&output[1], strCash, 1);
			memcpy(&output[2], ",", 1);
			memcpy(&output[3], &strCash[1], 3);
			memcpy(&output[6], ",", 1);
			memcpy(&output[7], &strCash[4], 3);
			break;

		case 8:
			memcpy(&output[1], strCash, 2);
			memcpy(&output[3], ",", 1);
			memcpy(&output[4], &strCash[2], 3);
			memcpy(&output[7], ",", 1);
			memcpy(&output[8], &strCash[5], 3);
			break;

		case 1:

		case 2:

		case 3:

		default:
			strcpy(&output[strlen(output)], strCash);
			break;
	}

	string Final;
	Final = output;
	return Final;
}

/***************************************************************
 * Function:	ClearPanel
 *
 **************************************************************/
void CDrawing::ClearPanel() {
	PanelLine1 = "";
	PanelLine2 = "";
	PanelLine3 = "";
	PanelLine4 = "";
	PanelLine5 = "";
	PanelLine6 = "";
	PanelLine7 = "";
	PanelMode = modeEmpty;
}

/***************************************************************
 * Function:	resetPlayerOffset
 *
 **************************************************************/
void CDrawing::resetPlayerOffset() {
	this->PlayerOffsetX = this->defaultPlayerOffsetX;
	this->PlayerOffsetY = this->defaultPlayerOffsetY;
}

/***************************************************************
 * Function:	getOffsetX
 *
 * @param x
 **************************************************************/
int CDrawing::getOffsetX(int x) {
	return x + this->PlayerOffsetX;
}

/***************************************************************
 * Function:	getOffsetY
 *
 * @param y
 **************************************************************/
int CDrawing::getOffsetY(int y) {
	return y + this->PlayerOffsetY;
}

/***************************************************************
 * Function:	isPositionInSight
 *
 * @param myX
 * @param myY
 * @param positionX
 * @param positionY
 **************************************************************/
bool CDrawing::isPositionInSight(int myX, int myY, int positionX, int positionY) {
	return this->isTileInSight(myX, myY, positionX/48, positionY/48);
}

/***************************************************************
 * Function:	isTileInSight
 *
 * @param myX
 * @param myY
 * @param tileX
 * @param tileY
 **************************************************************/
bool CDrawing::isTileInSight(int myX, int myY, int tileX, int tileY) {
	return
		(abs(tileX - (myX / 48)) < this->tilesToDraw)
		&&
		(abs(tileY - (myY / 48)) < this->tilesToDraw);
}

/***************************************************************
 * Function:	getPlayerTankImage
 *
 * @param playerIndex
 **************************************************************/
int CDrawing::getPlayerTankImage(CPlayer *player, CPlayer *me) {

	// If the player is alive, return -1 (none)
	if (player->isDead) {
		return -1;
	}

	// If the player has a custom tank, return it
	else if (player->Tank > 0) {
		return player->Tank;
	}

	// Else if the player is in my city
	else if (player->City == me->City) {

		// If Mayor, set friend mayor tank
		if (player->isMayor) {
			return 1;
		}

		// Else (not mayor), set friend commando tank
		else {
			return 0;
		}
	}

	// Else (player not in my city),
	else {
	
		// If mayor, set enemy mayor tank
		if (player->isMayor) {
			return 3;
		}

		// Else (not mayor), set enemy commando tank
		else {
			return 2;
		}
	}

	// Return -1 (none)
	return -1;
}


/***************************************************************
 * Function:	getPlayerNameColor
 *
 * @param playerIndex
 * @param myIndex
 **************************************************************/
COLORREF CDrawing::getPlayerNameColor(CPlayer *player, CPlayer *me) {

	// Color: DEAD
	if (player->isDead) {
		return COLOR_DEAD_NAME;
	}

	// Color: ADMIN
	else if (player->isAdmin()) {
		return COLOR_ADMIN_NAME;
	}

	// Color: LAGGING
	else if (player->isLagging) {
		return COLOR_LAGGING_NAME;
	}

	// Color: CUSTOM
	else if ((player->Red > 0) || (player->Green > 0) || (player->Blue > 0)) {
		
		// Color: RAINBOW
		if (player->RainbowName == 1) {
			return RGB(rand()%255, rand()%255, rand()%255);
		}

		// Color: CUSTOM
		else {
			return RGB(player->Red, player->Green, player->Blue);
		}
	}

	// Color: TEAM
	else if (player->City == me->City) {
		return COLOR_TEAM_NAME;
	}

	// Color: RED
	else {
		return COLOR_ENEMY_NAME;
	}
}

/***************************************************************
 * Function:	getPlayerCityColor
 *
 * @param playerIndex
 * @param myIndex
 **************************************************************/
COLORREF CDrawing::getPlayerCityColor(CPlayer *player, CPlayer *me) {

	// Color: DEAD
	if (player->isDead) {
		return COLOR_DEAD_NAME;
	}

	// Color: ADMIN
	else if (player->isAdmin()) {
		return COLOR_ADMIN_NAME;
	}

	// Color: LAGGING
	else if (player->isLagging) {
		return COLOR_LAGGING_NAME;
	}

	// Color: TEAM
	else if (player->City == me->City) {
		return COLOR_TEAM_NAME;
	}

	// Color: RED
	else {
		return COLOR_ENEMY_NAME;
	}
}

/***************************************************************
 * Function:	drawPlayerName
 *
 * @param player
 * @param me
 **************************************************************/
void CDrawing::drawPlayerName(CPlayer *player, CPlayer *me) {
	int nameWidth;
	int townWidth;
	int centerPoint;

	// Get the player's position relative to my position
	int relativeX = this->getOffsetX(me->X) - player->X;
	int relativeY = this->getOffsetY(me->Y) - player->Y;

	// Get text to show
	string nameString = player->NameString;
	string townString = player->TownString;

	// If alive and lagging, add Lagging
	if ((player->isDead == 0) && player->isLagging) {
		nameString += " (Lagging)";
	}

	// Get text lengths
	nameWidth = (nameString.length() * p->DDraw->TextWidth) / 2;
	if (player->Member) {
		nameWidth -= 8;
	}
	townWidth = (townString.length() * p->DDraw->TextWidth) / 2;
	centerPoint = relativeX + 16 + 3;

	// Draw text
	p->DDraw->DTextOut(centerPoint - nameWidth, relativeY - 31, nameString, this->getPlayerNameColor(player, me));
	p->DDraw->DTextOut(centerPoint - townWidth, relativeY - 16, townString, this->getPlayerCityColor(player, me));

	// If the player is a member, draw a star
	if (player->Member) {
		p->DDraw->Draw(p->DDraw->imgStar, centerPoint - nameWidth - 17, relativeY - 31, 16, 16);
	}
}