#include "CBullet.h"

CBullet *CBulletList::newBullet(int x, int y, int type, int angle, int owner)
{
	CBullet *blt = bullets;

	if (!blt)
	{
		bullets = new CBullet(x, y, type, angle, owner, p);
		return bullets;
	}
	else
	{
		while(blt->next)
			blt = blt->next;
		blt->next = new CBullet(x, y, type, angle, owner, p);
		blt->next->prev = blt;
		return blt->next;
	}

	return 0;
}

void CBulletList::cycle()
{
	Rect rp, rb;
	rb.w = 4;
	rb.h = 4;

	CBullet *blt = bullets;

	if (!blt)
		return;

	while (blt->prev)
		blt = blt->prev;

	while (blt)
	{
		float fDir = (float)-blt->angle+32;
		float MoveY = (float)(cos((float)(fDir)/16*3.14)) * (p->TimePassed * 0.80f);
		float MoveX = (float)(sin((float)(fDir)/16*3.14)) * (p->TimePassed * 0.80f);

		blt->y += MoveY;
		blt->x += MoveX;

		blt->life -= (p->TimePassed * 0.80f);

		if (blt->x < 0)	{blt->x = 0; blt->life = -1;}
		if (blt->y < 0)	{blt->y = 0; blt->life = -1;}
		if (blt->x > 24576)	{blt->x = 0; blt->life = -1;}
		if (blt->y > 24576)	{blt->y = 0; blt->life = -1;}

		rb.x = (long)blt->x;
		rb.y = (long)blt->y;
		rp.w = 48;
		rp.h = 48;

		for (int pl = 0; pl < MaxPlayers; pl++)
		{
			if (p->Player[pl]->State == State_Game && pl != blt->owner && blt->life > 0 && p->Player[pl]->isDead == false)
			{
				rp.x = (long)p->Player[pl]->X-48;
				rp.y = (long)p->Player[pl]->Y-48;
				if (p->Collision->checkCollision(rp,rb))
				{
					blt->life = -1;
				}
			}
		}

		if (blt->life > 0)
		{
			CItem *itm = p->Item->items;
			if (itm)
				while(itm->prev)
					itm = itm->prev;

			while (itm)
			{
				if (!itm) break;
				rp.x = itm->x * 48 - 48;
				rp.y = itm->y * 48 - 48;
				if (p->Collision->checkCollision(rp,rb) && itm->holder == -1)
				{
					if (itm->type >= 8)
					{
						char dmg = 0;
						switch (blt->type)
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
						itm->life -= dmg;
						if (itm->life < 0)
						{
							itm = p->Item->delItem(itm);
						}
						else
						{
							switch (itm->type)
							{
								case 8:
								case 11:
									if (itm->life < 21)
									{
										sSMItemLife item;
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;
								case 10:
									if (itm->life < 17)
									{
										sSMItemLife item;
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;
								case 9:
									if (itm->life < 9)
									{
										sSMItemLife item;
										item.id = itm->id;
										item.life = 1;
										p->Send->SendSectorArea(itm->x*48, itm->y*48, smItemLife, (char *)&item, sizeof(item));
									}
									break;

							}
						}
						blt->life = -1;
						break;
					}
					else
					{
						blt->life = -1;
						itm = p->Item->delItem(itm);
						break;
					}
				}
				if (itm->next)
					itm = itm->next;
				else
					break;
			}
		}


		// If the bullet has life left,
		if (blt->life > 0) {

			CBuilding *bld = p->Build->buildings;
			if (bld) {
				while(bld->prev) {
					bld = bld->prev;
				}
			}

			while (bld) {
				rp.x = (bld->x-3)*48;
				rp.y = (bld->y-3)*48;
				rp.w = 144;
				rp.h = 144;

				// If the building is a Factory or Hospital (anything with a bay?),
				if (bld->isFactory() || bld->isHospital()) {
					rp.y = (bld->y-2)*48;
					rp.h = 96;
				}
				else {
					bld->type = bld->type;
				}

				// If the bullet hits a building,
				if (p->Collision->checkCollision(rp,rb)) {

					// If that building is not a CC and has 0 (or less) life,
					if ((! bld->isCC()) && bld->pop <= 0) {

						// Destroy the building
						bld = p->Build->delBuilding(bld);
					}
					blt->life = -1;
					break;
				}
				bld = bld->next;
			}
		}

		if (blt->life > 0)
			if (p->Map->map[(int)(blt->x+48)/48][(int)(blt->y+48)/48] == 2) blt->life = -1;
		
		if (blt->life < 0)
		{
			blt = delBullet(blt);
		}
		else
		{
			blt = blt->next;
		}
	}
}

CBullet *CBulletList::delBullet(CBullet *del)
{
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
};
