#ifndef _CPersonality
#define _CPersonality

#include "CGame.h"

class CGame;

class CPersonality
{
public:
	CPersonality(CGame *game);
	~CPersonality();

	void ShowPersonalityDlg();
private:
	CGame *p;
protected:

};

#endif