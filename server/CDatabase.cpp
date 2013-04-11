/*
===============================================================================

    Battle City - CDatabase file
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
#include "CDatabase.h"

/***************************************************************
 * Constructor
 *
 * @param Server
  **************************************************************/
CDatabase::CDatabase(CServer *Server) {
	this->p = Server;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CDatabase::~CDatabase() {
	this->Database.close();
}

/***************************************************************
 * Function:	LoadDatabase
 *
 * @param tr
 * @param td
 **************************************************************/
void CDatabase::LoadDatabase() {
	cout << "Database::Load" << endl;
	cout << "SQLite Version: " << this->Database.SQLiteVersion() << endl;

	// If the database file exists,
	if (this->p->Exists("dat//BCData.db") == 1) {

		// Open the database,
		this->Database.open("dat//BCData.db");

		// Clean the database,
		this->CleanDatabase();
		
		// Vacuum the database,
		cout << "Database::Vacuum" << endl;
		this->Database.execDML("VACUUM");

		cout << "Database::Load::Success" << endl << endl;
	}

	// Else (database doesn't exist),
	else {

		// Create the database
		cout << "Database::Load::NoExist" << endl << "Database::Create" << endl;
		Database.open("dat//BCData.db");

		// Create the Accounts Table
		cout << "Database::Create::Accounts" << endl;
		Database.execDML("CREATE TABLE tAccounts(AccountID INT PRIMARY KEY, Account CHAR(15), Password CHAR(15), Email CHAR(50), Fullname CHAR(20), Town char(15), State CHAR(15), Points INT DEFAULT 0, MonthlyTop20 INT DEFAULT 0, Orbs INT DEFAULT 0, Assists INT DEFAULT 0, Deaths INT DEFAULT 0, Member INT DEFAULT 0, IsAdmin INT DEFAULT 0, Red INT DEFAULT 0, Green INT DEFAULT 0, Blue INT DEFAULT 0, Tank INT DEFAULT 0, Tank2 INT DEFAULT 0, Tank3 INT DEFAULT 0, Tank4 INT DEFAULT 0, Tank5 INT DEFAULT 0, Tank6 INT DEFAULT 0, Tank7 INT DEFAULT 0, Tank8 INT DEFAULT 0, Tank9 INT DEFAULT 0);");
		Database.execDML("CREATE INDEX idxPoints ON tAccounts (Points DESC);");
		Database.execDML("CREATE UNIQUE INDEX idxName ON tAccounts (Account);");

		// Create the Bans Table
		cout << "Database::Create::Bans" << endl;
		Database.execDML("CREATE TABLE tBans(BanID INT PRIMARY KEY, Account CHAR(15), IPAddress CHAR(15), Reason CHAR(30), UniqID CHAR(50));");

		cout << "Database::Create::Success" << endl << endl;
	}
}

/***************************************************************
 * Function:	CleanDatabase
 *
 * @param tr
 * @param td
 **************************************************************/
void CDatabase::CleanDatabase() {

	// Nothing
	// ???
	cout << "Database::Clean" << endl;
}
