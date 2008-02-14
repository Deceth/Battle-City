#ifndef __BULLETS__
#define __BULLETS__

#include "CGame.h"

class CGame;

class CBullet
{
public:
	CGame *p;

	float X;
	float Y;
	int Type;
	unsigned short Owner;
	float Life;
	char Angle;
	char Animation;
	unsigned short TurretID;

	CBullet *next;
	CBullet *prev;

	CBullet(int X, int Y, int Type, int Angle, unsigned short Owner, CGame *Game)
	{
		p = Game;
		this->Owner = Owner;
		this->X = (float)X;
		this->Y = (float)Y;
		this->Type = Type;
		this->Animation = 0;
		this->Angle = Angle;
		if (this->Type > 2) 
		{
			this->TurretID = this->Owner;
			this->Owner = 0;
			this->Type -= 3;
		}
		else this->TurretID = 0;
		switch(this->Type)
		{
			case 0:
				this->Life = 260;
				break;
			case 1:
			case 2:
				this->Life = 340;
				break;
		}

		prev = 0;
		next = 0;
	}
	~CBullet()
	{
		if (next)
			next->prev = prev;
		if (prev)
			prev->next = next;
	}
};

class CBulletList
{
public:
	CBullet *bullets;

	CBullet *newBullet(int x, int Y, int Type, int Angle, int Owner = 999);
	CBullet *delBullet(CBullet *del);

	CBulletList(CGame *game)
	{
		p = game;
		bullets = 0;
	}
	~CBulletList()
	{
		while (bullets)
			delBullet(bullets);
	}

	void Cycle();
	CGame *p;
};

#endif