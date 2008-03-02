#include "CSend.h"

CSend::CSend(CGame *game)
{
	p = game;
}

CSend::~CSend()
{

}

void CSend::SendLogin(int NewAccount)
{
	if (NewAccount == 0)
	{
		sCMLogin LoginData;
		memset(&LoginData, 0, sizeof(LoginData));
		strcpy(LoginData.User, p->Login->user);
		strcpy(LoginData.Pass, p->Login->pass);

		p->Winsock->SendData(cmLogin, (char *)&LoginData, sizeof(LoginData));
	}
	else
	{
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

void CSend::SendVersion()
{
	sCMVersion vers;
	memset(vers.Version, 0, 10);
	memset(vers.UniqID, 0, 50);

	strcpy(vers.Version, VERSION);
	string UniqID = p->ReturnUniqID();
	strcpy(vers.UniqID, UniqID.c_str());
	p->Winsock->SendData(cmVersion, (char *)&vers, sizeof(vers));
}

void CSend::SendAccountUpdate()
{
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

void CSend::SendAccountEdit()
{	
	sCMLogin LoginData;
	memset(&LoginData, 0, sizeof(LoginData));
	strcpy(LoginData.User, p->Login->user);
	strcpy(LoginData.Pass, p->Login->pass);

	p->Winsock->SendData(cmEditAccount, (char *)&LoginData, sizeof(LoginData));
}

void CSend::SendWhisper() {	
	string tmpString;
	char Recipient[100];
	memset(Recipient, 0, 100);
	int recipientStartIndex = 0;
	int recipientEndIndex = 0;
	int messageStartIndex = 0;
	char recipient[20];
	char playerName[20];
	string recipientName;
	string playerNameStr;
	sCMWhisper whisper;

	// Parse the ChatLine, assuming "/pm recipient message" format
	recipientStartIndex = p->InGame->ChatLine.find(" ",0) + 1;
	recipientEndIndex = p->InGame->ChatLine.find(" ",recipientStartIndex);
	messageStartIndex = recipientEndIndex + 1;

	// If the command is properly formatted (has recipient and message),
	if( (recipientStartIndex>0) && (recipientEndIndex>0) ) {
	
		// Get the recipient
		memset(recipient, '\0', 20);
		memset(playerName, '\0', 20);
		memset(whisper.Message, '\0', 128);
		p->InGame->ChatLine.copy(recipient, recipientEndIndex - recipientStartIndex, recipientStartIndex);
		p->InGame->ChatLine.copy(whisper.Message, tmpString.length() - recipientEndIndex, messageStartIndex);

		// Lowercase the recipient
		_strlwr(recipient);
		recipientName = recipient;

		// Loop through the players, looking for the recipient
		for (int i = 0; i < MaxPlayers; i++) {
			
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

	// If the recipient was found,
	if (whisper.Recipient > 0) {
		// Send the message, append the message to the sender's chat
		whisper.Sender = p->Winsock->MyIndex;
		tmpString = p->Player[p->Winsock->MyIndex]->Name + " (to " + p->Player[whisper.Recipient]->Name + "): " + whisper.Message;
		p->InGame->AppendChat(tmpString, RGB(255, 255, 255));
		p->Winsock->SendData(cmWhisper,(char *)&whisper,sizeof(whisper));
	}
	// Else, recipient wasn't found,
	else {
		// Print an error
		tmpString = "Player not found: " + recipientName;
		p->InGame->AppendChat(tmpString, RGB(255, 255, 255));
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
	p->InGame->AppendChat(tmpString, RGB(255, 165, 0));
}

void CSend::SendMessage() {	
	string tmpString;
	char SendString[255];
	memset(SendString, 0, 255);

	strcpy(SendString, p->InGame->ChatLine.c_str());
	tmpString = p->Player[p->Winsock->MyIndex]->Name + ": " + p->InGame->ChatLine;

	// Send the message
	// If sent by user with WALKIE, send as cmWalkie
	if (p->InGame->HasWalkie == 1) {
		p->Winsock->SendData(cmWalkie, SendString);
	}
	// Else, NO WALKIE, send as cmChatMessage
	else {
		p->Winsock->SendData(cmChatMessage, SendString);
	}

	// Append the message to the sender's chat
	// If sent by ADMIN player, show Admin color
	if (p->Player[p->Winsock->MyIndex]->isAdmin == 2) {
		p->InGame->AppendChat(tmpString, RGB(255, 165, 0));
	}
	// Else, if sent by DEAD player, show Dead color
	else if (p->Player[p->Winsock->MyIndex]->isDead == true) {
		p->InGame->AppendChat(tmpString, RGB(135, 135, 135));
	}
	// Else, sent by NORMAL player, show Team color (since this only specifies what the sender sees)
	else {
		p->InGame->AppendChat(tmpString, RGB(0, 255, 0));
	}
}