#ifndef __BULLETS__
#define __BULLETS__

#include "CGame.h"

class CGame;

class CBullet {

	public:
		CGame *p;

		float x;
		float y;
		int type;
		int damage;
		unsigned short owner;
		float life;
		char angle;
		char animation;
		unsigned short turretId;

		CBullet *next;
		CBullet *prev;

		CBullet(int x, int y, int type, int angle, unsigned short owner, CGame *game);
		~CBullet();
};


class CBulletList {
	public:
		CBullet *bulletListHead;

		CBullet *newBullet(int x, int y, int type, int angle, int owner = 999);
		CBullet *delBullet(CBullet *del);

		CBulletList(CGame *game);
		~CBulletList();

		void Cycle();
		CGame *p;
};

#endif
