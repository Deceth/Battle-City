#ifndef __COL__
#define __COL__

#include "CServer.h"

class CServer;

struct Rect {
	long x;
	long y;
	long w;
	long h;
};

class CCollision {

	public:
		CCollision(CServer *Server);
		~CCollision();

		int checkCollision(Rect r, Rect d);

	private:
		CServer *p;
		Rect tr;
		Rect td;

	protected:

};

#endif
