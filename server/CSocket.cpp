/*
===============================================================================

    Battle City - CSocket file
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
#include "CServer.h"

/// <summary>
/// Initializes a new instance of the <see cref="CSocket" /> class.
/// </summary>
/// <param name="Server">The server.</param>
CSocket::CSocket(CServer *Server) {
    //  Dereferenced pointer associated to this->p
    this->p = Server;
    //  File descriptor set to zero for master
    FD_ZERO(&master);
    //  File descripter set to zero for read_dfs
    FD_ZERO(&read_fds);
    //  Listener set to 0
    this->listener = 0;
    //  Time intervals
    this->timev.tv_sec = 0;
    this->timev.tv_usec = 0;
    //  Address length for remote address
    this->addrlen = sizeof(this->remoteaddr);
#ifdef WIN32
    // Method must be tested in order to initialize
    WSADATA WsaDat;
    if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0) {
        cout << "Winsock startup failed" << endl;
    } else {
        cout << "Winsock initialization finished" << endl;
    }
#endif
}


/// <summary>
/// Finalizes an instance of the <see cref="CSocket" /> class by
/// terminating Winsock 2 DLL
/// </summary>
CSocket::~CSocket() {
#ifdef WIN32
    WSACleanup();
#endif
}

/// <summary>
/// Initializes the Winsock 2 DLL
/// </summary>
void CSocket::InitWinsock() {
    this->InitTCP();
}

/// <summary>
/// Set up listener socket
/// </summary>
void CSocket::InitTCP() {
    //  Set listener to obtained socket; otherwise, print error message
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740506(v=vs.85).aspx
    if ((this->listener = (int)socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Unable to obtain socket.");
        exit(1);
    }
    //  Set options on listener; otherwise, print error message
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740476(v=vs.85).aspx
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int)) == -1) {
        perror("Unable to set socket options on listener.");
        exit(1);
    }
    //  Set "myaddr" sockaddr_in properties
    this->myaddr.sin_family = AF_INET;
    this->myaddr.sin_addr.s_addr = INADDR_ANY;
    //  TCPPORT defined in CSocket.h
    this->myaddr.sin_port = htons(TCPPORT);
    //  Padding to match size of SOCKADDR structure
    //  http://msdn.microsoft.com/en-us/library/1fdeehz6(v=vs.100).aspx
    memset(&(this->myaddr.sin_zero), '\0', sizeof(this->myaddr.sin_zero));
    //  Bind listener to "myaddr"; otherwise, print error message
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms737550(v=vs.85).aspx
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("Unable to bind listener with system.");
        exit(1);
    }
    //  Change listener to the LISTEN state; otherwise, print error message
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms739168(v=vs.85).aspx
    if (listen(listener, 10) == -1) {
        perror("Unable to change listener to LISTEN state.");
        exit(1);
    }
    //  Set listener to &master file descriptor set
    FD_SET(listener, &master);
}

/// <summary>
/// Generates data packet and sends to player UID
/// </summary>
/// <param name="playerId">Player UID</param>
/// <param name="packetId">Packet UID</param>
/// <param name="payload">Data payload</param>
/// <param name="payloadLength">Data payload length</param>
void CSocket::SendData(int playerId, unsigned char packetId, char payload[255], int payloadLength) {
    //  Set empty variable with 256 length
    char SendString[256];
    //  Populate SendString with zeros
    memset(SendString, 0, 256);
    //  Set temporary variables
    int length = 0;
    int checksum = 0;
    //  Verify that "payloadLength" is defined; if not, calculate
    //  "payload" string length then add two
    //  if set; add two to "payloadLength" value
    //  Length extended by two in order to compensate for the header data
    length = (payloadLength == -1) ? (int)(strlen(payload)+2) : (payloadLength +2);
    //  Generate checksum for "payload" by looping through each character in "payload"
    //  and adding each ASCII character value to checksum then adding 3412 finally
    //  dividing the value by 71;
    for (int j = 0; j < (length - 2); j++) {
        checksum += payload[j];
    }
    checksum += 3412;
    checksum = checksum % 71;
    //  Constructing SendString by setting the first byte to length
    //  then setting second byte to checksum finally setting third
    //  byte with PacketID.
    SendString[0] = (unsigned char)length;
    SendString[1] = (unsigned char)checksum;
    SendString[2] = (unsigned char)packetId;
    //  Starting at the third byte on SendString pointer populate
    //  with "payload" for the entire lenth.
    memcpy(&SendString[3], payload, length);
    //  Finally submit generated data to player UID
    this->SendAll(p->Player[playerId]->Socket, SendString, length + 1);
}

/// <summary>
/// Sends a raw packet to player UID
/// </summary>
/// <param name="playerId">Player UID</param>
/// <param name="payload">Data payload</param>
/// <param name="payloadLength">Data payload length</param>
void CSocket::SendRaw(int playerId, char payload[255], int payloadLength) {
    this->SendAll(p->Player[playerId]->Socket, payload, payloadLength);
}

/// <summary>
/// Perform network cycle
/// </summary>
void CSocket::Cycle() {
    //  Handle incoming connections and receive data from existing connections
    this->TCPCycle();
    this->ProcessData();
}

/// <summary>
/// Handle incoming connections and receive data from existing connections
/// </summary>
void CSocket::TCPCycle() {
    //
    //  Incoming connections
    //

    //  Set temporary variables
    int freeplayer;
    int nBytes = 0;
    //  Create dereferenced pointer to CPlayer object
    CPlayer* player;
    //  Copy master file descriptor set (listener) to read file descriptor set   
    read_fds = master;
    //  Determines status for available file descriptor sets; otherwise, print error message
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740141(v=vs.85).aspx
    if (select(0, &read_fds, 0, 0, &timev) == -1)  {
        perror("Unable to monitor file descriptor set");
        exit(1);
    }
    //  Confirms that listener is a member of the file descriptor set
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740141(v=vs.85).aspx
    if (FD_ISSET(listener, &read_fds))  {
        //  Set temporary variable to indicate new file descriptor
        int newfd = 0;
        //  Generate socket for incoming connection and populate "remoteaddr"
        //  and "addrlen" with incoming connection values; Test return value for failure
        //  then print error message
        //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms737526(v=vs.85).aspx
#ifndef WIN32
        if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) { 
#else
        if ((newfd = (int)accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)  { 
#endif
            perror("Unable to accept incoming connection.");
            cout << "CSocket @ TCPCycle was unable to accept incoming connection.";
        } else  {
            //  Allocate incoming connection to a player UID
            freeplayer = this->p->FreePlayer();
            //  Generate a CPlayer object for incoming connection
            player = this->p->Player[freeplayer];
            //  Associate generated socket to CPlayer object
            player->Socket = newfd;
            //  Cleanse incoming connection's ip address then save to CPlayer object
            player->IPAddress = inet_ntoa(remoteaddr.sin_addr);
            //  Set CPlayer to enum:State_Connected
            player->State = State_Connected;
            //  Set blocking mode to non-blocking
            //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms738573(v=vs.85).aspx
            u_long nNoBlock = 1;
            ioctlsocket(newfd, FIONBIO, &nNoBlock);
            cout << "CSocket @ TCPCycle [ Player connected @ " << player->IPAddress << " " << " Allocated Index: " << freeplayer << " ]" << endl;
        }
    }

    //
    //  Receive data from existing connections
    //
    
    //  Loop through each player slot
    for (int i = 0; i < MAX_PLAYERS; i++) {
        //  Initiate CPlayer object
        player = this->p->Player[i];
        //  Verify player is connected and player's socket has data to be received
        if (player->isConnected() && this->p->Winsock->hasData(player->Socket)) {
            //  Receives data from the player's socket
            //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740121(v=vs.85).aspx
            if ((nBytes = recv(player->Socket, player->Buffer + player->BufferLength, 2048 - player->BufferLength, 0)) <= 0) {
                //  Print error message and close connection upon detecting that received bytes is less than zero 
                if (nBytes != 0) {
#ifndef WIN32
                        perror("Unable to receive data due to bytes received is less than 0");
#else
                        int TheError = WSAGetLastError();
                        //  Error message codes available at
                        //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms741580(v=vs.85).aspx
                        if (TheError != 10035)  {
                            cerr << "Unable to receive data due to bytes received is less than 0: " << TheError << endl;
                        }
#endif
                }
                //  Handle closing player connection
                player->Clear();
            } else {
                //  Player bufferLength expanded to include incoming byte size
                player->BufferLength += nBytes;
            }
        }
    }
}

/// <summary>
/// Handle processing data for all players
/// </summary>
void CSocket::ProcessData() {
    //  Loop through each player slot
    for (int i = 0; i < MAX_PLAYERS; i++) {
        //  Verify BufferLength is greater than zero bytes
        if (this->p->Player[i]->BufferLength > 0) {
            //  Pass player UID to process buffer method
            this->ProcessBuffer(i);
        }
    }
}

/// <summary>
/// Process playerId's buffer
/// </summary>
/// <param name="playerId">Player UID</param>
void CSocket::ProcessBuffer(int playerId) {
    //  Set temporary variables
    int packetlength;
    char ValidPacket[256];
    int checksum;
    int checksum2;
    //  Initiate CPlayer object
    CPlayer* player = this->p->Player[playerId];
    //  Loop through BufferLength
    while (player->BufferLength > 0) {
        //  Calculate length of buffer plus 1
        packetlength = (unsigned char)player->Buffer[0] + 1;
        //  Break loop upon verifying that packetlength is greater
        //  than BufferLength or less than 3
        if ((packetlength > player->BufferLength) || (packetlength < 3)) {
            break;
        }
        //  Populate ValidPacket with zeros
        //  http://msdn.microsoft.com/en-us/library/1fdeehz6(v=vs.80).aspx
        memset(ValidPacket, 0, 256);
        //  Load player buffer to packet
        //  http://msdn.microsoft.com/en-us/library/wes2t00f(v=vs.80).aspx
        memcpy_s(ValidPacket,256,player->Buffer,packetlength);
        //  Clear buffer upon verifying that BufferLength equals packetlength
        if (player->BufferLength == packetlength)  {
            //  Clear buffer by populating with zeros
            memset(player->Buffer, 0, 2048);
            //  Define BufferLength as zero
            player->BufferLength = 0;
        } else {
            //  Buffer has more data to be processed; therefore,
            //  remove "processed" data by copying "unprocessed" data
            //  to the beginning of the variable
            memcpy_s(player->Buffer, 2048, &player->Buffer[packetlength], player->BufferLength - packetlength);
            player->BufferLength -= packetlength;
        }
        //
        //  Confirm sent checksum equals checksum for received data
        //

        //  Retrieve "checksum" from ValidPacket[1]
        checksum = (unsigned char)ValidPacket[1];
        //  Generate "checksum2" by defining variable as zero
        checksum2 = 0;
        //  Loop through "ValidPacket" excluding the header bytes then
        //  add each byte to "checksum2"
        for (int j = 0; j < (packetlength - 2); j++) {
            checksum2 += ValidPacket[3 + j];
        }
        //  Add 3412 to "checksum2"
        checksum2 += 3412;
        //  Divide "checksum2" by 71
        checksum2 = checksum2 % 71;
        //  Verify both checksums to determine validity; Matching then process valid packet
        //  Otherwise, drop packet and log client error
        if (checksum == checksum2) {
            this->p->Process->ProcessData(&ValidPacket[2], playerId);
        } else {
            this->p->Log->logClientError("Packet Dropped TCP - Invalid Checksum", playerId);
        }
    }
}

/// <summary>
/// Sends all.
/// </summary>
/// <param name="Socket">The socket.</param>
/// <param name="SendString">The send string.</param>
/// <param name="SendLength">Length of the send.</param>
/// <returns></returns>
int CSocket::SendAll(int Socket, char *SendString, int SendLength) {
    //  Stores the total bytes sent
    int TotalSent = 0;
    //  Stores the bytes remaining to be sent
    int BytesLeft = SendLength;
    //  Stores the amount sent
    int SendReturn;
    //  Loop while sent amount is less than send amount 
    while(TotalSent < SendLength) {
        //  Sends data to the Socket argument
        //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740149(v=vs.85).aspx
        SendReturn = send(Socket, SendString+TotalSent, BytesLeft, 0);
        //  Break upon verifying send failed
        if (SendReturn == -1) {
            break;
        }
        //  Increment total bytes sent by total number of bytes sent in this iteration
        TotalSent += SendReturn;
        //  Decrement bytes remaining by total number of bytes sent in this iteration
        BytesLeft -= SendReturn;
    }
    //  Return success or failure
    return (SendReturn==-1?-1:0);
} 


/// <summary>
/// Determines whether the specified sock has data.
/// </summary>
/// <param name="sock">The sock.</param>
/// <returns></returns>
bool CSocket::hasData(int sock) {
    //  Stores the time interval
    //  http://msdn.microsoft.com/en-us/library/windows/desktop/ms740560(v=vs.85).aspx
    timeval timev;
    //  Specify time interval in seconds
    timev.tv_sec = 0;
    //  Specify time interval in microseconds
    timev.tv_usec = 0;
    //  Set up file descriptor set
    fd_set dta;
    //  Initializes file descriptor set to null
    FD_ZERO(&dta);
    //  Add sock to file descriptor set
    FD_SET(sock,&dta);
    //  Determine socket status
    select((int)sock+1,&dta,0,0,&timev);
    //  Return true upon verifying that the socket has data set;
    //  otherwise, return false
    if (FD_ISSET(sock, &dta)) {
        return true;
    } else {
        return false;
    }
}
