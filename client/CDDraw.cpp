/*
===============================================================================

    Battle City - CDDraw
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
#include "CGame.h"
#include "DDUtil.h"

CDDraw::CDDraw(CGame *game)
{
	FullScreen = 0;
	XOff = 0;
	YOff = 0;

	p = game;
}

CDDraw::~CDDraw()
{
	//DestroySurfaces();
}


void CDDraw::InitWindowed(HWND hWnd)
{
	this->hWnd = hWnd;

	long hr;

	if ((hr = DirectDrawCreateEx(NULL, (void **)&DirectDraw, IID_IDirectDraw7, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: DirectDrawCreateEX", "Error", 0);
	}

	if ((hr = DirectDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL)) != DD_OK)
	{
		MessageBox(NULL, "Error: SetCooperativeLevel", "Error", 0);
	}

	DDSURFACEDESC2 ddsd;
	if ((hr = DirectDraw->CreateClipper(NULL, &Clipper, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateClipper", "Error", 0);
	}

	Clipper->SetHWnd(0, hWnd);

	memset (&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if ((hr = DirectDraw->CreateSurface(&ddsd, &Primary, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateSurface", "Error", 0);
	}

	if ((hr = Primary->SetClipper(Clipper)) != DD_OK)
	{
		MessageBox(NULL, "Error: SetClipper", "Error", 0);
	}

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = p->ResolutionX + XOff + 3;
	ddsd.dwHeight = p->ResolutionY + YOff + 3;

	if ((hr = DirectDraw->CreateSurface(&ddsd, &Back, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateBackSurface", "Error", 0);
	}

	LoadSurfaces();
	GetWindowRect(hWnd, &dest);
}

void CDDraw::InitFullscreen(HWND hWnd)
{
	MoveWindow(hWnd, 0, 0, p->ResolutionX, p->ResolutionY, 1);
	FullScreen = 1;
	this->hWnd = hWnd;

	long hr;

	if ((hr = DirectDrawCreateEx(NULL, (void **)&DirectDraw, IID_IDirectDraw7, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: DirectDrawCreateEX", "Error", 0);
	}

	if ((hr = DirectDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL)) != DD_OK)
	{
		MessageBox(NULL, "Error: SetCooperativeLevel", "Error", 0);
	}

	DDSURFACEDESC2 ddsd;
	if ((hr = DirectDraw->CreateClipper(NULL, &Clipper, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateClipper", "Error", 0);
	}

	if ((hr = DirectDraw->SetDisplayMode(p->ResolutionX, p->ResolutionY, 16, 0, 0)) != DD_OK)
	{
		MessageBox(NULL, "Error: SetDisplayMode", "Error", 0);
	}

	Clipper->SetHWnd(0, hWnd);

	memset (&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if ((hr = DirectDraw->CreateSurface(&ddsd, &Primary, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateSurface", "Error", 0);
	}

	if ((hr = Primary->SetClipper(Clipper)) != DD_OK)
	{
		MessageBox(NULL, "Error: SetClipper", "Error", 0);
	}

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = p->ResolutionX;
	ddsd.dwHeight = p->ResolutionY;

	if ((hr = DirectDraw->CreateSurface(&ddsd, &Back, NULL)) != DD_OK)
	{
		MessageBox(NULL, "Error: CreateBackSurface", "Error", 0);
	}

	LoadSurfaces();
	GetWindowRect(hWnd, &dest);
}

void CDDraw::Clear()
{
	DDBLTFX ddbltfX;
	memset(&ddbltfX, 0, sizeof(ddbltfX));
	ddbltfX.dwSize = sizeof(ddbltfX);
	Back->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfX);
}

void CDDraw::Flip()
{
	GetWindowRect(hWnd, &dest);
	Primary->Blt(&dest, Back, NULL, DDBLT_WAIT, NULL);
}

void CDDraw::Draw(IDirectDrawSurface7* Surface, int X, int Y, int width, int height, int _sX, int _sY, int _clipwidth, int _clipheight)
{
	if (_clipwidth == -1) _clipwidth = p->ResolutionX;
	if (_clipheight == -1) _clipheight = p->ResolutionY;

	X += XOff;
	Y += YOff;
	_clipwidth += XOff;
	_clipheight += YOff;

	long hr;
	RECT rct;
	int l = 0, t = 0, r = 0, b = 0;

	if (!Back) return;
	if (!Surface) return;

	if (X < 0) {l = abs(X); X = 0; width = width - l; if (width <= 0) return;} 
	if (Y < 0) {t = abs(Y); Y = 0; height = height - t; if (height <= 0) return;}

	l = l + _sX;
	t = t + _sY;
	r = l + width;
	b = t + height;

	if (X + width > _clipwidth) {r = r - (X + width - _clipwidth); if (r <= l) return;}
	if (Y + height > _clipheight) {b = b - (Y + height - _clipheight); if (b <= t) return;}

	rct.left = l;
	rct.top = t;
	rct.right = r;
	rct.bottom = b;

	hr = Back->BltFast(X, Y, Surface, &rct, DDBLTFAST_WAIT || DDBLTFAST_SRCCOLORKEY);
}
/// <summary>	Initializes surfaces for direct draw</summary>
void CDDraw::LoadSurfaces()
{ 
	//	Initialize text metric, basic structure for a physical font
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/dd145132(v=vs.85).aspx
	TEXTMETRIC tm;
	//	Initialize device context handler
	HDC surfDC;
	//	Populate surfDC with GDI-compatible handle of device context for the surface
	Back->GetDC(&surfDC);
	//	Populate tm structure with text metrics from device context
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/dd144941(v=vs.85).aspx
	GetTextMetrics(surfDC, &tm);
	//	Set TextWidth to average character width value
	TextWidth = tm.tmAveCharWidth;
	//	Release the surfDC device context
	Back->ReleaseDC(surfDC);
	//
	//	Loading resources into surface objects
	//	
	//	Load imgArrowsRed.bmp as surface
	imgArrowsRed = DDLoadBitmap(DirectDraw, "imgArrowsRed.bmp", 0, 0);
	//	Set the color key to be transparent
	DDSetColorKey(imgArrowsRed, COLOR_TRANSPARENCY);
	imgArrows = DDLoadBitmap(DirectDraw, "imgArrows.bmp", 0, 0);
	DDSetColorKey(imgArrows, COLOR_TRANSPARENCY);
	imgBCLogo = DDLoadBitmap(DirectDraw, "imgBCLogo.bmp", 0, 0);
	//	Set the color key to black
	DDSetColorKey(imgBCLogo, 0);
	imgBlackNumbers = DDLoadBitmap(DirectDraw, "imgBlackNumbers.bmp", 0, 0);
	DDSetColorKey(imgBlackNumbers, COLOR_TRANSPARENCY);
	imgBtnStaff = DDLoadBitmap(DirectDraw, "imgBtnStaff.bmp", 0, 0);
	DDSetColorKey(imgBtnStaff, 0);
	imgBuildIcons = DDLoadBitmap(DirectDraw, "imgBuildIcons.bmp", 0, 0);
	DDSetColorKey(imgBuildIcons, 0);
	imgBuildings = DDLoadBitmap(DirectDraw, "imgBuildings.bmp", 0, 0);
	DDSetColorKey(imgBuildings, COLOR_TRANSPARENCY);
	imgBullets = DDLoadBitmap(DirectDraw, "imgBullets.bmp", 0, 0);
	DDSetColorKey(imgBullets, COLOR_TRANSPARENCY);
	imgCompany = DDLoadBitmap(DirectDraw, "imgCompany.bmp", 0, 0);
	DDSetColorKey(imgCompany, 0);
	imgCursor = DDLoadBitmap(DirectDraw, "imgCursor.bmp", 0, 0);
	DDSetColorKey(imgCursor, COLOR_TRANSPARENCY);
	imgDemolish = DDLoadBitmap(DirectDraw, "imgDemolish.bmp", 0, 0);
	DDSetColorKey(imgDemolish, COLOR_TRANSPARENCY);
	imgGround = DDLoadBitmap(DirectDraw, "imgGround.bmp", 0, 0);
	DDSetColorKey(imgGround, 0);
	imgHealth = DDLoadBitmap(DirectDraw, "imgHealth.bmp", 0, 0);
	DDSetColorKey(imgHealth, 0);
	imgInterface = DDLoadBitmap(DirectDraw, "imgInterface.bmp", 0, 0);
	DDSetColorKey(imgInterface, COLOR_TRANSPARENCY);
	imgInterfaceBottom = DDLoadBitmap(DirectDraw, "imgInterfaceBottom.bmp", 0, 0);
	DDSetColorKey(imgInterfaceBottom, COLOR_TRANSPARENCY);
	imgInventorySelection = DDLoadBitmap(DirectDraw, "imgInventorySelection.bmp", 0, 0);
	DDSetColorKey(imgInventorySelection, 0);
	imgItems = DDLoadBitmap(DirectDraw, "imgItems.bmp", 0, 0);
	DDSetColorKey(imgItems, COLOR_TRANSPARENCY);
	imgLava = DDLoadBitmap(DirectDraw, "imgLava.bmp", 0, 0);
	DDSetColorKey(imgLava, COLOR_TRANSPARENCY);
	imgLoading = DDLoadBitmap(DirectDraw, "imgLoading.bmp", 0, 0);
	DDSetColorKey(imgLoading, COLOR_TRANSPARENCY);
	imgLExplosion = DDLoadBitmap(DirectDraw, "imgLExplosion.bmp", 0, 0);
	DDSetColorKey(imgLExplosion, COLOR_TRANSPARENCY);
	imgMiniMapColors = DDLoadBitmap(DirectDraw, "imgMiniMapColors.bmp", 0, 0);
	DDSetColorKey(imgMiniMapColors, 0);
	imgMoneyBox = DDLoadBitmap(DirectDraw, "imgMoneyBox.bmp", 0, 0);
	DDSetColorKey(imgMoneyBox, COLOR_TRANSPARENCY);
	imgMoneyDown = DDLoadBitmap(DirectDraw, "imgMoneyDown.bmp", 0, 0);
	DDSetColorKey(imgMoneyDown, COLOR_TRANSPARENCY);
	imgMoneyUp = DDLoadBitmap(DirectDraw, "imgMoneyUp.bmp", 0, 0);
	DDSetColorKey(imgMoneyUp, COLOR_TRANSPARENCY);
	imgMuzzleFlash = DDLoadBitmap(DirectDraw, "imgMuzzleFlash.bmp", 0, 0);
	DDSetColorKey(imgMuzzleFlash, COLOR_TRANSPARENCY);
	imgPopulation = DDLoadBitmap(DirectDraw, "imgPopulation.bmp", 0, 0);
	DDSetColorKey(imgPopulation, COLOR_TRANSPARENCY);
	imgRadarColors = DDLoadBitmap(DirectDraw, "imgRadarColors.bmp", 0, 0);
	DDSetColorKey(imgRadarColors, 0);
	imgResearch = DDLoadBitmap(DirectDraw, "imgResearch.bmp", 0, 0);
	DDSetColorKey(imgResearch, COLOR_TRANSPARENCY);
	imgResearchComplete = DDLoadBitmap(DirectDraw, "imgResearchComplete.bmp", 0, 0);
	DDSetColorKey(imgResearchComplete, COLOR_TRANSPARENCY);
	imgRocks = DDLoadBitmap(DirectDraw, "imgRocks.bmp", 0, 0);
	DDSetColorKey(imgRocks, COLOR_TRANSPARENCY);
	imgSExplosion = DDLoadBitmap(DirectDraw, "imgSExplosion.bmp", 0, 0);
	DDSetColorKey(imgSExplosion, COLOR_TRANSPARENCY);
	imgSmoke = DDLoadBitmap(DirectDraw, "imgSmoke.bmp", 0, 0);
	DDSetColorKey(imgSmoke, COLOR_TRANSPARENCY);
	imgStar = DDLoadBitmap(DirectDraw, "imgStar.bmp", 0, 0);
	DDSetColorKey(imgStar, COLOR_TRANSPARENCY);
	imgTanks = DDLoadBitmap(DirectDraw, "imgTanks.bmp", 0, 0);
	DDSetColorKey(imgTanks, COLOR_TRANSPARENCY);
	imgTurretBase = DDLoadBitmap(DirectDraw, "imgTurretBase.bmp", 0, 0);
	DDSetColorKey(imgTurretBase, COLOR_TRANSPARENCY);
	imgTurretHead = DDLoadBitmap(DirectDraw, "imgTurretHead.bmp", 0, 0);
	DDSetColorKey(imgTurretHead, COLOR_TRANSPARENCY);
}

void CDDraw::DestroySurfaces()
{
    if(DirectDraw != NULL)
    {
        if(Primary != NULL)
        {
            Primary->Release();
            Primary = NULL;
        }
        if(Back != NULL)
        {
            Back->Release();
            Back = NULL;
        }
		if(imgArrows != NULL)
		{
		  imgArrows->Release();
		  imgArrows = NULL;
		}
		if(imgArrowsRed != NULL)
		{
		  imgArrowsRed->Release();
		  imgArrowsRed = NULL;
		}
		if(imgBCLogo != NULL)
		{
		  imgBCLogo->Release();
		  imgBCLogo = NULL;
		}
		if(imgBlackNumbers != NULL)
		{
		  imgBlackNumbers->Release();
		  imgBlackNumbers = NULL;
		}
		if(imgBtnStaff != NULL)
		{
		  imgBtnStaff->Release();
		  imgBtnStaff = NULL;
		}
		if(imgBuildIcons != NULL)
		{
		  imgBuildIcons->Release();
		  imgBuildIcons = NULL;
		}
		if(imgBuildings != NULL)
		{
		  imgBuildings->Release();
		  imgBuildings = NULL;
		}
		if(imgBullets != NULL)
		{
		  imgBullets->Release();
		  imgBullets = NULL;
		}
		if(imgCompany != NULL)
		{
		  imgCompany->Release();
		  imgCompany = NULL;
		}
		if(imgCursor != NULL)
		{
		  imgCursor->Release();
		  imgCursor = NULL;
		}
		if(imgDemolish != NULL)
		{
		  imgDemolish->Release();
		  imgDemolish = NULL;
		}
		if(imgGround != NULL)
		{
		  imgGround->Release();
		  imgGround = NULL;
		}
		if(imgHealth != NULL)
		{
		  imgHealth->Release();
		  imgHealth = NULL;
		}
		if(imgInterface != NULL)
		{
		  imgInterface->Release();
		  imgInterface = NULL;
		}
		if(imgInterfaceBottom != NULL)
		{
		  imgInterfaceBottom->Release();
		  imgInterfaceBottom = NULL;
		}
		if(imgInventorySelection != NULL)
		{
		  imgInventorySelection->Release();
		  imgInventorySelection = NULL;
		}
		if(imgItems != NULL)
		{
		  imgItems->Release();
		  imgItems = NULL;
		}
		if(imgLava != NULL)
		{
		  imgLava->Release();
		  imgLava = NULL;
		}
		if(imgLExplosion != NULL)
		{
		  imgLExplosion->Release();
		  imgLExplosion = NULL;
		}
		if(imgLoading != NULL)
		{
		  imgLoading->Release();
		  imgLoading = NULL;
		}
		if(imgMiniMapColors != NULL)
		{
		  imgMiniMapColors->Release();
		  imgMiniMapColors = NULL;
		}
		if(imgMoneyBox != NULL)
		{
		  imgMoneyBox->Release();
		  imgMoneyBox = NULL;
		}
		if(imgMoneyDown != NULL)
		{
		  imgMoneyDown->Release();
		  imgMoneyDown = NULL;
		}
		if(imgMoneyUp != NULL)
		{
		  imgMoneyUp->Release();
		  imgMoneyUp = NULL;
		}
		if(imgMuzzleFlash != NULL)
		{
		  imgMuzzleFlash->Release();
		  imgMuzzleFlash = NULL;
		}
		if(imgPopulation != NULL)
		{
		  imgPopulation->Release();
		  imgPopulation = NULL;
		}
		if(imgRadarColors != NULL)
		{
		  imgRadarColors->Release();
		  imgRadarColors = NULL;
		}
		if(imgResearch != NULL)
		{
		  imgResearch->Release();
		  imgResearch = NULL;
		}
		if(imgResearchComplete != NULL)
		{
		  imgResearchComplete->Release();
		  imgResearchComplete = NULL;
		}
		if(imgRocks != NULL)
		{
		  imgRocks->Release();
		  imgRocks = NULL;
		}
		if(imgSExplosion != NULL)
		{
		  imgSExplosion->Release();
		  imgSExplosion = NULL;
		}
		if(imgSmoke != NULL)
		{
		  imgSmoke->Release();
		  imgSmoke = NULL;
		}
		if(imgStar != NULL)
		{
		  imgStar->Release();
		  imgStar = NULL;
		}
		if(imgTanks != NULL)
		{
		  imgTanks->Release();
		  imgTanks = NULL;
		}
		if(imgTurretBase != NULL)
		{
		  imgTurretBase->Release();
		  imgTurretBase = NULL;
		}
		if(imgTurretHead != NULL)
		{
		  imgTurretHead->Release();
		  imgTurretHead = NULL;
		}

    DirectDraw->Release();
    DirectDraw = NULL;
	}
}

void CDDraw::DDrawText(IDirectDrawSurface7* Surface, string Text, int X, int Y, int width, int height, int _flags) {
	X += XOff;
	Y += YOff;
	HDC surfDC;
	Surface->GetDC(&surfDC);
	RECT R1;
	R1.top = X;
	R1.left = Y;
	R1.right = X + width;
	R1.bottom = Y + height;
	DrawText(surfDC, Text.c_str(), (int)Text.length(), &R1, _flags);
	Surface->ReleaseDC(surfDC);
}


void CDDraw::DTextOut(int X, int Y, string Text, COLORREF ColorVal) 
{
    HDC surfDC;
    Back->GetDC(&surfDC);
	SelectObject(surfDC, GetStockObject(SYSTEM_FIXED_FONT));
    SetBkMode(surfDC, TRANSPARENT);
	SetTextColor(surfDC, 0);
    TextOut(surfDC, XOff + X + 1, YOff + Y + 2, Text.c_str(), (int)Text.length());
	SetTextColor(surfDC, ColorVal);
    TextOut(surfDC, XOff + X, YOff + Y, Text.c_str(), (int)Text.length());
    Back->ReleaseDC(surfDC);
}

void CDDraw::DFillRect(int X, int Y, int width, int height) {
	X += XOff;
	Y += YOff;

	DDBLTFX ddbltfX;
	memset(&ddbltfX, 0, sizeof(ddbltfX));
	ddbltfX.dwSize = sizeof(ddbltfX);

	RECT rect;
	rect.left = X;
	rect.top = Y;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	if (rect.right > p->ResolutionX + XOff) rect.right = p->ResolutionX + XOff;
	if (rect.bottom > p->ResolutionY + YOff) rect.bottom = p->ResolutionY + YOff;
	if (rect.left < 0) rect.left = 0;
	if (rect.top < 0) rect.top = 0;

	Back->Blt(&rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfX);
}