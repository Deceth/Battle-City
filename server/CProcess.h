/*
===============================================================================

    Battle City - CProcess header file
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
#ifndef _CPROCESS
#define _CPROCESS

#include "CServer.h"
#include "NetMessages.h"

class CServer;

class CProcess {

	public:
		CProcess(CServer *Server);
		~CProcess();

		CServer *p;

		void ProcessData(char *TheData, int Index);

	private:
		void ProcessIsHiring(char NotHiring, int Index);
		void ProcessBuild(char *TheData, int Index);
		void ProcessJobApp(char *TheData, int Index);
		void ProcessCancelJob(char *TheData, int Index);
		void ProcessAccept(char *TheData, int Index);
		void ProcessDeny(int Index);
		void ProcessUpdate(sCMUpdate *data, int Index);
		void ProcessItemUp(sCMItem *data, int Index);
		void ProcessItemDrop(sCMItem *data, int Index);
		void ProcessVersion(sCMVersion *vers, int Index);
		void ProcessLogin(sCMLogin *login, int Index);
		void ProcessNewAccount(sCMNewAccount *newaccount, int Index);
		void ProcessSetState(char *TheData, int Index);
		void ProcessShot(sCMShot *data, int Index);
		void ProcessDeath(char *TheData, int Index);
		void ProcessDemolish(sCMDemolish *data, int Index);
		void ProcessMedKit(int *data, int Index);
		void ProcessCloak(int *data, int Index);
		void ProcessHitObject(sCMItem *data, int Index);
		void ProcessCrash(char *TheData, int Index);
		void ProcessAdmin(sCMAdmin *admin, int Index);
		void ProcessNextStep(char TheStep, int Index);
		void ProcessSuccessor(char Successor, int Index);
		void ProcessBan(char *TheData, int Index);
		void ProcessRequestSector(sCMSector *sector, int Index);
		void ProcessEditAccount(sCMLogin *login, int Index);
		void ProcessUpdateAccount(sCMNewAccount *updateaccount, int Index);
		void ProcessRecover(char *TheData, int Index);
		void ProcessRequestInfo(int Index);
		void ProcessRightClickCity(int City, int Index);
		void ProcessAdminEdit(int Index, sCMAdminEdit *adminedit);
		void ProcessAdminEditRequest(int Index, sCMAdminEditRequest *admineditrequest);
		void ProcessClickPlayer(int Index, int Clicked);
		void ProcessChangeTank(int Index, int Tank);
		void ProcessAutoBuild(int Index, sCMAutoBuild *request);
		void ProcessCheatCheck(int Index, sCMCheatCheck* request);
		void ProcessStartingCity(int Index);
		void ProcessChangeStartingCity(int Index, sCMStartingCity* request);
};

#endif
