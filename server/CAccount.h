#ifndef _CACCOUNT
#define _CACCOUNT

#include "CServer.h"

class CServer;

class CAccount {
	public:
		CAccount(CServer *Server);
		~CAccount();

		int CheckAccount(string account);
		int CheckEmail(string email);
		int CheckAccountBan(string account);
		int CheckBan(string UniqID, string IPAddress);

		int CompareEmail(string email, int Index);
		int RecoverAccount(int Index, string Email);

		int CheckPassword(string account, string password); 
		int NewAccount(string account, string password, string email, string fullname, string town, string state);
		int UpdateAccount(int Index, string password, string email, string fullname, string town, string state);
		void GenerateTop20();
		bool MonthHasChanged();

		void AddPoints(int Index, int points);
		void AddDeath(int Index);
		int GetDeaths(int Index);
		void AddOrb(int Index);
		int GetOrbs(int Index);
		void AddAssist(int Index);
		int GetAssists(int Index);
		void Sub2Points(int Index);
		int GetMonthlyTop20(int Index);
		void GetLoginData(int Index);
		void GetStats(int Index);
		void SaveStats(int Index);

		string ReturnRank(int points);

		void SendAccountInformation(int Index);

	private:
		CServer *p;

	protected:

};

#endif
