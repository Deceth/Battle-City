/*
===============================================================================

    Battle City - CWinsock
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
#include "CWinsock.h"

CWinsock::CWinsock(CGame *game)  {
	p = game;
	Connected = false;
	MyIndex = 0;
	closesocket(Socket);

	BufferLength = 0;
	memset(Buffer, 0, 2048);
}

CWinsock::~CWinsock() {
	CloseWinsock();
}

void CWinsock::Init(char *IP) {
	ServerIP = IP;
}
/// <summary>   Establishes a TCP connection with the game server. </summary>
void CWinsock::StartTCP() {
    //  Close existing connections
	this->CloseWinsock();
    //  Start connection
	this->StartWinsock();
    //  Establish host entry structure
	struct hostent *host_entry;
    //  Populate host_entry with ip address
    host_entry = gethostbyname(ServerIP);
	if(host_entry==NULL)
    {
        MessageBox(p->hWnd,ServerIP,0,0);
    }
    //  Establish socket address object
    //  http://msdn.microsoft.com/en-us/library/aa917469.aspx
	sockaddr_in sockAddr;
    //  Populate sin_family with AF_INET
	sockAddr.sin_family = AF_INET;
    //  Converts a u_short from host to TCP/IP network byte order and assigns to sin_port
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms738557(v=vs.85).aspx
	sockAddr.sin_port = htons(TCPPORT);
    //  Sets IP address to sin_addr in network byte order
	sockAddr.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;
    //  Initialize socket 
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //  Non-blocking socket
    ioctlsocket(Socket,FIONBIO,(u_long *) 1);

    //  Upon verifying Socket connection failed display message box with an error message
	if (Socket == -1) {
		MessageBox(p->hWnd, "Invalid Socket!", 0, 0);
	} 

    //  Upon verifying a connection error close the connection and process the event
    //  ;Otherwise, set Connected as true and process the event
    //  1 - Error connecting    
    //  2 - Connected
	if (connect(Socket, (sockaddr*)(&sockAddr), sizeof(sockaddr_in)) != 0) {
		this->CloseWinsock();
		p->Process->ProcessEvent(1);
	} else {
		this->Connected = true;
		p->Process->ProcessEvent(2);
	}
}

void CWinsock::StartWinsock() {
	WSADATA WsaDat;
	WSAStartup(MAKEWORD(2, 2), &WsaDat);
}

void CWinsock::CloseWinsock() {
	closesocket(Socket);
	Connected = false;
	MyIndex = 0;
	Sleep(5);
	WSACleanup();
}

void CWinsock::SendData(unsigned char PacketID, char *TheData, int len) {
	char SendString[256];
	memset(SendString, 0, 256);
	int length = 0;

	if (len == -1)
		length = (int)(strlen(TheData) + 2);
	else 
		length = len + 2;

	int checksum = 0;
	for (int j = 0; j < (length - 2); j++) {
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

void CWinsock::Cycle() {
	if (hasData(Socket)) {
		RecvTCP();	
	}

	ProcessBuffer();
}

bool CWinsock::hasData(SOCKET sock) {
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

void CWinsock::RecvTCP() {
	int nbytes = 0;
	if ((nbytes = recv(Socket, this->Buffer + this->BufferLength, 2048 - this->BufferLength, 0)) <= 0) {
		p->Process->ProcessEvent(3); //Disconnected
	}
	else {
		this->BufferLength += nbytes;
	}
}

void CWinsock::ProcessBuffer() {
	while (this->BufferLength > 0) {
		int packetlength = (unsigned char)this->Buffer[0] + 1;

		if (packetlength > this->BufferLength || packetlength < 3) break;

		char ValidPacket[256];
		memset(ValidPacket, 0, 256);

		memcpy(ValidPacket, this->Buffer, packetlength);
		if (this->BufferLength == packetlength)  {
			memset(this->Buffer, 0, 2048);
			this->BufferLength = 0;
		}
		else {
			memcpy(this->Buffer, &this->Buffer[packetlength], this->BufferLength - packetlength);
			this->BufferLength -= packetlength;
		}
		
		int checksum = (unsigned char)ValidPacket[1];
		int checksum2 = 0;
		for (int j = 0; j < (packetlength - 2); j++) {
			checksum2 += ValidPacket[3 + j];
		}
		checksum2 += 3412;
		checksum2 = checksum2 % 71;
		if (checksum == checksum2) {
			p->Process->ProcessData(&ValidPacket[2]);
		}
		else {
			p->Engine->logerror("Packet Dropped TCP - Invalid Checksum");
		}
	}
}

int CWinsock::SendAll(char *SendString, int SendLength) {
    int TotalSent = 0;
    int bytesLeft = SendLength;
    int SendReturn;

    while(TotalSent < SendLength)  {
        SendReturn = send(Socket, SendString+TotalSent, bytesLeft, 0);
        if (SendReturn == -1) {
			break;
		}
        TotalSent += SendReturn;
        bytesLeft -= SendReturn;
    }

    return SendReturn==-1?-1:0;
} 