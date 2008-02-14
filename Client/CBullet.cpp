#include "CBullet.h"

CBullet *CBulletList::newBullet(int x, int Y, int Type, int Angle, int Owner)
{
#ifndef _DEBUG
	try {
#endif
	CBullet *blt = bullets;

	if (!blt)
	{
		bullets = new CBullet(x, Y, Type, Angle, Owner, p);
		return bullets;
	}
	else
	{
		while(blt->next)
			blt = blt->next;
		blt->next = new CBullet(x, Y, Type, Angle, Owner, p);
		blt->next->prev = blt;
		return blt->next;
	}

	return 0;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "NewBullet"); p->Engine->logerror("NewBullet"); return 0;}
#endif
}

void CBulletList::Cycle()
{
	float tick = p->Tick;

	CBullet *blt = bullets;

#ifndef _DEBUG
	try {
#endif

	Rect rp, rb;
	rb.w = 4;
	rb.h = 4;
	int me = p->Winsock->MyIndex;
	if (!blt)
		return;

	while (blt->prev)
		blt = blt->prev;

	while (blt)
	{
		float fDir = (float)-blt->Angle+32;
		float MoveY = (float)(cos((float)(fDir)/16*3.14)) * (p->TimePassed * 0.80f);
		float MoveX = (float)(sin((float)(fDir)/16*3.14)) * (p->TimePassed * 0.80f);
		if (MoveX > 20) MoveX = 20;
		if (MoveY > 20) MoveY = 20;

		blt->Y += MoveY;
		blt->X += MoveX;

		blt->Life -= (p->TimePassed * 0.80f);

		if (blt->X < 0)	{blt->X = 0; blt->Life = -1;}
		if (blt->Y < 0)	{blt->Y = 0; blt->Life = -1;}
		if (blt->X > 24576)	{blt->X = 0; blt->Life = -1;}
		if (blt->Y > 24576)	{blt->Y = 0; blt->Life = -1;}

		rb.X = (long)blt->X;
		rb.Y = (long)blt->Y;
		rp.w = 48;
		rp.h = 48;

		if (blt->TurretID == 0 && p->Player[blt->Owner]->isAdmin == 2) 
		{
			switch(blt->Type)
			{
			case 1:
				 p->Explode->newExplosion((int)blt->X+24, (int)blt->Y+24, 1);
				 break;
			case 2:
				 p->Explode->newExplosion((int)blt->X+72, (int)blt->Y+72, 2);
				 break;
			case 3:
				 p->Explode->newExplosion((int)blt->X+2, (int)blt->Y+2, 3);
				 break;
			}
		}

		for (int pl = 0; pl < MaxPlayers; pl++)
		{
			if (p->Player[pl]->isInGame && pl != blt->Owner)
			{
				if (p->Player[pl]->isDead == true)
				{

				}
				else
				{
 					rp.X = (long)p->Player[pl]->X-48;
					rp.Y = (long)p->Player[pl]->Y-48;
					if (p->Collision->RectCollision(rp,rb))
					{
						blt->Life = -1;
						if (pl == p->Winsock->MyIndex && p->Player[pl]->isAdmin != 2)
						{
							char dmg = 0;
							switch (blt->Type)
							{
								case 0:
									dmg = 5;
									break;
								case 1:
									dmg = 8;
									break;
								case 2:
 									dmg = 12;
									break;
							}
							if (p->Player[pl]->HP <= dmg || (blt->TurretID == 0 && p->Player[blt->Owner]->isAdmin == 2))
							{
								char packet[3];
								if (blt->TurretID == 0)
								{
									packet[0] = p->Player[blt->Owner]->City;
								}
								else
								{
									CItem *itm = p->Item->findItem(blt->TurretID);
									if (itm)
									{
										packet[0] = itm->City;
									}
								}
								packet[1] = 0;
								p->Player[p->Winsock->MyIndex]->isDead = true;
								p->InGame->timeDeath = p->Tick;
								p->Winsock->SendData(cmDeath, packet, 1);
								p->Sound->PlayWav(sDie, 5);
							}
							p->Player[pl]->SetHP(p->Player[pl]->HP - dmg);
							p->Sound->PlayWav(sHit, 13);
						}
					}
				}
			}
		}

		CItem *itm = p->Item->items;
		if (itm)
			while(itm->prev)
				itm = itm->prev;

		while (itm)
		{
			rp.X = itm->X * 48 - 48;
			rp.Y = itm->Y * 48 - 48;
			if (p->Collision->RectCollision(rp,rb) && itm->id != blt->TurretID)
			{
				if (blt->TurretID > 0)
				{
					if (itm->active && itm->Type >= 8)
					{
						blt->Life = -1;
						if ((abs(blt->X - p->Player[me]->X) < 1000) && (abs(blt->Y - p->Player[me]->Y) < 1000))
						{
							p->Explode->newExplosion((int)blt->X+24, (int)blt->Y+24, 1);
							p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->X, blt->Y);
						}
						break;
					}
				}
				else
				{
					blt->Life = -1;
					if ((abs(blt->X - p->Player[me]->X) < 1000) && (abs(blt->Y - p->Player[me]->Y) < 1000))
					{
						p->Explode->newExplosion((int)blt->X+24, (int)blt->Y+24, 1);
						p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->X, blt->Y);
					}
					break;
				}
			}
			itm = itm->next;
		}

		CBuilding *bld = p->Build->buildings;
		if (bld)
			while(bld->prev)
				bld = bld->prev;

		while (bld)
		{
			rp.X = (bld->X-3)*48;
			rp.Y = (bld->Y-3)*48;
			rp.w = 144;
			rp.h = 144;
			if (bld->Type / 100 <= 2)
			{
				rp.Y = (bld->Y-2)*48;
				rp.h = 96;
			}
			if (p->Collision->RectCollision(rp,rb))
			{
				blt->Life = -1;
				if ((abs(blt->X - p->Player[me]->X) < 1000) && (abs(blt->Y - p->Player[me]->Y) < 1000))
				{
					p->Explode->newExplosion((int)blt->X+24, (int)blt->Y+24, 1);
					p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->X, blt->Y);
				}
			}
			bld = bld->next;
		}

		if (p->Map->map[(int)(blt->X+48)/48][(int)(blt->Y+48)/48] == 2) 
		{
			if ((abs(blt->X - p->Player[me]->X) < 1000) && (abs(blt->Y - p->Player[me]->Y) < 1000))
			{
				p->Explode->newExplosion((int)blt->X+24, (int)blt->Y+24, 1);
				p->Sound->Play3dSound(p->Sound->s_eXplode, 100, blt->X, blt->Y);
			}
			blt->Life = -1;
		}
		
		if (blt->Life < 0)
		{ 
			blt = delBullet(blt);
		}
		else
		{
			blt = blt->next;
		}
	}

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "BulletCycle"); p->Engine->logerror("BulletCycle"); if (blt) delBullet(blt);}
#endif
}

CBullet *CBulletList::delBullet(CBullet *del)
{
#ifndef _DEBUG
	try {
#endif
	if (!bullets)
		return 0;
	if (del->prev)
		bullets = del->prev;
	else if (del->next)
		bullets = del->next;
	else
		bullets = 0;

	delete del;
	
	return bullets;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "delBullet"); p->Engine->logerror("delBullet");}
#endif
};