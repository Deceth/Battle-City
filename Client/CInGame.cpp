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

	/* 1024
	PlayerOffsetX = 316;
	PlayerOffsetY = 316;
	MaxMapX = 680;
	MaxMapY = 680;
	*/

	/* 640
	PlayerOffsetX = 216;
	PlayerOffsetY = 216;
	MaxMapX = 480;
	MaxMapY = 480;
	*/

	for (int i = 0; i < 27; i++)
	{
		CanBuild[i] = 0;
	}

	memset(HasSector, 0, sizeof(HasSector));
	memset(RequestedSector, 0, sizeof(RequestedSector));

}

CInGame::~CInGame()
{

}

void CInGame::Cycle()
{
	float curTick = p->Tick;
	for (int j = 0; j < MaxPlayers; j++)
	{
		p->Player[j]->Cycle();
	}
	p->Bullet->Cycle();
	p->Explode->Cycle();
	p->Item->Cycle();
	p->Build->Cycle();
	p->Inventory->Cycle();

	p->Draw->DrawGame();

	if (curTick > updateTick && p->Player[p->Winsock->MyIndex]->isDead == false)
	{
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

	if (curTick > PingTick)
	{
		this->CheckRefVariables();
		p->Winsock->SendData(cmTCPPing, " ");
		this->PingTimer = p->Tick;
		this->PingTick = curTick + 5000;
	}

	this->lastTick = p->Tick;
}

string CInGame::ReturnRank(int Points)
{
	string rank;

	if (Points < 100)
		rank = "Private";
	if (Points >= 100 && Points < 200)
		rank = "Corporal";
	if (Points >= 200 && Points < 500)
		rank = "Sergeant";
	if (Points >= 500 && Points < 1000)
		rank = "Sergeant Major";
	if (Points >= 1000 && Points < 2000)
		rank = "Lieutenant";
	if (Points >= 2000 && Points < 4000)
		rank = "Captain";
	if (Points >= 4000 && Points < 8000)
		rank = "Major";
	if (Points >= 8000 && Points < 16000)
		rank = "Colonel";
	if (Points >= 15000 && Points < 30000)
		rank = "Brigadier";
	if (Points >= 30000 && Points < 45000)
		rank = "General";
	if (Points >= 45000 && Points < 60000)
		rank = "Baron";
	if (Points >= 60000 && Points < 80000)
		rank = "Earl";
	if (Points >= 80000 && Points < 100000)
		rank = "Count";
	if (Points >= 100000 && Points < 125000)
		rank = "Duke";
	if (Points >= 125000 && Points < 150000)
		rank = "Archduke";
	if (Points >= 150000 && Points < 200000)
		rank = "Grand Duke";
	if (Points >= 200000 && Points < 250000)
		rank = "Lord";
	if (Points >= 250000 && Points < 300000)
		rank = "Chancellor";
	if (Points >= 300000 && Points < 350000)
		rank = "Royaume";
	if (Points >= 350000 && Points < 400000)
		rank = "Emperor";
	if (Points >= 400000 && Points < 500000)
		rank = "Auror";
	if (Points >= 500000)
		rank = "King";

	return rank;
}

void CInGame::PrintWhoData()
{
		string Players;
		int PlayerCount = 0;
		for (int i = 0; i < MaxPlayers; i++)
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

		string JoinString;
		JoinString = "There are ";
		ostringstream Converter;
		Converter << PlayerCount;
		JoinString += Converter.str();
		JoinString += " other players currently online:  ";
		JoinString += Players;
		AppendChat(JoinString, RGB(255, 215, 0));
}

void CInGame::AppendChat(string ChatText, COLORREF color)
{
	unsigned int MessageLength = 75;
	while (ChatText.length() > MessageLength)
	{
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

void CInGame::ClearOut()
{
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

	for (int i = 0; i < 27; i++)
	{
		CanBuild[i] = 0;
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (bld->Type != 6)
			{
				p->Build->delBuilding(bld);
				bld = p->Build->buildings;
				if (bld)
				{
					while (bld->prev)
						bld = bld->prev;
				}
			}
			bld = bld->next;
		}
	}

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			p->Item->delItem(itm);
			itm = p->Item->items;
			if (itm)
			{
				while (itm->prev)
					itm = itm->prev;
			}
		}
	}

	CExplosion *Xpl = p->Explode->explosions;
	if (Xpl)
	{
		while (Xpl->prev)
			Xpl = Xpl->prev;

		while (Xpl)
		{
			p->Explode->delExplosion(Xpl);
			Xpl = p->Explode->explosions;
			if (Xpl)
			{
				while (Xpl->prev)
					Xpl = Xpl->prev;
			}
		}
	}

	CBullet *blt = p->Bullet->bullets;
	if (blt)
	{
		while (blt->prev)
			blt = blt->prev;

		while (blt)
		{
			blt = p->Bullet->delBullet(blt);
			if (blt)
			{
				while (blt->prev)
					blt = blt->prev;
			}
		}
	}

	while (p->Inventory->items)
	{
		p->Inventory->delItem(p->Inventory->items);
	}
	p->Inventory->SelectedItemType = 0;

	for (int j = 0; j < MaxPlayers; j++)
	{
		p->Player[j]->InGameClear();
	}

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

void CInGame::RefreshArea()
{
	bool requestedSector = false;

	char me = p->Winsock->MyIndex;

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (((p->Player[me]->SectorX+i) >= 0) & ((p->Player[me]->SectorX+i) <= MaxSectors) 
				& ((p->Player[me]->SectorY+j) >= 0) & ((p->Player[me]->SectorY+j) <= MaxSectors))
			{
				if (this->HasSector[(p->Player[me]->SectorX+i)][(p->Player[me]->SectorY+j)] == 0 && this->RequestedSector[(p->Player[me]->SectorX+i)][(p->Player[me]->SectorY+j)] == false)
				{
					sCMSector sector;
					sector.x = p->Player[me]->SectorX + i;
					sector.y = p->Player[me]->SectorY + j;

					if ((sector.x >= 0) & (sector.y >= 0) & (sector.x <= MaxSectors) & (sector.y <= MaxSectors))
					{
						ClearSector(sector.x, sector.y);
						this->RequestedSector[sector.x][sector.y] = true;
						p->Winsock->SendData(cmRequestSector, (char *)&sector, sizeof(sector));
						requestedSector = true;
					}
				}
			}
		}
	}

	for (int i = 0; i <= MaxSectors; i++)
	{
		for (int j = 0; j <= MaxSectors; j++)
		{
			if ((abs(i - p->Player[me]->SectorX) > 1) | (abs(j - p->Player[me]->SectorY) > 1))
				ClearSector(i, j);
		}
	}

	if (requestedSector == true && p->InGame->ShowMap == 1) 
	{
		p->Winsock->SendData(cmMiniMap, " ");
	}
}

void CInGame::ClearSector(int SectorX, int SectorY)
{
	if (SectorX >= 0 && SectorY >= 0 && SectorX <= MaxSectors && SectorY <= MaxSectors && p->InGame->ShowMap == 0)
	{
		p->InGame->HasSector[SectorX][SectorY] = 0;
		p->InGame->RequestedSector[SectorX][SectorY] = false;

		CItem *itm = p->Item->items;
		if (itm)
		{
			while (itm->prev)
				itm = itm->prev;

			while (itm)
			{
				if ((itm->X / SectorSize) == SectorX && (itm->Y / SectorSize) == SectorY)
				{
					p->Item->delItem(itm);
					itm = p->Item->items;
					if (!itm)
						break;
					while (itm->prev)
						itm = itm->prev;
				}
				itm = itm->next;
			}
		}

		CBuilding *bld = p->Build->buildings;
		if (bld)
		{
			while (bld->prev)
				bld = bld->prev;

			while (bld)
			{
				if ((bld->X / SectorSize) == SectorX && (bld->Y / SectorSize) == SectorY && bld->Type != 6)
				{
					p->Build->delBuilding(bld);
					bld = p->Build->buildings;
					if (!bld)
						break;
					while (bld->prev)
						bld = bld->prev;
				}
				bld = bld->next;
			}
		}
	}
}

void CInGame::ClearAllSectors()
{
	memset(HasSector, 0, sizeof(HasSector));
	memset(RequestedSector, 0, sizeof(RequestedSector));

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			p->Item->delItem(itm);
			itm = p->Item->items;
			if (!itm)
				break;
			while (itm->prev)
				itm = itm->prev;
			itm = itm->next;
		}
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (bld->Type != 6)
			{
				p->Build->delBuilding(bld);
				bld = p->Build->buildings;
				if (!bld)
					break;
				while (bld->prev)
					bld = bld->prev;
			}
			bld = bld->next;
		}
	}
}

void CInGame::PrintFinanceReport()
{
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
	long NetIncome = p->InGame->Income - p->InGame->Research - p->InGame->Items - p->InGame->Hospital;
	if (NetIncome < 0)
	{
		p->Draw->PanelLine6 += "-";
	}
	p->Draw->PanelLine6 += p->Draw->CashFormat(abs(NetIncome));
}