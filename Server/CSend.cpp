#include "CSend.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CSend::CSend(CServer *Server) {
	this->p = Server;

	srand((int)GetTickCount());
	this->startingCity = 35;//27

	this->p = Server;
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
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game/apply/chat, and not the DoNotSend index,
		if (p->Player[i]->isInGameApplyOrChat() && (i != DoNotSend)) {

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
	for (int i = 0; i < MaxPlayers; i++) {

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
	for (int i = 0; i < MaxPlayers; i++) {

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
	for (int i = 0; i < MaxPlayers; i++) {

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

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game, and not the sender,
		if (p->Player[i]->isInGame() && (i != Index)) {

			// If the player is on the radar, or on the sender's team,
			if (((abs(int(p->Player[i]->X - p->Player[Index]->X)) < RadarSize) && (abs(int(p->Player[i]->Y - p->Player[Index]->Y)) < RadarSize)) || p->Player[i]->City == p->Player[Index]->City) {

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

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game, 
		if (p->Player[i]->State == State_Game) {

			// If the player is on the radar, or on the sender's team,
			if ((abs(int(p->Player[i]->X - x)) < RadarSize) && (abs(int(p->Player[i]->Y - y)) < RadarSize)) {

				// Send the data
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
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
	
	// If the player has a rental city,
	if (this->p->Player[Index]->RentalCity > 0) {

		// Send the rental city
		memset(packet, 0, 3);
		packet[0] = this->p->Player[Index]->RentalCity;
		packet[1] = this->p->City[p->Player[Index]->RentalCity]->Mayor;
		packet[2] = this->p->City[p->Player[Index]->RentalCity]->PlayerCount();
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
			for (int i = 0; i < MaxPlayers; i++){

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
	for (int i = 0; i < MaxPlayers; i++) {

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
	sSMPlayer player;
	sSMPoints points;

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game, apply, or chat,
		if (p->Player[i]->isInGameApplyOrChat()) {	

			// Send the player data
			strcpy(player.Name, p->Player[i]->Name.c_str());
			strcpy(player.Town, p->Player[i]->Town.c_str());
			player.Index = i;
			player.isAdmin = p->Player[i]->isAdmin;
			player.Red = p->Player[i]->Red;
			player.Green = p->Player[i]->Green;
			player.Blue = p->Player[i]->Blue;
			player.Member = p->Player[i]->Member;
			player.Tank = p->Player[i]->displayTank;
			p->Winsock->SendData(Index, smPlayerData, (char *)&player, sizeof(player));

			// Send the points data
			points.Index = i;
			points.Points = p->Player[i]->Points;
			points.Deaths = p->Player[i]->Deaths;
			points.Assists = p->Player[i]->Assists;
			points.Orbs = p->Player[i]->Orbs;
			points.MonthlyPoints = p->Player[i]->MonthlyPoints;
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

	// ???
	p->Winsock->SendData(Index, smNextStep, "A");

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game, and isn't the sender,
		if (p->Player[i]->isInGame() && (i != Index)) {	

			// Send the data to the player
			data.id = i;
			data.Mayor = p->Player[i]->Mayor;
			data.City = p->Player[i]->City;
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
	memset(packet, 0, 4);

	// For each possible city,
	for (int j = 0; j < MAX_CITIES; j++) {

		// If the city is hiring, is active, isn't full, and has a mayor,
		if (
				(p->City[j]->notHiring == 0)
				&& 
				(p->City[j]->active)
				&&
				(p->City[j]->PlayerCount() < PLAYERS_PER_CITY)
				&&
				(p->City[j]->Mayor > -1)
			) {

			// Send the city info
			packet[0] = j;
			packet[1] = p->City[j]->Mayor;
			packet[2] = p->City[j]->PlayerCount();
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

	// Get a pointer to the start of the item list
	itm = this->p->Item->items;
	if (itm) {
		while (itm->prev) {
			itm = itm->prev;
		}
	}

	// For each item,
	while (itm) {

		// If the item is in this sector, and is not being held,
		if ((itm->x / SectorSize) == XSector && (itm->y / SectorSize) == YSector && itm->holder == -1) {

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

		if (itm->next) {
			itm = itm->next;
		}
		else {
			break;
		}
	}

	// If there is still data to send, send it
	if (SendStringLength > 0) {
		this->p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}

	// Get a pointer to the start of the building list
	bld = this->p->Build->buildings;
	if (bld) {
		while (bld->prev) {
			bld = bld->prev;
		}
	}

	// For each building,
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
		if (bld->next) {
			bld = bld->next;
		}
		else {
			break;
		}
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

	// For each possible player,
	for (int i = 0; i < MaxPlayers; i++) {

		// If the player is in game,
		if (p->Player[i]->isInGame()) {

			// If the player is in the sector,
			if (((int)abs(((p->Player[i]->X / 48) / SectorSize) - SectorX) <= 1) & ((int)abs(((p->Player[i]->Y / 48) / SectorSize) - SectorY) <= 1)) {

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
	CBuilding *bld = p->Build->buildings;
	char SendString[255];
	int checksum;

	memset(SendString, 0, 255);

	if (bld) {
		while (bld->prev) {
			bld = bld->prev;
		}
	}

	// For each building,
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

		if (bld->next) {
			bld = bld->next;
		}
		else {
			break;
		}
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
	for (int j = 0; j < MaxPlayers; j++) {
		if (p->Player[j]->isInGameApplyOrChat()) {
			totalPlayers++;
		}
	}

	// Decide how many cities we want to open
	citiesWanted = ((int)(((float)(totalPlayers)) / 5.0f)) + 7;

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
			if( (targetCity>0) && (targetCity<MAX_CITIES) && isNeighbor ) {

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

