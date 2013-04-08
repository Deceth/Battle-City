/*
===============================================================================

    Battle City - CCollision file
    Copyright (C) 2005-2013  battlecity.org

    This file is part of Battle City.

    Battle City is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battle City is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battle City.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/
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
