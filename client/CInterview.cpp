/*
===============================================================================

    Battle City - CInterview
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
#include "CInterview.h"

void *CIntPointer;
WNDPROC wpOrigIntEditProc;
int CALLBACK MyIntEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int CALLBACK InterviewDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CIntPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			{
				p->Interview->hWnd = hwnd;
				SetDlgItemText(p->Interview->hWnd, IDCHAT, p->Interview->Chat.c_str());
				p->Interview->EditWnd = GetDlgItem(hwnd,IDSEND);
				p->Interview->ChatWnd = GetDlgItem(hwnd,IDCHAT);
				SendDlgItemMessage(hwnd, IDSEND, EM_LIMITTEXT, 200, 0);
				wpOrigIntEditProc = (WNDPROC) SetWindowLong(p->Interview->EditWnd, GWL_WNDPROC, (LONG)&MyIntEditProc); 
				return 1;
			}
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					p->Winsock->SendData(cmJobCancel, " ");
					p->Interview->Chat.clear();
					p->Meeting->ShowMeetingDlg();
					EndDialog(hwnd, 1);
                break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

int CALLBACK MyIntEditProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CIntPointer;
     switch(uMsg) {
          case WM_CHAR:
			  {
               if( wParam == 13 ) {
					char Msg[200];
					GetDlgItemText(p->Interview->hWnd, IDSEND, Msg, 200);
					p->Winsock->SendData(cmChatMessage, Msg);
					SetDlgItemText(p->Interview->hWnd, IDSEND, NULL);
					char tmpString[255];
					strcpy(&tmpString[0], p->Login->user);
					strcpy(&tmpString[strlen(tmpString)], ": ");
					strcpy(&tmpString[strlen(tmpString)], Msg);
					p->Interview->AppendData(tmpString);
					return(0);
               }
               else return (int)CallWindowProc(wpOrigIntEditProc, hDlg, uMsg, wParam, lParam);
			   break;
			  }
          default:
               return (int)CallWindowProc(wpOrigIntEditProc, hDlg, uMsg, wParam, lParam);
          break;
     }
     return(0);
}


CInterview::CInterview(CGame *game)
{
	p = game;
	CIntPointer = game;
	hWnd = 0;
}

CInterview::~CInterview()
{

}

void CInterview::ShowInterviewDlg()
{
	this->Chat.clear();
	p->State = STATE_INTERVIEW;
	p->Dialog->StartDialog = ID_INTERVIEW;
	p->Dialog->DialogProcessor = &InterviewDlgProc;
}

void CInterview::AppendData(string TheData)
{
	string tmpLine;
	tmpLine = TheData;
	tmpLine += "\r\n";
	if (this->Chat.length() > 1700)
	{
		string tmp;
		tmp.append(&this->Chat[300]);
		this->Chat = tmp;
	}
	this->Chat.append(tmpLine);
	SetDlgItemText(this->hWnd, IDCHAT, this->Chat.c_str());

	SCROLLINFO si;
    si.cbSize = sizeof (si);
    si.fMask  = SIF_ALL;
    GetScrollInfo(this->ChatWnd, SB_VERT, &si);
	PostMessage(this->ChatWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, si.nMax), 0);
	PostMessage(this->ChatWnd, WM_PAINT, 0, 0);
}