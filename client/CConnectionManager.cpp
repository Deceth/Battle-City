/*
===============================================================================

    Battle City - CLogin header file
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
#include "CConnectionManager.h"
#include <string>


HBRUSH g_hbrBackgroundConnectionManager;
HDC hdcStaticConnectionManager;

void *CConnectionPointer;
/// <summary>   Constructor. </summary>
///
/// <param name="game"> [in,out] If non-null, the game. </param>
CConnectionManager::CConnectionManager(CGame *game)
{
    p = game;
    CConnectionPointer = game;
}

/// <summary>   Destructor. </summary>
CConnectionManager::~CConnectionManager()
{
}

/// <summary>   Connection manager dialog procedure. </summary>
///
/// <param name="hwnd">     Window handler. </param>
/// <param name="Message">  Incoming message. </param>
/// <param name="wParam">   wParam </param>
/// <param name="lParam">   lParam </param>
///
/// <returns>   . </returns>
int CALLBACK ConnectionManagerDialogProcedure(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    //  Initialize the local game object pointer
    CGame *p = (CGame *)CConnectionPointer;
    //  Observe the incoming message
    switch(Message)
    {
        //  Initializing dialog box
        case WM_INITDIALOG:
            //  Set limit text for game address to 255 characters
            SendDlgItemMessage(hwnd,ID_GameServerAddress,EM_LIMITTEXT,255,0);
            //  Populate edit box with value received from Options
            SetDlgItemText(hwnd,ID_GameServerAddress,p->Options->gameServerAddress.c_str());
            //  Populate the combobox with past game servers
            p->ConnectionManager->loadPastGameServers(hwnd);
            return 1;
            break;
        case WM_COMMAND:
            switch(HIWORD(wParam)) 
            {
                case CBN_SELCHANGE:
                    //  Populate edit box with combobox selection
                    p->ConnectionManager->updateGameServerAddress(hwnd);
                    return 1;
                break;
            }
            switch(LOWORD(wParam))
            {
                case ID_Connect:
                    char _gameServerAddress[255];
                    GetDlgItemText(hwnd,ID_GameServerAddress,_gameServerAddress,255);
                    p->Options->gameServerAddress = _gameServerAddress;
                    p->Winsock->Init(_gameServerAddress);
                    p->Winsock->StartTCP();
                    EndDialog(hwnd,1);
                    break;
                case ID_Exit:
                    SendMessage(p->hWnd,WM_CLOSE,0,0);
                    break;
            }
            break;
        default:
            return 0;
            break;
    }
    return 1;
}

/// <summary>   Load past game servers from flat file then populate combo box </summary>
///
/// <param name="hwnd"> Window handler linking to the opened dialog box </param>
void CConnectionManager::loadPastGameServers(HWND hwnd)
{
    HWND CB_pastGameServers = GetDlgItem(hwnd,ID_PastGameServers);
    for(std::vector<std::string>::iterator i = p->Options->pastGameServers.begin(); i != p->Options->pastGameServers.end(); ++i)
    {
        SendMessage(CB_pastGameServers,CB_ADDSTRING,0,(LPARAM)(LPWSTR(i[0].c_str())));
    }
    SendMessage(CB_pastGameServers,CB_SETCURSEL,(WPARAM) 0, 0);
}

void CConnectionManager::updateGameServerAddress(HWND hwnd)
{
    HWND CB_pastGameServers = GetDlgItem(hwnd,ID_PastGameServers);
    int currentIndex = SendMessage(CB_pastGameServers,CB_GETCURSEL,0,0);
    char selectedValue[255];
    SendMessage(CB_pastGameServers,CB_GETLBTEXT,currentIndex,(LPARAM)selectedValue);
    SetDlgItemText(hwnd,ID_GameServerAddress,selectedValue);
}

/// <summary>   Shows the connection manager. </summary>
void CConnectionManager::showConnectionManager() 
{
    //  Set game state to STATE_LOGIN
	p->State = STATE_LOGIN;
    //  Initialize the ID_LOGIN dialog
	p->Dialog->StartDialog = ID_CONNECTIONMANAGER;
    //  Initialize the dialog processor
	p->Dialog->DialogProcessor = &ConnectionManagerDialogProcedure;
}
