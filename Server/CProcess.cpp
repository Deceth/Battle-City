#include "CServer.h"

CProcess::CProcess(CServer *Server)
{
	p = Server;
}

CProcess::~CProcess()
{

}

void CProcess::ProcessData(char *TheData, int Index)
{
#ifndef _DEBUG
	try {
#endif
		switch((unsigned char)TheData[0])  //Packet ID
		{
			case cmVersion: //***Version
				ProcessVersion((sCMVersion *)&TheData[1], Index);
				break;
			case cmLogin: //***Log In
				ProcessLogin((sCMLogin *)&TheData[1], Index);
				break;
			case cmNewAccount: //***New Account
				ProcessNewAccount((sCMNewAccount *)&TheData[1], Index);
				break;
			case cmChatMessage: //***Chat Message
				p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 0);
				break;
			case cmGlobal: //***Chat Message
				p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 1);
				break;
			case cmWalkie:
				p->Send->SendChatMessage((unsigned char)Index, &TheData[1], 2);
				break;
			case cmSetState: //***Player changing state
				ProcessSetState(&TheData[1], Index);
				break;
			case cmJobApp: // APPLY to CITY
				ProcessJobApp(&TheData[1], Index);
				break;
			case cmJobCancel:
				ProcessCancelJob(&TheData[1], Index);
				break;
			case cmHireAccept:
				ProcessAccept(&TheData[1], Index);
				break;
			case cmHireDecline:
				ProcessDeny(Index);
				break;
			case cmBuild:
				ProcessBuild(&TheData[1], Index);
				break;
			case cmItemUp:
				ProcessItemUp((sCMItem *)&TheData[1], Index);
				break;
			case cmItemDrop:
				ProcessItemDrop((sCMItem *)&TheData[1], Index);
				break;
			case cmHitObject:
				ProcessHitObject((sCMItem *)&TheData[1], Index);
				break;
			case cmComms:
				if (p->City[p->Player[Index]->City]->hiring > -1)
				{
					TheData[0] = Index;
					p->Winsock->SendData(p->City[p->Player[Index]->City]->hiring, smComms, TheData, (int)strlen(&TheData[1]) + 1);
				}
				break;
			case cmFired:
				if (p->Player[Index]->Mayor == 1 && p->Player[Index]->City == p->Player[TheData[1]]->City)
				{
					p->Player[TheData[1]]->LeaveGame(1);
					p->Winsock->SendData(TheData[1], smFired, " ");
				}
				break;
			case cmUpdate:
				ProcessUpdate((sCMUpdate *)&TheData[1], Index);
				break;
			case cmShoot:
				ProcessShot((sCMShot *)&TheData[1], Index);
				break;
			case cmDemolish:
				ProcessDemolish((sCMDemolish *)&TheData[1], Index);
				break;
			case cmBan:
				ProcessBan(&TheData[1], Index);
				break;
			case cmMedKit:
				ProcessMedKit((int *)&TheData[1], Index);
				break;
			case cmCrash:
				ProcessCrash(&TheData[1], Index);
				break;
			case cmNextStep:
				ProcessNextStep(TheData[1], Index);
				break;
			case cmSuccessor:
				ProcessSuccessor(TheData[1], Index);
				break;
			case cmAdmin:
				ProcessAdmin((sCMAdmin *)&TheData[1], Index);
				break;
			case cmIsHiring:
				ProcessIsHiring(TheData[1], Index);
				break;
			case cmDeath:
				ProcessDeath(&TheData[1], Index);
				break;
			case cmTCPPing:
				p->Winsock->SendData(Index, smTCPPong, " ");
				break;
			case cmRequestSector:
				ProcessRequestSector((sCMSector *)&TheData[1], Index);
				break;
			case cmEditAccount:
				ProcessEditAccount((sCMLogin *)&TheData[1], Index);
				break;
			case cmAccountUpdate:
				ProcessUpdateAccount((sCMNewAccount *)&TheData[1], Index);
				break;
			case cmRecover:
				ProcessRecover(&TheData[1], Index);
				break;
			case cmRightClickCity:
				ProcessRightClickCity(TheData[1], Index);
				break;
			case cmRequestInfo:
				ProcessRequestInfo(Index);
				break;
			case cmChangeNews:
				if (p->Player[Index]->isAdmin == 2) p->ChangeNews(&TheData[1]);
				break;
			case cmSetMayor:
				if (p->Player[TheData[1]]->State == State_Game && p->Player[TheData[1]]->City == p->Player[Index]->City && p->Player[Index]->Mayor == 1)
				{
					p->Player[TheData[1]]->setMayor(1);
					p->Player[Index]->setMayor(0);
				}
				break;
			case cmRefreshList:
				p->Send->SendCityList(Index);
				break;
			case cmMiniMap:
				p->Send->SendMiniMap(Index);
				break;
			case cmAdminEditRequest:
				ProcessAdminEditRequest(Index, (sCMAdminEditRequest *)&TheData[1]);
				break;
			case cmAdminEdit:
				ProcessAdminEdit(Index, (sCMAdminEdit *)&TheData[1]);
				break;
			case cmClickPlayer:
				ProcessClickPlayer(Index, TheData[1]);
				break;
			case cmChangeTank:
				ProcessChangeTank(Index);
				break;
			case cmWhisper:
				p->Send->SendWhisper(Index, (sCMWhisper *)&TheData[1]);
				break;
		}//(switch)

#ifndef _DEBUG
	} 
	catch (...) 
	{
		std::string temp;
		temp = "Unhandled exception in ProcessData from ";
		temp += p->Player[Index]->Name;
		temp += " :: ";
		temp += p->Player[Index]->IPAddress;
		p->Log->logServerError(temp);
		memset(p->Player[Index]->Buffer, 0, sizeof(p->Player[Index]->Buffer));
		p->Player[Index]->BufferLength = 0;
	}
#endif
} 

void CProcess::ProcessBuild(char *TheData,int Index)
{
	if (p->Player[Index]->Mayor && p->Player[Index]->State == State_Game && p->Player[Index]->isDead == false)
	{
		sCMBuild *data;
		sSMBuild bd;
		data = (sCMBuild *)TheData;

		if (p->City[p->Player[Index]->City]->canBuild[data->type - 1] == 1 || p->Player[Index]->isAdmin == 2)
		{
			p->City[p->Player[Index]->City]->cash -= COST_BUILDING;

			bd.City = p->Player[Index]->City;
			bd.count = 0;
			bd.type = data->type;
			bd.x = data->x;
			bd.y = data->y;
			p->Build->bldID++;
			bd.id = p->Build->bldID++;
			bd.pop = 0;
			p->Build->newBuilding(data->x,data->y,data->type,p->Player[Index]->City, bd.id);

			if (p->Build->bldID > 30000)
				p->Build->bldID = 1;

			p->Send->SendSectorArea(data->x*48, data->y*48,(unsigned char)smNewBuilding,(char *)&bd,sizeof(bd));

			if ((unsigned char)data->type > 2) // not houses, not hospitals
			{
				p->City[p->Player[Index]->City]->setCanBuild((unsigned char)data->type - 1,2);

				if ((unsigned char)data->type % 2) // research centers
				{
					if (p->City[p->Player[Index]->City]->research[(data->type - 3) / 2] != -1)
						p->City[p->Player[Index]->City]->research[(data->type - 3) / 2] = p->Tick + 20000;
				}
			}
		}
	}
}

void CProcess::ProcessJobApp(char *TheData,int Index)
{
	char packet[20];

	if ((unsigned char)TheData[0] < 64)
	{
		if (p->City[(unsigned char)TheData[0]]->Mayor == -1)
		{
			p->Player[Index]->City = (unsigned char)TheData[0];
			p->Player[Index]->setMayor(1);
			p->Player[Index]->StartJoin(); 
			packet[0] = Index;
			packet[1] = 68;
			p->Send->SendToChat(smChatCommand, packet, 2); 
		} 
		else 
		{
			if (p->City[(unsigned char)TheData[0]]->PlayerCount() < 4)
			{
				if (p->City[(unsigned char)TheData[0]]->hiring == -1)
				{
					p->City[(unsigned char)TheData[0]]->hiring = Index;
					p->Player[Index]->State = State_Apply;
					packet[0] = Index;
					p->Winsock->SendData(p->City[(unsigned char)TheData[0]]->Mayor,smMayorHire,packet,1);
					p->Winsock->SendData(Index, smInterview, " ", 1);
					packet[0] = Index;
					packet[1] = 68;
					p->Send->SendToChat(smChatCommand, packet, 2); 
				} 
				else 
				{
					p->Winsock->SendData(Index,smMayorInInterview," ",1);
				}
			}
			else 
			{
				p->Winsock->SendData(Index,smMayorInInterview," ",1);
			}
		}
	}
}

void CProcess::ProcessCancelJob(char *TheData, int Index)
{
	for (int j = 0; j < 64; j++)
	{
		if (p->City[j]->hiring == Index)
		{
			p->City[j]->hiring = -1;
			p->Winsock->SendData(p->City[j]->Mayor,smInterviewCancel," ");
		}
	}
}


void CProcess::ProcessAccept(char *TheData,int Index)
{
	if (p->Player[Index]->Mayor)
		if (p->City[p->Player[Index]->City]->hiring > -1)
			if (p->Player[p->City[p->Player[Index]->City]->hiring]->Socket)
			{
				p->Player[p->City[p->Player[Index]->City]->hiring]->City = p->Player[Index]->City;
				p->Player[p->City[p->Player[Index]->City]->hiring]->setMayor(0);
				p->Player[p->City[p->Player[Index]->City]->hiring]->StartJoin();
				p->Player[Index]->State = State_Game;
				p->City[p->Player[Index]->City]->hiring = -1;
			}

}

void CProcess::ProcessDeny(int Index)
{
	if (p->Player[Index]->Mayor)
	if (p->City[p->Player[Index]->City]->hiring > -1)
	{
		if (p->Player[p->City[p->Player[Index]->City]->hiring]->Socket)
		{
			p->Winsock->SendData(p->City[p->Player[Index]->City]->hiring,smMayorDeclined," ",1);
		}
		p->City[p->Player[Index]->City]->hiring = -1;
	}
}

void CProcess::ProcessUpdate(sCMUpdate *data, int Index)
{
	if (p->Player[Index]->isDead == true) return;
	sSMUpdate update;
	if (abs(int(data->x - p->Player[Index]->X)) < 300 && abs(int(data->y - p->Player[Index]->Y)) < 300 && (p->Player[Index]->lastMove + 3000 > p->Tick))
	{
		p->Player[Index]->lastMove = p->Tick;
		if (data->x > 24576) data->x = 24500;
		if (data->y > 24576) data->y = 24500;
		p->Player[Index]->X = update.x = data->x;
		p->Player[Index]->Y = update.y = data->y;
		update.turn = data->turn;
		update.id = (unsigned char)Index;
		update.dir = data->dir;
		update.move = data->move;
		p->Send->SendRadarNotIndex(Index,smUpdate,(char *)&update,sizeof(update));
	}
	else
	{
		p->Player[Index]->lastMove = p->Tick;
		sSMStateGame game;
		game.City = p->Player[Index]->City;
		game.x = (unsigned short)p->Player[Index]->X;
		game.y = (unsigned short)p->Player[Index]->Y;
		p->Winsock->SendData(Index, smWarp,(char *)&game,sizeof(game));
	}
}

void CProcess::ProcessItemUp(sCMItem *data, int Index)
{
	if (p->Player[Index]->isDead == true) return;
	CItem *item;
	sSMItem msg;

	item = p->Item->findItem(data->id);
	if (item)
	{
		if ((item->City == p->Player[Index]->City || p->Player[Index]->isAdmin == 2) && item->holder == -1)
		{
			item->holder = Index;
			msg.id = (unsigned short)data->id;

			sCMItem blargh;
			blargh.id = item->id;
			blargh.active = data->active;
			blargh.type = item->type;

			p->Winsock->SendData(Index,smPickedUp, (char *)&blargh, sizeof(blargh));

			p->Send->SendSectorArea(item->x*48, item->y*48 ,smRemItem, (char *)&msg, sizeof(msg));
		}
	}
}

void CProcess::ProcessItemDrop(sCMItem *data, int Index)
{
	if (p->Player[Index]->isDead == true) return;

	CItem *item;

	sCMItem *itm;
	
	itm = (sCMItem *)data;

	item = p->Item->findItem(itm->id);
	if (item)
	{
		if (item->holder == Index)
		{
			sCMItem blargh;
			blargh.id = item->id;
			blargh.active = data->active;
			blargh.type = item->type;

			item->active = data->active;

			p->Winsock->SendData(Index,smDropped, (char *)&blargh, sizeof(blargh));

			item->drop((int)((p->Player[Index]->X+24) / 48),(int)((p->Player[Index]->Y+24) / 48),Index);
		}
	}
}

void CProcess::ProcessVersion(sCMVersion *vers, int Index)
{
	if (strcmp(vers->Version, VERSION) != 0) //They don't have the latest version
	{
		p->Winsock->SendData(Index, smError, "F"); //F for Failure on the version check!
		cout << "Invalid Version " << vers->Version << " from " << p->Player[Index]->IPAddress << "\n";
	}
	else 
	{
		if (p->Account->CheckBan(vers->UniqID, p->Player[Index]->IPAddress) == 1)
		{
			p->Winsock->SendData(Index, smError, "G");
		}
		else //Correct version and isn't banned
		{
			p->Player[Index]->UniqID = vers->UniqID;
			p->Player[Index]->State = State_Verified;
			p->Player[Index]->id = Index;
		}
	}
}

void CProcess::ProcessLogin(sCMLogin *login, int Index)
{
	if (p->Player[Index]->State == State_Verified) //Must be verified at this point
	{
		if (p->Account->CheckAccount(login->User) == 1) //Does the account exist?
		{
			int CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
			if (CorrectPassword == 2) //Correct password and verified account?
			{
				if (p->Account->CheckAccountBan(login->User) == 0)
				{
					int foundmatch = 0;
					for (int j = 0; j < MaxPlayers; j++)
					{
						if (p->PlatformCaseCompare(login->User, p->Player[j]->Name.c_str()) == 0) {p->Player[j]->Clear(); foundmatch = 1;}
						if (p->PlatformCaseCompare(p->Player[Index]->UniqID.c_str(), p->Player[j]->UniqID.c_str()) == 0 && Index != j) { p->Player[j]->Clear(); foundmatch = 2;}
					}
					if (foundmatch == 1) //Already logged in?
					{
						cout << "Multilog from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "E"); //Account in use
					}
					else if (foundmatch == 2) //Computer already on
					{
						cout << "MultiCPU log from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "H"); //ComputerID in use
					}
					else p->Player[Index]->LoggedIn(login->User);
				}
				else {cout << "Banned log from " << login->User << endl; p->Winsock->SendData(Index, smError, "G");} //Banned
			}
			else if (CorrectPassword == 0) p->Winsock->SendData(Index, smError, "B"); //Incorrect Password 
		}
		else p->Winsock->SendData(Index, smError, "C"); //Account does not exist
	}
}

void CProcess::ProcessNewAccount(sCMNewAccount *newaccount, int Index)
{
	int CreateAccount = p->Account->NewAccount(newaccount->User, newaccount->Pass, newaccount->Email, newaccount->FullName, newaccount->Town, newaccount->State);
	if (CreateAccount == 1) //Creation successful
	{
		p->Winsock->SendData(Index, smError, "A"); //Account created successfully
	}
	if (CreateAccount == -1) p->Winsock->SendData(Index, smError, "K"); //Email address in use
	if (CreateAccount == 0) p->Winsock->SendData(Index, smError, "D"); //User name in use
}

void CProcess::ProcessSetState(char *TheData,int Index)
{
	if (p->Player[Index]->State >= State_Verified) //Make sure they are verified
	{
		char tmpString[5];
		memset(tmpString, 0, 5);
		tmpString[0] = (unsigned char)Index;
		switch(TheData[0])
		{
			case 65: //State A/Chat
			case 66: //State B/Daily Bullet
			case 67: //State C/City List
				{
					if (p->Player[Index]->State == State_Game) p->Player[Index]->LeaveGame(1);
					p->Send->SendMeetingRoom(Index);
					break;
				}
			case 68: //State D/Disconnected
				{
					strcpy(&tmpString[1], "D"); 
					p->Player[Index]->Clear(); 
					p->Send->SendToChat(smChatCommand, tmpString); 
					break;
				}
		}
	}
}

void CProcess::ProcessShot(sCMShot *data, int Index)
{
	if (p->Player[Index]->lastShot + 500 < p->Tick || p->Player[Index]->isAdmin == 2)
	{
		p->Player[Index]->lastShot = p->Tick;
		sSMShot shotsfired;
		shotsfired.id = Index;
		shotsfired.type = data->type;
		shotsfired.x = data->x;
		shotsfired.y = data->y;
		shotsfired.dir = data->dir;
		p->Bullet->newBullet(data->x,data->y,data->type,data->dir, Index);
		p->Send->SendRadarNotIndex(Index, smShoot, (char *)&shotsfired, sizeof(sSMShot));
	}
}

void CProcess::ProcessDeath(char *TheData, int Index)
{
	if (p->Player[Index]->State == State_Game)
	{
		p->Account->AddDeath(Index);

		char packet[3];
		packet[0] = (unsigned char)Index;
		packet[1] = 0;
		packet[2] = TheData[0];
		p->Send->SendGameAllBut(-1, smDeath, packet, 3);
		cout << p->Player[Index]->Name << " has died" << endl;
		p->Player[Index]->isDead = true;
		p->Player[Index]->timeDeath = p->Tick;

		if (p->Player[Index]->Points > 100)
		{
			CItem *itm = p->Item->items;
			if (itm)
			{
				int operations = 0;
				while (itm->prev)
				{
					operations++;
					if (operations > 50000) 
					{
						p->Log->logServerError("loop broken in death prev");
						break;
					}
					itm = itm->prev;
				}

				operations = 0;
				while (itm)
				{
					operations++;
					if (operations > 50000) 
					{
						p->Log->logServerError("loop broken in death prev");
						break;
					}
					if (itm->holder == Index)
						itm = p->Item->delItem(itm);
					if (itm) itm = itm->next;
				}
			}

			p->Account->Sub2Points(Index);

			if (p->Player[Index]->City != TheData[0])
			{
				for (int j = 0; j < MaxPlayers; j++)
				{
					if (p->Player[j]->State == State_Game && p->Player[j]->City == TheData[0])
					{
						p->Account->AddPoints(j, 2);
					}
				}
			}
		}
	}
}

void CProcess::ProcessMedKit(int *data, int Index)
{
	CItem *itm = p->Item->findItem(*data);
	if (itm)
	{
		if (itm->type == 2 && itm->holder == Index)
		{
			p->Item->delItem(itm);
			p->Winsock->SendData(Index, smMedKit, " ");
		}
	}
}

void CProcess::ProcessDemolish(sCMDemolish *data, int Index)
{
	if (p->Player[Index]->isDead == true) return;

	CBuilding *bld = p->Build->findBuilding(data->id);
	if (bld)
	{
		if (bld->City == p->Player[Index]->City)
		{
			p->Build->delBuilding(bld);
		}
	}
}

void CProcess::ProcessHitObject(sCMItem *data, int Index)
{
	CItem *item;

	item = p->Item->findItem(data->id);
	if (item)
	{
		p->Item->delItem(item);
	}
}

void CProcess::ProcessCrash(char *TheData,int Index)
{
	std::string theerror;
	theerror.clear();
	theerror.append(TheData);
	p->Log->logClientError(theerror, Index);
}

void CProcess::ProcessNextStep(char TheStep, int Index)
{
	switch (TheStep)
	{
	case 65: //A Ready to start receiving data
		p->Player[Index]->JoinGame();
		p->Winsock->SendData(Index, smNextStep, "B");
		break;
	case 66: //B Refreshed Area
		p->Winsock->SendData(Index, smNextStep, "C");
		break;
	case 67: //C Done
		
		break;
	}
}

void CProcess::ProcessSuccessor(char Successor, int Index)
{
	if (p->Player[Successor]->City == p->Player[Index]->City && p->Player[Index]->Mayor)
	{
		p->City[p->Player[Index]->City]->Successor = Successor;
	}
}

void CProcess::ProcessAdmin(sCMAdmin *admin, int Index)
{
	if (p->Player[Index]->isAdmin == 2)
	{
		switch (admin->command)
		{
			case 1: //Kicked
			{
				sSMAdmin sadmin;
				sadmin.adminIndex = Index;
				sadmin.command = 1;
				sadmin.id = admin->id;
				if (p->Player[admin->id]->State == State_Game)
				{
					string LogAdminString = "Kick ";
					LogAdminString += p->Player[admin->id]->Name;
					p->Log->logAdmin(LogAdminString, Index);
					cout << "Kick::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
					p->Player[admin->id]->LeaveGame(1);
					p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
				}
				p->Winsock->SendData(admin->id, smKicked, " ");
			}
			break;
			case 2: //Join City
			{
				if (admin->id >= 0 && admin->id < 64)
				{
					string LogAdminString = "Join City ";
					ostringstream convert;
					convert << admin->id;
					LogAdminString += convert.str();
					p->Log->logAdmin(LogAdminString, Index);
					p->Player[Index]->LeaveGame(1);
					p->Player[Index]->City = admin->id;
					if (p->City[admin->id]->Mayor == -1)
					{
						p->Player[Index]->setMayor(1);
					}
					p->Player[Index]->StartJoin();
				}

			}
			break;
			case 3: //Warp to Player
			{
				string LogAdminString = "Warp ";
				LogAdminString += p->Player[admin->id]->Name;
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Warp::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
				p->Player[Index]->X = p->Player[admin->id]->X;
				p->Player[Index]->Y = p->Player[admin->id]->Y;
			}
			break;
			case 4: //Warp Player
			{
				string LogAdminString = "Summon ";
				LogAdminString += p->Player[admin->id]->Name;
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Summon::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
				p->Player[admin->id]->X = p->Player[Index]->X;
				p->Player[admin->id]->Y = p->Player[Index]->Y;
			}
			break;
			case 5: //Banned
			{
				string DMLString;
				DMLString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
				DMLString += p->Player[admin->id]->Name;
				DMLString += "', '";
				DMLString += p->Player[admin->id]->IPAddress;
				DMLString += "', '";
				DMLString += p->Player[Index]->Name;
				DMLString += "', '";
				DMLString += p->Player[admin->id]->UniqID;
				DMLString += "');";
				p->Database->Database.execDML(DMLString.c_str());

				sSMAdmin sadmin;
				sadmin.adminIndex = Index;
				sadmin.command = 5;
				sadmin.id = admin->id;
				if (p->Player[admin->id]->State == State_Game)
				{
					string LogAdminString = "Ban " + p->Player[admin->id]->Name;
					p->Log->logAdmin(LogAdminString, Index);
					cout << "Ban::" << p->Player[Index]->Name << "::" << p->Player[admin->id]->Name << endl;
					p->Player[admin->id]->LeaveGame(1);
					p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
				}
				p->Winsock->SendData(admin->id, smKicked, " ");
			}
			break;
			case 6: //Shutdown
			{
				p->Log->logAdmin("Shutdown", Index);
				cout << "Shutdown::" << p->Player[Index]->Name << endl;
				p->running = 0;
			}
			break;
			case 7: //Spawn Item
			{
				string LogAdminString = "Spawn Item ";
				ostringstream convert;
				convert << admin->id;
				LogAdminString += convert.str();
				p->Log->logAdmin(LogAdminString, Index);
				cout << "Spawn Item::" << p->Player[Index]->Name << "::" << admin->id << endl;

				int tmpID = p->Item->itmID++;
				if (p->Item->itmID > 30000) p->Item->itmID = 1;
				CItem *itm = p->Item->newItem(0, 0, admin->id, p->Player[Index]->City, tmpID);

				itm->holder = Index;
				
				sCMItem PickUp;
				PickUp.id = itm->id;
				PickUp.active = itm->active;
				PickUp.type = itm->type;

				sSMItem msg;
				msg.id = itm->id;
				msg.x = 0;
				msg.y = 0;
				msg.City = p->Player[Index]->City;
				msg.active = 0;
				msg.type = (unsigned char)admin->id;

				p->Winsock->SendData(Index,smAddItem,(char *)&msg, sizeof(msg));
				p->Winsock->SendData(Index,smPickedUp, (char *)&PickUp, sizeof(PickUp));
				p->Winsock->SendData(Index,smRemItem, (char *)&msg, sizeof(msg));
			}
			break;
			case 8: //Request Ban
			{
				p->Log->logAdmin("Request Bans", Index);

				string QueryString;
				QueryString = "SELECT * FROM tBans";
				p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

				while (!p->Database->Query.eof())
				{
					sSMBan ban;
					memset(&ban, 0, sizeof(ban));

					strcpy(ban.Account, p->Database->Query.getStringField("Account"));
					strcpy(ban.IPAddress, p->Database->Query.getStringField("IPAddress"));
					strcpy(ban.Reason, p->Database->Query.getStringField("Reason"));

					p->Winsock->SendData(Index, smBan, (char *)&ban, sizeof(ban));

					p->Database->Query.nextRow();
				}

				p->Database->Query.finalize();
			}
			break;
			case 9: //Unban
			{
				p->Log->logAdmin("Unban Attempt", Index);

				string QueryString;
				QueryString = "SELECT * FROM tBans";
				p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

				for (int i = 0; i < admin->id; i++)
				{
					if (!p->Database->Query.eof())
					{
						p->Database->Query.nextRow();
					}
					else
					{
						p->Database->Query.finalize();
						return;
					}
				}

				if (!p->Database->Query.eof())
				{
					try
					{
						string DMLString = "DELETE FROM tBans WHERE Account = '";
						DMLString += p->Database->Query.getStringField("Account");
						DMLString += "';";
						p->Database->Query.finalize();
						p->Log->logAdmin(DMLString, Index);
						p->Database->Database.execDML(DMLString.c_str());
					}
					catch (CppSQLite3Exception& e)
					{
						cerr << e.errorCode() << ":" << e.errorMessage() << endl;
						return;
					}
				}
				else
				{
					p->Database->Query.finalize();
					return;
				}
			}
			break;
			case 10: //Request News
			{
				p->Log->logAdmin("Request News", Index);

				p->Send->SendAdminNews(Index);
			}
			case 11: //Update News
			{
				p->Log->logAdmin("Update News", Index);
			}
		}
	}
	else
	{
		p->Log->logClientError("Attempted Admin Command", Index);
	}
}

void CProcess::ProcessBan(char *TheData,int Index)
{
	string DMLString;
	DMLString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
	DMLString += p->Player[Index]->Name;
	DMLString += "', '";
	DMLString += p->Player[Index]->IPAddress;
	DMLString += "', '";
	DMLString += TheData;
	DMLString += "', '";
	DMLString += p->Player[Index]->UniqID;
	DMLString += "');";
	p->Database->Database.execDML(DMLString.c_str());
	cout << "Ban::" << p->Player[Index]->Name << endl;
}

void CProcess::ProcessIsHiring(char NotHiring, int Index)
{
	if (p->Player[Index]->Mayor && p->Player[Index]->State == State_Game)
	{
		p->City[p->Player[Index]->City]->notHiring = NotHiring;
	}
}

void CProcess::ProcessRequestSector(sCMSector *sector, int Index)
{
	p->Send->SendSector(Index, sector->x, sector->y);
}

void CProcess::ProcessEditAccount(sCMLogin *login, int Index)
{
	if (p->Player[Index]->State == State_Verified) //Must be verified at this point
	{
		if (p->Account->CheckAccount(login->User) == 1) //Does the account exist?
		{
			int CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
			if (CorrectPassword == 2) //Correct password and verified account?
			{
				if (p->Account->CheckAccountBan(login->User) == 0)
				{
					int foundmatch = 0;
					for (int j = 0; j < MaxPlayers; j++)
					{
						if (p->PlatformCaseCompare(login->User, p->Player[j]->Name.c_str()) == 0) foundmatch = 1;
						if (p->PlatformCaseCompare(p->Player[Index]->UniqID.c_str(), p->Player[j]->UniqID.c_str()) == 0 && Index != j) foundmatch = 2;
					}
					if (foundmatch == 1) //Already logged in?
					{
						cout << "Multilog from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "E"); //Account in use
					}
					else if (foundmatch == 2) //Computer already on
					{
						cout << "MultiCPU log from " << login->User << endl;
						p->Winsock->SendData(Index, smError, "H"); //ComputerID in use
					}
					else
					{
						p->Player[Index]->Name = login->User;
						p->Account->SendAccountInformation(Index);
						p->Player[Index]->State = State_Editing;
					}
				}
				else {cout << "Banned log from " << login->User << endl; p->Winsock->SendData(Index, smError, "G");} //Banned
			}
			else if (CorrectPassword == 0) p->Winsock->SendData(Index, smError, "B"); //Incorrect Password 
			else 
			{
				p->Player[Index]->Name = login->User;
				p->Account->SendAccountInformation(Index);
				p->Player[Index]->State = State_Editing;
			}
		}
		else p->Winsock->SendData(Index, smError, "C"); //Account does not exist
	}
}

void CProcess::ProcessUpdateAccount(sCMNewAccount *updateaccount, int Index)
{
	if (p->Player[Index]->State == State_Editing)
	{
		int UpdateAccount = p->Account->UpdateAccount(Index, updateaccount->Pass, updateaccount->Email, updateaccount->FullName, updateaccount->Town, updateaccount->State);
		if (UpdateAccount == 1) //Creation successful - No verification required
		{
			int CorrectPassword = p->Account->CheckPassword(updateaccount->User, updateaccount->Pass);
			if (CorrectPassword == 2) //Correct password and verified account?
				p->Player[Index]->LoggedIn(p->Player[Index]->Name);
			if (CorrectPassword == 0) //Wrong Password
				 p->Winsock->SendData(Index, smError, "B"); //Incorrect Password 
		}
		if (UpdateAccount == -1) p->Winsock->SendData(Index, smError, "K"); //Email address in use
	}
}

void CProcess::ProcessRecover(char *TheData, int Index)
{
	int i = p->Account->RecoverAccount(Index, TheData);
	if (i == 0)
	{
		p->Winsock->SendData(Index, smError, "L");
	}
	else
	{
		p->Winsock->SendData(Index, smError, "M");
	}
}

void CProcess::ProcessRequestInfo(int Index)
{
	int SearchX = (int)p->Player[Index]->X/48;
	int SearchY = (int)p->Player[Index]->Y/48;

	short int CityDistance[64];
	short int BuildCount[64];

	memset(CityDistance, 0, sizeof(CityDistance));
	memset(BuildCount, 0, sizeof(BuildCount));

	short int Closest50 = -1;
	short int Closest30 = -1;
	short int Closest20 = -1;
	unsigned char SelectedCity = 255;

	CBuilding *bld = p->Build->buildings;
	if (bld)
		while(bld->prev)
			bld = bld->prev;

	while (bld)
	{
		if (bld->City != -1) BuildCount[bld->City] += 1;
		bld = bld->next;
	}

	for (int i = 0; i < 64; i++)
	{
		if (i != p->Player[Index]->City)
		{
			if (p->City[i]->x == 0 || p->City[i]->y == 0)
			{
				p->City[i]->x = (unsigned short)(512*48)-(32+(i % 8*64)) * 48;
				p->City[i]->y = (unsigned short)(512*48)-(32+(i / 8*64)) * 48; 
			}

//			CityDistance[i] = (int)sqrt(pow(p->City[i]->x - SearchX), 2) + pow((p->City[i]->y - SearchY), 2));

			CityDistance[i] = (int)sqrt((float)((p->City[i]->x - SearchX)^2 + (p->City[i]->y - SearchY)^2));

			if (BuildCount[i] > 10)
			{
				if (BuildCount[i] < 15) //20 pointer
				{
					if (Closest50 == -1 && Closest30 == -1)
					{
						if (SelectedCity == 255)
						{
							SelectedCity = i;
							Closest20 = CityDistance[i];
						}
						else
						{
							if (CityDistance[i] < Closest20)
							{
								SelectedCity = i;
								Closest20 = CityDistance[i];
							}
						}
					}
				}
				else if (BuildCount[i] < 20) //30 pointer
				{
					if (Closest50 == -1)
					{
						if (SelectedCity == 255)
						{
							SelectedCity = i;
							Closest30 = CityDistance[i];
						}
						else
						{
							if (CityDistance[i] < Closest30)
							{
								SelectedCity = i;
								Closest30 = CityDistance[i];
							}
						}
					}
				}
				else //50 pointer
				{
					if (SelectedCity == 255)
					{
						SelectedCity = i;
						Closest50 = CityDistance[i];
					}
					else
					{
						if (Closest50 == -1)
						{
							Closest50 = CityDistance[i];
							SelectedCity = i;
						}
						else
						{
							if (CityDistance[i] < Closest50)
							{
								SelectedCity = i;
								Closest50 = CityDistance[i];
							}
						}
					}
				}
			}
		}
	}

	p->Send->SendInfoButton(Index, SelectedCity);
}

void CProcess::ProcessRightClickCity(int City, int Index)
{
	sSMRightClickCity cityinfo;
	cityinfo.BuildingCount = p->Build->GetBuildingCount(City);
	cityinfo.City = City;
	cityinfo.OrbPoints = p->Build->GetOrbPointCount(City);

	p->Winsock->SendData(Index, smRightClickCity, (char *)&cityinfo, sizeof(cityinfo));
}

void CProcess::ProcessAdminEdit(int Index, sCMAdminEdit *adminedit)
{
	if (p->Player[Index]->isAdmin)
	{
		if (
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Weebo") == 0)
			||
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Vindkast") == 0)
		) {
			if (p->Account->CheckAccount(adminedit->User) == 1)
			{
				string DMLString;

				DMLString = "UPDATE tAccounts SET Deaths = ";
				std::ostringstream DeathsConvert;
				DeathsConvert << adminedit->Deaths;
				DMLString += DeathsConvert.str();

				DMLString += ", Orbs = ";
				std::ostringstream OrbsConvert;
				OrbsConvert << adminedit->Orbs;
				DMLString += OrbsConvert.str();

				DMLString += ", Assists = ";
				std::ostringstream AssistsConvert;
				AssistsConvert << adminedit->Assists;
				DMLString += AssistsConvert.str();

				DMLString += ", Points = ";
				std::ostringstream PointsConvert;
				PointsConvert << adminedit->Points;
				DMLString += PointsConvert.str();

				DMLString += ", Fullname = '";
				DMLString += adminedit->FullName;

				DMLString += "', Town = '";
				DMLString += adminedit->Town;

				DMLString += "', State = '";
				DMLString += adminedit->State;

				DMLString += "', Password = '";
				DMLString += adminedit->Pass;

				DMLString += "', Email = '";
				DMLString += adminedit->Email;

				DMLString += "', Tank = ";
				std::ostringstream TankConvert;
				TankConvert << (int)adminedit->Tank;
				DMLString += TankConvert.str();

				DMLString += ", Tank2 = ";
				std::ostringstream Tank2Convert;
				Tank2Convert << (int)adminedit->Tank2;
				DMLString += Tank2Convert.str();

				DMLString += ", Tank3 = ";
				std::ostringstream Tank3Convert;
				Tank3Convert << (int)adminedit->Tank3;
				DMLString += Tank3Convert.str();

				DMLString += ", Tank4 = ";
				std::ostringstream Tank4Convert;
				Tank4Convert << (int)adminedit->Tank4;
				DMLString += Tank4Convert.str();

				DMLString += ", Red = ";
				std::ostringstream RedConvert;
				RedConvert << (int)adminedit->Red;
				DMLString += RedConvert.str();

				DMLString += ", Green = ";
				std::ostringstream GreenConvert;
				GreenConvert << (int)adminedit->Green;
				DMLString += GreenConvert.str();

				DMLString += ", Blue = ";
				std::ostringstream BlueConvert;
				BlueConvert << (int)adminedit->Blue;
				DMLString += BlueConvert.str();

				DMLString += ", Member = ";
				std::ostringstream MemberConvert;
				MemberConvert << (int)adminedit->Member;
				DMLString += MemberConvert.str();

				DMLString += ", RentalCity = ";
				std::ostringstream RentalCityConvert;
				RentalCityConvert << (int)adminedit->RentalCity;
				DMLString += RentalCityConvert.str();

				DMLString += ", IsAdmin = ";
				std::ostringstream IsAdminConvert;
				IsAdminConvert << (int)adminedit->IsAdmin;
				DMLString += IsAdminConvert.str();

				DMLString += " WHERE Account LIKE '";
				DMLString += adminedit->User;
				DMLString += "';";

				p->Log->logAdmin(DMLString, Index);

				p->Database->Database.execDML(DMLString.c_str());

				for (int i = 0; i < MaxPlayers; i++)
				{
					if (p->PlatformCaseCompare(adminedit->User, p->Player[i]->Name) == 0)
					{
						p->Player[i]->Red = adminedit->Red;
						p->Player[i]->Green = adminedit->Green;
						p->Player[i]->Blue = adminedit->Blue;
						p->Player[i]->Member = adminedit->Member;
						p->Player[i]->isAdmin = adminedit->IsAdmin;
						p->Player[i]->Tank = adminedit->Tank;
						p->Player[i]->Town = adminedit->Town;

						p->Player[i]->Points = adminedit->Points;
						p->Player[i]->Deaths = adminedit->Deaths;
						p->Player[i]->Assists = adminedit->Assists;
						p->Player[i]->Orbs = adminedit->Orbs;

						sSMPoints pts;
						pts.Index = i;
						pts.Points = p->Player[i]->Points;
						pts.Deaths = p->Player[i]->Deaths;
						pts.Assists = p->Player[i]->Assists;
						pts.Orbs = p->Player[i]->Orbs;
						pts.MonthlyPoints = p->Player[i]->MonthlyPoints;
						p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));

						sSMPlayer player;
						player.Red = p->Player[i]->Red;
						player.Green = p->Player[i]->Green;
						player.Blue = p->Player[i]->Blue;
						player.Member = p->Player[i]->Member;
						player.Index = i;
						player.isAdmin = p->Player[i]->isAdmin;
						player.Tank = p->Player[i]->Tank;
						strcpy(player.Name, p->Player[i]->Name.c_str());
						strcpy(player.Town, p->Player[i]->Town.c_str());

						p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
					}
				}
			}
		}
	}
}

void CProcess::ProcessAdminEditRequest(int Index, sCMAdminEditRequest *admineditrequest)
{
	if (p->Player[Index]->isAdmin)
	{
		if (
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Weebo") == 0)
			||
			(p->PlatformCaseCompare(p->Player[Index]->Name.c_str(), "Vindkast") == 0)
		) {	
			try
			{
				string QueryString;
				QueryString = "SELECT * FROM tAccounts WHERE Account LIKE '";
				QueryString += admineditrequest->User;
				QueryString += "';";
				p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());
				if (!p->Database->Query.eof())
				{
					p->Log->logAdmin(QueryString, Index);

					sCMAdminEdit newedit;
					strcpy(newedit.User, admineditrequest->User);
					strcpy(newedit.Pass, p->Database->Query.getStringField("Password"));
					strcpy(newedit.FullName, p->Database->Query.getStringField("Fullname"));
					strcpy(newedit.Town, p->Database->Query.getStringField("Town"));
					strcpy(newedit.State, p->Database->Query.getStringField("State"));
					strcpy(newedit.Email, p->Database->Query.getStringField("Email"));

					newedit.Points = p->Database->Query.getIntField("Points");
					newedit.Orbs = p->Database->Query.getIntField("Orbs");
					newedit.Deaths = p->Database->Query.getIntField("Deaths");
					newedit.Assists = p->Database->Query.getIntField("Assists");
					newedit.IsAdmin = p->Database->Query.getIntField("IsAdmin");
					newedit.Member = p->Database->Query.getIntField("Member");
					newedit.Red = p->Database->Query.getIntField("Red");
					newedit.Green = p->Database->Query.getIntField("Green");
					newedit.Blue = p->Database->Query.getIntField("Blue");
					newedit.Tank = p->Database->Query.getIntField("Tank");
					newedit.Tank2 = p->Database->Query.getIntField("Tank2");
					newedit.Tank3 = p->Database->Query.getIntField("Tank3");
					newedit.Tank4 = p->Database->Query.getIntField("Tank4");
					newedit.RentalCity = p->Database->Query.getIntField("RentalCity");

					p->Winsock->SendData(Index, smAdminEdit, (char *)&newedit, sizeof(newedit));
				}

				p->Database->Query.finalize();
			}
			catch (CppSQLite3Exception& e)
			{
				cerr << e.errorCode() << ":" << e.errorMessage() << endl;
			}
		}
	}
}

void CProcess::ProcessClickPlayer(int Index, int Clicked)
{
	sSMClickPlayer clickplayer;
	clickplayer.Index = Clicked;
	clickplayer.Orbs = p->Player[Clicked]->Orbs;
	clickplayer.Assists = p->Player[Clicked]->Assists;
	clickplayer.Deaths = p->Player[Clicked]->Deaths;

	p->Winsock->SendData(Index, smClickPlayer, (char *)&clickplayer, sizeof(clickplayer));
}


void CProcess::ProcessChangeTank(int Index)
{
	unsigned char tank = p->Player[Index]->Tank;
	unsigned char tank2 = p->Player[Index]->Tank2;
	unsigned char tank3 = p->Player[Index]->Tank3;
	unsigned char tank4 = p->Player[Index]->Tank4;

	p->Player[Index]->Tank = tank2;
	p->Player[Index]->Tank2 = tank3;
	p->Player[Index]->Tank3 = tank4;
	p->Player[Index]->Tank4 = tank;

	sSMPlayer player;
	player.Red = p->Player[Index]->Red;
	player.Green = p->Player[Index]->Green;
	player.Blue = p->Player[Index]->Blue;
	player.Member = p->Player[Index]->Member;
	player.Index = Index;
	player.isAdmin = p->Player[Index]->isAdmin;
	player.Tank = p->Player[Index]->Tank;
	strcpy(player.Name, p->Player[Index]->Name.c_str());
	strcpy(player.Town, p->Player[Index]->Town.c_str());

	p->Send->SendAllBut(-1, smPlayerData, (char *)&player, sizeof(player));
}
