#ifndef _CAdminEdit
#define _CAdminEdit

#include "CGame.h"
#include "NetMessages.h"

class CGame;

class CAdminEdit
{
public:
	CAdminEdit(CGame *game);
	~CAdminEdit();

	HWND hWnd;

	char IsOpen; 

	void ShowAdminEditDlg();

	char user[15];
	char pass[15];
	char email[50];
	char fullname[20];
	char town[15];
	char state[15];

	int points;
	int monthlyTop20;
	int orbs;
	int deaths;
	int assists;
	int playerType;
	char member;
	char tank;
	char tank2;
	char tank3;
	char tank4;
	char tank5;
	char tank6;
	char tank7;
	char tank8;
	char tank9;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	int rentalcity;

private:
	CGame *p;
protected:

};

#endif