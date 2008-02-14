#ifndef _COptions
#define _COptions

#include "CGame.h"

class CGame;

class COptions
{
public:
	COptions(CGame *game);
	~COptions();

	HWND hWnd;

	void ShowOptionsDialog();

	char music;
	char sound;
	char tanksound;
	char fullscreen;
	char newbietips;
	char sleep;
	char debug;
	char names;
	char limitfps;

	void LoadOptions();
	void SaveOptions();
private:
	CGame *p;
protected:

};

#endif