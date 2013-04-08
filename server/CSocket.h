/*
===============================================================================

    Battle City - CSocket header file
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
#ifndef _CSocket
#define _CSocket

#include "CServer.h"

#define TCPPORT 5643
//#define TCPPORT 6643

#ifdef WIN32
#define socklen_t int
#endif

class CServer;

class CSocket {

	public:
		CSocket(CServer *Server);
		~CSocket();

		void InitWinsock();
		void Cycle();
		void SendData(int i, unsigned char PacketID, char TheData[255], int len = -1);
		void SendRaw(int i, char *TheData, int len);
		int SendAll(int Socket, char *SendString, int SendLength);
		void ProcessData();
		void ProcessBuffer(int i);
		bool hasData(int sock);

	private:
		CServer *p;

		void InitTCP();
		void TCPCycle();

		int listener;
		fd_set master;
		fd_set read_fds;
		struct sockaddr_in myaddr; 
		struct sockaddr_in remoteaddr;
		socklen_t addrlen;
		timeval timev;

};

#endif
