#include "CServer.h"

CServer Server;

int main(int argc, char *argv[])
{
	float LastBulletTick = 0;

	float ItemTick = 0;
	float BuildingTick = 0;
	float BulletTick = 0;
	float Top20Tick = 0;
	float CityTick = 0;

	Server.Init();

	while (Server.running == 1)
	{
		Server.lastTick = Server.Tick;
		Server.Tick = Server.Timer->GetTime();

		Server.Winsock->Cycle();

		if (Server.Tick > ItemTick)
		{
			Server.Item->cycle();
			ItemTick = Server.Tick + 100;
		}

		if (Server.Tick > BuildingTick)
		{
			Server.Build->cycle();
			BuildingTick = BuildingTick + 50;
		}

		if (Server.Tick > BulletTick)
		{
			Server.TimePassed = Server.Tick - LastBulletTick;
			Server.Bullet->cycle();
			LastBulletTick = Server.Tick;
			BulletTick = Server.Tick + 12;
		}

		if (Server.Tick > Top20Tick)
		{
			Server.Account->GenerateTop20();
			Top20Tick = Server.Tick + 300000;
		}

		if (Server.Tick > CityTick)
		{
			for (int i = 0; i < 64; i++)
			{
				Server.City[i]->cycle();
			}
			CityTick = Server.Tick + 1000;
		}

		Server.PlatformSleep(1);
	}

	cout << "Server::Close" << endl;

	for (int i = 0; i < MaxPlayers; i++)
	{
		if (Server.Player[i]->State > State_Disconnected)
		{
			Server.Player[i]->Clear();
		}
	}
	
	cout << endl << endl << endl << "The server has been safely shutdown.  Press any key to exit" << endl;

#ifdef WIN32
	getch();
#endif

	return EXIT_SUCCESS;
}