#ifndef _CRecover
#define _CRecover

#include "CGame.h"

class CGame;

class CRecover
{
public:
	CRecover(CGame *game);
	~CRecover();

	HWND hWnd;

	void ShowRecoverDlg();
private:
	CGame *p;
protected:

};

#endif