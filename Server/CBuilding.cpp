#include "CBuilding.h"

CBuilding *CBuildingList::findBuilding(unsigned short id)
{
	CBuilding *bld = buildings;
	if (!bld)
		return 0;

	while (bld->prev)
		bld = bld->prev;

	while (bld)
	{
		if (bld->id == id)
			return bld;
		bld = bld->next;
	}
	return 0;
}

int CBuildingList::GetOrbPointCount(int theCity)
{
	int pointsgiven = 0;
	int count = p->Build->GetOrbBuildingCount(theCity);
	if (count < 11)
	{
		pointsgiven = 0;
	}
	else if (count < 16)
	{
		pointsgiven = 20;
	}
	else if (count < 21)
	{
		pointsgiven = 30;
	}
	else
	{
		pointsgiven = 50;
	}
	pointsgiven += (p->City[theCity]->Orbs * 5);
	return pointsgiven;
}

CBuilding *CBuildingList::newBuilding(int x, int y, int type, int City, unsigned short id)
{
	CBuilding *bld = buildings;

	if (!bld)
	{
		buildings = new CBuilding(x, y, type, City, id, p);
		return buildings;
	}
	else
	{
		while(bld->next)
			bld = bld->next;
		bld->next = new CBuilding(x, y, type, City, id, p);
		bld->next->prev = bld;
		return bld->next;
	}

	return 0;
}

CBuilding *CBuildingList::delBuilding(CBuilding *del)
{
	if (!buildings)
		return 0;

	CItem *item;
	if ((del->type % 2) == 0 && del->type > 2)
	{
		if (p->Item->items)
		{
			item = p->Item->items;
			while (item->prev)
				item = item->prev;
			
			while (item)
			{
				if 	(item->type == itemTypes[(del->type - 2) / 2 - 1] && item->City == del->City)
				{
					item = p->Item->delItem(item);
				} else
				item = item->next;
			}
		}
		p->City[del->City]->setCanBuild(del->type-1, 1);
	}
	else 
	{
		p->City[del->City]->setCanBuild(del->type-1, 1);
	}

	if (del->type == 2) //House
	{
		if (del->AttachedID > 0)
		{
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID);
			if (bld2)
			{
				bld2->AttachedID = 0;
				bld2->pop = 0;
				sSMPop pop;
				pop.id = bld2->id;
				pop.pop = bld2->pop / 8;
				p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}
		if (del->AttachedID2 > 0)
		{
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID2);
			if (bld2)
			{
				bld2->AttachedID = 0;
				bld2->pop = 0;
				sSMPop pop;
				pop.id = bld2->id;
				pop.pop = bld2->pop / 8;
				p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
			}
		}
	}
	else
	{
		if (del->AttachedID > 0)
		{
			CBuilding *bld2 = p->Build->findBuilding(del->AttachedID);
			if (bld2)
			{
				if (bld2->AttachedID == del->id)
				{
					bld2->AttachedPop = 0;
					bld2->AttachedID = 0;
					bld2->pop = bld2->AttachedPop2;
					sSMPop pop;
					pop.id = bld2->id;
					pop.pop = bld2->pop / 16;
					p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}
				else if (bld2->AttachedID2 == del->id)
				{
					bld2->AttachedPop2 = 0;
					bld2->AttachedID2 = 0;
					bld2->pop = bld2->AttachedPop;
					sSMPop pop;
					pop.id = bld2->id;
					pop.pop = bld2->pop / 16;
					p->Send->SendSectorArea(bld2->x*48,bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
				}
			}
		}
	}

	sSMBuild bv;
	bv.id = del->id;
	bv.x = 1;
	p->Send->SendSectorArea(del->x*48, del->y*48,smRemBuilding,(char *)&bv,sizeof(bv));

	if (del->prev)
		buildings = del->prev;
	else if (del->next)
		buildings = del->next;
	else
		buildings = 0;

	delete del;
	
	return buildings;
};

CBuilding::CBuilding(int x, int y, int type, int City, unsigned short id, CServer *Server)
{
	p = Server;
	prev = 0;
	next = 0;
	this->x = x;
	this->y = y;
	this->City = City;
	this->type = type;
	this->id = id;

	this->pop = 0;
	this->AttachedID = 0;
	this->AttachedID2 = 0;
	this->AttachedPop = 0;
	this->AttachedPop2 = 0;

	this->ProduceTick = 0;
	this->CashTick = 0;
	this->PopulationTick = 0;
	this->MoneyTick = 0;
	this->ResearchTick = 0;
}

CBuilding::~CBuilding()
{
	if (next)
		next->prev = prev;
	if (prev)
		prev->next = next;
}

int CBuildingList::GetBuildingCount(int theCity)
{
	CBuilding *bld = buildings;
	int count = 1;

	if (!bld)
		return 0;

	while (bld->prev)
		bld = bld->prev;

	while (bld)
	{
		if (bld->City == theCity)
			count++;
		bld = bld->next;
	}
	return count;
}

int CBuildingList::GetOrbBuildingCount(int theCity)
{
	CBuilding *bld = buildings;
	int count = 1;
	int foundorbfactory = 0;

	if (!bld)
		return 0;

	while (bld->prev)
		bld = bld->prev;

	while (bld)
	{
		if (bld->City == theCity)
		{
			if (bld->type == 16) //Orb Factory
				foundorbfactory = 1;
			count++;
		}
		bld = bld->next;
	}
	if (count < 11 && foundorbfactory == 1)
	{
		return 11;
	}
	else
		return count;
}

CBuilding *CBuildingList::remBuilding(CBuilding *del)
{
	if (!buildings)
		return 0;

	if (del->prev)
		buildings = del->prev;
	else if (del->next)
		buildings = del->next;
	else
		buildings = 0;

	delete del;
	
	return buildings;
};

void CBuildingList::cycle()
{
	sSMItem item;
	sSMItemCount c;
	sSMPop pop;

	CBuilding *bld = p->Build->buildings;

	if (bld)
		while (bld->prev)
			bld = bld->prev;
	if (bld)
	{
		while (bld)
		{
			if (p->Tick > bld->PopulationTick)
			{
				bld->PopulationTick = p->Tick + 250;
				if (bld->pop != 50 && bld->type != 2) //Not a house
				{
					if (bld->AttachedID > 0) //Is attached to a house
					{
						if ((bld->pop + 5) > 50) bld->pop = 50;
						else bld->pop += 5;

						CBuilding *bld2 = p->Build->findBuilding(bld->AttachedID);
						if (bld2)
						{
							if (bld->id == bld2->AttachedID)
							{
								bld2->AttachedPop = bld->pop;
								bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
								pop.id = bld2->id;
								pop.pop = bld2->pop / 16;
								p->Send->SendSectorArea(bld2->x*48, bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
							}
							else if (bld->id == bld2->AttachedID2)
							{
								bld2->AttachedPop2 = bld->pop;
								bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
								pop.id = bld2->id;
								pop.pop = bld2->pop / 16;
								p->Send->SendSectorArea(bld2->x*48, bld2->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
							}
						}

						pop.id = bld->id;
						pop.pop = bld->pop / 8;
						p->Send->SendSectorArea(bld->x*48, bld->y*48,smUpdatePop,(char *)&pop,sizeof(sSMPop));
					}
					else //We need to attach to a house
					{
						CBuilding *bld2 = p->Build->buildings;

						if (bld2)
						{
							while (bld2->prev)
								bld2 = bld2->prev;

							while (bld2)
							{
								if (bld2->City == bld->City)
								{
									if (bld2->type == 2) //House
									{
										if (bld2->AttachedID == 0)
										{
											bld2->AttachedPop = bld->pop;
											bld2->AttachedID = bld->id;
											bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
											bld->AttachedID = bld2->id;
											break;
										}
										else if (bld2->AttachedID2 == 0)
										{
											bld2->AttachedPop2 = bld->pop;
											bld2->AttachedID2 = bld->id;
											bld2->pop = bld2->AttachedPop + bld2->AttachedPop2;
											bld->AttachedID = bld2->id;
											break;
										}
									}
								}
								bld2 = bld2->next;
							}
						}
					}
				}
			}

			if (p->Tick > bld->MoneyTick)
			{
				bld->MoneyTick = p->Tick + 7000;
				if (bld->type == 2)
				{
					p->City[bld->City]->cash += bld->pop * 10000;
					p->City[bld->City]->income += bld->pop * 10000;
				}
				else if (bld->type % 2 && bld->type > 2 && bld->pop == 50)
				{
					p->City[bld->City]->cash -= 750000;
					p->City[bld->City]->cashresearch += 750000;
				}
				else if (bld->type == 1)
				{
					p->City[bld->City]->cash -= 2000000;
					p->City[bld->City]->hospital += 2000000;
				}
			}

			if (p->Tick > bld->ResearchTick)
			{
				bld->ResearchTick = p->Tick + 1000;
				if (bld->pop == 50 && bld->type % 2 && bld->type > 2)
				{
					if (p->City[bld->City]->research[(bld->type - 3) / 2] > 0)
					{
						if (p->City[bld->City]->research[(bld->type - 3) / 2] < p->Tick)
						{
							p->City[bld->City]->research[(bld->type - 3) / 2] = -1;
							p->City[bld->City]->setCanBuild(bld->type, 1);
							if (bld->type == 9) p->City[bld->City]->setCanBuild(0, 1);
							for (int l = 0;l < 12; l++)
							{
								if (buildTree[l] == (bld->type - 3) / 2)
								{
									p->City[bld->City]->setCanBuild((unsigned char)l * 2 + 2, 1);
								}
							}
						}
					}
				}
			}

			if (p->Tick > bld->ProduceTick)
			{
				if ((bld->type % 2) == 0 && bld->type > 2 && bld->pop == 50)
				{
					item.type = itemTypes[(bld->type - 2) / 2 - 1];
					if (p->City[bld->City]->itemC[item.type] < maxItems[item.type] && p->City[bld->City]->canBuild[bld->type - 2] == 2)
					{
						if (p->City[bld->City]->cash > 750000)
						{
							if (bld->ProduceTick == 0)
							{
								c.count = maxItems[item.type] - (unsigned char)p->City[bld->City]->itemC[item.type];
								c.id = bld->id;
								p->Send->SendSectorArea(bld->x*48, bld->y*48,smItemCount,(char *)&c,sizeof(c));
								bld->ProduceTick = p->Tick + 7000;
							}
							else
							{
								if (p->Tick > bld->ProduceTick)
								{
									p->City[bld->City]->cash -= 750000;
									p->City[bld->City]->itemproduction += 750000;
									
									item.x = bld->x - 1;
									item.y = bld->y - 2;
									item.City = bld->City;
									item.active = 0;
									item.id = p->Item->itmID++;
									if (p->Item->itmID > 30000) p->Item->itmID = 1;
									p->Send->SendSectorArea(item.x*48, item.y*48,smAddItem,(char *)&item, sizeof(item));
									p->Item->newItem(item.x,item.y,item.type,item.City,item.id);
									c.count = maxItems[item.type] - (unsigned char)p->City[bld->City]->itemC[item.type];
									c.id = bld->id;
									p->Send->SendSectorArea(item.x*48, item.y*48,smItemCount,(char *)&c,sizeof(c));
									if (c.count == 0)
										bld->ProduceTick = 0;
									else
										bld->ProduceTick = p->Tick + 7000;
								}
							}
						}
					}
				}
			}

			bld = bld->next;
		}
	}
}
