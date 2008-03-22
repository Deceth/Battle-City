#ifndef _CDatabase
#define _CDatabase

#include "CServer.h"

class CServer;

class CDatabase {

	public:
		CDatabase(CServer *Server);
		~CDatabase();

		void LoadDatabase();
		void CleanDatabase();

		CppSQLite3DB Database;
		CppSQLite3Query Query;

	private:
		CServer *p;

	protected:

};

#endif
