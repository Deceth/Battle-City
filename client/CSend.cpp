/*
===============================================================================

    Battle City - CSend
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
#include "CSend.h"

CSend::CSend(CGame *game) {
	this->p = game;
}

CSend::~CSend() {

}

void CSend::SendLogin(int NewAccount) {
	if (NewAccount == 0) {
		sCMLogin LoginData;
		memset(&LoginData, 0, sizeof(LoginData));
		strcpy(LoginData.User, p->Login->user);
		strcpy(LoginData.Pass, p->Login->pass);

		p->Winsock->SendData(cmLogin, (char *)&LoginData, sizeof(LoginData));
	}
	else {
		sCMNewAccount NewAccount;
		memset(&NewAccount, 0, sizeof(NewAccount));
		strcpy(NewAccount.Pass, p->NewAccount->pass);
		strcpy(NewAccount.User, p->NewAccount->user);
		strcpy(NewAccount.Town, p->NewAccount->town);
		strcpy(NewAccount.State, p->NewAccount->state);
		strcpy(NewAccount.Email, p->NewAccount->email);
		strcpy(NewAccount.FullName, p->NewAccount->fullname);

		p->Winsock->SendData(cmNewAccount, (char *)&NewAccount, sizeof(NewAccount));
	}
}
/// <summary>   Sends the version. </summary>
void CSend::SendVersion() {
    //  Set vers to use sCMVersion structure
	sCMVersion vers;
    //  Populate Version with 10 zeros
	memset(vers.Version, 0, 10);
    //  Populate UniqID with 50 zeros
	memset(vers.UniqID, 0, 50);
    //  Copy VERSION value to Version
	strcpy(vers.Version, VERSION);
    //  Set UniqID as String and populate with ReturnUniqID
    //  UniqID helps avoid user collision
	string UniqID = p->ReturnUniqID();
    //  Copy UniqID to structure
	strcpy(vers.UniqID, UniqID.c_str());
    //  Send version structure to game server
	p->Winsock->SendData(cmVersion, (char *)&vers, sizeof(vers));
}

void CSend::SendAccountUpdate() {
	sCMNewAccount NewAccount;
	memset(&NewAccount, 0, sizeof(NewAccount));
	strcpy(NewAccount.Pass, p->NewAccount->pass);
	strcpy(NewAccount.User, p->NewAccount->user);
	strcpy(NewAccount.Town, p->NewAccount->town);
	strcpy(NewAccount.State, p->NewAccount->state);
	strcpy(NewAccount.Email, p->NewAccount->email);
	strcpy(NewAccount.FullName, p->NewAccount->fullname);

	p->Winsock->SendData(cmAccountUpdate, (char *)&NewAccount, sizeof(NewAccount));
}

void CSend::SendAccountEdit() {	
	sCMLogin LoginData;
	memset(&LoginData, 0, sizeof(LoginData));
	strcpy(LoginData.User, p->Login->user);
	strcpy(LoginData.Pass, p->Login->pass);

	p->Winsock->SendData(cmEditAccount, (char *)&LoginData, sizeof(LoginData));
}

// Send Private Messages
void CSend::SendWhisper() {	
	int recipientStartIndex = 0;
	int recipientEndIndex = 0;
	int recipientLength = 0;
	int messageStartIndex = 0;
	int messageLength = 0;
	string tmpString;
	string recipientName;
	string playerNameStr;
	bool recipientTooLong = false;
	bool messageTooLong = false;

	char recipient[20];
	memset(recipient, 0, 20);

	char playerName[20];
	memset(playerName, 0, 20);

	sCMWhisper whisper;
	memset(&whisper, 0, sizeof(whisper));
	whisper.Recipient = -1;

	// Parse the ChatLine, assuming "/pm recipient message" format
	recipientStartIndex = p->InGame->ChatLine.find(" ",0) + 1;
	recipientEndIndex = p->InGame->ChatLine.find(" ",recipientStartIndex);
	messageStartIndex = recipientEndIndex + 1;

	// If the command is properly formatted (has recipient and message),
	if( (recipientStartIndex>0) && (recipientEndIndex>0) ) {
		recipientLength = recipientEndIndex - recipientStartIndex;
		messageLength = tmpString.length() - recipientEndIndex;

		// If the recipient name and message lengths name are within the required bounds,
		if (recipientLength > 20) {
			recipientTooLong = true;
		}
		else if (messageLength > 128) {
			messageTooLong = true;
		}
		else {

			// Get the recipient and message
			p->InGame->ChatLine.copy(recipient, recipientLength, recipientStartIndex);
			p->InGame->ChatLine.copy(whisper.Message, messageLength, messageStartIndex);

			// Lowercase the recipient
			_strlwr(recipient);
			recipientName = recipient;

			// Loop through the players, looking for the recipient
			for (int i = 0; i < MAX_PLAYERS; i++) {
				
				// If the player isn't the person sending the whisper,
				if (i != p->Winsock->MyIndex ) {

					// If the name isn't empty, lowercase it for comparison
					playerNameStr = p->Player[i]->Name;
					if ( ! playerNameStr.empty() ) {
						playerNameStr.copy(playerName, playerNameStr.length());
						_strlwr(playerName);
						playerNameStr = playerName;

						// If the player name starts with the recipient name, send and break
						if (playerNameStr.find(recipientName) == 0) {
							whisper.Recipient = i;
							break;
						}
					}
				}
			}
		}
	}

	// If the recipient was found,
	if (whisper.Recipient > -1) {
		// Send the message, append the message to the sender's chat
		whisper.Sender = p->Winsock->MyIndex;
		tmpString = p->Player[p->Winsock->MyIndex]->Name + " (to " + p->Player[whisper.Recipient]->Name + "): " + whisper.Message;
		p->InGame->AppendChat(tmpString, COLOR_WHISPER);
		p->Winsock->SendData(cmWhisper,(char *)&whisper,sizeof(whisper));
	}
	// Else if the recipient name was too long,
	else if (recipientTooLong) {
		tmpString = "Player not found: try adding more letters to the name!";
		p->InGame->AppendInfo(tmpString);
	}
	// Else if the message was too long, (note that this is impossible because the client cuts messages shorter than this)
	else if (messageTooLong) {
		tmpString = "tl;dr";
		p->InGame->AppendInfo(tmpString);
	}
	// Else, recipient wasn't found,
	else {
		tmpString = "Player not found: " + recipientName;
		p->InGame->AppendInfo(tmpString);
	}
}

void CSend::SendGlobal() {	
	int messageStartIndex = 0;
	string tmpString;
	char SendString[255];
	memset(SendString, 0, 255);

	// Parse the ChatLine, assuming "/g message" format
	messageStartIndex = p->InGame->ChatLine.find(" ",0) + 1;

	// Send the message
	p->InGame->ChatLine.copy(SendString, tmpString.length() - messageStartIndex, messageStartIndex);
	p->Winsock->SendData(cmGlobal, SendString);

	// Append the message to the sender's chat
	tmpString = p->Player[p->Winsock->MyIndex]->Name + " (Global): " + SendString;
	p->InGame->AppendChat(tmpString, COLOR_GLOBAL_CHAT);
}

void CSend::SendMessage() {	
	string tmpString;
	char SendString[255];
	memset(SendString, 0, 255);

	strcpy(SendString, p->InGame->ChatLine.c_str());
	tmpString = p->Player[p->Winsock->MyIndex]->Name + ": " + p->InGame->ChatLine;

	// Send the message as cmWalkie meaning to team and everyone on radar
	p->Winsock->SendData(cmWalkie, SendString);
	
	// Append the message to the sender's chat
	// If sent by ADMIN player, show Admin color
	if (p->Player[p->Winsock->MyIndex]->isAdmin()) {
		p->InGame->AppendChat(tmpString, COLOR_ADMIN_CHAT);
	}
	// Else, if sent by DEAD player, show Dead color
	else if (p->Player[p->Winsock->MyIndex]->isDead == true) {
		p->InGame->AppendChat(tmpString, COLOR_DEAD_CHAT);
	}
	// Else, sent by NORMAL player, show Team color (since this only specifies what the sender sees)
	else {
		p->InGame->AppendChat(tmpString, COLOR_TEAM_CHAT);
	}
}

void CSend::RequestStartingCity() {	
	p->Winsock->SendData(cmStartingCity, "");
}
