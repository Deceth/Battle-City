/*
===============================================================================

    Battle City - CAccount file
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
#include "CAccount.h"

/***************************************************************
 * Constructor:	CBuilding
 *
 * @param Server
 **************************************************************/
CAccount::CAccount(CServer *Server) {
	p = Server;
}

/***************************************************************
 * Destructor:	CBuilding
 *
 **************************************************************/
CAccount::~CAccount() {

}

/***************************************************************
 * Function:	CheckAccount
 *
 * @param account
 **************************************************************/
int CAccount::CheckAccount(string account) {
	try {
		// Select the account from the database, matching by account name
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + account + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		
		// If the result set has no rows, return 0
		if (p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 0;
		}
		// Else, return 1
		else {
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e) {
		cerr << "CAccount::CheckAccount : " << e.errorCode() << " : " << e.errorMessage() << endl;
		return 0;
    }
}

/***************************************************************
 * Function:	CheckEmail
 *
 * @param email
 **************************************************************/
int CAccount::CheckEmail(string email) {
	try {
		// Select the account from the database, matching by email address
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Email) = lower('" + email + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If the result set has no rows, return 0
		if (p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 0;
		}
		// Else, return 1
		else {
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

/***************************************************************
 * Function:	CompareEmail
 *
 * @param email
 * @param Index
 **************************************************************/
int CAccount::CompareEmail(string email, int Index) {
	try {
		// Select the account from the database, matching by name
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If the result set has any rows,
		if (!p->Database->Query.eof()) {

			// If the Email field matches the player's email, return 1
			if (p->PlatformCaseCompare(p->Database->Query.getStringField("Email"), email.c_str()) == 0) {
				p->Database->Query.finalize();
				return 1;
			}
		}

		// Else (no match), return 0
		p->Database->Query.finalize();
		return 0;
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

/***************************************************************
 * Function:	CheckPassword
 *
 * @param account
 * @param password
 **************************************************************/
int CAccount::CheckPassword(string account, string password) {
	try {
		// Select the account from the database, matching by name
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + account + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If the result set has any rows,
		if (!p->Database->Query.eof()) {

			// If the passwords match, return 2
			if (p->PlatformCaseCompare(p->Database->Query.getStringField("Password"), password.c_str()) == 0)  {
				p->Database->Query.finalize();
				return 2;
			}
		}

		// Else (no match), return 0
		p->Database->Query.finalize();
		return 0;
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

/***************************************************************
 * Function:	NewAccount
 *
 * @param account
 * @param password
 * @param email
 * @param fullname
 * @param town
 * @param state
 **************************************************************/
int CAccount::NewAccount(string account, string password, string email, string fullname, string town, string state) {

	// If the account name isn't already in use,
	if (this->CheckAccount(account) == 0) {

		// If the email isn't already in use,
		if (this->CheckEmail(email) == 0) {


			try {
				// Build the insert statement
				string DMLString = "INSERT INTO tAccounts (Account, Password, Email, Fullname, Town, State) ";
				DMLString += "VALUES ('" + account + "', '" + password + "', '" + email +"', '" + fullname + "', '" + town + "', '" + state + "');";

				// Insert the new account
				p->Database->Database.execDML(DMLString.c_str());
				cout << "NewAccount::" << account << endl;

				// Send the welcome email
				p->EMail->SendWelcome(email.c_str());
				return 1;
			}
			catch (CppSQLite3Exception& e) {
				cerr << e.errorCode() << ":" << e.errorMessage() << endl;
				return 0;
			}
		}

		// Else (email is in use), return -1
		else {
			return -1;
		}
	}

	// Else (account name is in use), return 0
	else {
		return 0;
	}
}

/***************************************************************
 * Function:	GenerateTop20
 *
 **************************************************************/
void CAccount::GenerateTop20() {
	string QueryString;
	string TempTop20;
	int Top20Pos = 1;
	stringstream Convert;
	float Points;
	float Deaths;
	char monthString[100];
	time_t tm = time(NULL);
	struct tm *ptr = localtime(&tm);

	/************************************************
	 * Header and News
	 ************************************************/
	// Clear the top 20
	p->Top20.clear();

	// Write the month to monthString
	strftime(monthString, 100, "%B", ptr);

	// Add the version
	p->Top20 += "BattleCity ";
	p->Top20 += VERSION;
	p->Top20 += ".\r\n\r\n";

	// Add the player count
	Convert.str("");
	Convert << p->TotalPlayers();
	p->Top20 += "There are currently " + Convert.str() + " players on the battlefield!\r\n\r\n";

	// Add the news
	p->Top20 += p->News;

	/************************************************
	 * Top 20 - Month
	 ************************************************/
	// Select the top 20 accounts for Monthly Top 20
	QueryString = "SELECT Account, Points, MonthlyTop20 FROM tAccounts WHERE Points AND MonthlyTop20 ORDER BY MonthlyTop20 DESC LIMIT 20";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// Start the Monthly Top 20
	p->Top20 += "Monthly Top 20 for ";
	p->Top20 += monthString;
	p->Top20 += "\r\n\r\n";

	// For each account,
	while (!p->Database->Query.eof()) {

		// Add the monthly position
		Convert.str("");
		Convert << Top20Pos;
		p->Top20 += Convert.str() + ". ";

		// Add the rank and name
		p->Top20 += p->Account->ReturnRank(p->Database->Query.getIntField("Points")) + " " + p->Database->Query.getStringField("Account") + " ";

		// Add the monthly points
		Points = p->Database->Query.getIntField("MonthlyTop20");
		Convert.str("");
		Convert << Points;
		p->Top20 += "(" + Convert.str() + ")";
		p->Top20 += "\r\n";

		// Get the next row
		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	// Finish the Monthly Top 20
	p->Database->Query.finalize();
	p->Top20 += "\r\n";
	
	/************************************************
	 * Top 20 - Overall
	 ************************************************/
	// Select the top 20 accounts for Top 20
	QueryString = "SELECT Account, Points FROM tAccounts WHERE Points ORDER BY Points DESC LIMIT 20";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// Start the Top 20 
	Top20Pos = 1;
	p->Top20 += "Overall Top 20:\r\n\r\n";

	// For each account,
	while (!p->Database->Query.eof()) {

		// Add the monthly position
		Convert.str("");
		Convert << Top20Pos;
		p->Top20 += Convert.str() + ". ";

		// Add the rank and name
		p->Top20 += p->Account->ReturnRank(p->Database->Query.getIntField("Points")) + " " + p->Database->Query.getStringField("Account") + " ";

		// Add the points
		Points = p->Database->Query.getIntField("Points");
		Convert.str("");
		Convert << Points;
		p->Top20 += "(" + Convert.str() + ")";
		p->Top20 += "\r\n";

		// Get the next row
		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	// Finish the Top 20
	p->Database->Query.finalize();
	p->Top20 += "\r\n";

	/************************************************
	 * Top 20 - Points Per Death
	 ************************************************/
	// Select the top 20 accounts for PPD
	QueryString = "SELECT Account, Points, Deaths FROM tAccounts WHERE Points AND Deaths > 100 ORDER BY ((Points*10000)/(Deaths)) DESC LIMIT 20";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// Start the Top 20 PPD
	Top20Pos = 1;
	p->Top20 += "Top 20 Points Per Death:\r\n\r\n";

	// For each account,
	while (!p->Database->Query.eof()) {

		// Add the monthly position
		Convert.str("");
		Convert << Top20Pos;
		p->Top20 += Convert.str() + ". ";

		// Add the rank and name
		p->Top20 += p->Account->ReturnRank(p->Database->Query.getIntField("Points")) + " " + p->Database->Query.getStringField("Account") + " ";

		// Add the PPD
		Points = p->Database->Query.getIntField("Points");
		Deaths = p->Database->Query.getIntField("Deaths");
		Convert.str("");
		Convert << Points/Deaths;
		p->Top20 += "(" + Convert.str() + ")";
		p->Top20 += "\r\n";

		// Get the next row
		Top20Pos += 1;
		p->Database->Query.nextRow();
	}

	// Finish the Top 20 PPD
	p->Database->Query.finalize();
	p->Top20 += "\r\n";

	cout << "Top20::Updated" << endl;

	// If the month has changed,
	if (this->MonthHasChanged() == true) {
		cout << "Month has changed -- Top 20 is being rebuilt." << endl;

		// Update all monthly points to 0
		QueryString = "UPDATE tAccounts SET MonthlyTop20 = 0;";
		p->Database->Database.execDML(QueryString.c_str());

		// Update finalmonthlytop20.txt
		ofstream Top20File ("finalmonthlytop20.txt");
		Top20File.write(p->Top20.c_str(), (int)p->Top20.length());
		Top20File.close();

		// Write the monthString to month.txt
		ofstream MonthFile ("month.txt");
		MonthFile.write(monthString, (int)strlen(monthString));
		MonthFile.close();

		// Reset the monthly points of every player in game
		for (int i = 0; i < MAX_PLAYERS; i++) {
			p->Player[i]->MonthlyPoints = 0;
		}

		// Re-run the Top20
		GenerateTop20();
	}
}

/***************************************************************
 * Function:	MonthHasChanged
 *
 **************************************************************/
bool CAccount::MonthHasChanged() {
	char monthString[100];
	struct tm *ptr;
	time_t tm;
	tm = time(NULL);
	ptr = localtime(&tm);
	char buffer[256];

	// Write the month to monthString
	strftime(monthString, 100, "%B", ptr);

	// Open the file
	ifstream MonthFile ("month.txt");

	// If the file can't be opened (missing), return true
	if (! MonthFile.is_open()) {
		return true;
	}

	// Read the first line of MonthFile into buffer
	MonthFile.getline (buffer,100);

	// If buffer does not equal monthString, return 0
	if (strcmp(buffer, monthString) == 0) {
		return false;
	}

	// Else (buffer equals monthString), return true
	return true;
}

/***************************************************************
 * Function:	ReturnRank
 *
 * @param Points
 **************************************************************/
string CAccount::ReturnRank(int Points) {
	string rank;

	if (Points < 100) {
		rank = "Private";
	}
	else if (Points < 200) {
		rank = "Corporal";
	}
	else if (Points < 500) {
		rank = "Sergeant";
	}
	else if (Points < 1000) {
		rank = "Sergeant Major";
	}
	else if (Points < 2000) {
		rank = "Lieutenant";
	}
	else if (Points < 4000) {
		rank = "Captain";
	}
	else if (Points < 8000) {
		rank = "Major";
	}
	else if (Points < 16000) {
		rank = "Colonel";
	}
	else if (Points < 30000) {
		rank = "Brigadier";
	}
	else if (Points < 45000) {
		rank = "General";
	}
	else if (Points < 60000) {
		rank = "Baron";
	}
	else if (Points < 80000) {
		rank = "Earl";
	}
	else if (Points < 100000) {
		rank = "Count";
	}
	else if (Points < 125000) {
		rank = "Duke";
	}
	else if (Points < 150000) {
		rank = "Archduke";
	}
	else if (Points < 200000) {
		rank = "Grand Duke";
	}
	else if (Points < 250000) {
		rank = "Lord";
	}
	else if (Points < 300000) {
		rank = "Chancellor";
	}
	else if (Points < 350000) {
		rank = "Royaume";
	}
	else if (Points < 400000) {
		rank = "Emperor";
	}
	else if (Points < 500000) {
		rank = "Auror";
	}
	else {
		rank = "King";
	}

	return rank;
}

/***************************************************************
 * Function:	AddPoints
 *
 * @param Index
 * @param points
 **************************************************************/
void CAccount::AddPoints(int Index, int points) {
	// Check if the point addition leads to a promotion
	string oldRank = ReturnRank(p->Player[Index]->Points);
	string newRank = ReturnRank(p->Player[Index]->Points + points);
	if (oldRank == newRank) {
		// No promotion, do nothing!
	}
	else {
		char tmpstring[255];
		int length = newRank.length();
		tmpstring[0] = (unsigned char)Index;
		strcpy(&tmpstring[1], newRank.c_str());		
		p->Send->SendGameAll(Index, smPromotion, tmpstring, length + 1);
	}

	// Add points to the players's Points and MonthlyPoints values
	p->Player[Index]->Points += points;
	p->Player[Index]->MonthlyPoints += points;

	// Send an update to all players on the server
	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

/***************************************************************
 * Function:	Sub2Points
 *
 * @param Index
 **************************************************************/
void CAccount::Sub2Points(int Index) {
	// Subtract 2 points from the players's Points and MonthlyPoints values
	p->Player[Index]->Points -= 2;
	p->Player[Index]->MonthlyPoints -= 2;

	// If either value is now negative, set the value to 0
	if (p->Player[Index]->Points < 0) {
		p->Player[Index]->Points = 0;
	}
	if (p->Player[Index]->MonthlyPoints < 0) {
		p->Player[Index]->MonthlyPoints = 0;
	}

	// Send an update to all players on the server
	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

/***************************************************************
 * Function:	GetMonthlyTop20
 *
 * @param Index
 **************************************************************/
int CAccount::GetMonthlyTop20(int Index) {
	// Select the account from the database, matching by name
	string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	
	// If the player is found, return the player's monthly points
	if (!p->Database->Query.eof()) {
		int i = p->Database->Query.getIntField("MonthlyTop20");
		p->Database->Query.finalize();
		return i;
	}

	// Else (player not found), return 0
	p->Database->Query.finalize();
	return 0;
}

/***************************************************************
 * Function:	CheckBan
 *
 * @param UniqID
 * @param IPAddress
 **************************************************************/
int CAccount::CheckBan(string UniqID, string IPAddress) {
	try {
		// Select the bans from the database, matching by ID
		string QueryString = "SELECT * FROM tBans WHERE UniqID = '" + UniqID + "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If a ban was found for this ID, return 1
		if (!p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 1;
		}

		// Else, continue with the method
		p->Database->Query.finalize();

		// Select the bans from the database, matching by IP
		QueryString = "SELECT * FROM tBans WHERE IPAddress = '" + IPAddress + "';";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If a ban was found for this ID, return 1
		if (!p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 1;
		}

		// Else (no bans found), return 0
		p->Database->Query.finalize();
		return 0;
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 1;
    }
}

/***************************************************************
 * Function:	CheckAccountBan
 *
 * @param account
 **************************************************************/
int CAccount::CheckAccountBan(string account) {
	try {
		// Select the bans from the database, matching by account name
		string QueryString = "SELECT * FROM tBans WHERE lower(Account) = lower('" + account + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If a ban was found for this account name, return 1
		if (!p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 1;
		}

		// Else (no ban found), return 0
		p->Database->Query.finalize();
		return 0;
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 1;
    }
}

/***************************************************************
 * Function:	SendAccountInformation
 *
 * @param Index
 **************************************************************/
void CAccount::SendAccountInformation(int Index) {
	try {
		// Select the account from the database, matching by account name
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If an account was found,
		if (!p->Database->Query.eof()) {
			char LoginData[240];
			char tmpUser[16];
			char tmpPass[16];
			char tmpEmail[51];
			char tmpFullName[21];
			char tmpTown[16];
			char tmpState[16];

			memset(LoginData, 0, 240);
			memset(tmpUser, 0, 16);
			memset(tmpPass, 0, 16);
			memset(tmpEmail, 0, 51);
			memset(tmpFullName, 0, 21);
			memset(tmpTown, 0, 16);
			memset(tmpState, 0, 16);

			// Copy the account information into temporary char[]s
			strcpy(tmpPass, p->Database->Query.getStringField("Password"));
			strcpy(tmpUser, p->Database->Query.getStringField("Account"));
			strcpy(tmpTown, p->Database->Query.getStringField("Town"));
			strcpy(tmpState, p->Database->Query.getStringField("State"));
			strcpy(tmpEmail, p->Database->Query.getStringField("Email"));
			strcpy(tmpFullName, p->Database->Query.getStringField("Fullname"));

			// Replace all 0s with 1s
			for (int k = 0; k < 15; k++) {
				if (tmpUser[k] == 0) {
					tmpUser[k] = 1;
				}
				if (tmpPass[k] == 0) {
					tmpPass[k] = 1;
				}
				if (tmpTown[k] == 0) {
					tmpTown[k] = 1;
				}
				if (tmpState[k] == 0) {
					tmpState[k] = 1;
				}
			}
			for (int k = 0; k < 20; k++) {
				if (tmpFullName[k] == 0) {
					tmpFullName[k] = 1;
				}
			}
			for (int k = 0; k < 50; k++) {
				if (tmpEmail[k] == 0) {
					tmpEmail[k] = 1;
				}
			}

			// Copy the temp char[]s into LoginData
			strcpy(LoginData, tmpUser);
			strcpy(&LoginData[strlen(LoginData)], tmpPass);
			strcpy(&LoginData[strlen(LoginData)], tmpEmail);
			strcpy(&LoginData[strlen(LoginData)], tmpFullName);
			strcpy(&LoginData[strlen(LoginData)], tmpTown);
			strcpy(&LoginData[strlen(LoginData)], tmpState);

			// Send LoginData
			p->Winsock->SendData(Index, smEditAccount, LoginData);
		}

		p->Database->Query.finalize();
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
    }
}

/***************************************************************
 * Function:	UpdateAccount
 *
 * @param Index
 * @param password
 * @param email
 * @param fullname
 * @param town
 * @param state
 **************************************************************/
int CAccount::UpdateAccount(int Index, string password, string email, string fullname, string town, string state) {
	string QueryString;

	// If an account with this name exists,
	if (this->CheckAccount(p->Player[Index]->Name) == 1) {
		
		try {
			// Build the update statement
			cout << "Saving account: " << p->Player[Index]->Name << endl;

			QueryString = "UPDATE tAccounts SET ";
			QueryString += "Password = '" + password + "', ";
			QueryString += "Fullname = '" + fullname + "', ";
			QueryString += "Email = '" + email + "', ";
			QueryString += "Town = '" + town + "', ";
			QueryString += "State = '" + state + "' ";
			QueryString += "WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";

			// Execute the update statement, return 1
			p->Database->Database.execDML(QueryString.c_str());
			cout << "UpdateAccount::" << p->Player[Index]->Name << endl;
			return 1;
		}
		catch (CppSQLite3Exception& e) {
			cerr << e.errorCode() << ":" << e.errorMessage() << endl;
			return 0;
		}
	}
	
	// Else (no account found), return 0
	return 0;
}

/***************************************************************
 * Function:	RecoverAccount
 *
 * @param Index
 * @param Email
 **************************************************************/
int CAccount::RecoverAccount(int Index, string Email) {
	try {
		// Select the account from the database, matching by email
		string QueryString = "SELECT * FROM tAccounts WHERE lower(Email) = lower('" + Email + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

		// If no account was found, return 0
		if (p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return 0;
		}
		// Else (account was found), sent the password
		else {
			p->EMail->SendPassword(Email, p->Database->Query.getStringField("Account"), p->Database->Query.getStringField("Password"));
			p->Database->Query.finalize();
			return 1;
		}
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return 0;
    }
}

/***************************************************************
 * Function:	GetLoginData
 *
 * @param Index
 **************************************************************/
void CAccount::GetLoginData(int Index) {
	CPlayer* player = this->p->Player[Index];

	try {
		// Select the account from the database, matching by account name
		string QueryString= "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + player->Name + "');";
		p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
		
		// If no account was found, return
		if (p->Database->Query.eof()) {
			p->Database->Query.finalize();
			return;
		}

		// Else (account was found),
		else {
			// Set the login data on the player, then return
			player->playerType = p->Database->Query.getIntField("IsAdmin");
			player->Town = p->Database->Query.getStringField("Town");
			player->Tank = p->Database->Query.getIntField("Tank");
			player->Tank2 = p->Database->Query.getIntField("Tank2");
			player->Tank3 = p->Database->Query.getIntField("Tank3");
			player->Tank4 = p->Database->Query.getIntField("Tank4");
			player->Tank5 = p->Database->Query.getIntField("Tank5");
			player->Tank6 = p->Database->Query.getIntField("Tank6");
			player->Tank7 = p->Database->Query.getIntField("Tank7");
			player->Tank8 = p->Database->Query.getIntField("Tank8");
			player->Tank9 = p->Database->Query.getIntField("Tank9");
			player->Red = p->Database->Query.getIntField("Red");
			player->Green = p->Database->Query.getIntField("Green");
			player->Blue = p->Database->Query.getIntField("Blue");
			player->Member = p->Database->Query.getIntField("Member");
			player->RentalCity = p->Database->Query.getIntField("RentalCity");
			p->Database->Query.finalize();

			// Set displayTank to Tank
			player->displayTank = player->Tank;

			// HACK:
			// If the player's rental city is 0, set it to -1
			// This is done because players default to RentalCity 0, which is Balkh
			// The long-term fix should be update tAccounts set RentalCity = -1 where RentalCity = 0
			if (player->RentalCity == 0) {
				player->RentalCity = -1;
			}
			return;
		}
	}
    catch (CppSQLite3Exception& e) {
        cerr << e.errorCode() << ":" << e.errorMessage() << endl;
		return;
    }
}

/***************************************************************
 * Function:	AddDeath
 *
 * @param Index
 **************************************************************/
void CAccount::AddDeath(int Index) {
	// Add a death to the player
	p->Player[Index]->Deaths++;

	// Send an update to all players on the server
	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}


/***************************************************************
 * Function:	AddOrb
 *
 * @param Index
 **************************************************************/
void CAccount::AddOrb(int Index) {
	// Add an orb to the player
	p->Player[Index]->Orbs++;

	// Send an update to all players on the server
	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

/***************************************************************
 * Function:	AddAssist
 *
 * @param Index
 **************************************************************/
void CAccount::AddAssist(int Index) {
	// Add an assist to the player
	p->Player[Index]->Assists++;

	// Send an update to all players on the server
	sSMPoints pts;
	pts.Index = Index;
	pts.Points = p->Player[Index]->Points;
	pts.Deaths = p->Player[Index]->Deaths;
	pts.Assists = p->Player[Index]->Assists;
	pts.Orbs = p->Player[Index]->Orbs;
	pts.MonthlyPoints = p->Player[Index]->MonthlyPoints;
	p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));
}

/***************************************************************
 * Function:	GetDeaths
 *
 * @param Index
 **************************************************************/
int CAccount::GetDeaths(int Index) {
	// Select the account from the database, matching by account name
	string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// If the account was found, return the Deaths column
	if (!p->Database->Query.eof()) {
		int i = p->Database->Query.getIntField("Deaths");
		p->Database->Query.finalize();
		return i;
	}

	// Else (no account found), return 0
	p->Database->Query.finalize();
	return 0;
}

/***************************************************************
 * Function:	GetOrbs
 *
 * @param Index
 **************************************************************/
int CAccount::GetOrbs(int Index) {
	// Select the account from the database, matching by account name
	string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// If the account was found, return the Orbs column
	if (!p->Database->Query.eof()) {
		int i = p->Database->Query.getIntField("Orbs");
		p->Database->Query.finalize();
		return i;
	}

	// Else (no account found), return 0
	p->Database->Query.finalize();
	return 0;
}

/***************************************************************
 * Function:	GetAssists
 *
 * @param Index
 **************************************************************/
int CAccount::GetAssists(int Index) {
	// Select the account from the database, matching by account name
	string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
	
	// If the account was found, return the Assists column
	if (!p->Database->Query.eof()) {
		int i = p->Database->Query.getIntField("Assists");
		p->Database->Query.finalize();
		return i;
	}

	// Else (account not found), return 0
	p->Database->Query.finalize();
	return 0;
}

/***************************************************************
 * Function:	GetStats
 *
 * @param Index
 **************************************************************/
void CAccount::GetStats(int Index) {
	// Select the account from the database, matching by account name
	string QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";
	p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

	// If the account was found, set the stats on the players
	if (!p->Database->Query.eof()) {
		p->Player[Index]->Points = p->Database->Query.getIntField("Points");
		p->Player[Index]->MonthlyPoints = p->Database->Query.getIntField("MonthlyTop20");
		p->Player[Index]->Orbs = p->Database->Query.getIntField("Orbs");
		p->Player[Index]->Assists = p->Database->Query.getIntField("Assists");
		p->Player[Index]->Deaths = p->Database->Query.getIntField("Deaths");
		p->Database->Query.finalize();
	}

	p->Database->Query.finalize();
}

/***************************************************************
 * Function:	SaveStats
 *
 * @param Index
 **************************************************************/
void CAccount::SaveStats(int Index) {
	string QueryString;
	std::ostringstream Converter;

	// Build the update statement
	QueryString = "UPDATE tAccounts SET ";
	
	Converter.str("");
	Converter << p->Player[Index]->Deaths;
	QueryString += "Deaths = " + Converter.str() + ", ";

	Converter.str("");
	Converter << p->Player[Index]->Orbs;
	QueryString += "Orbs = " + Converter.str() + ", ";

	Converter.str("");
	Converter << p->Player[Index]->Assists;
	QueryString += "Assists = " + Converter.str() + ", ";

	Converter.str("");
	Converter << p->Player[Index]->Points;
	QueryString += "Points = " + Converter.str() + ", ";

	Converter.str("");
	Converter << p->Player[Index]->MonthlyPoints;
	QueryString += "MonthlyTop20 = " + Converter.str() + " ";

	QueryString += " WHERE lower(Account) = lower('" + p->Player[Index]->Name + "');";

	// Execute the update statement
	p->Database->Database.execDML(QueryString.c_str());
}
