#ifndef _CHelp
#define _CHelp

#include "CGame.h"

class CGame;

class CHelp
{
public:
	CHelp(CGame *game);
	~CHelp();

	HWND hWnd;

	void ShowHelpDialog();

private:
	CGame *p;
protected:

};

#endif