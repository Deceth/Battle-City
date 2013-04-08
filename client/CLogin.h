#ifndef _CLogin
#define _CLogin

#include "CGame.h"

class CGame;

class CLogin
{
public:
	CLogin(CGame *game);
	~CLogin();

	string VersionString;

	void ShowLoginDlg();
	char user[15];
	char pass[15];
private:
	CGame *p;
protected:

};

#endif