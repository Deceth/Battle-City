/*
===============================================================================

    Battle City - CWinsock header file
    Copyright (C) 2005-2013  battlecity.org

    This file is part of Battle City.

    Battle City is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battle City is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battle City.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/
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
