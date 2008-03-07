#include "CGame.h"

CDrawing::CDrawing(CGame *game)
{
	p = game;

	PlayerOffsetX = 276;
	PlayerOffsetY = 276;
	MaxMapX = 600;
	MaxMapY = 600;

	arrowTick = 0;
	lastArrow = 0;

	BuildMenuX = 0;
	BuildMenuY = 0;
}

CDrawing::~CDrawing()
{

}

void CDrawing::DrawGame()
{
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
	this->DrawInventory();

	if (p->Player[p->Winsock->MyIndex]->isMayor)
	{
		if (p->InGame->ShowBuildMenu) this->DrawBuildMenu();
		if (p->InGame->IsBuilding != 0) this->DrawBuildPlacement();
	}

	if (p->InGame->ShowMap == 1)
	{
		this->DrawMiniMap();
	}
}

void CDrawing::ClearDrawing()
{
	PlayerOffsetX = 276;
	PlayerOffsetY = 276;
	MaxMapX = 600;
	MaxMapY = 600;

	arrowTick = 0;
	lastArrow = 0;

	BuildMenuX = 0;
	BuildMenuY = 0;
}

void CDrawing::DrawMap()
{
	int i = 0; int j = 0;
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int goffX = (int)p->Player[me]->X % 128;
	int goffY = (int)p->Player[me]->Y % 128;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;

	for (i = -1; i < 6; i++)
	{
		for (j = -1; j < 6; j++)
		{
			p->DDraw->Draw(p->DDraw->imgGround,128*i+goffX, 128*j+goffY, 144, 144, 0, 0, MaxMapX, MaxMapY);
		}
	}

	for (i = -14; i < 15; i ++)
	{
		for (j = -14; j < 15; j ++)
		{
				int tX = mYX + i;
				int tY = mYY + j;
				if (tX > 0 && tY > 0 && tX <= 512 && tY <= 512)
				{
					if (p->InGame->HasSector[(tX / SectorSize)][(tY / SectorSize)] == 1)
					{
					if (p->Map->map[tX][tY] == 1)
						{
							p->DDraw->Draw(p->DDraw->imgLava, (mYX-tX)*48+PlayerOffsetX+offX, (mYY-tY)*48+PlayerOffsetY+offY, 48, 48, p->Map->tiles[tX][tY], 0, MaxMapX, MaxMapY);
						}
						else if (p->Map->map[tX][tY] == 2)
						{
							p->DDraw->Draw(p->DDraw->imgRocks, (mYX-tX)*48+PlayerOffsetX+offX, (mYY-tY)*48+PlayerOffsetY+offY, 48, 48, p->Map->tiles[tX][tY], 0, MaxMapX, MaxMapY);	
						}
					}
					else
					{
						p->DDraw->DFillRect((mYX-tX)*48+PlayerOffsetX+offX, (mYY-tY)*48+PlayerOffsetY+offY, 48, 48);
					}
				}
				else
				{
					p->DDraw->DFillRect((mYX-tX)*48+PlayerOffsetX+offX, (mYY-tY)*48+PlayerOffsetY+offY, 48, 48);
				}
		}
	}
}

void CDrawing::DrawPanel()
{
	p->DDraw->Draw(p->DDraw->imgInterface, MaxMapX, 0, 200, 430);
	p->DDraw->Draw(p->DDraw->imgInterfaceBottom, MaxMapX, 430, 200, 170);

	//Mayor
	if (p->Player[p->Winsock->MyIndex]->isMayor)
	{
		p->DDraw->Draw(p->DDraw->imgMoneyBox, MaxMapX+2, 224, 120, 19);

		string output = CashFormat(p->InGame->Cash);

		int GrossIncome = p->InGame->Income - p->InGame->Hospital - p->InGame->Items - p->InGame->Research;
		if (GrossIncome < 0)
		{
			p->DDraw->Draw(p->DDraw->imgMoneyDown, MaxMapX+8, 225, 10, 14);
			p->DDraw->DTextOut(MaxMapX+21, 226, output, RGB(255, 0, 0));
		}
		else
		{
			p->DDraw->Draw(p->DDraw->imgMoneyUp, MaxMapX+8, 226, 10, 14);
			p->DDraw->DTextOut(MaxMapX+21, 226, output.c_str(), RGB(0, 255, 0));
		}

		if (p->Personnel->ApplicantWaiting)
		{
			if (p->Personnel->ApplicantAnim == 1)
				p->DDraw->Draw(p->DDraw->imgBtnStaff, MaxMapX + 147, 269, 42, 19);
			if (p->Tick > p->Personnel->ApplicantTick)
			{
				p->Personnel->ApplicantAnim = 1 - p->Personnel->ApplicantAnim;
				p->Personnel->ApplicantTick = p->Tick + 500;
			}
		}
	}
	this->DrawArrow();
	this->DrawHealth();


	//Admin
	if (p->Player[p->Winsock->MyIndex]->isAdmin == 2)
	{
		p->DDraw->Draw(p->DDraw->imgBtnStaff, MaxMapX + 110, 460, 42, 19);
	}

	p->DDraw->DTextOut(MaxMapX + 12, 465, PanelLine1, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 480, PanelLine2, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 495, PanelLine3, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 510, PanelLine4, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 525, PanelLine5, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 540, PanelLine6, RGB(255, 255, 255));
	p->DDraw->DTextOut(MaxMapX + 12, 555, PanelLine7, RGB(255, 255, 255));
}

void CDrawing::DrawTank()
{
	if (p->Player[p->Winsock->MyIndex]->isDead == true)
	{
		p->DDraw->DTextOut(220, 270, "You have been blown up!", RGB(255, 255, 255));
		p->DDraw->DTextOut(220, 285, "You will respawn in: ", RGB(255, 255, 255));
		std::ostringstream convert;
		int timeleft = (int)(((int)(p->InGame->timeDeath + TIMER_RESPAWN) - p->Tick) / 1000);
		if (timeleft < 0) timeleft = 0;
		convert << timeleft;
		p->DDraw->DTextOut(387, 285, convert.str(), RGB(255, 255, 255));
		return;
	}

	int me = p->Winsock->MyIndex;
	if (p->Player[me]->Tank > 0)
		p->DDraw->Draw(p->DDraw->imgTanks, PlayerOffsetX, PlayerOffsetY, 48, 48, (p->Player[me]->Direction / 2) * 48, p->Player[me]->Tank * 48);
	else
		p->DDraw->Draw(p->DDraw->imgTanks, PlayerOffsetX, PlayerOffsetY, 48, 48, (p->Player[me]->Direction / 2) * 48, p->Player[me]->isMayor * 48);
	
	int NameWidth = (int)p->Player[me]->NameString.length() * p->DDraw->TextWidth;
	if (p->Player[me]->Member) NameWidth -= 16;
	NameWidth = NameWidth / 2;

	int CenterPoint = PlayerOffsetX + 16 + 3;
	if (p->Options->names)
	{
		if (p->Player[me]->Red > 0 || p->Player[me]->Green > 0 || p->Player[me]->Blue > 0)
		{
			if (p->Player[me]->RainbowName == 1)
				p->DDraw->DTextOut(CenterPoint - NameWidth, (PlayerOffsetY-31), p->Player[me]->NameString, RGB(rand()%255, rand()%255, rand()%255));
			else
				p->DDraw->DTextOut(CenterPoint - NameWidth, (PlayerOffsetY-31), p->Player[me]->NameString, RGB(p->Player[me]->Red, p->Player[me]->Green, p->Player[me]->Blue));
		}
		else
		{
			p->DDraw->DTextOut(CenterPoint - NameWidth, (PlayerOffsetY-31), p->Player[me]->NameString, RGB(0, 255, 0));
		}
		int TownWidth = (int)p->Player[me]->TownString.length() * p->DDraw->TextWidth / 2;
		p->DDraw->DTextOut(CenterPoint - TownWidth, (PlayerOffsetY-16), p->Player[me]->TownString, RGB(0, 255, 0));
	}

	if (p->Player[me]->Member)
	{
		p->DDraw->Draw(p->DDraw->imgStar, CenterPoint - NameWidth - 17, (PlayerOffsetY-31), 16, 16);
	}
}
 
void CDrawing::DrawBuildings()
{
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;
	float curTick = p->Tick;

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (abs(bld->X - mYX) < 18)
			{
				if (abs(bld->Y - mYY) < 18)
				{
					int tX = (mYX - bld->X) * 48;
					int tY = (mYY - bld->Y) * 48;
					int buildingType = bld->Type / 100;
					int buildingSubType = bld->Type - buildingType*100;
					int by = 144*buildingType;
					int bX = (bld->Animation/2)*144;
					p->DDraw->Draw(p->DDraw->imgBuildings, tX+PlayerOffsetX+offX, tY+PlayerOffsetY+offY, 144, 144, bX, by, MaxMapX, MaxMapY);

				int moffX = 0;
				int moffY = 0;
				int popX = bld->pop * 48;
				int popY = 0;

				switch (buildingType)
					{
						case 0:
							popY = 48;
							p->DDraw->Draw(p->DDraw->imgPopulation, tX+PlayerOffsetX+offX+96, tY+PlayerOffsetY+offY+49, 48, 48, popX, popY, MaxMapX, MaxMapY);
							break;
						case 2:
							p->DDraw->Draw(p->DDraw->imgPopulation, tX+PlayerOffsetX+offX+96, tY+PlayerOffsetY+offY+33, 48, 48, popX, popY, MaxMapX, MaxMapY);
							break;
						case 3:
							p->DDraw->Draw(p->DDraw->imgPopulation, tX+PlayerOffsetX+offX+92, tY+PlayerOffsetY+offY+92, 48, 48, popX, popY, MaxMapX, MaxMapY);
							break;
						case 1:
						case 4:
						{
							popY = 0;
							int imgX = buildingSubType * 32;
							int popY = 0;

							switch (buildingType)
							{
								case 1: //Factory
								{
									moffX = 56;
									moffY = 52;
									int firstno = bld->ItemsLeft / 10;
									int secondno = bld->ItemsLeft - firstno*10;
									p->DDraw->Draw(p->DDraw->imgBlackNumbers, tX+PlayerOffsetX+offX+56, tY+PlayerOffsetY+offY+84, 16, 16, 16*firstno, 0, MaxMapX, MaxMapY);
									p->DDraw->Draw(p->DDraw->imgBlackNumbers, tX+PlayerOffsetX+offX+72, tY+PlayerOffsetY+offY+84, 16, 16, 16*secondno, 0, MaxMapX, MaxMapY);
									p->DDraw->Draw(p->DDraw->imgPopulation, tX+PlayerOffsetX+offX+96, tY+PlayerOffsetY+offY+48, 48, 48, popX, popY, MaxMapX, MaxMapY);
									if (bld->Smoke > 0)
									{
										p->DDraw->Draw(p->DDraw->imgSmoke, tX+PlayerOffsetX+offX+6, tY+PlayerOffsetY+offY-15, 180, 60, 0, (bld->Smoke-1)*60, MaxMapX, MaxMapY);
									}
									
								}
								break;
								case 4: //Research
								{
									moffX = 14;
									moffY = 98;
									p->DDraw->Draw(p->DDraw->imgPopulation, tX+PlayerOffsetX+offX+96, tY+PlayerOffsetY+offY+90, 48, 48, popX, popY, MaxMapX, MaxMapY);
								}
								break;
							}
							p->DDraw->Draw(p->DDraw->imgItems, tX+PlayerOffsetX+offX+moffX, tY+PlayerOffsetY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY);
							break;
						}
					}
				}
			}
		bld = bld->next;
		}
	}
}

void CDrawing::DrawPlayers()
{
	int me = p->Winsock->MyIndex;
	int tankimg = 0;

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->isInGame == 1)
		{
			if (me != i)
			{
				int tX = (int)(p->Player[me]->X - p->Player[i]->X + PlayerOffsetX);
				int tY = (int)(p->Player[me]->Y - p->Player[i]->Y + PlayerOffsetY);
				if (abs(tX) < 800 && abs(tY) < 800)
				{
					if (p->Player[i]->City == p->Player[me]->City)
					{
						if (p->Player[i]->isMayor) tankimg = 1; else tankimg = 0;
					}
					else
					{
						if (p->Player[i]->isMayor) tankimg = 3; else tankimg = 2;
					}
					if (p->Player[i]->Tank > 0) tankimg = p->Player[i]->Tank;
					if (p->Player[i]->isDead == false) p->DDraw->Draw(p->DDraw->imgTanks, tX, tY, 48, 48, (p->Player[i]->Direction / 2) * 48, tankimg * 48, MaxMapX, MaxMapY);
					if (p->Options->names)
					{
						if (p->Player[i]->isDead == true)
						{
							p->DDraw->DTextOut(tX - ((int)(p->Player[i]->NameString.length() / 2) * 4), tY - 31, p->Player[i]->NameString, RGB(135, 135, 135));
							p->DDraw->DTextOut(tX - ((int)(p->Player[i]->TownString.length() / 2) * 4), tY - 16, p->Player[i]->TownString, RGB(135, 135, 135));
						}
						else
						{
							int NameWidth = (int)p->Player[i]->NameString.length() * p->DDraw->TextWidth;
							if (p->Player[i]->Member) NameWidth -= 16;
							NameWidth = NameWidth / 2;
							int CenterPoint = tX + 16 + 3;
							if (p->Player[i]->isLagging)
							{
								string LagString;
								LagString = p->Player[i]->Name;
								LagString += " (Lagging)";
								int LagWidth = (int)LagString.length() * p->DDraw->TextWidth / 2;
								int TownLagWidth = (int)p->Player[i]->TownString.length() * p->DDraw->TextWidth / 2;
								p->DDraw->DTextOut(CenterPoint - LagWidth, tY - 31, LagString,  RGB(0, 0, 255));
								p->DDraw->DTextOut(CenterPoint - TownLagWidth, tY - 16, p->Player[i]->TownString,  RGB(0, 0, 255));
							}
							else
							{
								if (p->Player[i]->Red > 0 || p->Player[i]->Green > 0 || p->Player[i]->Blue > 0)
								{
									if (p->Player[i]->RainbowName == 1)
										p->DDraw->DTextOut(CenterPoint - NameWidth, tY - 31, p->Player[i]->NameString, RGB(rand()%255, rand()%255, rand()%255));
									else
										p->DDraw->DTextOut(CenterPoint - NameWidth, tY - 31, p->Player[i]->NameString, RGB(p->Player[i]->Red, p->Player[i]->Green, p->Player[i]->Blue));
								}
								else
								{
									if (p->Player[i]->City == p->Player[me]->City)
										p->DDraw->DTextOut(CenterPoint - NameWidth, tY - 31, p->Player[i]->NameString, RGB(0, 255, 0));
									else
										p->DDraw->DTextOut(CenterPoint - NameWidth, tY - 31, p->Player[i]->NameString, RGB(255, 0, 0));
								}
								int TownWidth = (int)p->Player[i]->TownString.length() * p->DDraw->TextWidth / 2;
								if (p->Player[i]->City == p->Player[me]->City)
									p->DDraw->DTextOut(CenterPoint - TownWidth, tY - 16, p->Player[i]->TownString, RGB(0, 255, 0));
								else
									p->DDraw->DTextOut(CenterPoint - TownWidth, tY - 16, p->Player[i]->TownString, RGB(255, 0, 0));

								if (p->Player[i]->Member)
								{
									p->DDraw->Draw(p->DDraw->imgStar, CenterPoint - NameWidth - 17, tY - 31, 16, 16);
								}
							}
						}
					}
				}
			}
		}
	}
}

void CDrawing::DrawBuildMenu()
{
	int minY = 0;
	for (int i = 0; i < 26; i++)
	{
		if (p->InGame->CanBuild[i] || p->Player[p->Winsock->MyIndex]->isAdmin == 2)
		{
			minY += 16;
		}
	}
		if(minY > BuildMenuY)
			BuildMenuY = minY;
		int DrawY = BuildMenuY;

		p->DDraw->Draw(p->DDraw->imgBuildIcons, BuildMenuX - 16, DrawY, 16, 16, 13*16);
		p->DDraw->DFillRect(BuildMenuX, DrawY, 180, 16);
		p->DDraw->DTextOut(BuildMenuX, DrawY, "Demolish", RGB(255, 215, 0));

		DrawY -= 16;
		for (int i = 0; i < 26; i++)
		{
			if (p->InGame->CanBuild[i] || p->Player[p->Winsock->MyIndex]->isAdmin == 2)
			{ 
				p->DDraw->Draw(p->DDraw->imgBuildIcons, BuildMenuX - 16, DrawY, 16, 16, buildButton[i]*16);
				p->DDraw->DFillRect(BuildMenuX, DrawY, 180, 16);
				p->DDraw->DTextOut(BuildMenuX, DrawY, buildNames[i], RGB(255, 215, 0));
				DrawY -= 16;
			}
		}
	
}

void CDrawing::DrawBuildPlacement()
{
	if (p->InGame->IsBuilding != 0)
	{
		int me = p->Winsock->MyIndex;
		int offX = (int)p->Player[me]->X % 48;
		int offY = (int)p->Player[me]->Y % 48;
		int tX = (p->Input->LastMouseX / 48) * 48 -24+12;
		int tY = (p->Input->LastMouseY / 48) * 48 -24+12;
		int buildingType = buildingTypes[p->InGame->IsBuilding-1] / 100;
		int buildingSubType = buildingTypes[p->InGame->IsBuilding-1] - buildingType*100;
		int moffX = 0;
		int moffY = 0; 

		if (p->InGame->IsBuilding > 0)
		{

			p->DDraw->Draw(p->DDraw->imgBuildings, tX+offX, tY+offY, 144, 144, 0, (buildingTypes[p->InGame->IsBuilding-1]/100)*144, MaxMapX, MaxMapY);

			int imgX = buildingSubType * 32;

			if (buildingType == 1) //factorY
			{
				moffX = 56;
				moffY = 52;
				p->DDraw->Draw(p->DDraw->imgItems, tX+offX+moffX, tY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY); 
			}
			else if(buildingType == 4) //research
			{
				moffX = 14;
				moffY = 98;
				p->DDraw->Draw(p->DDraw->imgItems, tX+offX+moffX, tY+offY+moffY, 32, 32, imgX, 0, MaxMapX, MaxMapY); 
			}
		}
		else
			p->DDraw->Draw(p->DDraw->imgDemolish, tX+offX+moffX, tY+offY+moffY, 32, 32, 0, 0, MaxMapX, MaxMapY); 
	}
}

void CDrawing::DrawArrow()
{
	int me = p->Winsock->MyIndex;
	if (p->Tick > arrowTick)
	{
		arrowTick = p->Tick + 100;
		int difX = (int)p->Player[me]->CityX - (int)p->Player[me]->X;
		int difY = (int)p->Player[me]->CityY - (int)p->Player[me]->Y;
		if (difX == 0) difX = 1;
		if (difY == 0) difY = 1;
		if (p->Player[me]->X <= p->Player[me]->CityX) // west
		{
			if (p->Player[me]->CityY >= p->Player[me]->Y) // north west
			{
				if ((difX) / (difY) > 2)
				{
					lastArrow = 160;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 160);
					return;
				} else if ((difY) / (difX) > 2)
				{
					lastArrow = 80;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 80);
					return;
				} else {
					lastArrow = 120;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 120);
					return;
				}
			}
			if (p->Player[me]->CityY < p->Player[me]->Y) // south west
			{
				if ((difX) / (difY) > 2)
				{
					lastArrow = 160;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 160);
					return;
				} else if ((difY) / (difX) > 2)
				{
					lastArrow = 240;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 240);
					return;
				} else {
					lastArrow = 200;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 200);
					return;
				}
			}
		} else // east
		{
			if (p->Player[me]->CityY >= p->Player[me]->Y) // north east
			{
				if ((difX) / (difY) > 2)
				{
					lastArrow = 0;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 0);
					return;
				} else if ((difY) / (difX) > 2)
				{
					lastArrow = 80;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 80);
					return;
				} else {
					lastArrow = 40;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 40);
					return;
				}
			}
			if (p->Player[me]->CityY < p->Player[me]->Y) // south east
			{
				if ((difX) / (difY) > 2)
				{
					lastArrow = 0;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 0);
					return;
				} else if ((difY) / (difX) > 2)
				{
					lastArrow = 240;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 240);
					return;
				} else {
					lastArrow = 280;
					p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, 280);
					return;
				}
			}
		}
	}
	else
	{
		p->DDraw->Draw(p->DDraw->imgArrows, MaxMapX + 5, 160, 40, 40, lastArrow);
	}
}

void CDrawing::DrawHealth()
{
	float percent = (float(p->Player[p->Winsock->MyIndex]->HP) / float(40));
	p->DDraw->Draw(p->DDraw->imgHealth, (MaxMapX + 137), 250-int(percent * 87), 38, int(percent * 87));
}

void CDrawing::DrawItems()
{
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;

		CItem *itm = p->Item->items;
		if (itm)
		{
			while (itm->prev)
				itm = itm->prev;

			while (itm)
			{
				if (p->InGame->HasSector[itm->X / SectorSize][itm->Y / SectorSize])
				{
					if ((abs(mYX - itm->X) < 15) && (abs(mYY - itm->Y) < 15))
					{
						int tX = ((mYX - itm->X) * 48)+PlayerOffsetX+offX;
						int tY = ((mYY - itm->Y) * 48)+PlayerOffsetY+10+offY;
						switch (itm->Type)
						{
							case 3: //Bomb
								if (itm->active) 
									p->DDraw->Draw(p->DDraw->imgItems, tX, tY, 48, 48, 144, 91, MaxMapX, MaxMapY);
								else 
									p->DDraw->Draw(p->DDraw->imgItems, tX, tY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
								break;
							case 5: //Orb
								p->DDraw->Draw(p->DDraw->imgItems, tX, tY, 48, 48, itm->Type*48, 42 + itm->Animation * 48, MaxMapX, MaxMapY);
								break;
							case 4: //Mine
							case 7: //DFG
								if (itm->active && itm->City != p->Player[me]->City && p->Player[me]->isAdmin != 2)
								{
									//Don't draw
								}
								else
									p->DDraw->Draw(p->DDraw->imgItems, tX, tY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
								break;
							case 10: //Sleeper
								if (itm->target > -1 || itm->City == p->Player[me]->City || p->Player[p->Winsock->MyIndex]->isAdmin == 2)
								{
									p->DDraw->Draw(p->DDraw->imgTurretBase, tX, tY-10, 48, 48, itm->Animation * 48, (itm->Type-9)*48, MaxMapX, MaxMapY);
									int orientation = (int)(itm->Angle/22.5);
									p->DDraw->Draw(p->DDraw->imgTurretHead, tX, tY-10, 48, 48, orientation*48, (itm->Type-9)*48, MaxMapX, MaxMapY);
								}
								break;
							case 0:
							case 1:
							case 2:
							case 6:
							case 8:
								p->DDraw->Draw(p->DDraw->imgItems, tX, tY, 48, 48, itm->Type*48, 42, MaxMapX, MaxMapY);
								break;
							case 9:
							case 11:
								p->DDraw->Draw(p->DDraw->imgTurretBase, tX, tY-10, 48, 48, itm->Animation * 48, (itm->Type-9)*48, MaxMapX, MaxMapY);
								int orientation = (int)(itm->Angle/22.5);
								p->DDraw->Draw(p->DDraw->imgTurretHead, tX, tY-10, 48, 48, orientation*48, (itm->Type-9)*48, MaxMapX, MaxMapY);
								break;
						}
					}
				}
				itm = itm->next;
			}
		}
}

void CDrawing::DrawChat()
{
	if (p->Input->MouseOverChat == 1)
	{
		p->DDraw->DFillRect(0, MaxMapY-84, MaxMapX, 84);
	}
	
	int Y = MaxMapX - 126;
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

	if (p->InGame->IsChatting == 1)
	{
		p->DDraw->DFillRect(0, Y, MaxMapX, 14);
		p->DDraw->DTextOut(0, Y, p->InGame->ChatLine, RGB(255, 215, 0));
	}

	if (p->Options->newbietips == 1)
	{
		p->DDraw->DFillRect(0, 0, MaxMapX, 15);
		p->DDraw->DTextOut(0, 0, p->InGame->NewbieTip, RGB(255, 215, 0));
	}
}

void CDrawing::DrawInventory()
{
	int ItemCount[12];
	int OrbAnimation = 0;

	memset(ItemCount, 0, sizeof(ItemCount));

	CItem *itm = p->Inventory->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			if (itm->Type == 5) OrbAnimation = itm->Animation;
			ItemCount[itm->Type]++;
			itm = itm->next;
		}
	}

	if (ItemCount[0] > 0) // Laser
	{
		int DrawX = (MaxMapX + 7);
		int DrawY = 267;
		if (p->Inventory->SelectedItemType == 0) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 0, 0);
		if (ItemCount[0] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[0];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[1] > 0) // Rocket
	{
		int DrawX = (MaxMapX + 42);
		int DrawY = 267;
		if (p->Inventory->SelectedItemType == 1) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 1*32, 0);
		if (ItemCount[1] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[1];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[2] > 0) // Medkit
	{
		int DrawX = (MaxMapX + 77);
		int DrawY = 267;
		if (p->Inventory->SelectedItemType == 2) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 2*32, 0);
		if (ItemCount[2] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[2];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[3] > 0) // Bomb
	{
		int DrawX = (MaxMapX + 7);
		int DrawY = 302;
		if (p->Inventory->SelectedItemType == 3) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		
		if (p->InGame->BombsAreActivated)
			p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 152, 89);
		else
			p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 3*32, 0);
		if (ItemCount[3] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[3];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[4] > 0) // Mine
	{
		int DrawX = (MaxMapX + 42);
		int DrawY = 302;
		if (p->Inventory->SelectedItemType == 4) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 4*32, 0);
		if (ItemCount[4] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[4];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[5] > 0) // Orb
	{
		int DrawX = (MaxMapX + 77);
		int DrawY = 302;
		if (p->Inventory->SelectedItemType == 5) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 250, 41 + (OrbAnimation * 32) + (OrbAnimation * 16));
		if (ItemCount[5] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[5];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[6] > 0) // Walkie
	{
		int DrawX = (MaxMapX + 7);
		int DrawY = 337;
		if (p->Inventory->SelectedItemType == 6) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 6*32, 0);
		if (ItemCount[6] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[6];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[7] > 0) // DFG
	{
		int DrawX = (MaxMapX + 42);
		int DrawY = 337;
		if (p->Inventory->SelectedItemType == 7) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 7*32, 0);
		if (ItemCount[7] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[7];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[8] > 0) // Wall
	{
		int DrawX = (MaxMapX + 77);
		int DrawY = 337;
		if (p->Inventory->SelectedItemType == 8) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 8*32, 0);
		if (ItemCount[8] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[8];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[9] > 0) // Turret
	{
		int DrawX = (MaxMapX + 7);
		int DrawY = 372;
		if (p->Inventory->SelectedItemType == 9) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 9*32, 0);
		if (ItemCount[9] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[9];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[10] > 0) // Sleeper
	{
		int DrawX = (MaxMapX + 42);
		int DrawY = 372;
		if (p->Inventory->SelectedItemType == 10) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 10*32, 0);
		if (ItemCount[10] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[10];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
	if (ItemCount[11] > 0) // Plasma
	{
		int DrawX = (MaxMapX + 77);
		int DrawY = 372;
		if (p->Inventory->SelectedItemType == 11) p->DDraw->Draw(p->DDraw->imgInventorySelection, DrawX, DrawY, 32, 32, 0, 0);
		p->DDraw->Draw(p->DDraw->imgItems, DrawX, DrawY, 32, 32, 11*32, 0);
		if (ItemCount[11] > 1)
		{
			ostringstream Convert;
			Convert << ItemCount[11];
			p->DDraw->DTextOut(DrawX+22, DrawY+12, Convert.str(), RGB(255, 215, 0));
		}
	}
}

void CDrawing::DrawBullets()
{
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;

	CBullet *blt = p->Bullet->bullets;
	if (blt)
	{
		while (blt->prev)
			blt = blt->prev;

		while (blt)
		{
			int tmpX = (int)p->Player[me]->X - (int)blt->X + PlayerOffsetX;
			int tmpY = (int)p->Player[me]->Y - (int)blt->Y + PlayerOffsetY;

			if (tmpX > 0 && tmpY > 0 && tmpX < 900 && tmpY < 900)
			{
				int dX = blt->Animation * 8;
				int dY = 0;

				blt->Animation++;
				if (blt->Animation > 3) blt->Animation = 0;

				dY = blt->Type * 8;

				p->DDraw->Draw(p->DDraw->imgBullets, tmpX,  tmpY, 8, 8, dX, dY, MaxMapX, MaxMapY);
			}
			blt = blt->next;
		}
	}
}

void CDrawing::DrawRadar()
{
	int me = p->Winsock->MyIndex;
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->isInGame == 1)
		{
			if (me != i)
			{
				//Radar
				int RadarSrcX = 0;
				int RadarX = 0;
				int RadarY = 0;
				int ratio = 24;

				if (p->Player[i]->X >= p->Player[me]->X - RadarSize && p->Player[i]->X <= p->Player[me]->X + RadarSize && p->Player[i]->Y >= p->Player[me]->Y - RadarSize && p->Player[i]->Y <= p->Player[me]->Y + RadarSize &&p->Player[i]->X != 0 &&p->Player[i]->Y != 0 && i != me)
				{	//player is in range
					RadarX = (int)((MaxMapX+100)-(p->Player[i]->X - p->Player[me]->X+70)/ratio);
					RadarY = (int)(80-(p->Player[i]->Y - p->Player[me]->Y+69)/ratio);
					if (p->Player[i]->City == p->Player[me]->City)
						RadarSrcX = 3;
					else
						RadarSrcX = 2;
					if (p->Player[i]->isAdmin == 2)
						RadarSrcX = 1;
				}
				if (RadarX >= (MaxMapX+28) && RadarX <= (MaxMapX+166) && RadarY >= 8 && RadarY <= 146)
				{
					if (p->Player[i]->isDead == false)
						p->DDraw->Draw(p->DDraw->imgRadarColors, RadarX, RadarY, 2, 2, RadarSrcX * 2);
					else
						p->DDraw->Draw(p->DDraw->imgMiniMapColors, RadarX, RadarY, 2, 2, 15);
				}
			}
		}
	}
}

void CDrawing::DrawExplosions()
{
	int me = p->Winsock->MyIndex;
	CExplosion *Xpl = p->Explode->explosions;
	if (Xpl)
	{

		while (Xpl->prev)
			Xpl = Xpl->prev;

		while (Xpl)
		{
			int tX = (int)(p->Player[me]->X - Xpl->X + PlayerOffsetX);
			int tY = (int)(p->Player[me]->Y - Xpl->Y + PlayerOffsetY);
			switch(Xpl->Type)
			{
				case 1: //Small eXplosion
					p->DDraw->Draw(p->DDraw->imgSExplosion, tX, tY, 48, 48, Xpl->Animation * 48, 0, MaxMapX, MaxMapY);
					break;
				case 2: //Large eXplosion
					p->DDraw->Draw(p->DDraw->imgLExplosion, tX, tY, 144, 144, 0, Xpl->Animation * 144, MaxMapX, MaxMapY);
					break;
				case 3: //Muzzle flash
					p->DDraw->Draw(p->DDraw->imgMuzzleFlash, tX, tY, 12, 12, Xpl->Animation * 12, 0, MaxMapX, MaxMapY);
					break;
			}
			Xpl = Xpl->next;
		}
	}
}

void CDrawing::DrawMiniMap()
{
	int me = p->Winsock->MyIndex;
	int offX = (int)p->Player[me]->X % 48;
	int offY = (int)p->Player[me]->Y % 48;
	int goffX = (int)p->Player[me]->X % 128;
	int goffY = (int)p->Player[me]->Y % 128;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;

	int radius = 48;
	int tilesize = 5;
	p->DDraw->DFillRect(10, 10, 460, 460);
	Rect build_rect = {0,0,3*tilesize,3*tilesize};
	Rect plr_rect = {242,242,tilesize,tilesize};
	Rect terrain_rect = {0,0,tilesize,tilesize};
	char DrawX = 0;
	
	for (int Y = (int)(p->Player[me]->Y/48)-radius; Y <= (int)(p->Player[me]->Y/48)+radius; Y++)
	{
		for (int X = (int)(p->Player[me]->X/48)-radius; X <= (int)(p->Player[me]->X/48)+radius; X++)
		{
			if (X > 0 && Y > 0 && X < 512 && Y < 512)
			{
				if (p->Map->map[X][Y] > 0 && p->Map->map[X][Y] < 3)
				{
					if (p->Map->map[X][Y] == 1) DrawX = 0; else DrawX = 1;
					terrain_rect.X = PlayerOffsetX+((mYX-7-X)*48)/(tilesize*2);
					terrain_rect.Y = PlayerOffsetY+((mYY-7-Y)*48)/(tilesize*2);
					if (terrain_rect.X > 10 && terrain_rect.X < 464 && terrain_rect.Y > 10 && terrain_rect.Y < 464)
						p->DDraw->Draw(p->DDraw->imgMiniMapColors, terrain_rect.X, terrain_rect.Y, terrain_rect.w, terrain_rect.h, DrawX*15);
				}
			}
		}
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
		while(bld->prev)
			bld = bld->prev;

	while (bld)
	{
		int tX = (mYX - 7 - bld->X) * 48;
		int tY = (mYY - 7 - bld->Y) * 48;
		build_rect.X = PlayerOffsetX+(tX)/(2*tilesize);
		build_rect.Y = PlayerOffsetY+(tY)/(2*tilesize);
		if (build_rect.X > 10 && build_rect.X < 464 && build_rect.Y > 10 && build_rect.Y < 464)
		{
			if (bld->City == p->Player[me]->City && bld->Type == 6) DrawX = 2; else DrawX = 3;
			p->DDraw->Draw(p->DDraw->imgMiniMapColors, build_rect.X, build_rect.Y, build_rect.w, build_rect.h, DrawX*15);
		}
	bld = bld->next;
	}

	DrawX = 2;
	p->DDraw->Draw(p->DDraw->imgMiniMapColors, plr_rect.X, plr_rect.Y, plr_rect.w, plr_rect.h, DrawX*15);
}

string CDrawing::CashFormat(long Number)
{
	char strCash[20];
	char output[20];
	memset(output, 0, 20);
	itoa(Number, strCash, 10);
	strcpy(output, "$");
	switch (strlen(strCash))
	{
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

void CDrawing::ClearPanel()
{
	PanelLine1 = "";
	PanelLine2 = "";
	PanelLine3 = "";
	PanelLine4 = "";
	PanelLine5 = "";
	PanelLine6 = "";
	PanelLine7 = "";
	PanelMode = modeEmpty;
}