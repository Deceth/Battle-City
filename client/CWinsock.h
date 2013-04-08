#ifndef _CWINSOCK
#define _CWINSOCK

#include "CGame.h"

class CGame;

typedef int socklen_t;

class CWinsock
{
public:
	CWinsock(CGame *game);
	~CWinsock();

	void StartTCP();
	void StartWinsock();
	void CloseWinsock();

	void Init(char *IP);

	void SendData(unsigned char PacketID, char *TheData, int len = -1);
	int SendAll(char *SendString, int SendLength);

	bool hasData(SOCKET sock);
	void Cycle();
	void RecvTCP();
	void ProcessBuffer();

	char Buffer[2048];
	int BufferLength;

	bool Connected;
	int MyIndex;

	SOCKET Socket;

	sockaddr_in server;
	sockaddr_in addr;

	char *ServerIP;
private:
	CGame *p;
protected:
};

#endif
