#ifndef _CDialog
#define _CDialog

#include "CGame.h"

class CGame;

class CDialog
{
public:
	CDialog(CGame *game);
	~CDialog();

	int StartDialog;
	DLGPROC DialogProcessor;

	void Start();
private:
	CGame *p;
protected:

};

#endif