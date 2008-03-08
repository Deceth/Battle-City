#include "CSend.h"

CSend::CSend(CServer *Server)
{
	p = Server;

	srand((int)GetTickCount());
	startingCity = 35;//27

	p = Server;
}

CSend::~CSend()
{

}

void CSend::SendAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State >= State_Chat && i != DoNotSend)
			p->Winsock->SendData(i, PacketID, TheData, len);
	}	
}

void CSend::SendGameAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game && i != DoNotSend)
			p->Winsock->SendData(i, PacketID, TheData, len);
	}	
}

void CSend::SendRadarNotIndex(int Index, unsigned char PacketID, char *TheData, int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game && i != Index)
		{
			if ((abs(int(p->Player[i]->X - p->Player[Index]->X)) < RadarSize) && (abs(int(p->Player[i]->Y - p->Player[Index]->Y)) < RadarSize))
				p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

void CSend::SendMeetingPlayers(int Index)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Chat)
		{
			char tmpPacket[5];
			memset(tmpPacket, 0, 5);
			tmpPacket[0] = i;
			strcpy(&tmpPacket[1], "A");
			p->Winsock->SendData(Index, smChatCommand, tmpPacket, 2);
		}
	}	
}

void CSend::SendMeetingRoom(unsigned char Index)
{
	p->Send->SendMeetingPlayers(Index); 

	p->Player[Index]->State = State_Chat;

	char tmpString[5];
	memset(tmpString, 0, 5);
	tmpString[0] = (unsigned char)Index;
	strcpy(&tmpString[1], "A"); 
	p->Send->SendToChat(smChatCommand, tmpString, sizeof(tmpString));

	p->Send->SendNews(Index);
	p->Send->SendCityList(Index);
}

void CSend::SendRadarAndTeam(int Index, unsigned char PacketID, char *TheData, int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game && i != Index)
		{
			if (((abs(int(p->Player[i]->X - p->Player[Index]->X)) < RadarSize) && (abs(int(p->Player[i]->Y - p->Player[Index]->Y)) < RadarSize)) || p->Player[i]->City == p->Player[Index]->City)
				p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

void CSend::SendRadar(int x, int y, unsigned char PacketID, char *TheData, int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game)
		{
			if ((abs(int(p->Player[i]->X - x)) < RadarSize) && (abs(int(p->Player[i]->Y - y)) < RadarSize))
				p->Winsock->SendData(i, PacketID, TheData, len);
		}
	}	
}

void CSend::SendCityList(int Index)
{
	// Rental City
	if (p->Player[Index]->RentalCity > 0)
	{
		char packet[3];
		memset(packet, 0, 3);

		packet[0] = p->Player[Index]->RentalCity;
		packet[1] = p->City[p->Player[Index]->RentalCity]->Mayor;
		p->Winsock->SendData(Index,smAddRemCity,packet,2);
	}
	SendCommandos(Index);
	SendTheCities(Index);
}

void CSend::SendChatMessage(int WhoSent, char Message[255], char global)
{
	char tmpstring[255];
	tmpstring[0] = (unsigned char)WhoSent;
	strcpy(&tmpstring[1], Message);
	int length = (int)strlen(Message);

	string outputString;
	outputString = p->Player[WhoSent]->Name;
	outputString += ": ";
	outputString += Message;
	cout << outputString << "\n";
	p->Log->logChat(outputString);

	switch (p->Player[WhoSent]->State){
		case State_Chat:
			for (int i = 0; i < MaxPlayers; i++){
				if (p->Player[i]->State == State_Chat && i != WhoSent){
					p->Winsock->SendData(i, cmChatMessage, tmpstring, length + 1);
				}
			}	
			break;
		case State_Game:
			switch (global)
			{
				// Global chat - All players receive message
				case 1:
					p->Send->SendGameAllBut(WhoSent, smGlobal, tmpstring, length + 1);
					break;
				// Normal chat - Teamates and players on radar receive message
				case 2:
					p->Send->SendRadarAndTeam(WhoSent, cmChatMessage, tmpstring, length + 1);
					break;
				// Disrupted chat - Only players on radar receive message
				case 0:
					p->Send->SendRadarNotIndex(WhoSent, cmChatMessage, tmpstring, length + 1);
					break;
			}
			break;
		case State_Apply:
			int tmpNum = p->Player[WhoSent]->FindApplyMayor();
			if (tmpNum >= 0) p->Winsock->SendData(tmpNum, smComms, tmpstring, length + 1);
			break;
	}
}

void CSend::SendToChat(char PacketID, char *TheData,int len)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Chat) //Players need to be in the chat room
			p->Winsock->SendData(i, PacketID, TheData, len);
	}	
}

void CSend::SendCurrentPlayers(int Index)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State >= State_Chat)
		{	
			sSMPlayer player;
			strcpy(player.Name, p->Player[i]->Name.c_str());
			strcpy(player.Town, p->Player[i]->Town.c_str());
			player.Index = i;
			player.isAdmin = p->Player[i]->isAdmin;
			player.Red = p->Player[i]->Red;
			player.Green = p->Player[i]->Green;
			player.Blue = p->Player[i]->Blue;
			player.Member = p->Player[i]->Member;
			player.Tank = p->Player[i]->Tank;
			p->Winsock->SendData(Index, smPlayerData, (char *)&player, sizeof(player));
			sSMPoints points;
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

void CSend::SendNews(int Index)
{
	int DataChunks = 0;
	int Length = 0;

	Length = (int)p->Top20.length();
	DataChunks = int(Length / 220) + 1;
	
	int startpos = 0;
	char TempString[255];

	for (int i = 0; i < DataChunks; i++)
	{
		if (p->Top20[startpos] != 0)
		{
			memset(TempString, 0, 255);
			memcpy(TempString, &p->Top20[startpos], 220);
			p->Winsock->SendData(Index, smAppendNews, TempString);
			startpos = startpos + 220;
		}
	}	
}

void CSend::SendAdminNews(int Index)
{
	if (strlen(p->News.c_str()) > 240)
	{
		p->Log->logServerError("News is too long!");
	}
	else
	{
		char TempArray[241];
		memset(TempArray, 0, sizeof(TempArray));
		strcpy(TempArray, p->News.c_str());
		p->Winsock->SendData(Index, smAppendNews, TempArray);
	}
}

void CSend::SendGameData(int Index)
{
	p->Winsock->SendData(Index, smNextStep, "A");

	sSMJoinData data;
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game && i != Index)
		{	
			data.id = i;
			data.Mayor = p->Player[i]->Mayor;
			data.City = p->Player[i]->City;
			p->Winsock->SendData(Index, smJoinData, (char *)&data, sizeof(data));
		}
	}
}

void CSend::SendCommandos(int Index)
{
	char packet[4];
	memset(packet, 0, 4);

	for (int j = 0; j < 64; j++)
	{
		if (p->City[j]->notHiring == 0 && p->City[j]->active && p->City[j]->PlayerCount() < 4 && p->City[j]->Mayor > -1)
		{
			packet[0] = j;
			packet[1] = p->City[j]->Mayor;
			p->Winsock->SendData(Index,smAddRemCity,packet,3);
		}
	}
}

void CSend::SendSector(int Index, int XSector, int YSector)
{
	int SendStringLength = 0;
	char SendString[255];
	memset(SendString, 0, 255);

	CItem *itm;
	itm = p->Item->items;

	if (itm)
		while (itm->prev)
			itm = itm->prev;

	while (itm)
	{
		if ((itm->x / SectorSize) == XSector && (itm->y / SectorSize) == YSector && itm->holder == -1)
		{
			sSMItem packet;
			packet.x = itm->x;
			packet.y = itm->y;
			packet.City = itm->City;
			packet.type = itm->type;
			packet.id = itm->id;
			packet.active = itm->active;

			int length = sizeof(packet) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(packet) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smAddItem;
			memcpy(&SendString[SendStringLength + 3], (char *)&packet, sizeof(packet));

			int checksum = 0;
			for (int j = 0; j < (length - 2); j++)
			{
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;

			SendStringLength += (length + 1);

			if (SendStringLength > 220)
			{
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}
		if (itm->next)
			itm = itm->next;
		else
			break;
	}

	if (SendStringLength > 0)
	{
		p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}

	CBuilding *bld;
	bld = p->Build->buildings;

	if (bld)
		while (bld->prev)
			bld = bld->prev;

	while (bld)
	{
		if ((bld->x / SectorSize) == XSector && (bld->y / SectorSize) == YSector && bld->type != 0)
		{
			sSMBuild packet;
			packet.x = bld->x;
			packet.y = bld->y;
			packet.type = bld->type;
			packet.id = bld->id;
			if (packet.type == 2)
				packet.pop = bld->pop/16;
			else
				packet.pop = bld->pop/8;

			packet.City = bld->City;

			if ((bld->type % 2) == 0 && bld->type > 2)
			{
				packet.count = maxItems[(bld->type - 2) / 2 - 1] - (unsigned char)p->City[bld->City]->itemC[(bld->type - 2) / 2 - 1];
			}

			int length = sizeof(packet) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(packet) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smPlaceBuilding;
			memcpy(&SendString[SendStringLength + 3], (char *)&packet, sizeof(packet));

			int checksum = 0;
			for (int j = 0; j < (length - 2); j++)
			{
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;

			SendStringLength += (length + 1);

			if (SendStringLength > 220)
			{
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}
		if (bld->next)
			bld = bld->next;
		else
			break;
	}

	if (SendStringLength > 0)
	{
		p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}

	sCMSector sector;
	sector.x = XSector;
	sector.y = YSector;
	p->Winsock->SendData(Index, smSectorSent, (char *)&sector, sizeof(sector));
}

void CSend::SendSectorArea(int x, int y, unsigned char PacketID, char *TheData, int len)
{
	unsigned char SectorX = ((x / 48) / SectorSize);
	unsigned char SectorY = ((y / 48) / SectorSize);

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (p->Player[i]->State == State_Game)
		{
			if (((int)abs(((p->Player[i]->X / 48) / SectorSize) - SectorX) <= 1) & ((int)abs(((p->Player[i]->Y / 48) / SectorSize) - SectorY) <= 1))
			{
				p->Winsock->SendData(i, PacketID, TheData, len);
			}
		}
	}	
}

void CSend::SendInfoButton(unsigned char Index, unsigned char City)
{
	sSMInfoButton closestcity;
	closestcity.city = City;

	p->Winsock->SendData(Index, smInfoButton, (char *)&closestcity, sizeof(closestcity));
}

void CSend::SendMiniMap(unsigned char Index)
{
	int SendStringLength = 0;
	char SendString[255];
	memset(SendString, 0, 255);

	int XSector = (int)p->Player[Index]->X/48/SectorSize;
	int YSector = (int)p->Player[Index]->Y/48/SectorSize;

	CBuilding *bld;
	bld = p->Build->buildings;

	if (bld)
		while (bld->prev)
			bld = bld->prev;

	while (bld)
	{
		if (abs(XSector - (bld->x / SectorSize)) < 4 && abs(YSector - (bld->y / SectorSize)) < 4 && bld->type != 0)
		{
			sSMBuild packet;
			packet.x = bld->x;
			packet.y = bld->y;
			packet.type = bld->type;
			packet.id = bld->id;
			if (packet.type == 2)
				packet.pop = bld->pop/16;
			else
				packet.pop = bld->pop/8;

			packet.City = bld->City;

			if ((bld->type % 2) == 0 && bld->type > 2)
			{
				packet.count = maxItems[(bld->type - 2) / 2 - 1] - (unsigned char)p->City[bld->City]->itemC[(bld->type - 2) / 2 - 1];
			}

			int length = sizeof(packet) + 2;
			SendString[SendStringLength + 0] = (unsigned char)(sizeof(packet) + 2);
			SendString[SendStringLength + 2] = (unsigned char)smPlaceBuilding;
			memcpy(&SendString[SendStringLength + 3], (char *)&packet, sizeof(packet));

			int checksum = 0;
			for (int j = 0; j < (length - 2); j++)
			{
				checksum += SendString[SendStringLength + 3 + j];
			}
			checksum += 3412;
			checksum = checksum % 71;
			SendString[SendStringLength + 1] = (unsigned char)checksum;

			SendStringLength += (length + 1);

			if (SendStringLength > 220)
			{
				p->Winsock->SendRaw(Index, SendString, SendStringLength);
				memset(SendString, 0, 255);
				SendStringLength = 0;
			}
		}
		if (bld->next)
			bld = bld->next;
		else
			break;
	}

	if (SendStringLength > 0)
	{
		p->Winsock->SendRaw(Index, SendString, SendStringLength);
		memset(SendString, 0, 255);
		SendStringLength = 0;
	}
}

void CSend::SendTheCities(int Index)
{
	int counter = 0;
	int lastCity = startingCity;
	int targetCity = startingCity;
	int citiesFound = 0;
	int citiesWanted;
	bool isNeighbor = true;
	int i;

	int totalPlayers = 0;
	for (int j = 0; j < MaxPlayers; j++)
	{
		if (p->Player[j]->State >= State_Chat)
		{
			totalPlayers++;
		}
	}
	citiesWanted = ((int)(((float)(totalPlayers)) / 5.0f)) + 7;

	if (p->City[startingCity]->Mayor == -1)
	{
		char packet[4];
		memset(packet, 0, 4);

		packet[0] = targetCity;
		packet[1] = p->City[targetCity]->Mayor;
		p->Winsock->SendData(Index,smAddRemCity,packet,2);
	}

	citiesFound++;

	while( citiesFound < citiesWanted ) {

		// Loops 1, 1, 2, 2, 3, 3 etc times to spiral outward from city
		for( i=0; i<(counter/2)+1; i++) {
			lastCity = targetCity;

			// Simulate moving right, down, left, or up on the map depending on the value of counter
			targetCity += 1 + 7*(counter%2) - 2*((counter/2)%2) - 14*((counter/2)%2)*(counter%2);

			// Check for edge-wrap-- if found, turn off isNeighbor until another edge-wrap occurs
			if( (targetCity%8)!=((lastCity%8)+1-(counter%4)) && (targetCity/8)!=((lastCity/8)+2-(counter%4))) {
				isNeighbor = !isNeighbor;
			}

			// If the targetCity is a valid index and didn't edgewrap,
			if( (targetCity>0) && (targetCity<64) && isNeighbor ) {

				//
				// Code to open the city at targetCity index
				//
				if (p->City[targetCity]->Mayor == -1)
				{
					char packet[3];
					memset(packet, 0, 3);

					packet[0] = targetCity;
					packet[1] = p->City[targetCity]->Mayor;
					p->Winsock->SendData(Index,smAddRemCity,packet,2);
				}

				citiesFound++;

				if(citiesFound == citiesWanted)
					break;
			}
		}
		counter++;
	}
}

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

