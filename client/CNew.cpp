/*
===============================================================================

    Battle City - CNew
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
#include "CNew.h"

void *CNewPointer;

int CALLBACK NewDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CNewPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			SendDlgItemMessage(hwnd, IDUSER, EM_LIMITTEXT, 15, 0);
			SendDlgItemMessage(hwnd, IDPASS, EM_LIMITTEXT, 15, 0);
			SendDlgItemMessage(hwnd, IDFULLNAME, EM_LIMITTEXT, 20, 0);
			SendDlgItemMessage(hwnd, IDEMAIL, EM_LIMITTEXT, 50, 0);
			SendDlgItemMessage(hwnd, IDTOWN, EM_LIMITTEXT, 15, 0);
			SendDlgItemMessage(hwnd, IDSTATE, EM_LIMITTEXT, 15, 0);
			p->NewAccount->hWnd = hwnd;
			p->NewAccount->Refill();
        return 1;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					if (p->NewAccount->Editing == 0)
					{
						GetDlgItemText(hwnd, IDUSER, p->NewAccount->user, 15);
						GetDlgItemText(hwnd, IDPASS, p->NewAccount->pass, 15);
						GetDlgItemText(hwnd, IDFULLNAME, p->NewAccount->fullname, 20);
						GetDlgItemText(hwnd, IDEMAIL, p->NewAccount->email, 50);
						GetDlgItemText(hwnd, IDTOWN, p->NewAccount->town, 15);
						GetDlgItemText(hwnd, IDSTATE, p->NewAccount->state, 15);
						if ((strlen(p->NewAccount->user) > 0) && (strlen(p->NewAccount->pass) > 0) && (strlen(p->NewAccount->fullname) > 0) && (strlen(p->NewAccount->email) > 0)&& (strlen(p->NewAccount->town) > 0) && (strlen(p->NewAccount->state) > 0))
						{
							p->Personality->ShowPersonalityDlg();
							EndDialog(hwnd, 1);
						}
						else MessageBox(p->hWnd, "All fields must be filled in!", "Error", 0);
					}
					else //Editing account
					{
						GetDlgItemText(hwnd, IDUSER, p->NewAccount->user, 15);
						GetDlgItemText(hwnd, IDPASS, p->NewAccount->pass, 15);
						GetDlgItemText(hwnd, IDFULLNAME, p->NewAccount->fullname, 20);
						GetDlgItemText(hwnd, IDEMAIL, p->NewAccount->email, 50);
						GetDlgItemText(hwnd, IDTOWN, p->NewAccount->town, 15);
						GetDlgItemText(hwnd, IDSTATE, p->NewAccount->state, 15);
						if ((strlen(p->NewAccount->user) > 0) && (strlen(p->NewAccount->pass) > 0) && (strlen(p->NewAccount->fullname) > 0) && (strlen(p->NewAccount->email) > 0)&& (strlen(p->NewAccount->town) > 0) && (strlen(p->NewAccount->state) > 0))
						{
							int response = MessageBox(p->hWnd, "Save the following changes to your account?", "BattleCity", MB_YESNO || MB_ICONQUESTION);
							if (response == 1)
							{
								p->Dialog->StartDialog = 0;
								EndDialog(hwnd, 1);
								p->Send->SendAccountUpdate();
							}
						}
						else MessageBox(p->hWnd, "All fields must be filled in!", "Error", 0);
					}
                break;
				case 2:
					p->Login->ShowLoginDlg();
					EndDialog(hwnd, 2);
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

CNew::CNew(CGame *game)
{
	p = game;
	CNewPointer = game;
	memset(user, 0, 15);
	memset(pass, 0, 15);
	memset(fullname, 0, 20);
	memset(email, 0, 50);
	memset(town, 0, 15);
	memset(state, 0, 15);

	Editing = 0;
}

CNew::~CNew()
{

}

void CNew::ShowNewDlg()
{
	this->Editing = 0;
	p->State = STATE_NEWACCOUNT;
	p->Dialog->StartDialog = ID_NEW;
	p->Dialog->DialogProcessor = &NewDlgProc;
}

void CNew::ShowEditDlg()
{
	this->Editing = 1;
	p->State = STATE_EDITING;
	p->Dialog->StartDialog = ID_NEW;
	p->Dialog->DialogProcessor = &NewDlgProc;
}

void CNew::Refill()
{
	SetDlgItemText(p->NewAccount->hWnd, IDUSER, p->NewAccount->user);
	SetDlgItemText(p->NewAccount->hWnd, IDPASS, p->NewAccount->pass);
	SetDlgItemText(p->NewAccount->hWnd, IDFULLNAME, p->NewAccount->fullname);
	SetDlgItemText(p->NewAccount->hWnd, IDEMAIL, p->NewAccount->email);
	SetDlgItemText(p->NewAccount->hWnd, IDTOWN, p->NewAccount->town);
	SetDlgItemText(p->NewAccount->hWnd, IDSTATE, p->NewAccount->state);
}