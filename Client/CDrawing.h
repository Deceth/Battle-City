#ifndef _CDrawing
#define _CDrawing

#include "CGame.h"

class CGame;

class CDrawing
{
public:
	CDrawing(CGame *game);
	~CDrawing();

	void DrawGame();
	void ClearDrawing();

	void DrawMap();
	void DrawPanel();

public:
	void DrawTank();
public:
	void DrawBuildings();
	void DrawPlayers();
	void DrawBuildMenu();
	void DrawBuildPlacement();
	void DrawArrow();
	void DrawHealth();
	void DrawItems();
	void DrawChat();
	void DrawInventory();
	void DrawBullets();
	void DrawRadar();
	void DrawExplosions();
	void DrawMiniMap();

	string CashFormat(long Number);

	int PlayerOffsetX;
	int PlayerOffsetY;
	int MaxMapX;
	int MaxMapY;

	float arrowTick;
	int lastArrow;

	int BuildMenuX;
	int BuildMenuY;

	string PanelLine1;
	string PanelLine2;
	string PanelLine3;
	string PanelLine4;
	string PanelLine5;
	string PanelLine6;
	string PanelLine7;
	int PanelMode;
	void ClearPanel();
private:
	CGame *p;
protected:

};

#endif