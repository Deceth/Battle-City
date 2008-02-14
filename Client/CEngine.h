#ifndef _CEngine
#define _CEngine

#include "CGame.h"

class CGame;

class CEngine
{
public:
	CEngine(CGame *game);
	~CEngine();

	void Init();

	void logerror(std::string error);

	int loops;
	int FPS;
	float FPSTick;
	char MsgUp;
	char MsgQueue;

	void ThreadMessage(std::string MsgText);
	void ThreadMessageQuit();
	std::string MsgText;
private:
	CGame *p;
protected:

};

#endif