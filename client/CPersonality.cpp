/*
===============================================================================

    Battle City - CPersonality
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
#include "CPersonality.h"

void *CPersPointer;

int CALLBACK PersDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CPersPointer;
    switch(Message)
    {
        case WM_INITDIALOG:

        return 1;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 1);
					p->Send->SendLogin(1);
                break;
				case 2:
                    EndDialog(hwnd, 2);
					p->NewAccount->ShowNewDlg();
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

CPersonality::CPersonality(CGame *game)
{
	p = game;
	CPersPointer = game;
}

CPersonality::~CPersonality()
{

}

void CPersonality::ShowPersonalityDlg()
{
	p->State = STATE_PERSONALITY;
	p->Dialog->StartDialog = ID_PERSONALITY;
	p->Dialog->DialogProcessor = &PersDlgProc;
}