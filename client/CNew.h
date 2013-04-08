#ifndef _CNew
#define _CNew

#include "CGame.h"

class CGame;

class CNew
{
public:
	CNew(CGame *game);
	~CNew();

	HWND hWnd;
	
	char user[15];
	char pass[15];
	char email[50];
	char fullname[20];
	char town[15];
	char state[15];

	char Editing;

	void ShowNewDlg();
	void ShowEditDlg();
	void Refill();
private:
	CGame *p;
protected:

};

#endif