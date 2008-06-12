#ifndef _CAdmin
#define _CAdmin

#include "CGame.h"
#include "NetMessages.h"

class CGame;

class CAdmin
{
public:
	CAdmin(CGame *game);
	~CAdmin();

	HWND hWnd;

	char IsOpen; 

	void ShowAdminDlg();
	void DrawPlayerList();
	void DrawItemList();

	void AddBan(sSMBan *ban);
	void SetNews(string TheNews);
	void SetStartingCity(int City);
private:
	CGame *p;
protected:

};

#endif