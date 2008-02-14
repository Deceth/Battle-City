#include "CServer.h"

CSocket::CSocket(CServer *Server)
{
	p = Server;
	
	FD_ZERO(&master);
    FD_ZERO(&read_fds);

	listener = 0;
	timev.tv_sec = 0;
	timev.tv_usec = 0;

	addrlen = sizeof(remoteaddr);

#ifdef WIN32
	WSADATA WsaDat;
	if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0)
	{
		cout << "Winsock startup failed" << endl;
	}
#endif
}

CSocket::~CSocket()
{
#ifdef WIN32
	WSACleanup();
#endif
}

void CSocket::InitWinsock()
{
	InitTCP();
}

void CSocket::InitTCP()
{
    if ((listener = (int)socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, "1",
                                                        sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(TCPPORT);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    FD_SET(listener, &master);
}

void CSocket::SendData(int i, unsigned char PacketID, char TheData[255], int len)
{
	char SendString[256];
	memset(SendString, 0, 256);
	int length = 0;

	if (len == -1)
		length = (int)(strlen(TheData) + 2);
	else 
		length = len + 2;

	int checksum = 0;
	for (int j = 0; j < (length - 2); j++)
	{
		checksum += TheData[j];
	}
	checksum += 3412;
	checksum = checksum % 71;

	SendString[1] = (unsigned char)checksum;
	SendString[2] = (unsigned char)PacketID;
	memcpy(&SendString[3], TheData, length);

	SendString[0] = (unsigned char)length;
	SendAll(p->Player[i]->Socket, SendString, length + 1);
}

void CSocket::SendRaw(int i, char TheData[255], int len)
{
	SendAll(p->Player[i]->Socket, TheData, len);
}

void CSocket::Cycle()
{
	TCPCycle();
	ProcessData();
}

void CSocket::TCPCycle()
{
    read_fds = master;
    if (select(0, &read_fds, 0, 0, &timev) == -1) 
	{
        perror("select");
        exit(1);
    }

	if (FD_ISSET(listener, &read_fds)) 
	{
		int newfd = 0;
	#ifndef WIN32
        if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) 
		{ 
	#else
		if ((newfd = (int)accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) 
		{ 
	#endif
            perror("accept");
			cout << "Connection Accepting error";
        } 
		else 
		{
			int freeplayer = p->FreePlayer();
			p->Player[freeplayer]->Socket = newfd;
			p->Player[freeplayer]->IPAddress = inet_ntoa(remoteaddr.sin_addr);
			p->Player[freeplayer]->State = State_Connected;
            u_long nNoBlock = 1;
            ioctlsocket(newfd, FIONBIO, &nNoBlock);
			cout << "Connect::" << p->Player[freeplayer]->IPAddress << " " << "Index: " << freeplayer << endl;
        }
    }

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State > State_Disconnected)
		{
			if (p->Player[i]->isDead == true && ((p->Tick - p->Player[i]->timeDeath) > 10000))
			{
				p->Player[i]->lastMove = p->Tick;
				sSMStateGame game;
				game.City = p->Player[i]->City;
				p->Player[i]->X = (float)p->City[p->Player[i]->City]->x;
				p->Player[i]->Y = (float)p->City[p->Player[i]->City]->y;
				game.x = (unsigned short)p->City[p->Player[i]->City]->x;
				game.y = (unsigned short)p->City[p->Player[i]->City]->y;
				p->Winsock->SendData(i, smWarp,(char *)&game,sizeof(game));
				p->Player[i]->isDead = false;
				char packet[2];
				packet[0] = i;
				packet[1] = 0;
				p->Send->SendAllBut(-1, smRespawn, packet, 1);
			}
			if (p->Winsock->hasData(p->Player[i]->Socket))
			{
				int nBytes = 0;
				if ((nBytes = recv(p->Player[i]->Socket, p->Player[i]->Buffer + p->Player[i]->BufferLength, 2048 - p->Player[i]->BufferLength, 0)) <= 0) 
				{
					if (nBytes == 0) 
					{
						cout << "Close::" << p->Player[i]->IPAddress << endl;
						p->Player[i]->Clear();
					} 
					else
					{
						#ifndef WIN32
							perror("Recv");
							cout << "Close::" << p->Player[it->Index]->IPAddress << endl;
							p->Player[i]->Clear();
						#else
							int TheError = WSAGetLastError();
							if (TheError != 10035) //WSAEWOULDBLOCK
							{
								cerr << "Recv Error: " << TheError << endl;
								cout << "Close::" << p->Player[i]->IPAddress << endl;
								p->Player[i]->Clear();
							}
						#endif
					}
				} 
				else 
				{
					p->Player[i]->BufferLength += nBytes;
				}
			}
		}
	}
}

void CSocket::ProcessData()
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->BufferLength > 0)
		{
			ProcessBuffer(i);
		}
	}
}

void CSocket::ProcessBuffer(int i)
{
	while (p->Player[i]->BufferLength > 0)
	{
		int packetlength = (unsigned char)p->Player[i]->Buffer[0] + 1;

		if (packetlength > p->Player[i]->BufferLength || packetlength < 3) break;

		char ValidPacket[256];
		memset(ValidPacket, 0, 256);

		memcpy(ValidPacket, p->Player[i]->Buffer, packetlength);
		if (p->Player[i]->BufferLength == packetlength) 
		{
			memset(p->Player[i]->Buffer, 0, 2048);
			p->Player[i]->BufferLength = 0;
		}
		else
		{
			memcpy(p->Player[i]->Buffer, &p->Player[i]->Buffer[packetlength], p->Player[i]->BufferLength - packetlength);
			p->Player[i]->BufferLength -= packetlength;
		}
		
		int checksum = (unsigned char)ValidPacket[1];
		int checksum2 = 0;
		for (int j = 0; j < (packetlength - 2); j++)
		{
			checksum2 += ValidPacket[3 + j];
		}
		checksum2 += 3412;
		checksum2 = checksum2 % 71;
		if (checksum == checksum2)
			p->Process->ProcessData(&ValidPacket[2], i);
		else
			p->Log->logClientError("Packet Dropped TCP - Invalid Checksum", i);
	}
}

int CSocket::SendAll(int Socket, char *SendString, int SendLength)
{
    int TotalSent = 0;
    int BytesLeft = SendLength;
    int SendReturn;

    while(TotalSent < SendLength) 
	{
        SendReturn = send(Socket, SendString+TotalSent, BytesLeft, 0);
        if (SendReturn == -1) { break; }
        TotalSent += SendReturn;
        BytesLeft -= SendReturn;
    }

    return SendReturn==-1?-1:0;
} 

bool CSocket::hasData(int sock)
{
	timeval timev;
	timev.tv_sec = 0;
	timev.tv_usec = 0;
	fd_set dta;
	FD_ZERO(&dta);
	FD_SET(sock,&dta);
	select((int)sock+1,&dta,0,0,&timev);
	if (FD_ISSET(sock, &dta)) return true;
	return false;
}