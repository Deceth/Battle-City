#ifndef __BULLETS__
#define __BULLETS__

#include "CServer.h"

class CServer;

class CBullet
{
public:
	CServer *p;

	float x;
	float y;
	int type;
	unsigned short owner;
	float life;
	char angle;
	char anim;
	unsigned short turid;

	CBullet *next;
	CBullet *prev;

	CBullet(int x, int y, int type, int angle, unsigned short owner, CServer *Server)
	{
		p = Server;
		this->owner = owner;
		this->x = (float)x;
		this->y = (float)y;
		this->type = type;
		this->anim = 0;
		this->angle = angle;
		if (this->type > 2) 
		{
			this->turid = this->owner;
			this->type -= 3;
		}
		else this->turid = 0;
		switch(this->type)
		{
			case 0:
				this->life = 260;
				break;
			case 1:
			case 2:
				this->life = 340;
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

	CBullet *newBullet(int x, int y, int type, int angle, int owner = 999);
	CBullet *delBullet(CBullet *del);

	CBulletList(CServer *server)
	{
		p = server;
		bullets = 0;
	}
	~CBulletList()
	{
		while (bullets)
			delBullet(bullets);
	}

	void cycle();
	CServer *p;
};

#endif
