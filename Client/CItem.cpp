#include "CItem.h"

CItem *CItemList::findItem(unsigned short id)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
		return 0;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->id == id)
			return itm;
		itm = itm->next;
	}
	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "findItem"); p->Engine->logerror("findItem");}
#endif
}

CItem *CItemList::findItembyLocation(int X, int Y)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
		return 0;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->X == X && itm->Y == Y)
			return itm;
		itm = itm->next;
	}
	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "findItembyLocation"); p->Engine->logerror("findItembyLocation");}
#endif
}

CItem *CItemList::findItembyLocationAndType(int X, int Y, int Type)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
		return 0;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->X == X && itm->Y == Y && itm->Type == Type)
			return itm;
		itm = itm->next;
	}
	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "findItembyLocationAndType"); p->Engine->logerror("findItembyLocationAndType");}
#endif
}

CItem *CItemList::newItem(int X, int Y, int Type, int City, unsigned short id, int active)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = 0;
	itm = findItem(id);
	if (itm) return 0;

	itm = items;

	if (!itm)
	{
		items = new CItem(X,Y,Type,City,id,active,p);
		return items;
	}
	else
	{
		while(itm->next)
			itm = itm->next;
			itm->next = new CItem(X,Y,Type,City,id,active,p);
			itm->next->prev = itm;
		return itm->next;
	}

	return 0;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "newItem"); p->Engine->logerror("newItem"); return 0;}
#endif
}

void CItemList::Cycle()
{
	CItem *itm = items;
#ifndef _DEBUG
	try {
#endif
	int me = p->Winsock->MyIndex;
	if (!itm)
		return;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->Type > 8 && itm->active)
		{
			if (itm->Life == 1 && p->Tick > itm->Animationtick)
			{
				itm->Animationtick = p->Tick + 50;
				if (itm->Animation == 2) itm->Animation = 1; else itm->Animation = 2;
			}

			itm->target = -1;
			//int distance = 455; 1024
			int distance = 360;
			int d;
			for (int i = 0; i < MaxPlayers; i++)
			{
				if (p->Player[i]->isInGame && itm->City != p->Player[i]->City && p->Player[i]->isAdmin != 2)
				{
					if (p->Player[i]->isDead == true)
					{

					}
					else
					{
						d = (int)sqrt((p->Player[i]->X-itm->X*48)*(p->Player[i]->X-itm->X*48)+(p->Player[i]->Y-itm->Y*48)*(p->Player[i]->Y-itm->Y*48));
						if (d < distance) // distance from turret < distance
						{
							distance = d; // distance from turret
							itm->target = (char)p->Player[i]->id;
						}
					}
				}
			}

			if (itm->target != -1)
			{
				itm->Angle = (float)atan((float)(p->Player[itm->target]->X / 48 - itm->X) / (p->Player[itm->target]->Y / 48 - itm->Y));
				itm->Angle = (float)(itm->Angle * 180 / 3.14);

				// find what quad

				if (p->Player[itm->target]->X / 48 < itm->X) //  player == right
				{
					if (p->Player[itm->target]->Y / 48 < itm->Y) // player == down
					{
						itm->Angle = 180 - itm->Angle;
					}
					else
					{
						itm->Angle = - itm->Angle;
					}
				} else
				{
					if (p->Player[itm->target]->Y / 48 < itm->Y) // player == down
					{
						itm->Angle = 180 - itm->Angle;
					}
					else
					{
						itm->Angle = 360 - itm->Angle;
					}
				}

				if (p->Tick > itm->lastturn)
				{
					if (!itm->bullet)
					{
						int tmpAngle = 0;
						tmpAngle = (int)(itm->Angle/1.125);
						if (tmpAngle % 10 >= 5)
							tmpAngle += 10;
						tmpAngle = tmpAngle / 10;
						int tmpdmg = 0;
						if (itm->Type == 11) tmpdmg = 5; else tmpdmg = 4;
						float fDir = (float)-tmpAngle+32;
						int FlashY = itm->Y * 48-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
						int FlashX = itm->X * 48-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
						p->Explode->newExplosion(FlashX, FlashY, 3);
						itm->bullet = p->Bullet->newBullet(FlashX, FlashY, tmpdmg, tmpAngle, itm->id);
						if ((abs(itm->X * 48-24 - p->Player[me]->X) < 1000) && (abs(itm->Y * 48-24 - p->Player[me]->Y) < 1000))
						itm->lastturn = p->Tick + 250;
						p->Sound->Play3dSound(p->Sound->s_bigturret, 100, (float)FlashX, (float)FlashY);
					}
					else
					{
						int foundbullet = 0;
						CBullet *blt = p->Bullet->bullets;
						if (blt)
						{
							while (blt->prev)
								blt = blt->prev;

							while (blt)
							{
								if (blt->TurretID == itm->id) foundbullet = 1;
								blt = blt->next;
							}

						}
						else itm->bullet = 0;

						if (foundbullet == 0) itm->bullet = 0;
					}
				}
			}
			else
			{
				if (itm->Type == 10) itm->Animation = 0;
			}
		}
		else
		{
			if (itm->Type == 5) //orb
			{
				if (p->Tick > itm->Animationtick)
				{
					itm->Animationtick = p->Tick + 1000;
					itm->Animation++;
					if (itm->Animation > 2) itm->Animation = 0;
				}
			}
		}
		itm = itm->next;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Item-Cycle"); p->Engine->logerror("Item-Cycle");}
#endif
}

CItem *CItemList::delItem(CItem *del)
{
#ifndef _DEBUG
	try {
#endif
	if (!items)
		return 0;
	if (del->prev)
		items = del->prev;
	else if (del->next)
		items = del->next;
	else
		items = 0;

	delete del;
	
	return items;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "delItem"); p->Engine->logerror("delItem");}
#endif
};

CItem *CInventory::findItem(unsigned short id)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
		return 0;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->id == id)
			return itm;
		itm = itm->next;
	}
	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Inventory-FindItem"); p->Engine->logerror("Inventory-FindItem");}
#endif
}

CItem *CInventory::newItem(int X, int Y, int Type, int City, unsigned short id, int active)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;

	if (!itm)
	{
		items = new CItem(X,Y,Type,City,id,active,p);
		this->SelectedItemType = items->Type;
		return items;
	}
	else
	{
		while(itm->next)
			itm = itm->next;

		itm->next = new CItem(X,Y,Type,City,id,active,p);
		if (!this->SelectedItemType) SelectedItemType = itm->next->Type;
		itm->next->prev = itm;

		return itm->next;
	}

	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Inventory-NewItem"); p->Engine->logerror("Inventory-NewItem"); return 0;}
#endif
}

CItem *CInventory::delItem(CItem *del)
{
#ifndef _DEBUG
	try {
#endif
	if (!items)
		return 0;
	if (del->prev)
	{
		items = del->prev;
	}
	else if (del->next)
	{
		items = del->next;
	}
	else
		items = 0;

	delete del;
	
	CItem *itm = items;
	int i = 0;

	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			i++;
			itm = itm->next;
		}
		i -= 4;
		if (i < 0) i = 0;
	}

	return items;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Inventory-DelItem"); p->Engine->logerror("Inventory-DelItem");}
#endif
};

void CItemList::DestroyCity(char theCity)
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
		if (itm->City == theCity)
		{
			delItem(itm);
			itm = items;
			if (!itm)
				return;

			while (itm->prev)
				itm = itm->prev;
		}
		itm = itm->next;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Item-DestroyCity"); p->Engine->logerror("Item-DestroyCity");}
#endif
}

void CInventory::Cycle()
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
		if (itm->Type == 5) //orb
		{
			if (p->Tick > itm->Animationtick)
			{
				itm->Animationtick = p->Tick + 1000;
				itm->Animation++;
				if (itm->Animation > 2) itm->Animation = 0;
			}
		}
		itm = itm->next;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Inventory-Cycle"); p->Engine->logerror("Inventory-Cycle");}
#endif
}

void CInventory::ItemCheck()
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
	{
		p->InGame->HasLaser = 0;
		p->InGame->HasRocket = 0;
		p->InGame->HasUpLink = 0; 
		return;
	}

	while (itm->prev)
		itm = itm->prev;

	int hasLaser = 0;
	int hasRocket = 0;
	int hasUpLink = 0;

	while (itm)
	{
		switch (itm->Type)
		{
			case 0:
				hasLaser = 1;
				break;
			case 1:
				hasRocket = 1;
				break;
			case 6:
				hasUpLink = 1;
				break;
		}
		itm = itm->next;
	}

	if (hasLaser == 1) p->InGame->HasLaser = 1; else p->InGame->HasLaser = 0;
	if (hasRocket == 1) p->InGame->HasRocket = 1; else p->InGame->HasRocket = 0;
	if (hasUpLink == 1) p->InGame->HasUpLink = 1; else p->InGame->HasUpLink = 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "ItemCheck"); p->Engine->logerror("ItemCheck");}
#endif
}

CItem *CInventory::findItembyType(int Type)
{
#ifndef _DEBUG
	try {
#endif
	CItem *itm = items;
	if (!itm)
		return 0;

	while (itm->prev)
		itm = itm->prev;

	while (itm)
	{
		if (itm->Type == Type)
			return itm;
		itm = itm->next;
	}
	return 0;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "findItembyType"); p->Engine->logerror("findItembyType");}
#endif
}

void CInventory::Drop()
{
	int me = p->Winsock->MyIndex;
	int mYX = (int)p->Player[me]->X / 48;
	int mYY = (int)p->Player[me]->Y / 48;

	if (p->Player[p->Winsock->MyIndex]->isFrozen == 0)
	{
		sCMItem ii;
		CItem *itm = p->Inventory->findItembyType(p->Inventory->SelectedItemType);
		if (itm)
		{
			ii.id = itm->id;
			if (itm->Type >= 8 || itm->Type == 4) 
			{
				if (p->Build->inRange() == 1)
				{
					itm->active = 1;
					ii.active = itm->active;
					ii.type = itm->Type;
					p->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
				}
			}
			else
			{
				if (itm->Type == 7) itm->active = 1;
				if (itm->Type == 3) itm->active = p->InGame->BombsAreActivated;
				ii.active = itm->active;
				ii.type = itm->Type;
				p->Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
			}
		}
	}
}

CItem::CItem(int X, int Y, int Type, int City, unsigned short id, int active, CGame *game)
{
	p = game;
	prev = 0;
	next = 0;
	Angle = 0;
	status = 0;
	dmg = 0;
	lastturn = 0;
	target = -1;
	bullet = 0;
	Life = 0;

	Animation = 0;
	Animationtick = 0;

	if (Type > 8)
	{
		lastturn = p->Tick + 2000;
		if (Type == 9 || Type == 10)
			dmg = 15;
		else
			dmg = 30;
	}
	this->X = X;
	this->Y = Y;
	this->Type = Type;
	this->id = id;
	this->City = City;
	this->active = active;
}
CItem::~CItem()
{
	if (next)
		next->prev = prev;
	if (prev)
		prev->next = next;
}