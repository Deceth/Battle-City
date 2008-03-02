#ifndef _CSend
#define _CSend

#include "CGame.h"

class CGame;

class CSend
{
public:
	CSend(CGame *game);
	~CSend();

	void SendLogin(int NewAccount);
	void SendVersion();
	void SendAccountEdit();
	void SendAccountUpdate();
	void SendWhisper();
	void SendGlobal();
	void SendMessage();

private:
	CGame *p;
protected:

};

#endif