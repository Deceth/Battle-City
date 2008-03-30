#include "CServer.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CSocket::CSocket(CServer *Server) {
	this->p = Server;
	
	FD_ZERO(&master);
    FD_ZERO(&read_fds);

	this->listener = 0;
	this->timev.tv_sec = 0;
	this->timev.tv_usec = 0;

	this->addrlen = sizeof(this->remoteaddr);

#ifdef WIN32
	WSADATA WsaDat;
	if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0) {
		cout << "Winsock startup failed" << endl;
	}
#endif
}

/***************************************************************
 * Destructor
 * 
 **************************************************************/
CSocket::~CSocket() {
#ifdef WIN32
	WSACleanup();
#endif
}

/***************************************************************
 * Function:	InitWinsock
 *
 **************************************************************/
void CSocket::InitWinsock() {
	this->InitTCP();
}

/***************************************************************
 * Function:	InitTCP
 *
 **************************************************************/
void CSocket::InitTCP() {

	// Get a new socket to listen on.  Error on fail.
    if ((this->listener = (int)socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

	// Set socket options.  Error on fail.
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

	// Set the socket properties
    this->myaddr.sin_family = AF_INET;
    this->myaddr.sin_addr.s_addr = INADDR_ANY;
    this->myaddr.sin_port = htons(TCPPORT);
    memset(&(this->myaddr.sin_zero), '\0', 8);

	// Bind to the socket.  Error on fail.
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(1);
    }

	// Listen on the socket.  Error on fail.
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    FD_SET(listener, &master);
}

/***************************************************************
 * Function:	SendData
 *
 * @param i
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSocket::SendData(int i, unsigned char PacketID, char TheData[255], int len) {
	char SendString[256];
	memset(SendString, 0, 256);
	int length = 0;
	int checksum = 0;

	// If the length wasn't passed in, set length to strlen(TheData), then add 2
	if (len == -1) {
		length = (int)(strlen(TheData) + 2);
	}
	// Else (length was passed in), add 2
	else  {
		length = len + 2;
	}

	// Handle checksum for the packet
	// ???
	for (int j = 0; j < (length - 2); j++) {
		checksum += TheData[j];
	}
	checksum += 3412;
	checksum = checksum % 71;

	// Set the length on the first byte
	SendString[0] = (unsigned char)length;
	// Set the checksum on the second byte
	SendString[1] = (unsigned char)checksum;
	// Set the packet type on the third byte
	SendString[2] = (unsigned char)PacketID;
	// Set the data on bytes 3 to length
	memcpy(&SendString[3], TheData, length);

	// Send the data
	this->SendAll(p->Player[i]->Socket, SendString, length + 1);
}

/***************************************************************
 * Function:	SendRaw
 *
 * @param i
 * @param TheDate
 * @param len
 **************************************************************/
void CSocket::SendRaw(int i, char TheData[255], int len) {
	// Send the data
	this->SendAll(p->Player[i]->Socket, TheData, len);
}

/***************************************************************
 * Function:	Cycle
 *
 **************************************************************/
void CSocket::Cycle() {
	this->TCPCycle();
	this->ProcessData();
}

/***************************************************************
 * Function:	TCPCycle
 *
 **************************************************************/
void CSocket::TCPCycle() {
	int freeplayer;
	int nBytes = 0;

    read_fds = master;

	// Select.  Error on fail.
    if (select(0, &read_fds, 0, 0, &timev) == -1)  {
        perror("select");
        exit(1);
    }

	// If we can listen on the packet,	
	if (FD_ISSET(listener, &read_fds))  {
		int newfd = 0;

		// Accept incoming connections.  Error on fail.
	#ifndef WIN32
        if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) { 
	#else
		if ((newfd = (int)accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)  { 
	#endif
            perror("accept");
			cout << "Connection Accepting error";
        }

		// Else (successful new connection),
		else  {

			// Add the user to the next open slot
			freeplayer = p->FreePlayer();
			p->Player[freeplayer]->Socket = newfd;
			p->Player[freeplayer]->IPAddress = inet_ntoa(remoteaddr.sin_addr);
			p->Player[freeplayer]->State = State_Connected;
            u_long nNoBlock = 1;
            ioctlsocket(newfd, FIONBIO, &nNoBlock);
			cout << "Connect::" << p->Player[freeplayer]->IPAddress << " " << "Index: " << freeplayer << endl;
        }
    }

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is connected,
		if (p->Player[i]->isConnected()) {

			// If we have data on the player's socket,
			if (p->Winsock->hasData(p->Player[i]->Socket)) {

				// Receive the data.  If nBytes <= 0,
				if ((nBytes = recv(p->Player[i]->Socket, p->Player[i]->Buffer + p->Player[i]->BufferLength, 2048 - p->Player[i]->BufferLength, 0)) <= 0) {

					// If nByes = 0, close the socket.
					if (nBytes == 0) {
						cout << "Close::" << p->Player[i]->IPAddress << endl;
						p->Player[i]->Clear();
					} 

					// Else (nbytes < 0), log packet error and close the socket.
					else {
						#ifndef WIN32
							perror("Recv");
							cout << "Close::" << p->Player[it->Index]->IPAddress << endl;
							p->Player[i]->Clear();
						#else
							int TheError = WSAGetLastError();
							// WSAEWOULDBLOCK
							if (TheError != 10035)  {
								cerr << "Recv Error: " << TheError << endl;
								cout << "Close::" << p->Player[i]->IPAddress << endl;
								p->Player[i]->Clear();
							}
						#endif
					}
				}

				// Else (nBytes > 0),
				else  {
					// Increase the size of the player's buffer
					p->Player[i]->BufferLength += nBytes;
				}
			}
		}
	}
}

/***************************************************************
 * Function:	ProcessData
 *
 **************************************************************/
void CSocket::ProcessData() {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If there is data in the player's buffer,
		if (this->p->Player[i]->BufferLength > 0) {

			// Process the data
			this->ProcessBuffer(i);
		}
	}
}

/***************************************************************
 * Function:	ProcessBuffer
 *
 * @param i
 **************************************************************/
void CSocket::ProcessBuffer(int i) {
	int packetlength;
	char ValidPacket[256];
	int checksum;
	int checksum2;

	// While there is data left to process,
	while (p->Player[i]->BufferLength > 0) {
		packetlength = (unsigned char)p->Player[i]->Buffer[0] + 1;

		// If there is too much or too little data to process, break
		if ((packetlength > p->Player[i]->BufferLength) || (packetlength < 3)) {
			break;
		}

		// Clear the packet, load the packet
		memset(ValidPacket, 0, 256);
		memcpy(ValidPacket, p->Player[i]->Buffer, packetlength);

		// If the packet comprises the full buffer,
		if (p->Player[i]->BufferLength == packetlength)  {
			
			// Clear the buffer
			memset(p->Player[i]->Buffer, 0, 2048);
			p->Player[i]->BufferLength = 0;
		}

		// Else (more data in buffer),
		else {

			// Clear the packet data from the buffer, leave the rest
			memcpy(p->Player[i]->Buffer, &p->Player[i]->Buffer[packetlength], p->Player[i]->BufferLength - packetlength);
			p->Player[i]->BufferLength -= packetlength;
		}
		
		// Handle the checksum for the packet
		// ???
		checksum = (unsigned char)ValidPacket[1];
		checksum2 = 0;
		for (int j = 0; j < (packetlength - 2); j++) {
			checksum2 += ValidPacket[3 + j];
		}
		checksum2 += 3412;
		checksum2 = checksum2 % 71;
		if (checksum == checksum2) {
			p->Process->ProcessData(&ValidPacket[2], i);
		}
		else {
			p->Log->logClientError("Packet Dropped TCP - Invalid Checksum", i);
		}
	}
}

/***************************************************************
 * Function:	SendAll
 *
 * @param Socket
 * @param SendString
 * @param SendLength
 **************************************************************/
int CSocket::SendAll(int Socket, char *SendString, int SendLength) {
    int TotalSent = 0;
    int BytesLeft = SendLength;
    int SendReturn;

	// While there is more data left to send,
    while(TotalSent < SendLength) {

		// Send the data
        SendReturn = send(Socket, SendString+TotalSent, BytesLeft, 0);

		// If the send failed, break
        if (SendReturn == -1) {
			break;
		}
        TotalSent += SendReturn;
        BytesLeft -= SendReturn;
    }

	// Return -1 if send failed, 0 otherwise
    return (SendReturn==-1?-1:0);
} 

/***************************************************************
 * Function:	hasData
 *
 * @param sock
 **************************************************************/
bool CSocket::hasData(int sock) {
	timeval timev;

	timev.tv_sec = 0;
	timev.tv_usec = 0;
	fd_set dta;
	FD_ZERO(&dta);
	FD_SET(sock,&dta);

	// Load the socket
	select((int)sock+1,&dta,0,0,&timev);

	// If there is data on the socket, return true
	if (FD_ISSET(sock, &dta)) {
		return true;
	}

	// Else (no data), return false
	return false;
}
