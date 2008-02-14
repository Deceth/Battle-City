#include "CExplode.h"

CExplosion *CExplosionList::newExplosion(int X, int Y, int Type)
{
#ifndef _DEBUG
	try {
#endif
	CExplosion *Xpl = explosions;

	if (!Xpl)
	{
		explosions = new CExplosion(X, Y, Type, p);
		explosions->tick = p->Tick;
		return explosions;
	}
	else
	{
		while(Xpl->next)
			Xpl = Xpl->next;
		Xpl->next = new CExplosion(X, Y, Type, p);
		Xpl->next->prev = Xpl;
		Xpl->next->tick = p->Tick;
		return Xpl->next;
	}

	return 0;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "newExplosion"); p->Engine->logerror("newExplosion"); return 0;}
#endif
}

void CExplosionList::Cycle()
{
#ifndef _DEBUG
	try {
#endif
	CExplosion *Xpl = explosions;
	if (!Xpl)
		return;

	while (Xpl->prev)
		Xpl = Xpl->prev;

	while (Xpl)
	{
		if (p->Tick > Xpl->tick)
		{
			Xpl->Animation++;
			Xpl->tick = p->Tick+20;
			switch(Xpl->Type)
			{
			case 1:
			case 2:				
				if (Xpl->Animation == 10) Xpl = this->delExplosion(Xpl);
				break;
			case 3:
				if (Xpl->Animation == 3) Xpl = this->delExplosion(Xpl);
				break;
			}
		}
		if (Xpl)
			Xpl = Xpl->next;
	}
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Explosion-Cycle"); p->Engine->logerror("Explosion-Cycle");}
#endif
}

CExplosion *CExplosionList::delExplosion(CExplosion *del)
{
#ifndef _DEBUG
	try {
#endif
	if (!explosions)
		return 0;
	if (del->prev)
		explosions = del->prev;
	else if (del->next)
		explosions = del->next;
	else
		explosions = 0;

	delete del;
	
	return explosions;
#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "delExplosion"); p->Engine->logerror("delExplosion");}
#endif
};