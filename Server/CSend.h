#ifndef __SEND__
#define __SEND__

#include "CServer.h"

class CServer;

class CSend
{
public:
	CSend(CServer *Server);
	~CSend();

	int startingCity;

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
	void SendGameAllBut(int DoNotSend, unsigned char PacketID, char *TheData, int len = -1);
	void SendRadarNotIndex(int Index, unsigned char PacketID, char *TheData, int len = -1);
	void SendRadarAndTeam(int Index, unsigned char PacketID, char *TheData, int len = -1);
	void SendRadar(int x, int y, unsigned char PacketID, char *TheData, int len = -1);
	void SendChatMessage(int WhoSent, char Message[255], char global);
	void SendToChat(char PacketID, char *TheData, int len = -1);
	void SendSectorArea(int x, int y, unsigned char PacketID, char *TheData, int len = -1);

	void SendCommandos(int Index);
	void SendTheCities(int Index);

	void SendSector(int Index, int XSector, int YSector);

	CServer *p;
private:
protected:
};

#endif
