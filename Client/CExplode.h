#ifndef __CEXPLODE__
#define __CEXPLODE__

#include "CGame.h"

class CGame;

class CExplosion
{
public:
	CGame *p;

	int X, Y;
	int Type;
	int Animation;
	float tick;

	CExplosion *next;
	CExplosion *prev;

	CExplosion(int X, int Y, int Type, CGame *Game)
	{
		p = Game;
		this->X = X;
		this->Y = Y;
		this->Type = Type;
		this->Animation = 0;
		this->tick = 0;

		prev = 0;
		next = 0;
	}
	~CExplosion()
	{
		if (next)
			next->prev = prev;
		if (prev)
			prev->next = next;
	}
};

class CExplosionList
{
public:
	CExplosion *explosions;

	CExplosion *newExplosion(int X, int Y, int Type);
	CExplosion *delExplosion(CExplosion *del);

	CExplosionList(CGame *game)
	{
		p = game;
		explosions = 0;
	}
	~CExplosionList()
	{
		while (explosions)
			delExplosion(explosions);
	}

	void Cycle();
	CGame *p;
};

#endif