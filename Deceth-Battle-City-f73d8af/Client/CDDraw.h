#ifndef _CDDRAW
#define _CDDRAW

#include "CGame.h"

class CDDraw
{
public:
	void InitWindowed(HWND hWnd);
	void InitFullscreen(HWND hWnd);
	void LoadSurfaces();
	void DestroySurfaces();

	void Clear();
	void Flip();
	void Draw(IDirectDrawSurface7* Surface, int X, int Y, int width, int height, int _sX = 0, int _sY = 0, int _clipwidth = -1, int _clipheight = -1);
    void DDrawText(IDirectDrawSurface7* Surface, string Text, int X, int Y, int width, int height, int _flags = 0);
	void DTextOut(int X, int Y, string text, COLORREF ColorVal);
	void DFillRect(int X, int Y, int width, int height);
    
	IDirectDrawSurface7 *Back;

	IDirectDrawSurface7 *imgArrows;
	IDirectDrawSurface7 *imgArrowsRed;
	IDirectDrawSurface7 *imgBCLogo;
	IDirectDrawSurface7 *imgBlackNumbers;
	IDirectDrawSurface7 *imgBtnStaff;
	IDirectDrawSurface7 *imgBuildIcons;
	IDirectDrawSurface7 *imgBuildings;
	IDirectDrawSurface7 *imgBullets;
	IDirectDrawSurface7 *imgCompany;
	IDirectDrawSurface7 *imgCursor;
	IDirectDrawSurface7 *imgDemolish;
	IDirectDrawSurface7 *imgGround;
	IDirectDrawSurface7 *imgHealth;
	IDirectDrawSurface7 *imgInterface;
	IDirectDrawSurface7 *imgInterfaceBottom;
	IDirectDrawSurface7 *imgInventorySelection;
	IDirectDrawSurface7 *imgItems;
	IDirectDrawSurface7 *imgLava;
	IDirectDrawSurface7 *imgLoading;
	IDirectDrawSurface7 *imgLExplosion;
	IDirectDrawSurface7 *imgMiniMapColors;
	IDirectDrawSurface7 *imgMoneyBox;
	IDirectDrawSurface7 *imgMoneyDown;
	IDirectDrawSurface7 *imgMoneyUp;
	IDirectDrawSurface7 *imgMuzzleFlash;
	IDirectDrawSurface7 *imgPopulation;
	IDirectDrawSurface7 *imgRadarColors;
	IDirectDrawSurface7 *imgResearch;
	IDirectDrawSurface7 *imgResearchComplete;
	IDirectDrawSurface7 *imgRocks;
	IDirectDrawSurface7 *imgSExplosion;
	IDirectDrawSurface7 *imgSmoke;
	IDirectDrawSurface7 *imgStar;
	IDirectDrawSurface7 *imgTanks;
	IDirectDrawSurface7 *imgTurretBase;
	IDirectDrawSurface7 *imgTurretHead;
    
	CDDraw(CGame *game);
	~CDDraw();

	char FullScreen;
	
	int XOff;
	int YOff;

	int TextWidth;

	IDirectDraw7 *DirectDraw;
private:
	IDirectDrawSurface7 *Primary;
	IDirectDrawClipper *Clipper;

	RECT dest;
	HWND hWnd;

	CGame *p;
protected:
};

#endif

