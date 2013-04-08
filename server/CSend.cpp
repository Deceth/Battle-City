/*
===============================================================================

    Battle City - CSend file
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
#include "CSend.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CSend::CSend(CServer *Server) {
	this->p = Server;

	// Set options for possible starting cities
	startingCityOptions[0] = 18;
	startingCityOptions[1] = 19;
	startingCityOptions[2] = 20;
	startingCityOptions[3] = 26;
	startingCityOptions[4] = 27;
	startingCityOptions[5] = 28;
	startingCityOptions[6] = 34;
	startingCityOptions[7] = 35;
	startingCityOptions[8] = 36;

	startingCity = startingCityOptions[0];
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CSend::~CSend() {
}

/***************************************************************
 * Function:	SendAllBut
 *
 * @param DoNotSend
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game/apply/chat, and not the DoNotSend index,
		if (p->Player[i]->isInGameApplyOrChat() && (i != DoNotSend)) {

			// Send the data
			p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

/***************************************************************
 * Function:	SendGameAll
 *
 * @param Index
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendGameAll(int Index, unsigned char PacketID, char *TheData, int len) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game
		if (p->Player[i]->isInGame()) {

			// Send the data
			p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}


/***************************************************************
 * Function:	SendGameAllBut
 *
 * @param DoNotSend
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendGameAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, and not the DoNotSend index,
		if (p->Player[i]->isInGame() && (i != DoNotSend)) {

			// Send the data
			p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

/***************************************************************
 * Function:	SendRadarNotIndex
 *
 * @param Index
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendRadarNotIndex(int Index, unsigned char PacketID, char *TheData, int len) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in game, and not the DoNotSend index,
		if (p->Player[i]->isInGame() && (i != Index)) {

			// If the player is in radar range,
			if ((abs(int(p->Player[i]->X - p->Player[Index]->X)) < RadarSize) && (abs(int(p->Player[i]->Y - p->Player[Index]->Y)) < RadarSize)) {

				// Send the data
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
		}
	}	
}

/***************************************************************
 * Function:	SendMeetingPlayers
 *
 * @param Index
 **************************************************************/
void CSend::SendMeetingPlayers(int Index) {
	char tmpPacket[5];
	
	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in Chat,
		if (p->Player[i]->isInChat()) {

			// Send the data
			memset(tmpPacket, 0, 5);
			tmpPacket[0] = i;
			strcpy(&tmpPacket[1], "A");
			p->Winsock->SendData(Index, smChatCommand, tmpPacket, 2);
		}
	}	
}

/***************************************************************
 * Function:	SendMeetingRoom
 *
 * @param Index
 **************************************************************/
void CSend::SendMeetingRoom(unsigned char Index) {
	char tmpString[5];

	// Send the meeting room players
	p->Send->SendMeetingPlayers(Index); 

	// Set the player to Chat
	p->Player[Index]->State = State_Chat;

	// Tell everyone the player joined
	memset(tmpString, 0, 5);
	tmpString[0] = (unsigned char)Index;
	strcpy(&tmpString[1], "A"); 
	p->Send->SendToChat(smChatCommand, tmpString, sizeof(tmpString));

	// Send the news and city list
	p->Send->SendNews(Index);
	p->Send->SendCityList(Index);
}


/***************************************************************
 * Function:	SendRadarAndTeam
 *
 * @param Index
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendRadarAndTeam(int Index, unsigned char PacketID, char *TheData, int len) {
	CPlayer* playerMe = this->p->Player[Index];
	CPlayer* playerToCompare;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		playerToCompare = this->p->Player[i];

		// If the player is in game, and not the sender,
		if (playerToCompare->isInGame() && (i != Index)) {

			// If the player is on the radar, or on the sender's team,
			if (((abs(int(playerToCompare->X - playerMe->X)) < RadarSize) && (abs(int(playerToCompare->Y - playerMe->Y)) < RadarSize)) || playerToCompare->City == playerMe->City) {

				// Send the data
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
		}
	}	
}

/***************************************************************
 * Function:	SendRadar
 *
 * @param x
 * @param y
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendRadar(int x, int y, unsigned char PacketID, char *TheData, int len) {
	CPlayer* player;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game, 
		if (player->State == State_Game) {

			// If the player is on the radar, or on the sender's team,
			if ((abs(int(player->X - x)) < RadarSize) && (abs(int(player->Y - y)) < RadarSize)) {

				// Send the data
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
		}
	}	
}

/***************************************************************
 * Function:	SendTeam
 *
 * @param city
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendTeam(int city, unsigned char PacketID, char *TheData, int len) {
	CPlayer* player;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game, and is in the target city,
		if (player->isInGame() && (player->City == city)) {

			// Send the data
			p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

/***************************************************************
 * Function:	SendCityList
 *
 * @param Index
 **************************************************************/
void CSend::SendCityList(int Index) {
	char packet[3];
	CPlayer* player = this->p->Player[Index];
	CCity* rentalCity;
	
	// If the player has a valid rental city,
	if (CCity::isValidCityIndex(player->RentalCity)) {
		rentalCity = this->p->City[player->RentalCity];
	
		// Send the rental city
		memset(packet, 0, 3);
		packet[0] = player->RentalCity;
		packet[1] = rentalCity->Mayor;
		packet[2] = rentalCity->PlayerCount();
		this->p->Winsock->SendData(Index,smAddRemCity,packet,2);
	}

	// Send the cities needing commandos
	this->SendCommandos(Index);

	// Send the cities needing mayors
	this->SendTheCities(Index);
}

/***************************************************************
 * Function:	SendChatMessage
 *
 * @param WhoSent
 * @param Message
 * @param global
 **************************************************************/
void CSend::SendChatMessage(int WhoSent, char Message[255], char global) {
	char tmpstring[255];
	int length = (int)strlen(Message);
	tmpstring[0] = (unsigned char)WhoSent;
	strcpy(&tmpstring[1], Message);
	string outputString;
	int tmpNum;

	// Format the message to add sender's name
	outputString = p->Player[WhoSent]->Name;
	outputString += ": ";
	outputString += Message;
	cout << outputString << "\n";
	p->Log->logChat(outputString);

	// Switch on the sender's state
	switch (p->Player[WhoSent]->State){

		// State: MEETING ROOM
		case State_Chat:

			// For each possible player,
			for (int i = 0; i < MAX_PLAYERS; i++){

				// If the player is in chat, and isn't the sender,
				if (p->Player[i]->isInChat() && (i != WhoSent)) {

					// Send the message
					p->Winsock->SendData(i, cmChatMessage, tmpstring, length + 1);
				}
			}	
			break;

		// State: GAME
		case State_Game:

			// Switch on the Global flag
			switch (global) {

				// Global message, send to everyone but sender
				case 1:
					p->Send->SendGameAllBut(WhoSent, smGlobal, tmpstring, length + 1);
					break;

				// Normal chat, send to radar and team
				case 2:
					p->Send->SendRadarAndTeam(WhoSent, cmChatMessage, tmpstring, length + 1);
					break;

				// Disrupted chat, send to radar only
				case 0:
					p->Send->SendRadarNotIndex(WhoSent, cmChatMessage, tmpstring, length + 1);
					break;
			}
			break;

		// State: APPLY WINDOW
		case State_Apply:

			// Find the mayor to whom the player is applying
			tmpNum = p->Player[WhoSent]->FindApplyMayor();

			// If the mayor was found, send the message to the mayor
			if (tmpNum >= 0) {
				p->Winsock->SendData(tmpNum, smComms, tmpstring, length + 1);
			}
			break;
	}
}

/***************************************************************
 * Function:	SendToChat
 *
 * @param PacketID
 * @param TheData
 * @param len
 **************************************************************/
void CSend::SendToChat(char PacketID, char *TheData, int len) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player is in the chat room,
		if (p->Player[i]->isInChat()) {

			// Send the data
			p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

/***************************************************************
 * Function:	SendCurrentPlayers
 *
 * @param Index
 **************************************************************/
void CSend::SendCurrentPlayers(int Index) {
	sSMPlayer playerPacket;
	sSMPoints points;
	CPlayer* player;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game, apply, or chat,
		if (player->isInGameApplyOrChat()) {	

			// Send the player data
			strcpy(playerPacket.Name, player->Name.c_str());
			strcpy(playerPacket.Town, player->Town.c_str());
			playerPacket.Index = i;
			playerPacket.playerType = player->playerType;
			playerPacket.Red = player->Red;
			playerPacket.Green = player->Green;
			playerPacket.Blue = player->Blue;
			playerPacket.Member = player->Member;
			playerPacket.Tank = player->displayTank;
			p->Winsock->SendData(Index, smPlayerData, (char *)&playerPacket, sizeof(playerPacket));

			// Send the points data
			points.Index = i;
			points.Points = player->Points;
			points.Deaths = player->Deaths;
			points.Assists = player->Assists;
			points.Orbs = player->Orbs;
			points.MonthlyPoints = player->MonthlyPoints;
			p->Winsock->SendData(Index, smPointsUpdate, (char *)&points, sizeof(points));
		}
	}	
}

/***************************************************************
 * Function:	SendNews
 *
 * @param Index
 **************************************************************/
void CSend::SendNews(int Index) {
	int Length = (int)p->Top20.length();
	int DataChunks = int(Length / 220) + 1;
	int startpos = 0;
	char TempString[255];

	// For each "chunk" of 220 bytes in the News's length,
	for (int i = 0; i < DataChunks; i++) {

		// If there is data to send, 
		if (p->Top20[startpos] != 0) {

			// Send the data
			memset(TempString, 0, 255);
			memcpy(TempString, &p->Top20[startpos], 220);
			p->Winsock->SendData(Index, smAppendNews, TempString);
			startpos = startpos + 220;
		}
	}	
}

/***************************************************************
 * Function:	SendAdminNews
 *
 * @param Index
 **************************************************************/
void CSend::SendAdminNews(int Index) {
	char TempArray[241];

	// If news is more than 240 chars, send error
	if (strlen(p->News.c_str()) > 240) {
		p->Log->logServerError("News is too long!");
	}

	// Else (news under 240 chars),
	else {

		// Send the news
		memset(TempArray, 0, sizeof(TempArray));
		strcpy(TempArray, p->News.c_str());
		p->Winsock->SendData(Index, smAppendNews, TempArray);
	}
}

/***************************************************************
 * Function:	SendGameData
 *
 * @param Index
 **************************************************************/
void CSend::SendGameData(int Index) {
	sSMJoinData data;
	CPlayer* player;

	// ???
	p->Winsock->SendData(Index, smNextStep, "A");

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game, and isn't the sender,
		if (player->isInGame() && (i != Index)) {	

			// Send the data to the player
			data.id = i;
			data.Mayor = player->Mayor;
			data.City = player->City;
			p->Winsock->SendData(Index, smJoinData, (char *)&data, sizeof(data));
		}
	}
}

/***************************************************************
 * Function:	SendCommandos
 *
 * @param Index
 **************************************************************/
void CSend::SendCommandos(int Index) {
	char packet[4];
	CCity* city;
	int playerCount;
	memset(packet, 0, 4);

	// For each possible city,
	for (int j = 0; j < MAX_CITIES; j++) {
		city = this->p->City[j];
		playerCount = city->PlayerCount();

		// If the city is hiring, is active, isn't full, isn't empty, and has a mayor,
		if (
				(city->notHiring == 0)
				&& 
				(city->active)
				&&
				(playerCount < MAX_PLAYERS_PER_CITY)
				&&
				(playerCount > 0)
				&&
				(city->Mayor > -1)
			) {

			// Send the city info
			packet[0] = j;
			packet[1] = city->Mayor;
			packet[2] = playerCount;
			p->Winsock->SendData(Index,smAddRemCity,packet,3);
		}
	}
}

/***************************************************************
 * Function:	SendSector
 *
 * @param Index
 * @param XSector
 * @param YSector
 **************************************************************/
void CSend::SendSector(int Index, int XSector, int YSector) {
	int SendStringLength = 0;
	char SendString[255];
	memset(SendString, 0, 255);
	CItem *itm;
	sSMItem itemPacket;
	CBuilding *bld;
	sSMBuild buildingPacket;
	int length;
	int checksum;
	sCMSector sector;

	// For each item,
	itm = this->p->Item->itemListHead;
	while (itm) {

		// If the item is in this sector, and is not being held,
		if (((itm->x / SectorSize) == XSector) && ((itm->y / SectorSize) == YSector) && (itm->holder == -1)) {

			// Create a packet for the item
			itemPacket.x = itm->x;
			itemPacket.y = itm->y;
			itemPacket.City = itm->City;
			itemPacket.type = itm->type;
			itemPacket.id = itm->id;
			itemPacket.active = itm->active;

			// Collect multiple packets into one string SendString
			length = sizeof(itemPacket) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(itemPacket) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smAddItem;
			memcpy(&SendString[SendStringLength + 3], (char *)&itemPacket, sizeof(itemPacket));

			// Handle checksum for the packet
			// ???
			checksum = 0;
			for (int j = 0; j < (length - 2); j++) {
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;
			SendStringLength += (length + 1);

			// If the send string is over 220 chars, send it
			if (SendStringLength > 220) {
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}

		// Get the next item
		itm = itm->next;
	}

	// If there is still data to send, send it
	if (SendStringLength > 0) {
		this->p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}

	// For each building,
	bld = this->p->Build->buildingListHead;
	while (bld) {

		// If the building is in the sector and is not a CC,
		if ((bld->x / SectorSize) == XSector && (bld->y / SectorSize) == YSector && (! bld->isCC())) {
			buildingPacket.x = bld->x;
			buildingPacket.y = bld->y;
			buildingPacket.type = bld->type;
			buildingPacket.id = bld->id;
			buildingPacket.City = bld->City;

			// If the building is a House, set pop on packet to pop/16
			if (bld->isHouse()) {
				buildingPacket.pop = bld->pop/16;
			}

			// Else (not a House), set pop on packet to pop/8
			else {
				buildingPacket.pop = bld->pop/8;
			}

			// If the building is a factory,
			if (bld->isFactory()) {
				buildingPacket.count = maxItems[(bld->type - 2) / 2 - 1] - (unsigned char)p->City[bld->City]->itemC[(bld->type - 2) / 2 - 1];
			}

			// Collect multiple packets into one string SendString
			length = sizeof(buildingPacket) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(buildingPacket) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smPlaceBuilding;
			memcpy(&SendString[SendStringLength + 3], (char *)&buildingPacket, sizeof(buildingPacket));

			// Handle checksum for the packet
			// ???
			checksum = 0;
			for (int j = 0; j < (length - 2); j++) {
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;
			SendStringLength += (length + 1);

			// If the send string is over 220 chars, send it
			if (SendStringLength > 220) {
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}

		bld = bld->next;
	}

	// If there is still data to send, send it
	if (SendStringLength > 0) {
		p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}

	// Tell the client the sector is sent
	sector.x = XSector;
	sector.y = YSector;
	p->Winsock->SendData(Index, smSectorSent, (char *)&sector, sizeof(sector));
}

/***************************************************************
 * Function:	SendSectorArea
 *
 * @param x
 * @param y
 * @param PacketID
 * @param TheDate
 * @param len
 **************************************************************/
void CSend::SendSectorArea(int x, int y, unsigned char PacketID, char *TheData, int len) {
	unsigned char SectorX = ((x / 48) / SectorSize);
	unsigned char SectorY = ((y / 48) / SectorSize);
	CPlayer* player;

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player = this->p->Player[i];

		// If the player is in game,
		if (player->isInGame()) {

			// If the player is in the sector,
			if (((int)abs(((player->X / 48) / SectorSize) - SectorX) <= 1) & ((int)abs(((player->Y / 48) / SectorSize) - SectorY) <= 1)) {

				// Send the data
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
		}
	}	
}

/***************************************************************
 * Function:	SendInfoButton
 *
 * @param Index
 * @param City
 **************************************************************/
void CSend::SendInfoButton(unsigned char Index, unsigned char City) {
	sSMInfoButton closestcity;
	closestcity.city = City;
	p->Winsock->SendData(Index, smInfoButton, (char *)&closestcity, sizeof(closestcity));
}

/***************************************************************
 * Function:	SendMiniMap
 *
 * @param Index
 **************************************************************/
void CSend::SendMiniMap(unsigned char Index) {
	int SendStringLength = 0;
	sSMBuild packet;
	int length;
	int XSector = (int)p->Player[Index]->X/48/SectorSize;
	int YSector = (int)p->Player[Index]->Y/48/SectorSize;
	CBuilding *bld;
	char SendString[255];
	int checksum;

	memset(SendString, 0, 255);

	// For each building,
	bld = p->Build->buildingListHead;
	while (bld) {

		// If the building is in the sector and is not a CC,
		if (abs(XSector - (bld->x / SectorSize)) < 4 && abs(YSector - (bld->y / SectorSize)) < 4 && (! bld->isCC())) { 
			packet.x = bld->x;
			packet.y = bld->y;
			packet.type = bld->type;
			packet.id = bld->id;
			packet.City = bld->City;

			// If the building is a House, set pop on packet to pop/16
			if (bld->isHouse()) {
				packet.pop = bld->pop/16;
			}

			// Else (not a House), set pop on packet to pop/8
			else {
				packet.pop = bld->pop/8;
			}

			// If the building is a factory,
			if (bld->isFactory()) {
				packet.count = maxItems[(bld->type - 2) / 2 - 1] - (unsigned char)p->City[bld->City]->itemC[(bld->type - 2) / 2 - 1];
			}


			// Collect multiple packets into one string SendString
			length = sizeof(packet) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(packet) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smPlaceBuilding;
			memcpy(&SendString[SendStringLength + 3], (char *)&packet, sizeof(packet));

			// Handle checksum for the packet
			// ???
			checksum = 0;
			for (int j = 0; j < (length - 2); j++) {
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;
			SendStringLength += (length + 1);

			// If the send string is over 220 chars, send it
			if (SendStringLength > 220) {
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}

		bld = bld->next;
	}

	// If there is still data to send, send it
	if (SendStringLength > 0) {
		p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}
}

/***************************************************************
 * Function:	SendTheCities
 *
 * @param Index
 **************************************************************/
void CSend::SendTheCities(int Index) {
	int counter = 0;
	int lastCity = this->startingCity;
	int targetCity = this->startingCity;
	int citiesFound = 0;
	int citiesWanted;
	bool isNeighbor = true;
	int i;
	int totalPlayers = 0;
	char packet[4];

	// Count the number of players in game
	for (int j = 0; j < MAX_PLAYERS; j++) {
		if (p->Player[j]->isInGameApplyOrChat()) {
			totalPlayers++;
		}
	}

	// Decide how many cities we want to open
	citiesWanted = ((int)(((float)(totalPlayers)) / 5.0f)) + 6;

	// If the startingCity is open, send it
	if (p->City[this->startingCity]->Mayor == -1) {
		memset(packet, 0, 4);
		packet[0] = targetCity;
		packet[1] = p->City[targetCity]->Mayor;
		p->Winsock->SendData(Index,smAddRemCity,packet,4);
	}
	citiesFound++;

	// While we still want to open more cities,
	while( citiesFound < citiesWanted ) {

		// Spiral outward from the startingCity, moving 1, 1, 2, 2, 3, 3 etc((counter/2)+1) times in a direction before changing direction
		// So, counter represents the number of cities "in a row" that we open before changing direction
		for( i=0; i<(counter/2)+1; i++) {
			lastCity = targetCity;

			// Simulate moving right, down, left, or up on the map depending on the value of counter
			targetCity += 1 + 7*(counter%2) - 2*((counter/2)%2) - 14*((counter/2)%2)*(counter%2);

			// Check for edge-wrap-- if found, turn off isNeighbor until another edge-wrap occurs
			if( (targetCity%8)!=((lastCity%8)+1-(counter%4)) && (targetCity/8)!=((lastCity/8)+2-(counter%4))) {
				isNeighbor = !isNeighbor;
			}

			// If the targetCity is a valid index and didn't edgewrap,
			if( CCity::isValidCityIndex(targetCity) && isNeighbor ) {

				// Open the city at targetCity index
				if (p->City[targetCity]->Mayor == -1) {
					memset(packet, 0, 4);
					packet[0] = targetCity;
					packet[1] = p->City[targetCity]->Mayor;
					packet[2] = p->City[targetCity]->PlayerCount();
					p->Winsock->SendData(Index,smAddRemCity,packet,4);
				}
				citiesFound++;

				// If we opened the cities we want to open, break
				if(citiesFound == citiesWanted) {
					break;
				}
			}
		}
		counter++;
	}
}

/***************************************************************
 * Function:	SendWhisper
 *
 * @param Index
 **************************************************************/
void CSend::SendWhisper(int WhoSent, sCMWhisper *whisper) {
	string senderName = p->Player[whisper->Recipient]->Name;
	string recipientName = p->Player[WhoSent]->Name;
	string message = whisper->Message;
	char tmpstring[128];
	tmpstring[0] = (unsigned char)WhoSent;
	strcpy(&tmpstring[1], whisper->Message);

	// Log the message
	string outputString = senderName + " (to " + recipientName + "): " + message;
	cout << outputString << "\n";
	p->Log->logChat(outputString);

	// Send the message
	p->Winsock->SendData(whisper->Recipient, smWhisper, tmpstring, sizeof(tmpstring));
}

/***************************************************************
 * Function:	ResetStartingCC
 *
 **************************************************************/
void CSend::ResetStartingCC() {
	this->ResetStartingCC(false);
}

/***************************************************************
 * Function:	ResetStartingCC
 *
 * @param randomReset
 **************************************************************/
void CSend::ResetStartingCC(bool randomReset) {
	int startingCityOptionIndex;

	// If randomReset, only reset on a random condition
	if (randomReset) {

		// If a random number is not divisible by three, return
		srand ( time(NULL) );
		if ((rand() % 3) != 0) {
			cout << " -- no city reset: random condition failed" << endl;
			return;
		}
	}

	// Reset the CC
	srand((int)GetTickCount());
	startingCityOptionIndex = (rand() % (sizeof(startingCityOptions)/sizeof(int))) ;
	this->startingCity = startingCityOptions[startingCityOptionIndex];

	cout << " -- starting city: index(" << startingCityOptionIndex << ") value(" << startingCityOptions[startingCityOptionIndex] << ")" << endl;
}

