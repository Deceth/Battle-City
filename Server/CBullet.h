#ifndef __BULLETS__
#define __BULLETS__

#include "CServer.h"

class CServer;

class CBullet {

	public:
		CServer *p;

		float x;
		float y;
		int type;
		int damage;
		unsigned short owner;
		float life;
		char angle;
		char anim;
		unsigned short turid;

		CBullet *next;
		CBullet *prev;

		CBullet(int x, int y, int type, int angle, unsigned short owner, CServer *Server);
		~CBullet();
};


class CBulletList {
	public:
		CBullet *bulletListHead;

		CBullet *newBullet(int x, int y, int type, int angle, int owner = 999);
		CBullet *delBullet(CBullet *del);

		CBulletList(CServer *server);
		~CBulletList();

		void cycle();
		CServer *p;
};

#endif
