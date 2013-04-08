#include "CCollision.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CCollision::CCollision(CServer *Server) {
	this->p = Server;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CCollision::~CCollision() {
}

/***************************************************************
 * Function:	checkCollision
 *
 * @param tr
 * @param td
 **************************************************************/
int CCollision::checkCollision(Rect tr, Rect td) {
	//memcpy(&tr,&r,sizeof(r));
	//memcpy(&td,&d,sizeof(d));
	tr.w += tr.x;
	tr.h += tr.y;
	td.w += td.x;
	td.h += td.y;

	// If tr or td overlap X, return 0
	if (tr.w < td.x) {
		return 0;
	}
	if (td.w < tr.x) {
		return 0;
	}

	// If tr or td overlap Y, return 0
	if (tr.h < td.y) {
		return 0;
	}
	if (td.h < tr.y) {
		return 0;
	}

	// If no collision, return 1
	return 1;
}
