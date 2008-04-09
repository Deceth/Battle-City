#ifndef _CDrawing
#define _CDrawing

#include "CGame.h"

class CGame;

class CDrawing {

	public:
		CDrawing(CGame *game);
		~CDrawing();

		void DrawGame();
		void ClearDrawing();

		void DrawMap();
		void DrawPanel();
		void DrawTank();
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

		int defaultPlayerOffsetX;
		int defaultPlayerOffsetY;
		int PlayerOffsetX;
		int PlayerOffsetY;
		int MaxMapX;
		int MaxMapY;
		int groundTilesToDraw;
		int tilesToDraw;
		int buildingCursorOffsetX;
		int buildingCursorOffsetY;
		int demoCursorOffsetX;
		int demoCursorOffsetY;
		int minimapOffsetX;
		int minimapOffsetY;
		int clickOffsetX;
		int clickOffsetY;
		int chatBarWidth;

		float arrowTick;
		float arrowFlashTick;
		int lastArrow;
		bool flashArrow;

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
		void resetPlayerOffset();

		int getOffsetX(int x);
		int getOffsetY(int y);
		bool isPositionInSight(int myX, int myY, int positionX, int positionY);
		bool isTileInSight(int myX, int myY, int locationX, int locationY);
		COLORREF getPlayerCityColor(CPlayer *player, CPlayer *me);
		COLORREF getPlayerNameColor(CPlayer *player, CPlayer *me);
		int getPlayerTankImage(CPlayer *player, CPlayer *me);
		void drawPlayerName(CPlayer *player, CPlayer *me);

	private:
		CGame *p;

	protected:

};

#endif