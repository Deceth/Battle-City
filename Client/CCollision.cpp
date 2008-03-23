#include "CCollision.h"

CCollision::CCollision(CGame *game) {
	this->p = game;
}

CCollision::~CCollision() {

}

int CCollision::CheckPlayerCollision(unsigned short id) {
	
	// If the player is an admin, return
	if (p->Player[id]->isAdmin == 2) {
		return 0;
	}

	Rect PlayerRect;
	Rect tPlayerRect;
	Rect BuildingRect;
	Rect ItemRect;
	int gap = 8;

	PlayerRect.X = (long)p->Player[id]->X + gap;
	PlayerRect.Y = (long)p->Player[id]->Y + gap;
	PlayerRect.w = (long)48 - gap - gap;
	PlayerRect.h = (long)48 - gap - gap; 

	//fill in a smaller collision area
	tPlayerRect.X = (long)p->Player[id]->X+24;
	tPlayerRect.Y = (long)p->Player[id]->Y+24;
	tPlayerRect.w = (long)1;
	tPlayerRect.h = (long)1;

	//Map edges
	if (p->Player[id]->X < 0) return 200;
	if (p->Player[id]->X > (511 * 48)) return 201;
	if (p->Player[id]->Y < 0) return 202;
	if (p->Player[id]->Y > (511 * 48)) return 203;

	//Map Terrain (lava, rocks, CC corners)
	if (p->Map->map[(int)(PlayerRect.X-1)/48][(int)(PlayerRect.Y-1)/48] != 0) return 2; //top left corner
	if (p->Map->map[(int)(PlayerRect.X+PlayerRect.w)/48][(int)(PlayerRect.Y-1)/48] != 0) return 2; //top right corner
	if (p->Map->map[(int)(PlayerRect.X+PlayerRect.w)/48][(int)(PlayerRect.Y+PlayerRect.h)/48] != 0) return 2; //bottom right corner
	if (p->Map->map[(int)(PlayerRect.X-1)/48][(int)(PlayerRect.Y+PlayerRect.h)/48] != 0) return 2; //bottom left corner

	CBuilding *bld = p->Build->buildings;
	if (bld) {
		while (bld->prev) {
			bld = bld->prev;
		}

		while (bld) {
			BuildingRect.X = (bld->X-2)*48;
			BuildingRect.Y = (bld->Y-2)*48;
			BuildingRect.w = 144;
			BuildingRect.h = 144;

			if (bld->Type / 100 <= 2) {

				if (bld->Type == 200 && p->Tick > p->InGame->hospTick) {

					if (id == p->Winsock->MyIndex && RectCollision(tPlayerRect, BuildingRect)) {

						BuildingRect.Y = (bld->Y-1)*48;
						BuildingRect.h = 96;

						if (!RectCollision(tPlayerRect, BuildingRect)) {
							p->Player[p->Winsock->MyIndex]->SetHP(p->Player[p->Winsock->MyIndex]->HP + 5);

							if (p->Player[p->Winsock->MyIndex]->HP > 40) {
								p->Player[p->Winsock->MyIndex]->SetHP(40);
							}

							p->InGame->hospTick = p->Tick + 500;
						}
					}
					else {
						BuildingRect.Y = (bld->Y-1)*48;
						BuildingRect.h = 96;
					}
				}
				else {
					BuildingRect.Y = (bld->Y-1)*48;
					BuildingRect.h = 96;
				}
			}

			/*
			if (RectCollision(tPlayerRect, BuildingRect)) {
				return 2;
			}
			*/
			
			if (RectCollision(PlayerRect, BuildingRect)) {
				return 2;
			}

			bld = bld->next;
		}
	}

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			ItemRect.X = itm->X*48+gap;
			ItemRect.Y = itm->Y*48+gap;
			ItemRect.w = 48-gap;
			ItemRect.h = 48-gap;
			
			if (RectCollision(tPlayerRect, ItemRect) && itm->Type >= 8 && itm->active == 1) //collision when placed on top of an item
			{
				return 2;
			}
			
			if (itm->active)
			{
				if (RectCollision(PlayerRect, ItemRect) && itm->Type >= 8 && itm->active)	return 2; //walls and turrets

				//Mine and DFG checking, use smaller player collision area for player
				if (RectCollision(tPlayerRect, ItemRect) && itm->Type == 4 && p->Player[id]->City != itm->City) return 101;
				if (RectCollision(tPlayerRect, ItemRect) && itm->Type == 7 && p->Player[id]->City != itm->City) return 103;

			}
			itm = itm->next; //isMoving to next item
		}
	}
	return 0;
}

int CCollision::RectCollision(Rect rect1, Rect rect2) {
	rect1.w += rect1.X; 
	rect1.h += rect1.Y;
	rect2.w += rect2.X;
	rect2.h += rect2.Y;
	
	if (rect1.w < rect2.X)
		return 0;
	if (rect2.w < rect1.X)
		return 0;

	if (rect1.h < rect2.Y)
		return 0;
	if (rect2.h < rect1.Y)
		return 0;

	return 1;
}

int CCollision::CheckBuildingCollision(int X, int Y) {
	int me = p->Winsock->MyIndex;

	if (X < 0 || Y < 0) return 1;

	//map objects
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (p->Map->map[X-j][Y-i] != 0)
				return 1;

	CItem *itm = p->Item->items;
	if (itm)
	{
		while (itm->prev)
			itm = itm->prev;

		while (itm)
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				if (X == itm->X+j && Y == itm->Y+i)
					return 1;
			itm = itm->next;
		}
	}

	CBuilding *bld = p->Build->buildings;
	if (bld)
	{
		while (bld->prev)
			bld = bld->prev;

		while (bld)
		{
			if (X >= bld->X-2 && X <= bld->X+2 && Y >= bld->Y-2 && Y <= bld->Y+2)
				return 1;
			bld = bld->next;
		}
	}

	//players
	for (int a = 0; a < MaxPlayers; a++)
	{
		if (p->Player[a]->X-48 <= X*48 && p->Player[a]->X+48 >= (X-2)*48 && p->Player[a]->Y-48 <= Y*48 && p->Player[a]->Y+48 >= (Y-2)*48) return 1;		
	}
	if (p->Player[me]->X-48 <= X*48 && p->Player[me]->X+48 >= (X-2)*48 && p->Player[me]->Y-48 <= Y*48 && p->Player[me]->Y+48 >= (Y-2)*48) return 1;

	return 0; //no colliding object
}