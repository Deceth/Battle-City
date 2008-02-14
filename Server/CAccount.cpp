#include "CAccount.h"

CAccount::CAccount(CServer *Server)
{
	p = Server;
}

CAccount::~CAccount()
{

}

int CAccount::CheckAccount(string account)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
		QueryString += account;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 0;
		}
		else
		{
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

int CAccount::CheckEmail(string email)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Email LIKE '";
		QueryString += email;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 0;
		}
		else
		{
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

int CAccount::CompareEmail(string email, int Index)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
		QueryString += p->Player[Index]->Name;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
			if (p->PlatformCaseCompare(p->Database->Query.getStringField("Email"), email.c_str()) == 0)
			{
				p->Database->Query.finalize();
				return 1;
			}
			else
			{
				p->Database->Query.finalize();
				return 0;
			}
		}
		else
		{
			p->Database->Query.finalize();
			return 0;
		}
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

int CAccount::CheckPassword(string account, string password)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
		QueryString += account;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
			if (p->PlatformCaseCompare(p->Database->Query.getStringField("Password"), password.c_str()) == 0) 
			{
				p->Database->Query.finalize();
				return 2;
			}
		}

		p->Database->Query.finalize();
		return 0;
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

int CAccount::NewAccount(string account, string password, string email, string fullname, string town, string state)
{
	if (this->CheckAccount(account) == 0)
	{
		if (this->CheckEmail(email) == 0)
		{
			try
			{
				string DMLString;
				DMLString = "INSERT INTO tAccounts (Account, Password, Email, Fullname, Town, State) VALUES ('";
				DMLString += account;
				DMLString += "', '";
				DMLString += password;
				DMLString += "', '";
				DMLString += email;
				DMLString += "', '";
				DMLString += fullname;
				DMLString += "', '";
				DMLString += town;
				DMLString += "', '";
				DMLString += state;
				DMLString += "');";
				p->Database->Database.execDML(DMLString.c_str());
				cout << "NewAccount::" << account << endl;

				p->EMail->SendWelcome(email.c_str());
				return 1;
			}
			catch (CppSQLite3Exception& e)
			{
				cerr << e.errorCode() << ":" << e.errorMessage() << endl;
				return 0;
			}
		}
		else
		{
			return -1;
		}
	}
	else return 0;
}

void CAccount::GenerateTop20()
{
	p->Top20.clear();

	p->Top20 += "BattleCity ";
	p->Top20 += VERSION;
	p->Top20 += ".\r\n\r\n";
	
	string StringUsers;
	stringstream ConvertUsers;
	ConvertUsers << p->TotalPlayers();
	StringUsers = "There are currently ";
	StringUsers += ConvertUsers.str();
	StringUsers += " players on the battlefield!\r\n\r\n";
	p->Top20 += StringUsers;

	p->Top20 += p->News;

	string QueryString;
	
	// Monthly Top 20
	p->Top20 += "Monthly Top 20 for ";

	char tempstring[100];
	struct tm *ptr;
	time_t tm;
	tm = time(NULL);
	ptr = localtime(&tm);
	strftime(tempstring, 100, "%B", ptr);
	p->Top20 += tempstring;
	QueryString = "SELECT * FROM tAccounts ORDER BY MonthlyTop20 DESC LIMIT 20";
	p->Top20 += "\r\n";
	p->Top20 += "\r\n";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	string TempTop20;
	int Top20Pos = 1;

	while (!p->Database->Query.eof())
	{
		string Top20Generate;
		stringstream Convert;
		Convert << Top20Pos;

		int Points = p->Database->Query.getIntField("MonthlyTop20");
		Top20Generate = Convert.str();
		Top20Generate += ". ";
		Top20Generate += p->Account->ReturnRank(p->Database->Query.getIntField("Points"));
		Top20Generate += " ";			
		Top20Generate += p->Database->Query.getStringField("Account");
		Top20Generate += " (";
		stringstream Convert2;
		Convert2.clear();
		Convert2 << Points;
		Top20Generate += Convert2.str();
		Top20Generate += ")";

		TempTop20 += Top20Generate;
		TempTop20 += "\r\n";

		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	p->Database->Query.finalize();
	p->Top20 += TempTop20;
	p->Top20 += "\r\n";
	//



	QueryString = "SELECT * FROM tAccounts ORDER BY Points DESC LIMIT 20";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	TempTop20.clear();
	Top20Pos = 1;
	p->Top20 += "Overall Top 20:";
	p->Top20 += "\r\n";
	p->Top20 += "\r\n";

	while (!p->Database->Query.eof())
	{
		string Top20Generate;
		stringstream Convert;
		Convert << Top20Pos;

		int Points = p->Database->Query.getIntField("Points");
		Top20Generate = Convert.str();
		Top20Generate += ". ";
		Top20Generate += p->Account->ReturnRank(Points);
		Top20Generate += " ";			
		Top20Generate += p->Database->Query.getStringField("Account");
		Top20Generate += " (";
		stringstream Convert2;
		Convert2.clear();
		Convert2 << Points;
		Top20Generate += Convert2.str();
		Top20Generate += ")";

		TempTop20 += Top20Generate;
		TempTop20 += "\r\n";

		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	p->Database->Query.finalize();

	p->Top20 += TempTop20;
	p->Top20 += "\r\n";



	QueryString = "SELECT * FROM tAccounts WHERE Deaths > 100 ORDER BY (CAST(Points as float)/CAST(Deaths as float)) DESC LIMIT 20";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	TempTop20.clear();
	Top20Pos = 1;
	p->Top20 += "Top 20 Points Per Death:";
	p->Top20 += "\r\n";
	p->Top20 += "\r\n";

	while (!p->Database->Query.eof())
	{
		string Top20Generate;
		stringstream Convert;
		Convert << Top20Pos;

		float Points = p->Database->Query.getIntField("Points");
		float Deaths = p->Database->Query.getIntField("Deaths");
		Top20Generate = Convert.str();
		Top20Generate += ". ";
		Top20Generate += p->Account->ReturnRank(Points);
		Top20Generate += " ";			
		Top20Generate += p->Database->Query.getStringField("Account");
		Top20Generate += " (";
		stringstream Convert2;
		Convert2.clear();
		Convert2 << Points/Deaths;
		Top20Generate += Convert2.str();
		Top20Generate += ")";

		TempTop20 += Top20Generate;
		TempTop20 += "\r\n";

		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	p->Database->Query.finalize();

	p->Top20 += TempTop20;
	p->Top20 += "\r\n";

	cout << "Top20::Updated" << endl;

	if (this->MonthHasChanged() == true)
	{
		cout << "Month has changed -- Top 20 is being rebuilt." << endl;
		string DMLString;
		DMLString = "UPDATE tAccounts SET MonthlyTop20 = 0;";
		p->Database->Database.execDML(DMLString.c_str());

		ofstream Top20File ("finalmonthlytop20.txt");
		Top20File.write(p->Top20.c_str(), (int)p->Top20.length());
		Top20File.close();

		ofstream MonthFile ("month.txt");

		char tempstring[100];
		memset(tempstring, 0, sizeof(tempstring));
		struct tm *ptr;
		time_t tm;
		tm = time(NULL);
		ptr = localtime(&tm);
		strftime(tempstring, 100, "%B", ptr);

		MonthFile.write(tempstring, (int)strlen(tempstring));
		MonthFile.close();

		for (int i = 0; i < MaxPlayers; i++)
		{
			p->Player[i]->MonthlyPoints = 0;
		}

		GenerateTop20();
	}
}

bool CAccount::MonthHasChanged()
{
	char tempstring[100];
	struct tm *ptr;
	time_t tm;
	tm = time(NULL);
	ptr = localtime(&tm);
	strftime(tempstring, 100, "%B", ptr);
	
	char buffer[256];
	ifstream MonthFile ("month.txt");
	if (! MonthFile.is_open()) return true;

	MonthFile.getline (buffer,100);
	if (strcmp(buffer, tempstring) == 0)
	{
		return false;
	}

	return true;
}

string CAccount::ReturnRank(int Points)
{
	string rank;

	if (Points < 100)
		rank = "Private";
	if (Points >= 100 && Points < 200)
		rank = "Corporal";
	if (Points >= 200 && Points < 500)
		rank = "Sergeant";
	if (Points >= 500 && Points < 1000)
		rank = "Sergeant Major";
	if (Points >= 1000 && Points < 2000)
		rank = "Lieutenant";
	if (Points >= 2000 && Points < 4000)
		rank = "Captain";
	if (Points >= 4000 && Points < 8000)
		rank = "Major";
	if (Points >= 8000 && Points < 16000)
		rank = "Colonel";
	if (Points >= 15000 && Points < 30000)
		rank = "Brigadier";
	if (Points >= 30000 && Points < 45000)
		rank = "General";
	if (Points >= 45000 && Points < 60000)
		rank = "Baron";
	if (Points >= 60000 && Points < 80000)
		rank = "Earl";
	if (Points >= 80000 && Points < 100000)
		rank = "Count";
	if (Points >= 100000 && Points < 125000)
		rank = "Duke";
	if (Points >= 125000 && Points < 150000)
		rank = "Archduke";
	if (Points >= 150000 && Points < 200000)
		rank = "Grand Duke";
	if (Points >= 200000 && Points < 250000)
		rank = "Lord";
	if (Points >= 250000 && Points < 300000)
		rank = "Chancellor";
	if (Points >= 300000 && Points < 350000)
		rank = "Royaume";
	if (Points >= 350000 && Points < 400000)
		rank = "Emperor";
	if (Points >= 400000 && Points < 500000)
		rank = "Auror";
	if (Points >= 500000)
		rank = "King";

	return rank;
}

void CAccount::AddPoints(int Index, int points)
{
	p->Player[Index]->Points += points;
	p->Player[Index]->MonthlyPoints += points;

	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

void CAccount::Sub2Points(int Index)
{
	p->Player[Index]->Points -= 2;
	p->Player[Index]->MonthlyPoints -= 2;
	if (p->Player[Index]->Points < 0) p->Player[Index]->Points = 0;
	if (p->Player[Index]->MonthlyPoints < 0) p->Player[Index]->MonthlyPoints = 0;

	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

int CAccount::GetMonthlyTop20(int Index)
{
	string QueryString;
	QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
	QueryString += p->Player[Index]->Name;
	QueryString += "';";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	if (!p->Database->Query.eof())
	{
		int i = p->Database->Query.getIntField("MonthlyTop20");
		p->Database->Query.finalize();
		return i;
	}

	p->Database->Query.finalize();
	return 0;
}


int CAccount::CheckBan(string UniqID, string IPAddress)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tBans WHERE UniqID = '";
		QueryString += UniqID;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 1;
		}

		p->Database->Query.finalize();

		QueryString = "SELECT * FROM tBans WHERE IPAddress = '";
		QueryString += IPAddress;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 1;
		}

		p->Database->Query.finalize();

		return 0;
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 1;
    }
}

int CAccount::CheckAccountBan(string account)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tBans WHERE Account LIKE '";
		QueryString += account;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 1;
		}

		p->Database->Query.finalize();

		return 0;
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 1;
    }
}

void CAccount::SendAccountInformation(int Index)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
		QueryString += p->Player[Index]->Name;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (!p->Database->Query.eof())
		{
				char LoginData[240];
				memset(LoginData, 0, 240);

				char tmpUser[16];
				char tmpPass[16];
				char tmpEmail[51];
				char tmpFullName[21];
				char tmpTown[16];
				char tmpState[16];
				memset(tmpUser, 0, 16);
				memset(tmpPass, 0, 16);
				memset(tmpEmail, 0, 51);
				memset(tmpFullName, 0, 21);
				memset(tmpTown, 0, 16);
				memset(tmpState, 0, 16);

				strcpy(tmpPass, p->Database->Query.getStringField("Password"));
				strcpy(tmpUser, p->Database->Query.getStringField("Account"));
				strcpy(tmpTown, p->Database->Query.getStringField("Town"));
				strcpy(tmpState, p->Database->Query.getStringField("State"));
				strcpy(tmpEmail, p->Database->Query.getStringField("Email"));
				strcpy(tmpFullName, p->Database->Query.getStringField("Fullname"));
				for (int k = 0; k < 15; k++)
				{
					if (tmpUser[k] == 0) tmpUser[k] = 1;
					if (tmpPass[k] == 0) tmpPass[k] = 1;
					if (tmpTown[k] == 0) tmpTown[k] = 1;
					if (tmpState[k] == 0) tmpState[k] = 1;
				}
				for (int k = 0; k < 20; k++)
				{
					if (tmpFullName[k] == 0) tmpFullName[k] = 1;
				}
				for (int k = 0; k < 50; k++)
				{
					if (tmpEmail[k] == 0) tmpEmail[k] = 1;
				}

				strcpy(LoginData, tmpUser);
				strcpy(&LoginData[strlen(LoginData)], tmpPass);
				strcpy(&LoginData[strlen(LoginData)], tmpEmail);
				strcpy(&LoginData[strlen(LoginData)], tmpFullName);
				strcpy(&LoginData[strlen(LoginData)], tmpTown);
				strcpy(&LoginData[strlen(LoginData)], tmpState);

				p->Winsock->SendData(Index, smEditAccount, LoginData);
		}

		p->Database->Query.finalize();
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
}

int CAccount::UpdateAccount(int Index, string password, string email, string fullname, string town, string state)
{
	if (this->CheckAccount(p->Player[Index]->Name) == 1)
	{
		try
		{
			string DMLString;
			DMLString = "UPDATE tAccounts SET Password = '";
			DMLString += password;
			DMLString += "', Fullname = '";
			DMLString += fullname;
			DMLString += "', Email = '";
			DMLString += email;
			DMLString += "', Town = '";
			DMLString += town;
			DMLString += "', State = '";
			DMLString += state;
			DMLString += "' WHERE Account LIKE '";
			DMLString += p->Player[Index]->Name;
			DMLString += "';";
			p->Database->Database.execDML(DMLString.c_str());
			cout << "UpdateAccount::" << p->Player[Index]->Name << endl;

			return 1;
		}
		catch (CppSQLite3Exception& e)
		{
			cerr << e.errorCode() << ":" << e.errorMessage() << endl;
			return 0;
		}
	}
	else return 0;
}

int CAccount::RecoverAccount(int Index, string Email)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Email LIKE '";
		QueryString += Email;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return 0;
		}
		else
		{
			p->EMail->SendPassword(Email, p->Database->Query.getStringField("Account"), p->Database->Query.getStringField("Password"));
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

void CAccount::GetLoginData(int Index)
{
	try
	{
		string QueryString;
		QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
		QueryString += p->Player[Index]->Name;
		QueryString += "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		if (p->Database->Query.eof())
		{
			p->Database->Query.finalize();
			return;
		}
		else
		{
			p->Player[Index]->isAdmin = p->Database->Query.getIntField("IsAdmin");
			p->Player[Index]->Town = p->Database->Query.getStringField("Town");
			p->Player[Index]->Tank = p->Database->Query.getIntField("Tank");
			p->Player[Index]->Tank2 = p->Database->Query.getIntField("Tank2");
			p->Player[Index]->Tank3 = p->Database->Query.getIntField("Tank3");
			p->Player[Index]->Tank4 = p->Database->Query.getIntField("Tank4");
			p->Player[Index]->Red = p->Database->Query.getIntField("Red");
			p->Player[Index]->Green = p->Database->Query.getIntField("Green");
			p->Player[Index]->Blue = p->Database->Query.getIntField("Blue");
			p->Player[Index]->Member = p->Database->Query.getIntField("Member");
			p->Player[Index]->RentalCity = p->Database->Query.getIntField("RentalCity");
			p->Database->Query.finalize();
			return;
		}
	}
    catch (CppSQLite3Exception& e)
    {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return;
    }
}

void CAccount::AddDeath(int Index)
{
	p->Player[Index]->Deaths++;

	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

void CAccount::AddOrb(int Index)
{
	p->Player[Index]->Orbs++;

	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

void CAccount::AddAssist(int Index)
{
	p->Player[Index]->Assists++;

	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

int CAccount::GetDeaths(int Index)
{
	string QueryString;
	QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
	QueryString += p->Player[Index]->Name;
	QueryString += "';";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	if (!p->Database->Query.eof())
	{
		int i = p->Database->Query.getIntField("Deaths");
		p->Database->Query.finalize();
		return i;
	}

	p->Database->Query.finalize();
	return 0;
}

int CAccount::GetOrbs(int Index)
{
	string QueryString;
	QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
	QueryString += p->Player[Index]->Name;
	QueryString += "';";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	if (!p->Database->Query.eof())
	{
		int i = p->Database->Query.getIntField("Orbs");
		p->Database->Query.finalize();
		return i;
	}

	p->Database->Query.finalize();
	return 0;
}

int CAccount::GetAssists(int Index)
{
	string QueryString;
	QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
	QueryString += p->Player[Index]->Name;
	QueryString += "';";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	if (!p->Database->Query.eof())
	{
		int i = p->Database->Query.getIntField("Assists");
		p->Database->Query.finalize();
		return i;
	}

	p->Database->Query.finalize();
	return 0;
}

void CAccount::GetStats(int Index)
{
	string QueryString;
	QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
	QueryString += p->Player[Index]->Name;
	QueryString += "';";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	if (!p->Database->Query.eof())
	{
		p->Player[Index]->Points = p->Database->Query.getIntField("Points");
		p->Player[Index]->MonthlyPoints = p->Database->Query.getIntField("MonthlyTop20");
		p->Player[Index]->Orbs = p->Database->Query.getIntField("Orbs");
		p->Player[Index]->Assists = p->Database->Query.getIntField("Assists");
		p->Player[Index]->Deaths = p->Database->Query.getIntField("Deaths");
		p->Database->Query.finalize();
	}

	p->Database->Query.finalize();
}

void CAccount::SaveStats(int Index)
{
	string DMLString;

	DMLString = "UPDATE tAccounts SET Deaths = ";
	std::ostringstream DeathsConvert;
	DeathsConvert << p->Player[Index]->Deaths;
	DMLString += DeathsConvert.str();

	DMLString += ", Orbs = ";
	std::ostringstream OrbsConvert;
	OrbsConvert << p->Player[Index]->Orbs;
	DMLString += OrbsConvert.str();

	DMLString += ", Assists = ";
	std::ostringstream AssistsConvert;
	AssistsConvert << p->Player[Index]->Assists;
	DMLString += AssistsConvert.str();

	DMLString += ", Points = ";
	std::ostringstream PointsConvert;
	PointsConvert << p->Player[Index]->Points;
	DMLString += PointsConvert.str();

	DMLString += ", MonthlyTop20 = ";
	std::ostringstream MonthlyConvert;
	MonthlyConvert << p->Player[Index]->MonthlyPoints;
	DMLString += MonthlyConvert.str();

	DMLString += " WHERE Account LIKE '";
	DMLString += p->Player[Index]->Name;
	DMLString += "';";
	p->Database->Database.execDML(DMLString.c_str());
}