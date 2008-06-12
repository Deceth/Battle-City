#ifndef CCITY
#define CCITY

#include "CServer.h"

class CServer;
class CBuildingList;

class CCity {

	public:
		CCity(CServer *Server, int id);
		~CCity();

		float DestructTimer;
		float moneyTick;
		float cityVerificationTick;

		int Mayor;
		int Successor;
		int Orbs;
		unsigned char id;
		int active;
		int hiring;
		int notHiring;
		int currentBuildingCount;
		int maxBuildingCount;
		float startTime;
		bool hadBombFactory;
		bool hadOrbFactory;

		long cash;
		long cashresearch;
		long itemproduction;
		long hospital;
		long income;
		int itemC[12];

		float research[20]; // -1 == complete, 0 == unbuilt, > 0 == tick research will complete
		int canBuild[30];

		int x;
		int y;

		void cycle();
		void resetTimers();

		void setCanBuild(int i, int can, bool allowReset);
		void resetToDefault();
		void destroy();

		void didOrb(int City, int index);
		void wasOrbed();
		void addBuilding(int type);
		void subtractBuilding(int type);
		int getOrbValue();
		bool isOrbable();
		int getUptime();
		int getUptimeInSeconds();
		int getUptimeInMinutes();

		int PlayerCount();
		static bool isValidCityIndex(int city);

	protected:

	private:
		CServer *p;
};

#endif
