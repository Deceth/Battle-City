#ifndef _CPersonnel
#define _CPersonnel

#include "CGame.h"

class CGame;

class CPersonnel
{
public:
	CPersonnel(CGame *game);
	~CPersonnel();

	HWND hWnd;
	HWND EditWnd;
	HWND ChatWnd;
	string Chat;

	void AppendData(string TheData);
	void ShowPersonnelDialog();

	int DenyApplicants;
	int ApplicantWaiting;
	int Successor;

	float ApplicantTick;
	int ApplicantAnim;
private:
	CGame *p;
protected:

};

#endif