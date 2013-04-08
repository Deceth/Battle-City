/*
===============================================================================

    Battle City - CPersonnel
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
#include "CPersonnel.h"

void *CPersonnelPointer;
WNDPROC wpOrigPersEditProc;
int CALLBACK MyPersEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int CALLBACK PersonnelDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CPersonnelPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			{
				p->Personnel->hWnd = hwnd;
				SetDlgItemText(p->Personnel->hWnd, IDCHAT, p->Personnel->Chat.c_str());
				p->Personnel->EditWnd = GetDlgItem(hwnd,IDSEND);
				p->Personnel->ChatWnd = GetDlgItem(hwnd,IDCHAT);
				SendDlgItemMessage(hwnd, IDSEND, EM_LIMITTEXT, 200, 0);
				wpOrigPersEditProc = (WNDPROC) SetWindowLong(p->Personnel->EditWnd, GWL_WNDPROC, (LONG)&MyPersEditProc); 
				SCROLLINFO si;
				si.cbSize = sizeof (si);
				si.fMask  = SIF_ALL;
				GetScrollInfo(p->Personnel->ChatWnd, SB_VERT, &si);
				PostMessage(p->Personnel->ChatWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, si.nMax), 0);
				PostMessage(p->Personnel->ChatWnd, WM_PAINT, 0, 0);
				if (p->Personnel->DenyApplicants) CheckDlgButton(hwnd, 1104, p->Personnel->DenyApplicants);
				return 1;
			}
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					{
						int i = IsDlgButtonChecked(hwnd, 1104);
						if (p->Personnel->DenyApplicants != i)
						{
							p->Personnel->DenyApplicants = i;
							char packet[2];
							packet[0] = (char)p->Personnel->DenyApplicants;
							packet[1] = 0;
							p->Winsock->SendData(cmIsHiring, packet, 1);
						}
						p->Dialog->StartDialog = 0;
						EndDialog(hwnd, 0);
					}
                break;
				case IDREJECT:
					{
						int i = IsDlgButtonChecked(hwnd, 1104);
						if (p->Personnel->DenyApplicants != i)
						{
							p->Personnel->DenyApplicants = i;
							char packet[2];
							packet[0] = (char)p->Personnel->DenyApplicants;
							packet[1] = 0;
							p->Winsock->SendData(cmIsHiring, packet, 1);
						}
						p->Winsock->SendData(cmHireDecline, " ", 1);
						p->Dialog->StartDialog = 0;
						p->Personnel->ApplicantWaiting = 0;
						EndDialog(hwnd, 0);
					}
				break;
				case IDWELCOME:
					{
						int i = IsDlgButtonChecked(hwnd, 1104);
						if (p->Personnel->DenyApplicants != i)
						{
							p->Personnel->DenyApplicants = i;
							char packet[2];
							packet[0] = (char)p->Personnel->DenyApplicants;
							packet[1] = 0;
							p->Winsock->SendData(cmIsHiring, packet, 1);
						}
						p->Winsock->SendData(cmHireAccept, " ");
						p->Dialog->StartDialog = 0;
						p->Personnel->ApplicantWaiting = 0;
						EndDialog(hwnd, 0);
					}
				break;
				case IDORGANIZE:
					{
						int i = IsDlgButtonChecked(hwnd, 1104);
						if (p->Personnel->DenyApplicants != i)
						{
							p->Personnel->DenyApplicants = i;
							char packet[2];
							packet[0] = (char)p->Personnel->DenyApplicants;
							packet[1] = 0;
							p->Winsock->SendData(cmIsHiring, packet, 1);
						}
						p->Manage->ShowManageDlg();
						EndDialog(hwnd, 0);
					}
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

int CALLBACK MyPersEditProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CPersonnelPointer;
     switch(uMsg) {
          case WM_CHAR:
			  {
               if( wParam == 13 ) {
					char Msg[200];
					GetDlgItemText(p->Personnel->hWnd, IDSEND, Msg, 200);
					p->Winsock->SendData(cmComms, Msg);
					SetDlgItemText(p->Personnel->hWnd, IDSEND, NULL);
					char tmpString[255];
					strcpy(&tmpString[0], p->Login->user);
					strcpy(&tmpString[strlen(tmpString)], ": ");
					strcpy(&tmpString[strlen(tmpString)], Msg);
					p->Personnel->AppendData(tmpString);
					return(0);
               }
               else return (int)CallWindowProc(wpOrigPersEditProc, hDlg, uMsg, wParam, lParam);
			   break;
			  }
          default:
               return (int)CallWindowProc(wpOrigPersEditProc, hDlg, uMsg, wParam, lParam);
          break;
     }
     return(0);
}


CPersonnel::CPersonnel(CGame *game)
{
	p = game;
	CPersonnelPointer = game;
	this->Chat.clear();
	DenyApplicants = 0;
	Successor = 0;
	ApplicantWaiting = 0;
	ApplicantTick = 0;
	ApplicantAnim = 0;
}

CPersonnel::~CPersonnel()
{

}

void CPersonnel::ShowPersonnelDialog()
{
	p->Dialog->StartDialog = ID_PERSONNEL;
	p->Dialog->DialogProcessor = &PersonnelDlgProc;
}

void CPersonnel::AppendData(string TheData)
{
	string tmpLine;
	tmpLine.clear();
	tmpLine = TheData + "\r\n";
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