#ifndef _CCollision
#define _CCollision

#include "CGame.h"

struct Rect
{
	long X;
	long Y;
	long w;
	long h;
};

class CGame;
class CPlayer;

class CCollision
{
public:
	CCollision(CGame *game);
	~CCollision();

	int CheckPlayerCollision(unsigned short id);
	int RectCollision(Rect rect1, Rect rect2);
	bool RectCollision(int X, int Y, int width, int height,
			int tolerance1, int X2, int Y2, int width2, int height2,
			int tolerance2);
	int CheckBuildingCollision(int X, int Y);
private:
	CGame *p;
protected:

};

#endif