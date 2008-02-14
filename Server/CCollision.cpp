#include "CCollision.h"
Rect tr, td;

CCollision::CCollision(CServer *Server)
{
	p = Server;
}

CCollision::~CCollision()
{

}

int CCollision::checkCollision(Rect tr, Rect td)
{
	//memcpy(&tr,&r,sizeof(r));
	//memcpy(&td,&d,sizeof(d));
	tr.w += tr.x;
	tr.h += tr.y;
	td.w += td.x;
	td.h += td.y;

	if (tr.w < td.x)
		return 0;
	if (td.w < tr.x)
		return 0;

	if (tr.h < td.y)
		return 0;
	if (td.h < tr.y)
		return 0;

	return 1;
}
