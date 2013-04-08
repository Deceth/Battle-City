/*
===============================================================================

    Battle City - CAdmin
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
#include "CAdmin.h"

void *CAdminPointer;

int CALLBACK AdminDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CAdminPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			p->Admin->hWnd = hwnd;
			p->Admin->DrawPlayerList();
			p->Admin->DrawItemList();
			break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
				case IDKICK:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 1; //Kick player
					admin.id = Index;
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDWARP:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 3; //Warp to player
					admin.id = Index;
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDSUMMON:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 4; //Warp player (summon)
					admin.id = Index;
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDBAN:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 5; //Ban player
					admin.id = Index;
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDSHUTDOWN:
				{
					sCMAdmin admin;
					admin.command = 6; //Shut down server
					admin.id = 0;
					p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDCANCEL:
				{
					p->Admin->IsOpen = 0;
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, IDCANCEL);
				}
				break;
				case IDJOIN:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDCITIES, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 2; //Join City
					admin.id = Index;
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDSPAWN:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDITEMS, LB_GETCURSEL, 0, 0);
					sCMAdmin admin;
					admin.command = 7; //Spawn Item
					admin.id = Index; //Item
					if (Index >= 0) p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDREQUESTBANS:
				{
					SendDlgItemMessage(p->Admin->hWnd, IDBANS, LB_RESETCONTENT, 0, 0);
					sCMAdmin admin;
					admin.command = 8; //Request Bans
					admin.id = 0;
					p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDUNBAN:
				{
					int Index = (int)SendDlgItemMessage(p->Admin->hWnd, IDBANS, LB_GETCURSEL, 0, 0);
					SendDlgItemMessage(p->Admin->hWnd, IDBANS, LB_RESETCONTENT, 0, 0);
					sCMAdmin admin;
					admin.command = 9; //Unban
					admin.id = Index;
					p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDREQUESTNEWS:
				{
					sCMAdmin admin;
					admin.command = 10; //Request News
					admin.id = 0;
					p->Winsock->SendData(cmAdmin, (char *)&admin, sizeof(admin));
				}
				break;
				case IDUPDATENEWS:
				{
					char NewNews[241];
					memset(NewNews, 0, sizeof(NewNews));
					GetDlgItemText(p->Admin->hWnd, IDNEWS, NewNews, 240);
					p->Winsock->SendData(cmChangeNews, NewNews, sizeof(NewNews));
				}
				break;
				case IDACCOUNTEDITOR:
				{
					p->Admin->IsOpen = 0;
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, IDCANCEL);
					p->AdminEdit->ShowAdminEditDlg();
				}
				break;
				case IDUPDATESTARTINGCITY:
				{
					// Get the string
					char NewStartingCity[3];
					memset(NewStartingCity, 0, sizeof(NewStartingCity));
					GetDlgItemText(p->Admin->hWnd, IDSTARTINGCITY, NewStartingCity, sizeof(NewStartingCity));

					// Convert it to an int
					istringstream iss(NewStartingCity);
					int NewStartingCityInt;

					// If the conversion worked (valid number), send it to the server
					if (iss>>NewStartingCityInt) {

						sCMStartingCity request;
						request.City = NewStartingCityInt;
						p->Winsock->SendData(cmChangeStartingCity, (char *)&request, sizeof(request));
					}
				}
				break;
            }
			break;
        default:
            return 0;
			break;
    }
    return 1;
}

CAdmin::CAdmin(CGame *game)
{
	p = game;
	CAdminPointer = game;

	IsOpen = 0;
}

CAdmin::~CAdmin()
{

}

void CAdmin::ShowAdminDlg()
{
	p->Dialog->StartDialog = ID_ADMIN;
	p->Dialog->DialogProcessor = &AdminDlgProc;
	IsOpen = 1;
}

void CAdmin::DrawPlayerList()
{
	int cityPlayers[64];
	memset(cityPlayers, 0, sizeof(cityPlayers));

	SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		string tmpString;
		if (p->Player[i]->Name.length() > 0)
		{
			tmpString = p->Player[i]->Name;
			if (p->Player[i]->isInGame) 
			{
				cityPlayers[p->Player[i]->City]++;
				tmpString += " (InGame)";
			}
		}
		else
		{
			tmpString = "[Empty]";
		}
		SendDlgItemMessage(p->Admin->hWnd, IDPLAYERS, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
	}

	SendDlgItemMessage(p->Admin->hWnd, IDCITIES, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < 64; i++)
	{
		ostringstream CityNumber;
		CityNumber << i;

		string tmpString;
		tmpString = CityNumber.str();
		tmpString += ": ";
		tmpString += CityList[i];

		if (cityPlayers[i] > 0)
		{
			std::ostringstream Convert;
			tmpString += " (";
			Convert << cityPlayers[i];
			tmpString += Convert.str();
			tmpString += ")";
		}
		else
		{
			tmpString += " [Empty]";
		}
		SendDlgItemMessage(p->Admin->hWnd, IDCITIES, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
	}
}

void CAdmin::DrawItemList()
{
	SendDlgItemMessage(p->Admin->hWnd, IDITEMS, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < 12; i++)
	{
		string tmpString;

		tmpString = ItemList[i];

		SendDlgItemMessage(p->Admin->hWnd, IDITEMS, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
	}
}

void CAdmin::AddBan(sSMBan *ban)
{
	string tmpString;
	tmpString = ban->Account;
	tmpString += " - ";
	tmpString += ban->IPAddress;
	tmpString += " - ";
	tmpString += ban->Reason;

	SendDlgItemMessage(p->Admin->hWnd, IDBANS, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
}

void CAdmin::SetNews(string TheNews)
{
	SetDlgItemText(p->Admin->hWnd, IDNEWS, TheNews.c_str());
}


void CAdmin::SetStartingCity(int City)
{
	// Required to make sure the panel is drawn before we try to set its contents
	Sleep(100);

	stringstream ss;
	string cityString;
	ss << City;
	cityString = ss.str();
	SetDlgItemText(p->Admin->hWnd, IDSTARTINGCITY, cityString.c_str());
}