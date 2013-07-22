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
//  Methods from CServer inherited
#include "CServer.h"
//  Default listening port for server application
#define TCPPORT 5643
#ifdef WIN32
//  WS2tcpip.h requires socklen_t as integer
#define socklen_t int
#endif
//  Inherit methods from CServer
class CServer;

/// <summary>
/// Network abstraction
/// </summary>
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
        /// <summary>
        /// Set up deferenced pointer
        /// </summary>
        CServer *p;
        /// <summary>
        /// Set up listener socket
        /// </summary>
		void InitTCP();
        /// <summary>
        /// Handle incoming connections and receive data from existing connections
        /// </summary>
        void TCPCycle();
        //  Listener socket
		int listener;
        //  Master File Descriptor Set
		fd_set master;
        //  Read-only File Descriptor Set
		fd_set read_fds;
        //  Socket definition for local machine
		struct sockaddr_in myaddr; 
        //  Socket definition for remote machine
		struct sockaddr_in remoteaddr;
        //  Address length
		socklen_t addrlen;
        //  Socket time interval
		timeval timev;
};
#endif