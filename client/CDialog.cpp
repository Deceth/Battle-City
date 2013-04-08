#include "CDialog.h"

void _cdecl thrDialog(void * pParam);

CDialog::CDialog(CGame *game)
{
	p = game;
	this->StartDialog = 0;
	this->DialogProcessor = 0;
}

CDialog::~CDialog()
{

}

void CDialog::Start()
{
	_beginthread(thrDialog,0,p);
}

void _cdecl thrDialog(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	while (1)
	{
		if (p->Dialog->StartDialog > 0)
		{
			DialogBox(p->hInst, MAKEINTRESOURCE(p->Dialog->StartDialog), p->hWnd, p->Dialog->DialogProcessor);
		}
		Sleep(5);
	}

	_endthread();
}