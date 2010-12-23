#ifndef _CManage
#define _CManage

#include "CGame.h"

class CManage;

struct Commando
{
	int IsSuccessor;
	int Index;
	int TheCommando;
};

class CManage
{
public:
	CManage(CGame *game);
	~CManage();

	HWND CommandoWnd;
	HWND hWnd;

	void ShowManageDlg();
	void AddCommando(int Commando, int Sucessor);
	int FreeCommando();
	void DisplayCommandos();

	Commando Commando[4];
private:
	CGame *p;
protected:

};

#endif