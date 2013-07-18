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

int CALLBACK ConnectionManagerDialogProcedure(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CGame *p = (CGame *)CConnectionPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
            SendDlgItemMessage(hwnd,ID_GameServerAddress,EM_LIMITTEXT,255,0);
            SetDlgItemText(hwnd,ID_GameServerAddress,p->Options->gameServerAddress.c_str());
            return 1;
            break;
        case WM_COMMAND:
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
