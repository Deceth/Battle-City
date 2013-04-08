#ifndef _CInterview
#define _CInterview

#include "CGame.h"

class CGame;

class CInterview
{
public:
	CInterview(CGame *game);
	~CInterview();

	HWND hWnd;
	HWND EditWnd;
	HWND ChatWnd;
	string Chat;

	void AppendData(string TheData);
	void ShowInterviewDlg();
private:
	CGame *p;
protected:

};

#endif