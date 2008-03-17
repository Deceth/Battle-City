#include "CWinsock.h"

CWinsock::CWinsock(CGame *game) 
{
	p = game;
	Connected = false;
	MyIndex = 0;
	closesocket(Socket);

	BufferLength = 0;
	memset(Buffer, 0, 2048);
}

CWinsock::~CWinsock()
{
#ifndef _DEBUG
	try {
#endif

	CloseWinsock();

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "CloseWinsock"); p->Engine->logerror("CloseWinsock");}
#endif
}

void CWinsock::Init(char *IP)
{
#ifndef _DEBUG
	try {
#endif

	ServerIP = IP;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Winsock::Init"); p->Engine->logerror("Winsock::Init");}
#endif
}

void CWinsock::StartTCP()
{
#ifndef _DEBUG
	try {
#endif

	this->CloseWinsock();
	this->StartWinsock();

	struct hostent *host_entry;
	host_entry = gethostbyname(ServerIP);
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;

	sockAddr.sin_port = htons(TCPPORT);

	sockAddr.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (Socket == -1) 
	{
		MessageBox(p->hWnd, "Invalid Socket!", 0, 0);
	} 

	if (connect(Socket, (sockaddr*)(&sockAddr), sizeof(sockaddr_in)) != 0)
	{
		this->CloseWinsock();
		p->Process->ProcessEvent(1); //Error connecting
	}
	else
	{
		this->Connected = true;
		p->Process->ProcessEvent(2); //Connected
	}

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "StartTCP"); p->Engine->logerror("StartTCP");}
#endif
}

void CWinsock::StartWinsock()
{
#ifndef _DEBUG
	try {
#endif

		WSADATA WsaDat;
		WSAStartup(MAKEWORD(2, 2), &WsaDat);

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "StartWinsock"); p->Engine->logerror("StartWinsock");}
#endif
}

void CWinsock::CloseWinsock()
{
#ifndef _DEBUG
	try {
#endif

	closesocket(Socket);
	Connected = false;
	MyIndex = 0;
	Sleep(5);
	WSACleanup();

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "CloseWinsock"); p->Engine->logerror("CloseWinsock");}
#endif
}

void CWinsock::SendData(unsigned char PacketID, char *TheData, int len)
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
	SendAll(SendString, length + 1);
}

void CWinsock::Cycle()
{
#ifndef _DEBUG
	try {
#endif

	if (hasData(Socket))
	{
		RecvTCP();	
	}

	ProcessBuffer();

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "Winsock::Cycle"); p->Engine->logerror("Winsock::Cycle");}
#endif
}

bool CWinsock::hasData(SOCKET sock)
{
#ifndef _DEBUG
	try {
#endif

	timeval timev;
	timev.tv_sec = 0;
	timev.tv_usec = 0;
	fd_set dta;
	FD_ZERO(&dta);
	FD_SET(sock,&dta);
	select((int)sock+1,&dta,0,0,&timev);
	if (FD_ISSET(sock, &dta)) return true;
	return false;

#ifndef _DEBUG
	}
	catch (...) {p->Winsock->SendData(cmCrash, "hasData"); p->Engine->logerror("hasData");}
#endif
}

void CWinsock::RecvTCP()
{
	int nbytes = 0;
	if ((nbytes = recv(Socket, this->Buffer + this->BufferLength, 2048 - this->BufferLength, 0)) <= 0)
	{
		p->Process->ProcessEvent(3); //Disconnected
	}
	else
	{
		this->BufferLength += nbytes;
	}
}

void CWinsock::ProcessBuffer()
{
	while (this->BufferLength > 0)
	{
		int packetlength = (unsigned char)this->Buffer[0] + 1;

		if (packetlength > this->BufferLength || packetlength < 3) break;

		char ValidPacket[256];
		memset(ValidPacket, 0, 256);

		memcpy(ValidPacket, this->Buffer, packetlength);
		if (this->BufferLength == packetlength) 
		{
			memset(this->Buffer, 0, 2048);
			this->BufferLength = 0;
		}
		else
		{
			memcpy(this->Buffer, &this->Buffer[packetlength], this->BufferLength - packetlength);
			this->BufferLength -= packetlength;
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
			p->Process->ProcessData(&ValidPacket[2]);
		else
			p->Engine->logerror("Packet Dropped TCP - Invalid Checksum");
	}
}

int CWinsock::SendAll(char *SendString, int SendLength)
{
    int TotalSent = 0;
    int bytesLeft = SendLength;
    int SendReturn;

    while(TotalSent < SendLength) 
	{
        SendReturn = send(Socket, SendString+TotalSent, bytesLeft, 0);
        if (SendReturn == -1) { break; }
        TotalSent += SendReturn;
        bytesLeft -= SendReturn;
    }

    return SendReturn==-1?-1:0;
} 