/*
===============================================================================

    Battle City - CProcess file
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
#include "CServer.h"


/// <summary>
/// Initializes a new instance of the <see cref="CProcess"/> class.
/// </summary>
/// <param name="Server">Dereferneced CServer pointer</param>
CProcess::CProcess(CServer *Server) {
    this->p = Server;
}

/// <summary>
/// Finalizes an instance of the <see cref="CProcess"/> class.
/// </summary>
CProcess::~CProcess() {
}

/// <summary>
/// Process packet data then pass to specific processor method
/// </summary>
/// <param name="TheData">Packet data</param>
/// <param name="playerId">Player UID</param>
void CProcess::ProcessData(char *TheData, int playerId) {
    //  TheData[0] identifies the packet type
    switch((unsigned char)TheData[0]) {
        //  cmVersion: Apply sCMVersion structure to TheData[1]
        case cmVersion:
            this->ProcessVersion((sCMVersion *)&TheData[1], playerId);
            break;

        // Packet: cmLogin
        case cmLogin:
            this->ProcessLogin((sCMLogin *)&TheData[1], playerId);
            break;

        // Packet: cmNewAccount
        case cmNewAccount:
            this->ProcessNewAccount((sCMNewAccount *)&TheData[1], playerId);
            break;

        // Packet: cmChatMessage
        case cmChatMessage:
            this->p->Send->SendChatMessage((unsigned char)playerId, &TheData[1], 0);
            break;

        // Packet: cmGlobal
        case cmGlobal:
            this->p->Send->SendChatMessage((unsigned char)playerId, &TheData[1], 1);
            break;

        // Packet: cmWalkie
        case cmWalkie:
            this->p->Send->SendChatMessage((unsigned char)playerId, &TheData[1], 2);
            break;

        // Packet: cmSetState
        case cmSetState:
            this->ProcessSetState(&TheData[1], playerId);
            break;

        // Packet: cmJobApp
        case cmJobApp:
            this->ProcessJobApp(&TheData[1], playerId);
            break;

        // Packet: cmJobCancel
        case cmJobCancel:
            this->ProcessCancelJob(&TheData[1], playerId);
            break;

        // Packet: cmHireAccept
        case cmHireAccept:
            this->ProcessAccept(&TheData[1], playerId);
            break;

        // Packet: cmHireDecline
        case cmHireDecline:
            this->ProcessDeny(playerId);
            break;

        // Packet: cmBuild
        case cmBuild:
            this->ProcessBuild(&TheData[1], playerId);
            break;

        // Packet: cmItemUp
        case cmItemUp:
            this->ProcessItemUp((sCMItem *)&TheData[1], playerId);
            break;

        // Packet: cmItemDrop
        case cmItemDrop:
            this->ProcessItemDrop((sCMItem *)&TheData[1], playerId);
            break;

        // Packet: cmHitObject
        case cmHitObject:
            this->ProcessHitObject((sCMItem *)&TheData[1], playerId);
            break;

        // Packet: cmComms
        case cmComms:

            // If the player is hiring,
            if (this->p->City[this->p->Player[playerId]->City]->hiring > -1) {
                TheData[0] = playerId;
                this->p->Winsock->SendData(this->p->City[this->p->Player[playerId]->City]->hiring, smComms, TheData, (int)strlen(&TheData[1]) + 1);
            }
            break;

        // Packet: cmFired
        case cmFired:

            // If the player is the one getting fired, leave
            if ( (this->p->Player[playerId]->Mayor == 1) && (this->p->Player[playerId]->City == this->p->Player[TheData[1]]->City) ) {
                this->p->Player[TheData[1]]->LeaveGame(true);
                this->p->Winsock->SendData(TheData[1], smFired, " ");
            }
            break;

        // Packet: cmUpdate
        case cmUpdate:
            this->ProcessUpdate((sCMUpdate *)&TheData[1], playerId);
            break;

        // Packet: cmShoot
        case cmShoot:
            this->ProcessShot((sCMShot *)&TheData[1], playerId);
            break;

        // Packet: cmDemolish
        case cmDemolish:
            this->ProcessDemolish((sCMDemolish *)&TheData[1], playerId);
            break;

        // Packet: cmBan
        case cmBan:
            this->ProcessBan(&TheData[1], playerId);
            break;

        // Packet: cmMedKit
        case cmMedKit:
            this->ProcessMedKit((int *)&TheData[1], playerId);
            break;

        // Packet: cmCloak
        case cmCloak:
            this->ProcessCloak((int *)&TheData[1], playerId);
            break;

        // Packet: cmCrash
        case cmCrash:
            this->ProcessCrash(&TheData[1], playerId);
            break;

        // Packet: cmNextStep
        case cmNextStep:
            this->ProcessNextStep(TheData[1], playerId);
            break;

        // Packet: cmSuccessor
        case cmSuccessor:
            this->ProcessSuccessor(TheData[1], playerId);
            break;

        // Packet: cmAdmin
        case cmAdmin:
            this->ProcessAdmin((sCMAdmin *)&TheData[1], playerId);
            break;

        // Packet: cmIsHiring
        case cmIsHiring:
            this->ProcessIsHiring(TheData[1], playerId);
            break;

        // Packet: cmDeath
        case cmDeath:
            this->ProcessDeath(&TheData[1], playerId);
            break;

        // Packet: cmTCPPing
        case cmTCPPing:
            this->p->Winsock->SendData(playerId, smTCPPong, " ");
            break;

        // Packet: cmRequestSector
        case cmRequestSector:
            this->ProcessRequestSector((sCMSector *)&TheData[1], playerId);
            break;

        // Packet: cmEditAccount
        case cmEditAccount:
            this->ProcessEditAccount((sCMLogin *)&TheData[1], playerId);
            break;

        // Packet: cmAccountUpdate
        case cmAccountUpdate:
            this->ProcessUpdateAccount((sCMNewAccount *)&TheData[1], playerId);
            break;

        // Packet: cmRecover
        case cmRecover:
            this->ProcessRecover(&TheData[1], playerId);
            break;

        // Packet: cmRightClickCity
        case cmRightClickCity:
            this->ProcessRightClickCity(TheData[1], playerId);
            break;

        // Packet: cmRequestInfo
        case cmRequestInfo:
            this->ProcessRequestInfo(playerId);
            break;

        // Packet: cmChangeNews
        case cmChangeNews:

            // If the player is an admin, change the news
            if (this->p->Player[playerId]->isAdmin()) {
                this->p->ChangeNews(&TheData[1]);
            }
            break;

        // Packet: cmSetMayor
        case cmSetMayor:

            // If the player is in game, in the city, and mayor,
            if ( (this->p->Player[TheData[1]]->isInGame())
                &&
                (this->p->Player[TheData[1]]->City == this->p->Player[playerId]->City)
                &&
                (this->p->Player[playerId]->Mayor)
                ) {

                // Un-mayor this player and mayor the other guy
                this->p->Player[TheData[1]]->setMayor(true);
                this->p->Player[playerId]->setMayor(false);
            }
            break;

        // Packet: cmRefreshList
        case cmRefreshList:
            this->p->Send->SendCityList(playerId);
            break;

        // Packet: cmMiniMap
        case cmMiniMap:
            this->p->Send->SendMiniMap(playerId);
            break;

        // Packet: cmAdminEditRequest
        case cmAdminEditRequest:
            this->ProcessAdminEditRequest(playerId, (sCMAdminEditRequest *)&TheData[1]);
            break;

        // Packet: cmAdminEdit
        case cmAdminEdit:
            this->ProcessAdminEdit(playerId, (sCMAdminEdit *)&TheData[1]);
            break;

        // Packet: cmClickPlayer
        case cmClickPlayer:
            this->ProcessClickPlayer(playerId, TheData[1]);
            break;

        // Packet: cmChangeTank
        case cmChangeTank:
            this->ProcessChangeTank(playerId, TheData[1]);
            break;

        // Packet: cmWhisper
        case cmWhisper:
            this->p->Send->SendWhisper(playerId, (sCMWhisper *)&TheData[1]);
            break;

        // Packet: cmAutoBuild
        case cmAutoBuild:
            this->ProcessAutoBuild(playerId, (sCMAutoBuild *)&TheData[1]);
            break;

        // Packet: cmCheatCheck
        case cmCheatCheck:
            this->ProcessCheatCheck(playerId, (sCMCheatCheck *)&TheData[1]);
            break;
            
        // Packet: cmStartingCity
        case cmStartingCity:
            this->ProcessStartingCity(playerId);
            break;

        // Packet: cmChangeStartingCity
        case cmChangeStartingCity:
            this->ProcessChangeStartingCity(playerId, (sCMStartingCity *)&TheData[1]);
            break;
    }
} 

/***************************************************************
 * Function:	ProcessBuild
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessBuild(char *TheData, int Index) {
    sCMBuild *data = (sCMBuild *)TheData;
    sSMBuild bd;
    bool allowBuild;
    CPlayer* player = this->p->Player[Index];
    CCity* city = this->p->City[player->City];

    // If the player is not an admin, verify that the build request is valid
    if (player->isAdmin() == false ) { 

        // If the player is not in game, return
        if (player->State != State_Game) {
            return;
        }

        // If the player is not Mayor, return
        if (! player->Mayor) {
            return;
        }

        // If the player is dead, return
        if (player->isDead) {
            return;
        }

        // If isAutoBuild, but city already has building, return
        if (data->isAutoBuild && (city->canBuild[data->type - 1] == 2)) {
            return;
        }

        // If isAutoBuild is false, and city can't make that building, return
        if ((data->isAutoBuild==false) && (city->canBuild[data->type - 1] == 0)) {
            return;
        }
    }

    // Build is valid

    // Subtract the cost of the building,
    city->cash -= COST_BUILDING;

    // Create the building with the next ID, add it to the server linked list
    bd.City = player->City;
    bd.count = 0;
    bd.type = data->type;
    bd.x = data->x;
    bd.y = data->y;
    p->Build->bldID++;
    bd.id = p->Build->bldID++;
    bd.pop = 0;
    p->Build->newBuilding(data->x,data->y,data->type,player->City, bd.id);

    // HACK: If the ID is over 30000, cycle around to 1
    if (p->Build->bldID > 30000) {
        p->Build->bldID = 1;
    }

    // Tell everyone in the sector about the new building
    p->Send->SendSectorArea(data->x*48, data->y*48,(unsigned char)smNewBuilding,(char *)&bd,sizeof(bd));

    // If the building is not a House,
    if (! CBuilding::isHouse(data->type)) {

        // Set canBuild for the building type to "already has"
        city->setCanBuild((unsigned char)data->type - 1, 2, false);

        // If the building is a Research,
        if (CBuilding::isResearch(data->type)) {

            // If research is not finished, set the research timer to 0
            // The CBuilding.cycle() Research timer will start the research when the building is populated
            if (city->research[(data->type - 3) / 2] != -1) {
                city->research[(data->type - 3) / 2] = 0;
            }
        }
    }
}

/***************************************************************
 * Function:	ProcessJobApp
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessJobApp(char *TheData, int Index) {
    char packet[20];
    CPlayer* player = this->p->Player[Index];
    unsigned char cityIndex = (unsigned char) TheData[0];
    CCity* city;

    // If the city index is less than MAX_CITIES,
    if (cityIndex < MAX_CITIES) {
        city = this->p->City[cityIndex];

        // If the city has no mayor,
        if (city->Mayor == -1) {

            // Add the player to the city as mayor
            player->City = (unsigned char)TheData[0];
            player->StartJoin(); 
            player->setMayor(true);
            packet[0] = Index;
            packet[1] = 68;
            p->Send->SendToChat(smChatCommand, packet, 2); 
        } 

        // Else (city has a mayor)
        else {

            // If the player count is less than MAX_PLAYERS_PER_CITY
            if (city->PlayerCount() < MAX_PLAYERS_PER_CITY) {
                
                // If the mayor is not in an interview,
                if (city->hiring == -1) {

                    // Set the player as the mayor's interviewee
                    city->hiring = Index;
                    player->State = State_Apply;
                    packet[0] = Index;
                    p->Winsock->SendData(city->Mayor,smMayorHire,packet,1);
                    p->Winsock->SendData(Index, smInterview, " ", 1);
                    packet[0] = Index;
                    packet[1] = 68;
                    p->Send->SendToChat(smChatCommand, packet, 2); 
                } 

                // Else (mayor is in an interview), send busy signal
                else {
                    p->Winsock->SendData(Index,smMayorInInterview," ",1);
                }
            }

            // Else (player count not less than MAX_PLAYERS_PER_CITY), send busy signal
            else {
                p->Winsock->SendData(Index,smMayorInInterview," ",1);
            }
        }
    }
}

/***************************************************************
 * Function:	ProcessCancelJob
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessCancelJob(char *TheData, int Index) {

    // For each possible city,
    for (int j = 0; j < MAX_CITIES; j++) {

        // If that city is interviewing this player,
        if (p->City[j]->hiring == Index) {

            // Reset the interviewee and tell the mayor the interviewee left
            p->City[j]->hiring = -1;
            p->Winsock->SendData(p->City[j]->Mayor,smInterviewCancel," ");
        }
    }
}


/***************************************************************
 * Function:	ProcessAccept
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessAccept(char *TheData, int Index) {
    CPlayer* player = this->p->Player[Index];
    CCity* city = this->p->City[player->City];
    CPlayer* applicant;

    // If the player is mayor,
    if (player->Mayor) {

        // If the player is hiring someone,
        if (city->hiring > -1) {
            applicant = this->p->Player[city->hiring];

            // If the interviewee is connected,
            if (applicant->Socket) {

                // Add the interviewee
                applicant->City = player->City;
                applicant->setMayor(false);
                applicant->StartJoin();

                // Return the mayor to the game
                player->State = State_Game;
            }
            city->hiring = -1;
        }
    }
}

/***************************************************************
 * Function:	ProcessDeny
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessDeny(int Index) {
    CPlayer* player = this->p->Player[Index];
    CCity* city = this->p->City[player->City];

    // If the player is mayor,
    if (player->Mayor) {

        // If the player is hiring someone,
        if (city->hiring > -1) {

            // If the interviewee is connected,
            if (p->Player[city->hiring]->Socket) {

                // Tell the interviewee to buzz off
                p->Winsock->SendData(city->hiring,smMayorDeclined," ",1);
            }
            city->hiring = -1;
        }
    }
}

/***************************************************************
 * Function:	ProcessUpdate
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessUpdate(sCMUpdate *data, int Index) {
    sSMUpdate update;
    sSMStateGame game;
    CPlayer* player = this->p->Player[Index];

    // If the player is dead, return
    if (player->isDead == true) {
        return;
    }

    // If the player is within 300 x and 300 y of the update position, and the Movement timer is up,
    if (
        (abs(int(data->x - player->X)) < 300)
        &&
        (abs(int(data->y - player->Y)) < 300)
        &&
        ((player->lastMove + 3000) > p->Tick)
        ) {

        // Reset the Movement timer
        player->lastMove = p->Tick;

        // HACK: Cap data->x and data->y at 24500
        if (data->x > 24576) {
            data->x = 24500;
        }
        if (data->y > 24576) {
            data->y = 24500;
        }

        // Set the player's position to the location
        player->X = update.x = data->x;
        player->Y = update.y = data->y;

        // Send the update to the radar, minus the player
        update.turn = data->turn;
        update.id = (unsigned char)Index;
        update.dir = data->dir;
        update.move = data->move;
        p->Send->SendRadarNotIndex(Index,smUpdate,(char *)&update,sizeof(update));
    }

    // Else (player not within 300x300),
    else {

        // Warp the player
        // ???
        player->lastMove = p->Tick;
        game.City = player->City;
        game.x = (unsigned short)player->X;
        game.y = (unsigned short)player->Y;
        p->Winsock->SendData(Index, smWarp,(char *)&game,sizeof(game));
    }
}

/***************************************************************
 * Function:	ProcessItemUp
 *
 * @param User
 **************************************************************/
void CProcess::ProcessItemUp(sCMItem *data, int Index) {
    CItem *item;
    sSMItem msg;
    sCMItem itemPacket;
    CPlayer* player = this->p->Player[Index];

    // If the player is dead, return
    if (player->isDead == true) {
        return;
    }

    // If the item is not found, return
    item = p->Item->findItem(data->id);
    if (! item) {
        return;
    }

    // If someone already is holding the item, return
    if (item->holder != -1) {
        return;
    }

    // If the player isn't an admin 
    if (! player->isAdmin()) {

        // If the item doesn't belong to the player's city, return
        if (item->City != player->City) {
            return;
        }
    }

    // Pick up the item
    item->holder = Index;
    msg.id = (unsigned short)data->id;

    // Tell the player he picked it up
    itemPacket.id = item->id;
    itemPacket.active = data->active;
    itemPacket.type = item->type;
    p->Winsock->SendData(Index,smPickedUp, (char *)&itemPacket, sizeof(itemPacket));

    // Tell the sector the item is gone
    p->Send->SendSectorArea(item->x*48, item->y*48 ,smRemItem, (char *)&msg, sizeof(msg));
}

/***************************************************************
 * Function:	ProcessItemDrop
 *
 * @param User
 **************************************************************/
void CProcess::ProcessItemDrop(sCMItem *data, int Index) {
    CItem *item;
    sCMItem *itm = (sCMItem *)data;
    sCMItem itemPacket;
    CPlayer* player = this->p->Player[Index];

    // If the player is dead, return
    if (player->isDead) {
        return;
    }

    // If the item is not found, return
    item = p->Item->findItem(itm->id);
    if (! item) {
        return;
    }

    // If someone else is holding this item, return
    if (item->holder != Index) {
        return;
    }

    // Tell the player to drop the item
    itemPacket.id = item->id;
    itemPacket.active = data->active;
    itemPacket.type = item->type;
    p->Winsock->SendData(Index, smDropped, (char *)&itemPacket, sizeof(itemPacket));

    // Activate and drop the item
    item->active = data->active;
    item->drop(player->getTileX(), player->getTileY(), Index);
}

/// <summary>
/// Compare packet version with server version for player Id
/// </summary>
/// <param name="vers">The vers.</param>
/// <param name="Index">The index.</param>
void CProcess::ProcessVersion(sCMVersion *vers, int Index) {
    //  Initialize CPlayer object
    CPlayer* player = this->p->Player[Index];
    //  Verify received "vers" against VERSION constant
    //  Upon failure, send data to Player; otherwise, version matches
    if (strcmp(vers->Version, VERSION) != 0) {
        //  Using CServer object, send player error message (enum)
        p->Winsock->SendData(Index, smError, "F");
        //  Print error message to console
        cout << "Invalid Version " << vers->Version << " from " << player->IPAddress << "\n";
    } else {
        //  Verify ban status on player's IP address
        //  Upon true, send player banned message
        //  Upon false, save UniqID and State with CPlayer object
        if (p->Account->CheckBan(vers->UniqID, player->IPAddress) == 1) {
            p->Winsock->SendData(Index, smError, "G");
        } else  {
            player->UniqID = vers->UniqID;
            player->State = State_Verified;
        }
    }
}

/// <summary>
/// Processes the login.
/// </summary>
/// <param name="login">The login.</param>
/// <param name="Index">The index.</param>
void CProcess::ProcessLogin(sCMLogin *login, int Index) {
    int CorrectPassword;
    int foundmatch;
    CPlayer* player = this->p->Player[Index];
    CPlayer* playerToCompare;

    // If the player's state is Verified (past version check and ban check),
    if (player->State == State_Verified)  {

        // If the account exists,
        if (p->Account->CheckAccount(login->User) == 1) {

            // If the password is correct,
            CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
            if (CorrectPassword == 2) {

                // If the user isn't banned,
                if (p->Account->CheckAccountBan(login->User) == 0) {
                    foundmatch = 0;

                    // HACK: FIX "- 1"
                    // TODO: FIX "- 1"
                    // For each possible player,
                    for (int j = 0; j < (MAX_PLAYERS-1); j++) {
                        playerToCompare = this->p->Player[j];
                        
                        // If the account is already in use, clear the player and set foundmatch=1
                        if (p->PlatformCaseCompare(login->User, playerToCompare->Name.c_str()) == 0) {
                            playerToCompare->Clear();
                            foundmatch = 1;
                        }

/*
                        // If the CPU is already connected, clear the player and set foundmatch=2
                        if (p->PlatformCaseCompare(player->UniqID.c_str(), playerToCompare->UniqID.c_str()) == 0 && Index != j) {
                            playerToCompare->Clear();
                            foundmatch = 2;
                        }
*/
                    }

                    // If the account is already logged in,
                    if (foundmatch == 1) {

                        // Send account already in use error
                        cout << "Multilog from " << login->User << endl;
                        p->Winsock->SendData(Index, smError, "E");
                    }

                    // Else if the computer is already connected,
/*
                    else if (foundmatch == 2) {

                        // Send MultiCPU error
                        cout << "MultiCPU log from " << login->User << endl;
                        p->Winsock->SendData(Index, smError, "H");
                    }
*/

                    // Else (no Multilog, no Multicpu), log the player in
                    else {	
                        player->LoggedIn(login->User);
                    }
                }

                // Else (ban check failed), send banned message
                else {
                    cout << "Banned log from " << login->User << endl;
                    p->Winsock->SendData(Index, smError, "G");
                } //Banned
            }

            // Else if incorrect password, send incorrect password message
            else if (CorrectPassword == 0) {
                p->Winsock->SendData(Index, smError, "B");
            }
        }

        // Else (account doesn't exist), send account doesn't exist message
        else {
            p->Winsock->SendData(Index, smError, "C");
        }
    }
}

/***************************************************************
 * Function:	ProcessNewAccount
 *
 * @param newaccount
 * @param Index
 **************************************************************/
void CProcess::ProcessNewAccount(sCMNewAccount *newaccount, int Index) {
    int CreateAccount = p->Account->NewAccount(newaccount->User, newaccount->Pass, newaccount->Email, newaccount->FullName, newaccount->Town, newaccount->State);

    // If the account was created, send success
    if (CreateAccount == 1) {
        p->Winsock->SendData(Index, smError, "A");
    }

    // Else if email address in use, send email in use
    else if (CreateAccount == -1) {
        p->Winsock->SendData(Index, smError, "K");
    }

    // Else if name in use, send name in use
    else if (CreateAccount == 0) {
        p->Winsock->SendData(Index, smError, "D");
    }
}

/***************************************************************
 * Function:	ProcessSetState
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessSetState(char *TheData, int Index) {
    char tmpString[5];
    CPlayer* player = this->p->Player[Index];

    // If the player is Verified,
    if (player->State >= State_Verified) {
        memset(tmpString, 0, 5);
        tmpString[0] = (unsigned char)Index;

        switch(TheData[0]) {
            
            //State A/Chat
            case 65: 
            
            //State B/Daily Bullet
            case 66: 

            //State C/City List
            case 67: 
                if (player->State == State_Game) {
                    player->LeaveGame(true);
                }
                p->Send->SendMeetingRoom(Index);
                break;
            
            //State D/Disconnected
            case 68: 
                strcpy(&tmpString[1], "D"); 
                player->Clear(); 
                p->Send->SendToChat(smChatCommand, tmpString); 
                break;
        }
    }
}

/***************************************************************
 * Function:	ProcessShot
 *
 * @param User
 * @param Index
 **************************************************************/
void CProcess::ProcessShot(sCMShot *data, int Index) {
    sSMShot shotsfired;
    CPlayer* player = this->p->Player[Index];

    // If the player is not an admin,
    if (! player->isAdmin()) {

        // If the Shot timer is not up, return
        if (p->Tick <= (player->lastShot + 500)) {
            return;
        }
    }

    // Reset the Shot timer
    player->lastShot = p->Tick;

    // Create a new shot
    shotsfired.id = Index;
    shotsfired.type = data->type;
    shotsfired.x = data->x;
    shotsfired.y = data->y;
    shotsfired.dir = data->dir;
    
    if(shotsfired.type == 3) {
        p->Bullet->newBullet(data->x,data->y,data->type,data->dir+4, Index);
        p->Bullet->newBullet(data->x,data->y,data->type,data->dir, Index);
        p->Bullet->newBullet(data->x,data->y,data->type,data->dir-4, Index);
    }
    else {
        p->Bullet->newBullet(data->x,data->y,data->type,data->dir, Index);
    }


    // Tell everyone but the player about the new shot
    p->Send->SendRadarNotIndex(Index, smShoot, (char *)&shotsfired, sizeof(sSMShot));

/*
    // Possible replacement for code above, once SHOT_TYPE_LASER and SHOT_TYPE_ROCKET are defined
    bool canShoot = false;

    if (p->Player[Index]->isAdmin()) {
        canShoot == true;
    }
    else if (SHOT_TYPE_LASER == data->type) {
        if( p->Player[Index]->lastShot + TIMER_SHOOT_LASER < p->Tick ) {
            canShoot = true;
        }
    }
    else if (SHOT_TYPE_ROCKET == data->type) {
        if( p->Player[Index]->lastShot + TIMER_SHOOT_ROCKET < p->Tick ) {
            canShoot = true;
        }
    }

    if (canShoot) {
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
*/

}

/***************************************************************
 * Function:	ProcessDeath
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessDeath(char *TheData, int Index) {
    char packet[3];
    CPlayer* player = this->p->Player[Index];
    CPlayer* playerToGivePoints;
    int cityIndexThatKilledPlayer = TheData[0];

    // If the player is in game,
    if (player->State == State_Game) {

        // Add the death
        p->Account->AddDeath(Index);

        // Tell everyone about the death,
        packet[0] = (unsigned char)Index;
        packet[1] = 0;
        packet[2] = cityIndexThatKilledPlayer;
        p->Send->SendGameAllBut(-1, smDeath, packet, 3);
        cout << player->Name << " has died" << endl;
        player->isDead = true;
        player->timeDeath = p->Tick;

        // Delete the player's items
        p->Item->deleteItemsByPlayer(Index);

        // If the player has more than 100 points,
        if (player->Points > 100) {

            // Subtract two points
            p->Account->Sub2Points(Index);

            // If the player's not in your city,
            if (player->City != cityIndexThatKilledPlayer) {

                // For each possible player,
                for (int j = 0; j < MAX_PLAYERS; j++) {
                    playerToGivePoints = this->p->Player[j];

                    // If the player is in that city and in game,
                    if ((playerToGivePoints->isInGame()) && (playerToGivePoints->City == cityIndexThatKilledPlayer)) {

                        // Give that player two points
                        p->Account->AddPoints(j, 2);
                    }
                }
            }
        }
    }
}

/***************************************************************
 * Function:	ProcessMedKit
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessMedKit(int *data, int Index) {
    CItem* itm = p->Item->findItem(*data);
    CPlayer* player = this->p->Player[Index];
    stringstream ss;

    // If the item is found, the player holds it, and it's a medkit,
    if ((itm) && (itm->type == ITEM_TYPE_MEDKIT) && (itm->holder == Index)) {

        // Delete the medkit
        p->Item->delItem(itm);
    }

    else {
        ss << "Illegal Medkit: " << player->Name << " " << itm;
        cout << ss.str();
        this->p->Log->logServerError(ss.str());
    }
}

/***************************************************************
 * Function:	ProcessCloak
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessCloak(int *data, int Index) {
    CItem *itm = this->p->Item->findItem(*data);
    CPlayer* player = this->p->Player[Index];
    char packet[3];

    // If the item is found, the player holds it, and it's a Cloak,
    if (itm) {
        if ( (itm->type == ITEM_TYPE_CLOAK) && (itm->holder == Index)) {

            // Use and delete the Cloak
            this->p->Item->delItem(itm);
            player->setCloak(true);

            // Tell everyone else about the cloak
            packet[0] = (unsigned char)Index;
            packet[1] = 0;
            packet[2] = data[0];
            this->p->Send->SendGameAllBut(-1, smCloak, packet, 3);

        }
    }
}

/***************************************************************
 * Function:	ProcessDemolish
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessDemolish(sCMDemolish *data, int Index) {
    CBuilding *bld;
    CPlayer* player = this->p->Player[Index];

    // If the player is dead, return
    if (player->isDead == true) {
        return;
    }

    // If the building is not found, return
    bld = this->p->Build->findBuilding(data->id);
    if (! bld) {
        return;
    }

    // If the building is not the player's building, return
    if(bld->City != player->City) {
        return;
    }

    // Delete building
    this->p->Build->delBuilding(bld);
}

/***************************************************************
 * Function:	ProcessHitObject
 *
 * @param data
 * @param Index
 **************************************************************/
void CProcess::ProcessHitObject(sCMItem *data, int Index) {
    CItem *item;

    // If the item is found, delete it
    item = p->Item->findItem(data->id);
    if (item) {
        p->Item->delItem(item);
    }
}

/***************************************************************
 * Function:	ProcessCrash
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessCrash(char *TheData,int Index) {
    std::string theerror;

    theerror.clear();
    theerror.append(TheData);
    p->Log->logClientError(theerror, Index);
}

/***************************************************************
 * Function:	ProcessNextStep
 *
 * @param TheStep
 * @param Index
 **************************************************************/
void CProcess::ProcessNextStep(char TheStep, int Index) {
    CPlayer* player = this->p->Player[Index];

    switch (TheStep) {

        // A Ready to start receiving data
        case 65: 
            player->JoinGame();
            p->Winsock->SendData(Index, smNextStep, "B");
            break;

        //B Refreshed Area
        case 66: 
            p->Winsock->SendData(Index, smNextStep, "C");
            break;

        //C Done
        case 67:
            break;
    }
}

/***************************************************************
 * Function:	ProcessSuccessor
 *
 * @param Successor
 * @param Index
 **************************************************************/
void CProcess::ProcessSuccessor(char successorIndex, int Index) {
    CPlayer* player = this->p->Player[Index];
    CPlayer* successor = this->p->Player[successorIndex];

    // If the player is not mayor, return
    if (! player->Mayor) {
        return;
    }

    // If the successor is not in game, return
    if (! successor->isInGame()) {
        return;
    }

    // If the player and successor are in different cities, return
    if (player->City != successor->City) {
        return;
    }

    // Set the successor
    this->p->City[player->City]->Successor = successorIndex;
}

/***************************************************************
 * Function:	ProcessAdmin
 *
 * @param admin
 * @param Index
 **************************************************************/
void CProcess::ProcessAdmin(sCMAdmin *admin, int Index) {
    CPlayer* player = this->p->Player[Index];
    CPlayer* playerTarget;
    CCity* cityTarget;
    sSMAdmin sadmin;
    string LogAdminString;
    ostringstream convert;
    string queryString;
    int tmpID;
    CItem *itm;
    sCMItem PickUp;
    sSMItem msg;
    sSMBan ban;

    // If the player is not an admin, return
    if (! player->isAdmin()) {
        this->p->Log->logClientError("Attempted Admin Command", Index);
        return;
    }

    // Switch on the Admin command
    switch (admin->command) {
        
        // Command: KICK
        case 1:
            playerTarget = this->p->Player[admin->id];

            sadmin.adminIndex = Index;
            sadmin.command = 1;
            sadmin.id = admin->id;
            
            // If the player is in game, kick the player
            if (playerTarget->isInGame()) {
                LogAdminString = "Kick ";
                LogAdminString += playerTarget->Name;
                p->Log->logAdmin(LogAdminString, Index);
                cout << "Kick::" << playerTarget->Name << "::" << playerTarget->Name << endl;
                playerTarget->LeaveGame(true);
                p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
                p->Winsock->SendData(admin->id, smKicked, " ");
            }
            break;

        // Command: JOIN CITY
        case 2:

            // If the city is a valid city,
            if (admin->id >= 0 && admin->id < MAX_CITIES) {
                cityTarget = this->p->City[admin->id];

                LogAdminString = "Join City ";		
                convert << admin->id;
                LogAdminString += convert.str();
                p->Log->logAdmin(LogAdminString, Index);
                player->LeaveGame(true);
                player->City = admin->id;

                // If the city has no mayor, set the admin to mayor
                if (cityTarget->Mayor == -1) {
                    player->setMayor(true, false);
                }
                player->StartJoin();
            }
            break;

        // Command: WARP
        case 3:
            playerTarget = this->p->Player[admin->id];

            // Warp the player
            LogAdminString = "Warp ";
            LogAdminString += playerTarget->Name;
            p->Log->logAdmin(LogAdminString, Index);
            cout << "Warp::" << player->Name << "::" << playerTarget->Name << endl;
            player->X = playerTarget->X;
            player->Y = playerTarget->Y;
            break;

        // Command: SUMMON
        case 4:
            playerTarget = this->p->Player[admin->id];

            // Summon the player
            LogAdminString = "Summon ";
            LogAdminString += playerTarget->Name;
            p->Log->logAdmin(LogAdminString, Index);
            cout << "Summon::" << player->Name << "::" << playerTarget->Name << endl;
            playerTarget->X = player->X;
            playerTarget->Y = player->Y;
            break;

        // Command: BAN
        case 5:
            playerTarget = this->p->Player[admin->id];

            queryString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
            queryString += playerTarget->Name;
            queryString += "', '";
            queryString += playerTarget->IPAddress;
            queryString += "', '";
            queryString += player->Name;
            queryString += "', '";
            queryString += playerTarget->UniqID;
            queryString += "');";
            p->Database->Database.execDML(queryString.c_str());

            sadmin.adminIndex = Index;
            sadmin.command = 5;
            sadmin.id = admin->id;

            // If the player is in game, boot the player
            if (playerTarget->isInGame()) {
                LogAdminString = "Ban " + playerTarget->Name;
                p->Log->logAdmin(LogAdminString, Index);
                cout << "Ban::" << player->Name << "::" << playerTarget->Name << endl;
                playerTarget->LeaveGame(true);
                p->Send->SendGameAllBut(admin->id, smAdmin, (char *)&sadmin, sizeof(sadmin));
                p->Winsock->SendData(admin->id, smKicked, " ");
            }
            break;

        // Command: SHUTDOWN
        case 6:
            p->Log->logAdmin("Shutdown", Index);
            cout << "Shutdown::" << player->Name << endl;
            p->running = 0;
            break;

        // Command: SPAWN ITEM
        case 7:

            // Spawn the item
            LogAdminString = "Spawn Item ";
            convert << admin->id;
            LogAdminString += convert.str();
            p->Log->logAdmin(LogAdminString, Index);
            cout << "Spawn Item::" << player->Name << "::" << admin->id << endl;

            // HACK: cap itemID at 30000
            tmpID = p->Item->itmID++;
            if (p->Item->itmID > 30000) {
                p->Item->itmID = 1;
            }
            
            // Create the item, add it to the user
            itm = p->Item->newItem(0, 0, admin->id, player->City, tmpID);
            itm->holder = Index;
            
            PickUp.id = itm->id;
            PickUp.active = itm->active;
            PickUp.type = itm->type;

            // Tell everone you created, picked up, and removed the item
            msg.id = itm->id;
            msg.x = 0;
            msg.y = 0;
            msg.City = player->City;
            msg.active = 0;
            msg.type = (unsigned char)admin->id;

            p->Winsock->SendData(Index,smAddItem,(char *)&msg, sizeof(msg));
            p->Winsock->SendData(Index,smPickedUp, (char *)&PickUp, sizeof(PickUp));
            p->Winsock->SendData(Index,smRemItem, (char *)&msg, sizeof(msg));
            break;

        // Command: GET BANS
        case 8:

            // Get the bans
            p->Log->logAdmin("Request Bans", Index);
            queryString = "SELECT * FROM tBans";
            p->Database->Query = p->Database->Database.execQuery(queryString.c_str());

            // For each ban,
            while (!p->Database->Query.eof()) {

                // Tell the admin about the ban
                memset(&ban, 0, sizeof(ban));
                strcpy(ban.Account, p->Database->Query.getStringField("Account"));
                strcpy(ban.IPAddress, p->Database->Query.getStringField("IPAddress"));
                strcpy(ban.Reason, p->Database->Query.getStringField("Reason"));
                p->Winsock->SendData(Index, smBan, (char *)&ban, sizeof(ban));

                p->Database->Query.nextRow();
            }

            p->Database->Query.finalize();
            break;

        // Command: UNBAN
        case 9:

            // Get the bans
            p->Log->logAdmin("Unban Attempt", Index);
            queryString = "SELECT * FROM tBans";
            p->Database->Query = p->Database->Database.execQuery(queryString.c_str());

            // For each ban,
            for (int i = 0; i < admin->id; i++) {
                if (!p->Database->Query.eof()) {
                    p->Database->Query.nextRow();
                }
                else {
                    p->Database->Query.finalize();
                    return;
                }
            }

            if (!p->Database->Query.eof()) {
                try {
                    // Delete the ban
                    queryString = "DELETE FROM tBans WHERE Account = '";
                    queryString += p->Database->Query.getStringField("Account");
                    queryString += "';";
                    p->Database->Query.finalize();
                    p->Log->logAdmin(queryString, Index);
                    p->Database->Database.execDML(queryString.c_str());
                }
                catch (CppSQLite3Exception& e) {
                    cerr << e.errorCode() << ":" << e.errorMessage() << endl;
                    return;
                }
            }
            else {
                p->Database->Query.finalize();
                return;
            }
            break;

        // Command: REQUEST NEWS
        case 10: 
            p->Log->logAdmin("Request News", Index);
            p->Send->SendAdminNews(Index);
            break;

        // Command: UPDATE NEWS
        case 11:
            p->Log->logAdmin("Update News", Index);
            break;

    }
}

/***************************************************************
 * Function:	ProcessBan
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessBan(char *TheData, int Index) {
    CPlayer* player = this->p->Player[Index];
    string DMLString;

    DMLString = "INSERT INTO tBans (Account, IPAddress, Reason, UniqID) VALUES ('";
    DMLString += player->Name;
    DMLString += "', '";
    DMLString += player->IPAddress;
    DMLString += "', '";
    DMLString += TheData;
    DMLString += "', '";
    DMLString += player->UniqID;
    DMLString += "');";
    p->Database->Database.execDML(DMLString.c_str());
    cout << "Ban::" << player->Name << endl;
}

/***************************************************************
 * Function:	ProcessIsHiring
 *
 * @param NotHiring
 * @param Index
 **************************************************************/
void CProcess::ProcessIsHiring(char NotHiring, int Index) {
    CPlayer* player = this->p->Player[Index];

    // If the player is Mayor and in game, set notHiring
    if (player->Mayor && player->isInGame()) {
        p->City[player->City]->notHiring = NotHiring;
    }
}

/***************************************************************
 * Function:	ProcessRequestSector
 *
 * @param sector
 * @param Index
 **************************************************************/
void CProcess::ProcessRequestSector(sCMSector *sector, int Index) {
    p->Send->SendSector(Index, sector->x, sector->y);
}

/***************************************************************
 * Function:	ProcessEditAccount
 *
 * @param login
 * @param Index
 **************************************************************/
void CProcess::ProcessEditAccount(sCMLogin *login, int Index) {
    int CorrectPassword;
    int foundmatch = 0;
    CPlayer* player = this->p->Player[Index];

    // If the player is at Verified state,
    if (player->State == State_Verified) {

        // If the account exists,
        if (p->Account->CheckAccount(login->User) == 1) {

            // If the password is correct,
            CorrectPassword = p->Account->CheckPassword(login->User, login->Pass);
            if (CorrectPassword == 2) {

                // If the player is not banned,
                if (p->Account->CheckAccountBan(login->User) == 0) {

                    // For each possible player,
                    // HACK: FIX "- 1"
                    // TODO: FIX "- 1"
                    for (int j = 0; j < (MAX_PLAYERS-1); j++) {

                        // If the account is already in use, set foundmatch=1
                        if (p->PlatformCaseCompare(login->User, p->Player[j]->Name.c_str()) == 0) {
                            foundmatch = 1;
                        }

                        // If the CPU is already connected, set foundmatch=2
                        if (p->PlatformCaseCompare(player->UniqID.c_str(), p->Player[j]->UniqID.c_str()) == 0 && Index != j) {
                            foundmatch = 2;
                        }
                    }

                    // If the account is already logged in, send account already in use error
                    if (foundmatch == 1) {
                        cout << "Multilog from " << login->User << endl;
                        p->Winsock->SendData(Index, smError, "E");
                    }

                    // Else if the computer is already logged in, send computer already in use error
                    else if (foundmatch == 2) {
                        cout << "MultiCPU log from " << login->User << endl;
                        p->Winsock->SendData(Index, smError, "H");
                    }

                    // Else, send successful login and set state to Editing
                    else {
                        player->Name = login->User;
                        p->Account->SendAccountInformation(Index);
                        player->State = State_Editing;
                    }
                }

                // Else (user is banned), send Banned error
                else {
                    cout << "Banned log from " << login->User << endl; p->Winsock->SendData(Index, smError, "G");
                }
            }

            // Else if password is incorrect, send incorrect password error
            else if (CorrectPassword == 0) {
                p->Winsock->SendData(Index, smError, "B");
            }

            // Else (CorrectPassword == 1), allow edit
            // ???
            else  {
                player->Name = login->User;
                p->Account->SendAccountInformation(Index);
                player->State = State_Editing;
            }
        }

        // Else (account does not exist), sent account not found error
        else {
            p->Winsock->SendData(Index, smError, "C");
        }
    }
}

/***************************************************************
 * Function:	ProcessUpdateAccount
 *
 * @param updateaccount
 * @param Index
 **************************************************************/
void CProcess::ProcessUpdateAccount(sCMNewAccount *updateaccount, int Index) {
    int UpdateAccount;
    int CorrectPassword;
    CPlayer* player = this->p->Player[Index];

    // If the player is Editing,
    if (player->State == State_Editing) {

        // Try to update the account
        UpdateAccount = p->Account->UpdateAccount(Index, updateaccount->Pass, updateaccount->Email, updateaccount->FullName, updateaccount->Town, updateaccount->State);

        // If the update succeeded,
        if (UpdateAccount == 1) {

            // Check the password
            CorrectPassword = p->Account->CheckPassword(updateaccount->User, updateaccount->Pass);

            // If the password is correct, log the player in
            if (CorrectPassword == 2) {
                player->LoggedIn(player->Name);
            }

            // If the password is wrong, send wrong password error
            else if (CorrectPassword == 0) {
                p->Winsock->SendData(Index, smError, "B"); 
            }
        }

        // Else, if the update failed, send Email already in use error
        else if (UpdateAccount == -1) {
            p->Winsock->SendData(Index, smError, "K");
        }
    }
}

/***************************************************************
 * Function:	ProcessRecover
 *
 * @param TheData
 * @param Index
 **************************************************************/
void CProcess::ProcessRecover(char *TheData, int Index) {
    int i = p->Account->RecoverAccount(Index, TheData);
    
    // TODO: explain what this does

    // If RecoverAccount returns 0, send L
    if (i == 0) {
        p->Winsock->SendData(Index, smError, "L");
    }
    // Else, send M
    else {
        p->Winsock->SendData(Index, smError, "M");
    }
}

/***************************************************************
 * Function:	ProcessRequestInfo
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessRequestInfo(int Index) {
    CPlayer* player = this->p->Player[Index];
    CCity* city;
    int searchX = player->getTileX();
    int searchY = player->getTileY();
    int myCityIndex = player->City;

    short int selectedCity = 255;

    int bestOrbValue = 0;
    int bestDistance = 255;

    int orbValue;
    int distance;

    // For each possible city,
    for (int i = 0; i < MAX_CITIES; i++) {

        // If the city isn't this player's city,
        if (i != myCityIndex) {

            city = this->p->City[i];

            // If the city is orbable,
            if (city->isOrbable()) {

                // Get the city's orb value and distance from the player's city
                orbValue = city->getOrbValue();
                distance = (int)sqrt((float)((city->x - searchX)^2 + (city->y - searchY)^2));

                // If the city is worth more than the others found so far, select it
                if (orbValue > bestOrbValue) {
                    selectedCity = i;
                    bestDistance = distance;
                    bestOrbValue = orbValue;
                }

                // Else, if the city is worth the same as the max but is closer, select it
                else if ( (orbValue==bestOrbValue) && (distance < bestDistance) ) {
                    selectedCity = i;
                    bestDistance = distance;
                    bestOrbValue = orbValue;
                }
            }
        }
    }

    p->Send->SendInfoButton(Index, selectedCity);
}

/***************************************************************
 * Function:	ProcessRightClickCity
 *
 * @param City
 * @param Index
 **************************************************************/
void CProcess::ProcessRightClickCity(int cityIndex, int Index) {
    CCity* city = this->p->City[cityIndex];
    sSMRightClickCity response;

    // Send the city's building count and orb points
    response.BuildingCount = city->currentBuildingCount;
    response.City = cityIndex;
    response.IsOrbable = city->isOrbable();
    response.Orbs = city->Orbs;
    response.OrbPoints = city->getOrbValue();
    response.UptimeInMinutes = city->getUptimeInMinutes();
    p->Winsock->SendData(Index, smRightClickCity, (char *)&response, sizeof(response));
}

/***************************************************************
 * Function:	ProcessAdminEdit
 *
 * @param Index
 * @param sCMAdminEdit
**************************************************************/
void CProcess::ProcessAdminEdit(int Index, sCMAdminEdit *adminedit) {
    string DMLString;
    std::ostringstream converter;
    sSMPoints pts;
    sSMPlayer response;
    CPlayer *playerEditing = this->p->Player[Index];
    CPlayer *playerToEdit;

    // If the player is an admin,
    if (playerEditing->isAdmin()) {

        // If the player is in a certain set of names,
        if (
            (p->PlatformCaseCompare(playerEditing->Name.c_str(), "Weebo") == 0)
            ||
            (p->PlatformCaseCompare(playerEditing->Name.c_str(), "Vindkast") == 0)
        ) {
            
            // If the account to edit exists,
            if (p->Account->CheckAccount(adminedit->User) == 1) {

                // Create the Update statement
                DMLString = "UPDATE tAccounts SET Deaths = ";
                converter.str("");
                converter << adminedit->Deaths;
                DMLString += converter.str();

                DMLString += ", Orbs = ";
                converter.str("");
                converter << adminedit->Orbs;
                DMLString += converter.str();

                DMLString += ", Assists = ";
                converter.str("");
                converter << adminedit->Assists;
                DMLString += converter.str();

                DMLString += ", Points = ";
                converter.str("");
                converter << adminedit->Points;
                DMLString += converter.str();

                DMLString += ", MonthlyTop20 = ";
                converter.str("");
                converter << adminedit->MonthlyPoints;
                DMLString += converter.str();

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
                converter.str("");
                converter << (int)adminedit->Tank;
                DMLString += converter.str();

                DMLString += ", Tank2 = ";
                converter.str("");
                converter  << (int)adminedit->Tank2;
                DMLString += converter.str();

                DMLString += ", Tank3 = ";
                converter.str("");
                converter  << (int)adminedit->Tank3;
                DMLString += converter.str();

                DMLString += ", Tank4 = ";
                converter.str("");
                converter  << (int)adminedit->Tank4;
                DMLString += converter.str();
                
                DMLString += ", Tank5 = ";
                converter.str("");
                converter  << (int)adminedit->Tank5;
                DMLString += converter.str();
                                
                DMLString += ", Tank6 = ";
                converter.str("");
                converter  << (int)adminedit->Tank6;
                DMLString += converter.str();
                                
                DMLString += ", Tank7 = ";
                converter.str("");
                converter  << (int)adminedit->Tank7;
                DMLString += converter.str();
                                
                DMLString += ", Tank8 = ";
                converter.str("");
                converter  << (int)adminedit->Tank8;
                DMLString += converter.str();
                                
                DMLString += ", Tank9 = ";
                converter.str("");
                converter << (int)adminedit->Tank9;
                DMLString += converter.str();

                DMLString += ", Red = ";
                converter.str("");
                converter  << (int)adminedit->Red;
                DMLString += converter.str();

                DMLString += ", Green = ";
                converter.str("");
                converter  << (int)adminedit->Green;
                DMLString += converter.str();

                DMLString += ", Blue = ";
                converter.str("");
                converter  << (int)adminedit->Blue;
                DMLString += converter.str();

                DMLString += ", Member = ";
                converter.str("");
                converter  << (int)adminedit->Member;
                DMLString += converter.str();

                DMLString += ", RentalCity = ";
                converter.str("");
                converter  << (int)adminedit->RentalCity;
                DMLString += converter.str();

                DMLString += ", IsAdmin = ";
                converter.str("");
                converter  << adminedit->playerType;
                DMLString += converter.str();

                DMLString += " WHERE lower(Account) = lower('";
                DMLString += adminedit->User;
                DMLString += "');";

                p->Log->logAdmin(DMLString, Index);

                p->Database->Database.execDML(DMLString.c_str());

                // For each possible player,
                // HACK: FIX "- 1"
                // TODO: FIX "- 1"
                for (int i = 0; i < MAX_PLAYERS-1; i++) {
                    playerToEdit = this->p->Player[i];

                    // If that player is the player we are editing,
                    if (p->PlatformCaseCompare(adminedit->User, playerToEdit->Name) == 0) {

                        // Set the new information on the player
                        playerToEdit->Red = adminedit->Red;
                        playerToEdit->Green = adminedit->Green;
                        playerToEdit->Blue = adminedit->Blue;
                        playerToEdit->Member = adminedit->Member;
                        playerToEdit->playerType = adminedit->playerType;
                        playerToEdit->Tank = adminedit->Tank;
                        playerToEdit->Tank2 = adminedit->Tank2;
                        playerToEdit->Tank3 = adminedit->Tank3;
                        playerToEdit->Tank4 = adminedit->Tank4;
                        playerToEdit->Tank5 = adminedit->Tank5;
                        playerToEdit->Tank6 = adminedit->Tank6;
                        playerToEdit->Tank7 = adminedit->Tank7;
                        playerToEdit->Tank8 = adminedit->Tank8;
                        playerToEdit->Tank9 = adminedit->Tank9;
                        playerToEdit->Town = adminedit->Town;

                        playerToEdit->Points = adminedit->Points;
                        playerToEdit->MonthlyPoints = adminedit->MonthlyPoints;
                        playerToEdit->Deaths = adminedit->Deaths;
                        playerToEdit->Assists = adminedit->Assists;
                        playerToEdit->Orbs = adminedit->Orbs;

                        // Send the points to everyone but the player
                        pts.Index = i;
                        pts.Points = playerToEdit->Points;
                        pts.Deaths = playerToEdit->Deaths;
                        pts.Assists = playerToEdit->Assists;
                        pts.Orbs = playerToEdit->Orbs;
                        pts.MonthlyPoints = playerToEdit->MonthlyPoints;
                        p->Send->SendAllBut(-1, smPointsUpdate, (char *)&pts, sizeof(pts));

                        // Send the player info to everyone but the player
                        response.Red = playerToEdit->Red;
                        response.Green = playerToEdit->Green;
                        response.Blue = playerToEdit->Blue;
                        response.Member = playerToEdit->Member;
                        response.Index = i;
                        response.playerType = playerToEdit->playerType;
                        response.Tank = playerToEdit->displayTank;
                        strcpy(response.Name, playerToEdit->Name.c_str());
                        strcpy(response.Town, playerToEdit->Town.c_str());
                        p->Send->SendAllBut(-1, smPlayerData, (char *)&response, sizeof(response));
                    }
                }
            }
        }
    }
}

/***************************************************************
 * Function:	ProcessAdminEditRequest
 *
 * @param Index
 * @param admineditrequest
 **************************************************************/
void CProcess::ProcessAdminEditRequest(int Index, sCMAdminEditRequest *admineditrequest) {
    string QueryString;
    sCMAdminEdit newedit;
    CPlayer *player = this->p->Player[Index];

    // If the player is an admin,
    if (player->isAdmin()) {

        // If the player is in a certain set of names,
        if (
            (p->PlatformCaseCompare(player->Name.c_str(), "Weebo") == 0)
            ||
            (p->PlatformCaseCompare(player->Name.c_str(), "Vindkast") == 0)
        ) {	

            // Try to select the account from the database
            try {
                QueryString = "SELECT * FROM tAccounts WHERE lower(Account) = lower('";
                QueryString += admineditrequest->User;
                QueryString += "');";
                p->Database->Query = p->Database->Database.execQuery(QueryString.c_str());

                // If the query returned an account,
                if (!p->Database->Query.eof()) {
                    p->Log->logAdmin(QueryString, Index);

                    // Copy the user information into the newedit struct
                    strcpy(newedit.User, admineditrequest->User);
                    strcpy(newedit.Pass, p->Database->Query.getStringField("Password"));
                    strcpy(newedit.FullName, p->Database->Query.getStringField("Fullname"));
                    strcpy(newedit.Town, p->Database->Query.getStringField("Town"));
                    strcpy(newedit.State, p->Database->Query.getStringField("State"));
                    strcpy(newedit.Email, p->Database->Query.getStringField("Email"));

                    newedit.Points = p->Database->Query.getIntField("Points");
                    newedit.MonthlyPoints = p->Database->Query.getIntField("MonthlyTop20");
                    newedit.Orbs = p->Database->Query.getIntField("Orbs");
                    newedit.Deaths = p->Database->Query.getIntField("Deaths");
                    newedit.Assists = p->Database->Query.getIntField("Assists");
                    newedit.playerType = p->Database->Query.getIntField("IsAdmin");
                    newedit.Member = p->Database->Query.getIntField("Member");
                    newedit.Red = p->Database->Query.getIntField("Red");
                    newedit.Green = p->Database->Query.getIntField("Green");
                    newedit.Blue = p->Database->Query.getIntField("Blue");
                    newedit.Tank = p->Database->Query.getIntField("Tank");
                    newedit.Tank2 = p->Database->Query.getIntField("Tank2");
                    newedit.Tank3 = p->Database->Query.getIntField("Tank3");
                    newedit.Tank4 = p->Database->Query.getIntField("Tank4");
                    newedit.Tank5 = p->Database->Query.getIntField("Tank5");
                    newedit.Tank6 = p->Database->Query.getIntField("Tank6");
                    newedit.Tank7 = p->Database->Query.getIntField("Tank7");
                    newedit.Tank8 = p->Database->Query.getIntField("Tank8");
                    newedit.Tank9 = p->Database->Query.getIntField("Tank9");
                    newedit.RentalCity = p->Database->Query.getIntField("RentalCity");
    
                    // Send the data back to the admin
                    p->Winsock->SendData(Index, smAdminEdit, (char *)&newedit, sizeof(newedit));
                }

                p->Database->Query.finalize();
            }
            catch (CppSQLite3Exception& e) {
                cerr << e.errorCode() << ":" << e.errorMessage() << endl;
            }
        }
    }
}

/***************************************************************
 * Function:	ProcessClickPlayer
 *
* @param Index
* @param Clicked
 **************************************************************/
void CProcess::ProcessClickPlayer(int Index, int Clicked) {
    CPlayer *playerClicked = this->p->Player[Clicked];
    sSMClickPlayer response;

    // Send the player info back to the clicker
    response.Index = Clicked;
    response.Orbs = playerClicked->Orbs;
    response.Assists = playerClicked->Assists;
    response.Deaths = playerClicked->Deaths;
    p->Winsock->SendData(Index, smClickPlayer, (char *)&response, sizeof(response));
}

/***************************************************************
 * Function:	ProcessChangeTank
 *
 * @param Index
 * @param tankIndex
 **************************************************************/
void CProcess::ProcessChangeTank(int Index, int tankIndex) {
    unsigned char tank;
    CPlayer *player = this->p->Player[Index];
    sSMPlayer response;

    // If player wants tank index 0, set no-custom-tank
    if (tankIndex == 0) {
        tank = 0;
    }

    // Else, set tank to the requested tank index
    else if (tankIndex == 1) {
        tank = player->Tank;
    }
    else if (tankIndex == 2) {
        tank = player->Tank2;
    }
    else if (tankIndex == 3) {
        tank = player->Tank3;
    }
    else if (tankIndex == 4) {
        tank = player->Tank4;
    }
    else if (tankIndex == 5) {
        tank = player->Tank5;
    }
    else if (tankIndex == 6) {
        tank = player->Tank6;
    }
    else if (tankIndex == 7) {
        tank = player->Tank7;
    }
    else if (tankIndex == 8) {
        tank = player->Tank8;
    }
    else if (tankIndex == 9) {
        tank = player->Tank9;
    }

    // If the player is already using this tank as the displayTank, return
    if (tank == player->displayTank) {
        return;
    }

    // Else, change the player's displayTank, and notify all players
    player->displayTank = tank;

    // Tell everyone about the tank change
    response.Red = player->Red;
    response.Green = player->Green;
    response.Blue = player->Blue;
    response.Member = player->Member;
    response.Index = Index;
    response.playerType = player->playerType;
    response.Tank = player->displayTank;
    strcpy(response.Name, player->Name.c_str());
    strcpy(response.Town, player->Town.c_str());
    p->Send->SendAllBut(-1, smPlayerData, (char *)&response, sizeof(response));
}

/***************************************************************
 * Function:	ProcessAutoBuild
 *
 * @param Index
 * @param request
 **************************************************************/
void CProcess::ProcessAutoBuild(int Index, sCMAutoBuild *request) {
    sSMAutoBuild response;
    CPlayer *player = this->p->Player[Index];
    CCity *city = this->p->City[player->City];
    bool isAllowed = true;

    // If the player is not an admin,
    if (player->isAdmin() == false) {

        // If player is not mayor, return
        if (! player->Mayor) {
            isAllowed = false;
        }

        // If city is orbable, return
        if (city->isOrbable()) {
            isAllowed = false;
        }
    }

    // Send the response
    response.isAllowed = isAllowed;
    strcpy(response.filename, request->filename);
    p->Winsock->SendData(Index, smAutoBuild, (char *)&response, sizeof(response));
}


/***************************************************************
 * Function:	ProcessCheatCheck
 *
 * @param Index
 * @param cheatCheck
 **************************************************************/
void CProcess::ProcessCheatCheck(int Index, sCMCheatCheck* cheatCheck) {
    stringstream ss;
    CPlayer* player;
    sSMAdmin adminPacket;

    // If any of the client's constants don't match the server's,
    if (
        (cheatCheck->costBuilding != COST_BUILDING)
        ||
        (cheatCheck->damageLaser != DAMAGE_LASER)
        ||
        (cheatCheck->damageMine != DAMAGE_MINE)
        ||
        (cheatCheck->damageRocket != DAMAGE_ROCKET)
        ||
        (cheatCheck->maxHealth != MAX_HEALTH)
        ||
        (cheatCheck->movementBullet != MOVEMENT_SPEED_BULLET)
        ||
        (cheatCheck->movementPlayer != MOVEMENT_SPEED_PLAYER)
        ||
        (cheatCheck->timerCloak != TIMER_CLOAK)
        ||
        (cheatCheck->timerDfg != TIMER_DFG)
        ||
        (cheatCheck->timerRespawn != TIMER_RESPAWN)
        ||
        (cheatCheck->timerLaser != TIMER_SHOOT_LASER)
        ||
        (cheatCheck->timerRocket != TIMER_SHOOT_ROCKET)
    ) {

        // Log the error,
        player = this->p->Player[Index];
        ss << endl;
        ss << "---------------------------------------------" << endl;
        ss << "Cheat Check Failed!  Player(" << player->Name << ") Index(" << Index << ")" << endl;
        ss << "---------------------------------------------" << endl;
        ss << "costBuilding:   Client(" << cheatCheck->costBuilding << ") Server(" << COST_BUILDING << ")" << endl;
        ss << "damageLaser:    Client(" << cheatCheck->damageLaser << ") Server(" << DAMAGE_LASER << ")" << endl;
        ss << "damageMine:     Client(" << cheatCheck->damageMine << ") Server(" << DAMAGE_MINE << ")" << endl;
        ss << "damageRocket:   Client(" << cheatCheck->damageRocket << ") Server(" << DAMAGE_ROCKET << ")" << endl;
        ss << "maxHealth:      Client(" << cheatCheck->maxHealth << ") Server(" << MAX_HEALTH << ")" << endl;
        ss << "movementBullet: Client(" << cheatCheck->movementBullet << ") Server(" << MOVEMENT_SPEED_BULLET << ")" << endl;
        ss << "timerCloak:     Client(" << cheatCheck->timerCloak << ") Server(" << TIMER_CLOAK << ")" << endl;
        ss << "timerDfg:       Client(" << cheatCheck->timerDfg << ") Server(" << TIMER_DFG << ")" << endl;
        ss << "timerRespawn:   Client(" << cheatCheck->timerRespawn << ") Server(" << TIMER_RESPAWN << ")" << endl;
        ss << "timerLaser:     Client(" << cheatCheck->timerLaser << ") Server(" << TIMER_SHOOT_LASER << ")" << endl;
        ss << "timerRocket:    Client(" << cheatCheck->timerRocket << ") Server(" << TIMER_SHOOT_ROCKET << ")" << endl;
        ss << "---------------------------------------------" << endl;

        cout << ss.str();
        this->p->Log->logServerError(ss.str());

        // Kick the player
        player->LeaveGame(true);
        adminPacket.adminIndex = Index;
        adminPacket.command = 1;
        adminPacket.id = Index;
        this->p->Send->SendGameAllBut(Index, smAdmin, (char *)&adminPacket, sizeof(adminPacket));
        this->p->Winsock->SendData(Index, smKicked, " ");
    }
}

/***************************************************************
 * Function:	ProcessStartingCity
 *
 * @param Index
 **************************************************************/
void CProcess::ProcessStartingCity(int Index) {
    sSMStartingCity response;
    response.City = this->p->Send->startingCity;
    p->Winsock->SendData(Index, smStartingCity, (char *)&response, sizeof(response));
}


/***************************************************************
 * Function:	ProcessChangeStartingCity
 *
 * @param Index
 * @param request
 **************************************************************/
void CProcess::ProcessChangeStartingCity(int Index, sCMStartingCity* request) {
    // If the city is valid, set it as the starting city
    if (CCity::isValidCityIndex(request->City)) {
        this->p->Send->startingCity = request->City;
    }
}
