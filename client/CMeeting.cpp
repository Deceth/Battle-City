/*
===============================================================================

    Battle City - CMeeting
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
#include "CMeeting.h"

CGame *CMeetPointer;
WNDPROC wpOrigEditProc;
int CALLBACK MyEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigListProc;
int CALLBACK MyListProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigNewsProc;
int CALLBACK MyNewsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigChatProc;
int CALLBACK MyChatProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

/***************************************************************
 * Constructor:	CMeeting
 *
 * @param game
 **************************************************************/
CMeeting::CMeeting(CGame *game) {
	this->p = game;
	CMeetPointer = game;

	for (int i = 0; i < 30; i++) {
		City[i].Mayor = -1;
		City[i].RealCity = 0;
		City[i].Index = 0;
		City[i].PlayerCount = 0;
	}
}

/***************************************************************
 * Destructor:	CMeeting
 *
 **************************************************************/
CMeeting::~CMeeting() {
}

/***************************************************************
 * Function:	MeetDlgProc
 *
 * @param hwnd
 * @param Message
 * @param wParam
 * @param lParam
 **************************************************************/
int CALLBACK MeetDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;

    switch(Message) {
        case WM_INITDIALOG: {
			p->Winsock->SendData(cmSetState, "C");
			p->Meeting->hWnd = hwnd;
			SetDlgItemText(p->Meeting->hWnd, IDCHAT, p->Meeting->Chat.c_str());
			p->Meeting->EditWnd = GetDlgItem(hwnd,IDSEND);
			p->Meeting->ChatWnd = GetDlgItem(hwnd,IDCHAT);
			p->Meeting->NewsWnd = GetDlgItem(hwnd,IDNEWS);
			SendDlgItemMessage(hwnd, IDSEND, EM_LIMITTEXT, 200, 0);
			wpOrigEditProc = (WNDPROC) SetWindowLong(p->Meeting->EditWnd, GWL_WNDPROC, (LONG)&MyEditProc); 
			p->Meeting->RedrawList();

			SetDlgItemText(hwnd, IDNEWS, p->Meeting->News.c_str());
			wpOrigNewsProc = (WNDPROC) SetWindowLong(p->Meeting->NewsWnd, GWL_WNDPROC, (LONG)&MyNewsProc); 
			wpOrigChatProc = (WNDPROC) SetWindowLong(p->Meeting->ChatWnd, GWL_WNDPROC, (LONG)&MyChatProc); 

			p->Meeting->CityWnd = GetDlgItem(hwnd, IDAPPLYLIST);
			wpOrigListProc = (WNDPROC) SetWindowLong(p->Meeting->CityWnd, GWL_WNDPROC, (LONG)&MyListProc); 

			return 1;
		}
        case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDQUIT:
					SendMessage(p->hWnd, WM_CLOSE, 0, 0);
					break;
				case IDREFRESH:
					p->Winsock->SendData(cmRefreshList, " ");
					for (int i = 0; i < 30; i++) {
						p->Meeting->City[i].Mayor = -1;
						p->Meeting->City[i].RealCity = 0;
						p->Meeting->City[i].Index = 0;
						p->Meeting->City[i].PlayerCount = 0;
					}
					SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_RESETCONTENT, 0, 0);
					break;
			}
			return 1;
		}
        default:
            return 0;
    }
    return 1;
}

/***************************************************************
 * Function:	MyEditProc
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 **************************************************************/
int CALLBACK MyEditProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
	char Msg[200];
	char tmpString[255];

	switch(uMsg) {
		case WM_CHAR: {
			if( wParam == 13 ) {
				GetDlgItemText(p->Meeting->hWnd, IDSEND, Msg, 200);
				p->Winsock->SendData(cmChatMessage, Msg);
				SetDlgItemText(p->Meeting->hWnd, IDSEND, NULL);
				strcpy(&tmpString[0], p->Login->user);
				strcpy(&tmpString[strlen(tmpString)], ": ");
				strcpy(&tmpString[strlen(tmpString)], Msg);
				p->Meeting->AppendData(tmpString);
				return(0);
			}
			else {
				return (int)CallWindowProc(wpOrigEditProc, hDlg, uMsg, wParam, lParam);
			}
			break;
		}
		default:
			return (int)CallWindowProc(wpOrigEditProc, hDlg, uMsg, wParam, lParam);
			break;
	}
	return(0);
}

/***************************************************************
 * Function:	MyNewsProc
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 **************************************************************/
int CALLBACK MyNewsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;

	switch(uMsg) {
		default:
			return (int)CallWindowProc(wpOrigNewsProc, hDlg, uMsg, wParam, lParam);
			break;
	}
	return(0);
}

/***************************************************************
 * Function:	MyChatProc
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 **************************************************************/
int CALLBACK MyChatProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;

	switch(uMsg) {
		default:
			return (int)CallWindowProc(wpOrigChatProc, hDlg, uMsg, wParam, lParam);
			break;
	}
	return(0);
}








/***************************************************************
 * Function:	ShowMeetingDlg
 *
 **************************************************************/
void CMeeting::ShowMeetingDlg() {
	this->p->InGame->ClearOut();
	this->ClearList();
	this->Chat.clear();
	this->News.clear();
	this->hWnd = 0;
	this->p->State = STATE_MEETING;
	this->p->Dialog->StartDialog = ID_MEETING;
	this->p->Dialog->DialogProcessor = &MeetDlgProc;
}

/***************************************************************
 * Function:	AppendData
 *
 * @param hDlg
 * @param uMsg
 * @param wParam
 * @param lParam
 **************************************************************/
void CMeeting::AppendData(string TheData) {
	string tmp;
	SCROLLINFO si;

	// Add a new line
	TheData += "\r\n";

	// If text length is over 1700, limit the length to 300
	// ????
	if (this->Chat.length() > 1700) {
		tmp.append(&this->Chat[300]);
		this->Chat = tmp;
	}

	// Append the text
	this->Chat.append(TheData);
	SetDlgItemText(this->hWnd, IDCHAT, this->Chat.c_str());
    si.cbSize = sizeof (si);
    si.fMask  = SIF_ALL;
    GetScrollInfo(this->ChatWnd, SB_VERT, &si);
	PostMessage(this->ChatWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, si.nMax), 0);
	PostMessage(this->ChatWnd, WM_PAINT, 0, 0);
}

/***************************************************************
 * Function:	ClearList
 *
 **************************************************************/
void CMeeting::ClearList() {
	for (int j = 0; j < MAX_PLAYERS; j++) {
		this->lstPlayers[j] = -1;
	}
}

/***************************************************************
 * Function:	AddPlayer
 *
 * @param Index
 **************************************************************/
void CMeeting::AddPlayer(int Index) {
	int i;

	// If the player is not already in the chat,
	if (this->inUse(Index) == false) {
		
		// Get the next open slot in the chat list
		i = this->FreePlayer();

		// If there is an open spot, add the player
		if (i != -1) {
			this->lstPlayers[i] = Index;
			this->RedrawList();
		}
	}
}

/***************************************************************
 * Function:	ClearPlayer
 *
 * @param Index
 **************************************************************/
void CMeeting::ClearPlayer(int Index) {
	
	// For each possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {
		
		// If the player matches the Index param,
		if (this->lstPlayers[j] == Index)  {

			// Remove the player
			this->lstPlayers[j] = -1;
			this->RedrawList();
			break;
		}
	}
}

/***************************************************************
 * Function:	RedrawList
 *
 **************************************************************/
void CMeeting::RedrawList() {

	// Clear the content
	SendDlgItemMessage(p->Meeting->hWnd, IDLIST, LB_RESETCONTENT, 0, 0);

	// For each possible player,
	for (int j = 0; j < MAX_PLAYERS; j++) {

		// If the player is in the meeting room list,
		if (p->Meeting->lstPlayers[j] != -1) {

			// Add the player
			SendDlgItemMessage(p->Meeting->hWnd, IDLIST, LB_ADDSTRING, 0, (LPARAM)p->Player[p->Meeting->lstPlayers[j]]->Name.c_str());
		}
	}
}

/***************************************************************
 * Function:	FreePlayer
 *
 **************************************************************/
int CMeeting::FreePlayer() {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the slot in the chat list is empty, return it
		if (this->lstPlayers[i] == -1) {
			return i;
		}
	}

	// If no empty slot was found, return -1
	return -1;
}

/***************************************************************
 * Function:	AppendNews
 *
 * @param TheData
 **************************************************************/
void CMeeting::AppendNews(string TheData) {
	this->News.append(TheData);
	SetDlgItemText(this->hWnd, IDNEWS, this->News.c_str());
}

/***************************************************************
 * Function:	MyChatProc
 *
 * Create list of available cities to be displayed in the Meeting Room
 *
 * @param Index
 **************************************************************/
void CMeeting::AddCity(int City, int Mayor, int playerCount) {
	int i;
	int index;
	string tmpString;

	// Get the next free city
	i = FreeCity();

	// If the city isn't 255,
	if (i != 255) {

		// Add the parameters to that city
		this->City[i].RealCity = City;
		this->City[i].Mayor = Mayor;
		this->City[i].PlayerCount = playerCount;

		// If the city has no mayor,
		if (p->Meeting->City[i].Mayor == 255) {

			// Add the Mayor Required line
			tmpString = "Mayor required for city of ";
			tmpString += CityList[p->Meeting->City[i].RealCity];
			index = (int)SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			p->Meeting->City[i].Index = index;

			// Add explanation and padding
			tmpString = "<Double click to become mayor of this city>";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			tmpString = "----------------------------------------------------------------------------------------------";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
		}

		// Else (city has a mayor),
		else {

			// Add the Commando Required line
			tmpString = "Commando required for city of "; 
			tmpString += CityList[p->Meeting->City[i].RealCity];
			index = (int)SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			p->Meeting->City[i].Index = index;

			// Add Mayor info and padding
			tmpString = "Mayor " + p->Player[p->Meeting->City[i].Mayor]->Name; 
			tmpString += " of "; 
			tmpString += p->Player[p->Meeting->City[i].Mayor]->Town; 
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			tmpString = "----------------------------------------------------------------------------------------------";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
		}
	}
}

/***************************************************************
 * Function:	FreeCity
 *
 **************************************************************/
int CMeeting::FreeCity() {

	// For each possible city,
	for (int i = 0; i < 30; i++) {

		// If the city is empty (no mayor), return it
		if (City[i].Mayor == -1) {
			return i;
		}
	}

	// If no empty city was found, return -1
	return 255;
}

/***************************************************************
 * Function:	inUse
 *
 * @param Index
 **************************************************************/
bool CMeeting::inUse(int Index) {

	// For each possible player,
	for (int i = 0; i < MAX_PLAYERS; i++) {

		// If the player matches the Index param, return true
		if (this->lstPlayers[i] == Index) {
			return true;
		}
	}

	// If the player wasn't found, return false
	return false;
}

/***************************************************************
 * Function:	MyListProc
 *
 * @param Index
 **************************************************************/
int CALLBACK MyListProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
	int index;
	char packet[1];

	switch(uMsg) {

		// Left Double Click
		case WM_LBUTTONDBLCLK:
			index = (int)SendMessage(hDlg, LB_GETCURSEL, 0, 0);

			// For each city,
			for (int i = 0; i < 30; i++) {

				// If the city matches the index of a line (or the line above it),
				if ((p->Meeting->City[i].Index == index) || (p->Meeting->City[i].Index == (index-1))) {
					packet[0] = (unsigned char)p->Meeting->City[i].RealCity;
					p->Winsock->SendData(cmJobApp,packet,1);
					p->Dialog->StartDialog = 0;

					// Clear the cities
					for (int i = 0; i < 30; i++) {
						p->Meeting->City[i].Mayor = -1;
						p->Meeting->City[i].RealCity = 0;
						p->Meeting->City[i].Index = 0;
						p->Meeting->City[i].PlayerCount = 0;
					}

					// Close the dialog and return
					EndDialog(p->Meeting->hWnd, 1);
					return 0;
				}
			}
			break;

		// Any other, ignore
		default:
            return (int)CallWindowProc(wpOrigListProc, hDlg, uMsg, wParam, lParam);
			break;
	}

	return(0);
}
