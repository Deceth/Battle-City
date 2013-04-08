/*
===============================================================================

    Battle City - CSend header file
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
#ifndef __SEND__
#define __SEND__

#include "CServer.h"

class CServer;

class CSend {

	public:
		CSend(CServer *Server);
		~CSend();

		int startingCity;
		int startingCityOptions[9];

		void SendNews(int Index);
		void SendAdminNews(int Index);
		void SendMeetingPlayers(int Index);
		void SendCityList(int Index);
		void SendCurrentPlayers(int Index);
		void SendGameData(int Index);
		void SendCity(int Index, int City);
		void SendInfoButton(unsigned char Index, unsigned char City);
		void SendMeetingRoom(unsigned char Index);
		void SendMiniMap(unsigned char Index);

		void SendAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len = -1);
		void SendGameAll(int Index, unsigned char PacketID, char *TheData, int len = -1);
		void SendGameAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len = -1);
		void SendRadarNotIndex(int Index, unsigned char PacketID, char *TheData, int len = -1);
		void SendRadarAndTeam(int Index, unsigned char PacketID, char *TheData, int len = -1);
		void SendRadar(int x, int y, unsigned char PacketID, char *TheData, int len = -1);
		void SendTeam(int city, unsigned char PacketID, char *TheData, int len = -1);
		void SendChatMessage(int WhoSent, char Message[255], char global);
		void SendToChat(char PacketID, char *TheData, int len = -1);
		void SendSectorArea(int x, int y, unsigned char PacketID, char *TheData, int len = -1);

		void SendCommandos(int Index);
		void SendTheCities(int Index);

		void SendSector(int Index, int XSector, int YSector);
		void SendWhisper(int Index, sCMWhisper *whisper);

		void ResetStartingCC();
		void ResetStartingCC(bool randomReset);

		CServer *p;

	private:

};

#endif
