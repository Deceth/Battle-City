#ifndef _CLOG
#define _CLOG

#include "CServer.h"

class CServer;

class CLog
{
public:
	CLog(CServer *Server);
	~CLog();

	void logClientError(std::string error, int Index);
	void logServerError(std::string error);
	void logChat(std::string chat);
	void logOrb(int Orber, int EnemyCity, int BuildCount);
	void logAccount(std::string account);
	void logAdmin(std::string admin, int Index);
private:
	CServer *p;
protected:
};

#endif
