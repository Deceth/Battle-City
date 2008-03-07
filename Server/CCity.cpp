#include "CCity.h"

CCity::CCity(CServer *Server)
{
	p = Server;
	notHiring = 0;
	Orbs = 0;
	hiring = -1;
	destroy();
}


CCity::~CCity()
{
}

void CCity::destroy()
{
	int i ;
	active = 1;

	CBuilding *bld;
	bld = p->Build->buildings;

	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (bld->City == id)
			{
				bld = p->Build->remBuilding(bld);
			} else
			if (bld)
				bld = bld->next;
		}
	}

	Mayor = -1;
	if (hiring > -1) p->Winsock->SendData(hiring, smMayorChanged, " ");
	hiring = -1;

	for (i = 0; i <= 26; i++)
	{
		canBuild[i] = 0;
	}

	
	//canBuild[0] = 1;
	canBuild[1] = 1;
	canBuild[2] = 1;
	canBuild[4] = 1;

	Orbs = 0;

	cash = MONEY_STARTING_VALUE;
	income = 0;
	itemproduction = 0;
	cashresearch = 0;
	hospital = 0;

	Successor = -1;

	DestructTimer = 0;

	CItem *itm;
	itm = p->Item->items;

	if (itm)
		while (itm->prev)
			itm = itm->prev;
	if (itm)
		while (itm)
		{
			if (itm->City == id)
			{
				itm = p->Item->remItem(itm);
			} else
			if (itm)
				itm = itm->next;
		}

	for (i = 0; i < 20; i++)
		research[i] = 0;

	for (i = 0; i < 12; i++)
		itemC[i] = 0;

	p->Send->SendGameAllBut(-1,smDestroyCity,(char *)&id, sizeof(id));
}

void CCity::setCanBuild(int i, int can)
{
	char packet[4];
	canBuild[i] = can;
	int iCan = 0;
	
	if (can)
		iCan = 1;

	if (can == 2) iCan = 0;

	if (p->Player[Mayor]->State == State_Game)
	{
		packet[0] = i+1;
		packet[1] = iCan;
		packet[2] = 0;
		p->Winsock->SendData(Mayor,(unsigned char)smCanBuild,packet);
	}
}

void CCity::cycle()
{
	if (this->Mayor == -1)
	{
		if (DestructTimer > 0)
		{
			if (p->Tick > DestructTimer) destroy();
		}
		return;
	}

	if (x == 0 || y == 0)
	{
		x = (unsigned short)(512*48)-(32+(id % 8*64)) * 48;
		y = (unsigned short)(512*48)-(32+(id / 8*64)) * 48; 
	}
	
	if (moneyCycle < p->Tick)
	{
		//if (cash > 40000000)
		//	cash = 40000000;
		//if (cash < 0)
		//	cash = 0;

		if (cash > MONEY_MAX_VALUE)
			cash = MONEY_MAX_VALUE;
		if (cash < 0)
		{
			cash = 0;

			// but randomly give back cash
			int random_int;
			srand ( time(NULL) );

			random_int = rand()%4 +1;
			if (random_int == 1)
			{
				cash = 2000000;
			}
		}

		moneyCycle = p->Tick + 7000;
		
		sSMFinance finance;
		finance.Cash = cash;
		finance.Income = income;
		finance.Items = itemproduction;
		finance.Hospital = hospital;
		finance.Research = cashresearch;
		p->Winsock->SendData(Mayor,smFinance,(char *)&finance,sizeof(finance));

		income = 0;
		itemproduction = 0;
		cashresearch = 0;
		hospital = 0;
	}
}

void CCity::wasOrbed()
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->City == id && p->Player[i]->State == State_Game)
		{
			cout << "Orbed::" << p->Player[i]->Name << endl;
			p->Player[i]->LeaveGame(0);
		}
	}
	destroy();
}

void CCity::didOrb(int City, int index)
{
	Orbs++;
	int pointsgiven = p->Build->GetOrbPointCount(City);

	sSMOrbedCity orbed;
	orbed.points = (unsigned int)pointsgiven;
	orbed.OrberCity = id;
	orbed.City = City;
	orbed.OrberCityPoints = p->Build->GetOrbPointCount(id);
	p->Send->SendAllBut(-1,smOrbed,(char *)&orbed,sizeof(orbed));

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->City == id && p->Player[i]->State == State_Game)
		{
			if (i == index)
			{
				p->Account->AddOrb(i);
			}
			else
			{
				p->Account->AddAssist(i);
			}
			p->Account->AddPoints(i, pointsgiven);
		}
	}
}



int CCity::PlayerCount()
{
	int count = 0;

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game && p->Player[i]->City == id)
			count++;
	}	

	return count;
}

