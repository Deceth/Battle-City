/*
===============================================================================

    Battle City - CHelp
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
#include "CHelp.h"

void *CHelpPointer;

int CALLBACK HelpDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	CGame *p = (CGame *)CHelpPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			p->Help->hWnd = hwnd;			
        return 1;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					//ok
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 1);
                break;
				case 2:
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 2);
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

CHelp::CHelp(CGame *game)
{
	p = game;
	CHelpPointer = game;
}

CHelp::~CHelp()
{

}

void CHelp::ShowHelpDialog()
{
	p->Dialog->StartDialog = ID_BCHELP;
	p->Dialog->DialogProcessor = &HelpDlgProc;

}