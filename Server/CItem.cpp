#include "CServer.h"

void CItem::drop(int x, int y, int owner)
{	
	sSMItem msg;
	if (holder == owner)
	{
		holder = -1;

		if (this->type == 3 && this->active == 1) this->explodetick = p->Tick + 5000;
		if (this->type == 5) //Orb
		{
			int deleted = 0;
			active = 0;
			for (int i = 0; i < 64; i++)
			{
				if (i != p->Player[owner]->City)
				{
					int CityX = (int)(512)-(32+(i % 8*64));
					int CityY = (int)(512)-(32+(i / 8*64));

					int CalcX = CityX - x;
					int CalcY = CityY - y;

					if (i > 76) CalcX++; //Fix for bottom row bug

					if (CalcX >= 0 && CalcX <= 2 && CalcY == 2)
					{
						int j = p->Build->GetOrbBuildingCount(i);
						if (j >= ORBABLE_SIZE) //Is Orbable
						{
							p->Log->logOrb(owner, i, j);
							p->City[p->Player[owner]->City]->didOrb(i, owner);
							p->City[i]->wasOrbed();
							p->Item->delItem(this);
							deleted = 1;
							break;
						}
					}
				}
			}
			if (deleted == 0)
			{
				sSMItem msg;

				holder = -1;

				this->City = p->Player[owner]->City;
				this->x = x; 
				this->y = y; 

				msg.active = (unsigned char)this->active;
				msg.City = this->City;
				msg.id = this->id;
				msg.type = this->type;
				msg.x = this->x;
				msg.y = this->y;

				p->Send->SendSectorArea(this->x*48, this->y*48, smAddItem, (char *)&msg, sizeof(msg));
			}
		}
		else
		{
			holder = -1;

			this->City = p->Player[owner]->City;
			this->x = x; 
			this->y = y; 

			msg.active = (unsigned char)this->active;
			msg.City = this->City;
			msg.id = this->id;
			msg.type = this->type;
			msg.x = this->x;
			msg.y = this->y;

			p->Send->SendSectorArea(this->x*48, this->y*48 ,smAddItem, (char *)&msg, sizeof(msg));
		}
	}
}

CItem *CItemList::findItem(unsigned short id)
{
	CItem *itm = items;
	if (!itm)
	{
		return 0;
	}

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->id == id)
		{
			return itm;
		}
		itm = itm->next;
	}

	return 0;
}

CItem *CItemList::newItem(int x, int y, int type, int City, unsigned short id)
{
	CItem *itm = items;

	if (!itm)
	{
		items = new CItem(x,y,type,City,id,p);
			
		p->City[City]->itemC[type] += 1;
		return items;
	}
	else
	{
		while(itm->next)
			itm = itm->next;
		
		itm->next = new CItem(x,y,type,City,id,p);
		itm->next->prev = itm;
		p->City[City]->itemC[type] += 1;
		return itm->next;
	}

	return 0;
}

CItem *CItemList::delItem(CItem *del)
{
	if (!items)
	{
		return 0;
	}
		if (del->prev)
		items = del->prev;
	else if (del->next)
		items = del->next;
	else
		items = 0;

	p->City[del->City]->itemC[del->type] -= 1;

	sSMItem packet;
	packet.id = del->id;
	if (del->holder == -1)
		p->Send->SendSectorArea(del->x*48, del->y*48, smRemItem,(char *)&packet,sizeof(packet));
	else
		p->Winsock->SendData(del->holder, smRemItem,(char *)&packet,sizeof(packet));

	delete del;
	
	return items;
};

void CItemList::cycle()
{
#ifndef _DEBUG
	try {
#endif

	CItem *itm = items;
	if (!itm)
		return;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->type == 3 && itm->active == 1) //Ticking Bomb
		{
			if (p->Tick > itm->explodetick && itm->holder == -1) //Boom!
			{
				sSMExplode bomb;
				bomb.City = itm->City;
				bomb.x = (unsigned short)itm->x+1;
				bomb.y = (unsigned short)itm->y+1;
				itm = this->delItem(itm);
				p->Send->SendSectorArea(bomb.x*48, bomb.y*48, smExplosion, (char *)&bomb, sizeof(bomb));

				CBuilding *bld = p->Build->buildings;
				if (bld)
				{
					while (bld->prev)
						bld = bld->prev;

					while (bld)
					{
						if (abs(bld->x - bomb.x) < 3 && abs(bld->y - bomb.y) < 3 && bld->City >= 0)
						{
							bld = p->Build->delBuilding(bld);
						}
						bld = bld->next;
					}
				}

				itm = p->Item->items;

				if (itm)
				{
					while (itm->prev)
						itm = itm->prev;

					while (itm)
					{
						if (abs((itm->x + 1) - bomb.x) < 2 && abs((itm->y + 1) - bomb.y) < 2 && itm->holder == -1)
						{
							itm = delItem(itm);
						}
						itm = itm->next;
					}
				}
			}
		}
		if (itm)
			itm = itm->next;
	}

#ifndef _DEBUG
	}
	catch (...) {p->Log->logServerError("Item-Cycle");}
#endif
}

CItem *CItemList::remItem(CItem *del)
{
	if (!items)
	{
		return 0;
	}
	if (del->prev)
		items = del->prev;
	else if (del->next)
		items = del->next;
	else
		items = 0;

	delete del;
	
	return items;
};
