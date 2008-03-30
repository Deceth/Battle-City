#ifndef __CEXPLODE__
#define __CEXPLODE__

#include "CGame.h"

class CGame;

class CExplosion {

	public:
		CGame *p;

		int x;
		int y;
		int type;
		int animation;
		float tick;

		CExplosion *next;
		CExplosion *prev;

		CExplosion(int x, int y, int type, CGame *game);
		~CExplosion();
};


class CExplosionList {

	public:
		CExplosion *explosionListHead;

		CExplosion *newExplosion(int X, int Y, int Type);
		CExplosion *delExplosion(CExplosion *del);

		CExplosionList(CGame *game);
		~CExplosionList();

		void Cycle();
		CGame *p;
};

#endif