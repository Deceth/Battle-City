#include "CPlayer.h"

CPlayer::CPlayer(CServer *Server)
{
	p = Server;
	this->X = 0;
	this->Y = 0;
	this->City = 0;
	this->Name.clear();
	this->Town.clear();
	this->UniqID.clear();
	this->IPAddress.clear();
	this->Points = 0;
	Orbs = 0;
	Assists = 0;
	Deaths = 0;
	MonthlyPoints = 0;
	this->Mayor = 0;
	this->State = State_Disconnected;
	shutdown(this->Socket, 2);

	isAdmin = 1;
	Tank = 0;
	Tank2 = 0;
	Tank3 = 0;
	Tank4 = 0;
	Red = 0;
	Green = 0;
	Blue = 0;
	Member = 0;
	RentalCity = 0;

	id = 0;
	lastMove = 0;
	lastShot = 0;
	this->isDead = false;
	this->timeDeath = 0;

	memset(Buffer, 0, 2048);
	BufferLength = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::setMayor(int set)
{
	Mayor = set;
	State = State_Game;

	if (set)
	{
		p->City[City]->id = City;
		p->City[City]->Mayor = id;
		if (p->City[City]->hiring > -1) p->Winsock->SendData(p->City[City]->hiring,smMayorChanged," ");
		p->City[City]->Successor = -1;
		p->City[City]->notHiring = 0;
		p->City[City]->hiring = -1;

		for (int j = 0; j <= 26; j++)
		{
			p->City[City]->setCanBuild(j,p->City[City]->canBuild[j]);
		}

		sSMFinance finance;
		finance.Cash = p->City[this->City]->cash;
		finance.Income = p->City[this->City]->income;
		finance.Items = p->City[this->City]->itemproduction;
		finance.Hospital = p->City[this->City]->hospital;
		finance.Research = p->City[this->City]->cashresearch;
		p->Winsock->SendData(id,smFinance,(char *)&finance,sizeof(finance));

		sSMMayorUpdate Mayorupdate;
		Mayorupdate.Index = id;
		Mayorupdate.IsMayor = 1;
		p->Send->SendGameAllBut(-1, smMayorUpdate, (char *)&Mayorupdate, sizeof(Mayorupdate));
	} else {
		sSMMayorUpdate Mayorupdate;
		Mayorupdate.Index = id;
		Mayorupdate.IsMayor = 0;
		p->Send->SendGameAllBut(-1, smMayorUpdate, (char *)&Mayorupdate, sizeof(Mayorupdate));
	}
}

void CPlayer::Clear()
{
	if (p->Player[id]->State == State_Game)
		p->Player[id]->LeaveGame(1);

	char packet[1];
	packet[0] = id;
	p->Send->SendAllBut(-1, smClearPlayer, packet, 1);

	this->X = 0;
	this->Y = 0;
	this->City = 0;
	this->Name.clear();
	this->Town.clear();
	this->UniqID.clear();
	this->IPAddress.clear();
	this->Points = 0;
	Orbs = 0;
	Assists = 0;
	Deaths = 0;
	MonthlyPoints = 0;
	this->Mayor = 0;
	this->State = State_Disconnected;
	shutdown(this->Socket, 2);
	this->lastMove = 0;
	this->lastShot = 0;
	this->isDead = false;
	this->timeDeath = 0;
	id = 0;
	memset(Buffer, 0, 2048);
	BufferLength = 0;

	isAdmin = 1;
	Tank = 0;
	Tank2 = 0;
	Tank3 = 0;
	Tank4 = 0;
	Red = 0;
	Green = 0;
	Blue = 0;
	Member = 0;
	RentalCity = 0;
}

int CPlayer::FindApplyMayor()
{
	for (int j = 0; j < 64; j++)
	{
		if (p->City[j]->hiring == id)
		{
			return p->City[j]->Mayor;
		}
	}
	return -1;
}

//--------------------------- Actions ---------------------------\\

void CPlayer::JoinGame()
{
	sSMStateGame stategame;
	p->City[City]->y = (int)(512*48)-(34+(City / 8*64)) * 48; 
	p->City[City]->x = (int)(512*48)-(33+(City % 8*64)) * 48;
	stategame.x = p->City[City]->x;
	stategame.y = p->City[City]->y;
	p->Player[id]->X = stategame.x;
	p->Player[id]->Y = stategame.y;
	p->Account->GetStats(id);
	MonthlyPoints = p->Account->GetMonthlyTop20(id);
	stategame.City = City;

	p->Winsock->SendData(id,smStateGame,(char *)&stategame,sizeof(stategame));

	sSMJoinData join;
	join.id = id;
	join.Mayor = Mayor;
	join.City = City;
	p->Send->SendGameAllBut(id,smJoinData,(char *)&join,sizeof(join));
	p->Player[id]->lastMove = p->Tick;
	cout << "Join::" << Name.c_str() << endl;
}

void CPlayer::StartJoin()
{
	State = State_Game;
	p->Send->SendGameData(id);
}

void CPlayer::LeaveGame(int showmessage)
{
	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			if (itm->holder == id)
				itm = p->Item->delItem(itm);
			if (itm) itm = itm->next;
		}
	}
	if (Mayor)	
	{
		int failed = 0;
		if (p->City[City]->Successor > -1)
		{
			if (p->Player[p->City[City]->Successor]->State == State_Game && p->Player[p->City[City]->Successor]->City == City && p->City[City]->Successor != id)
			{
				p->Player[p->City[City]->Successor]->setMayor(1);
			}
			else failed = 1;
		}
		else
		{
			failed = 1;
		}
		if (failed == 1)
		{
			for (int j = 0; j < MaxPlayers; j++)
			{
				if (p->Player[j]->City == City && id != j && p->Player[j]->State == State_Game)
				{
					p->Player[j]->setMayor(1);
					break;
				}
			}
			if (p->City[City]->Mayor == id)
			{
				if (p->Build->GetOrbBuildingCount(City) < 11)
				{
					p->City[City]->destroy();
				}
				else
				{
					p->City[City]->DestructTimer = p->Tick + 120000;
					p->City[City]->Mayor = -1;
					p->City[City]->notHiring = 0;
				}
			}
		}
	}

	p->Player[id]->X = 0;
	p->Player[id]->Y = 0;
	p->Player[id]->City = 0;
	p->Player[id]->isDead = false;
	p->Player[id]->timeDeath = 0;

	if (this->State == State_Game)
	{
		p->Account->SaveStats(id);

		if (showmessage == 1)
		{
			char packet[2];
			packet[0] = (char)id;
			packet[1] = 69;
			p->Send->SendGameAllBut(id, smChatCommand, packet, 2); 
			cout << "Left::" << Name << endl;
		}
	}

	p->Player[id]->State = State_Chat;
}

void CPlayer::LoggedIn(string User)
{
	Name = User;
	p->Account->GetLoginData(id);
	State = State_Chat;

	cout << "LoggedIn::" << User << "\n";

	if (isAdmin == 1) isAdmin = 2; else isAdmin = 1;

	char TempString[20];
	memset(&TempString, 0, sizeof(TempString));
	TempString[0] = (unsigned char)id;
	TempString[1] = (unsigned char)isAdmin;
	p->Winsock->SendData(id, 1, TempString, 2); //Log in succesful

	sSMPlayer player;
	strcpy(player.Name, Name.c_str());
	strcpy(player.Town, Town.c_str());
	player.Index = id;
	player.isAdmin = isAdmin;
	player.Red = Red;
	player.Green = Green;
	player.Blue = Blue;
	player.Member = Member;
	player.Tank = Tank;
	p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
	p->Send->SendCurrentPlayers(id);

	p->Account->GetStats(id);
	sSMPoints pts;
	pts.Index = id;
	pts.Points = p->Player[id]->Points;
	pts.Deaths = p->Player[id]->Deaths;
	pts.Assists = p->Player[id]->Assists;
	pts.Orbs = p->Player[id]->Orbs;
	pts.MonthlyPoints = p->Player[id]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));

	State = State_Verified;

	p->Log->logAccount("Login ::  " + User + " :: " + this->IPAddress + " :: " + this->UniqID);
}
