#include "CDatabase.h"

CDatabase::CDatabase(CServer *Server)
{
	p = Server;
}

CDatabase::~CDatabase()
{
	Database.close();
}

void CDatabase::LoadDatabase()
{
	cout << "Database::Load" << endl;
	cout << "SQLite Version: " << Database.SQLiteVersion() << endl;

	if (p->Exists("dat//BCData.db") == 1)
	{
		Database.open("dat//BCData.db");

		CleanDatabase();
		
		cout << "Database::Vacuum" << endl;
		Database.execDML("VACUUM");

		cout << "Database::Load::Success" << endl << endl;
	}
	else
	{
		cout << "Database::Load::NoExist" << endl << "Database::Create" << endl;
		Database.open("dat//BCData.db");

		cout << "Database::Create::Accounts" << endl;

		//Accounts Table
		Database.execDML("CREATE TABLE tAccounts(AccountID INT PRIMARY KEY, Account CHAR(15), Password CHAR(15), Email CHAR(50), Fullname CHAR(20), Town char(15), State CHAR(15), Points INT DEFAULT 0, MonthlyTop20 INT DEFAULT 0, Orbs INT DEFAULT 0, Assists INT DEFAULT 0, Deaths INT DEFAULT 0, Member INT DEFAULT 0, IsAdmin INT DEFAULT 0);");
		Database.execDML("CREATE INDEX idxPoints ON tAccounts (Points DESC);");
		Database.execDML("CREATE UNIQUE INDEX idxName ON tAccounts (Account);");

		//Bans Table
		cout << "Database::Create::Bans" << endl;
		Database.execDML("CREATE TABLE tBans(BanID INT PRIMARY KEY, Account CHAR(15), IPAddress CHAR(15), Reason CHAR(30), UniqID CHAR(50));");

		cout << "Database::Create::Success" << endl << endl;
	}
}

void CDatabase::CleanDatabase()
{
	cout << "Database::Clean" << endl;

}