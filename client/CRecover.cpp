/*
===============================================================================

    Battle City - CRecover
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
#include "CRecover.h"

void *CRecoverPointer;

int CALLBACK RecoverDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CRecoverPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			SendDlgItemMessage(hwnd, ID_CODE, EM_LIMITTEXT, 50, 0);
        return 1;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
					p->Dialog->StartDialog = 0;
					char Email[51];
					memset(Email, 0, 51);
					GetDlgItemText(hwnd, IDEMAIL, Email, 50);
					p->Winsock->SendData(cmRecover, Email);
					EndDialog(hwnd, IDOK);
				break;
				case IDCANCEL:
					p->Login->ShowLoginDlg();
					EndDialog(hwnd, IDCANCEL);
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

CRecover::CRecover(CGame *game)
{
	p = game;
	CRecoverPointer = game;
}

CRecover::~CRecover()
{

}

void CRecover::ShowRecoverDlg()
{
	p->State = STATE_RECOVER;
	p->Dialog->StartDialog = ID_RECOVER;
	p->Dialog->DialogProcessor = &RecoverDlgProc;

}