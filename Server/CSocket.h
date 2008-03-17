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

	protected:

};

#endif
