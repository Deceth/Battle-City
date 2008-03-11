#include "CMeeting.h"

HBRUSH g_hbrBackground;
HDC hdcStatic;

void *CMeetPointer;
WNDPROC wpOrigEditProc;
int CALLBACK MyEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigListProc;
int CALLBACK MyListProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigNewsProc;
int CALLBACK MyNewsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpOrigChatProc;
int CALLBACK MyChatProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int CALLBACK MeetDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CMeetPointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			{
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
        case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case IDQUIT:
						SendMessage(p->hWnd, WM_CLOSE, 0, 0);
						break;
					case IDREFRESH:
						p->Winsock->SendData(cmRefreshList, " ");
						for (int i = 0; i < 30; i++)
						{
							p->Meeting->City[i].Mayor = -1;
							p->Meeting->City[i].RealCity = 0;
							p->Meeting->City[i].Index = 0;
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

int CALLBACK MyEditProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
     switch(uMsg) {
          case WM_CHAR:
			  {
               if( wParam == 13 ) {
					char Msg[200];
					GetDlgItemText(p->Meeting->hWnd, IDSEND, Msg, 200);
					p->Winsock->SendData(cmChatMessage, Msg);
					SetDlgItemText(p->Meeting->hWnd, IDSEND, NULL);
					char tmpString[255];
					strcpy(&tmpString[0], p->Login->user);
					strcpy(&tmpString[strlen(tmpString)], ": ");
					strcpy(&tmpString[strlen(tmpString)], Msg);
					p->Meeting->AppendData(tmpString);
					return(0);
               }
               else return (int)CallWindowProc(wpOrigEditProc, hDlg, uMsg, wParam, lParam);
			   break;
			  }
          default:
               return (int)CallWindowProc(wpOrigEditProc, hDlg, uMsg, wParam, lParam);
          break;
     }
     return(0);
}

int CALLBACK MyNewsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
     switch(uMsg) {
          default:
               return (int)CallWindowProc(wpOrigNewsProc, hDlg, uMsg, wParam, lParam);
          break;
     }
     return(0);
}

int CALLBACK MyChatProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
     switch(uMsg) {
          default:
               return (int)CallWindowProc(wpOrigChatProc, hDlg, uMsg, wParam, lParam);
          break;
     }
     return(0);
}

CMeeting::CMeeting(CGame *game)
{
	p = game;
	CMeetPointer = game;

	for (int i = 0; i < 30; i++)
	{
		City[i].Mayor = -1;
		City[i].RealCity = 0;
		City[i].Index = 0;
	}
}

CMeeting::~CMeeting()
{

}

void CMeeting::ShowMeetingDlg()
{
	p->InGame->ClearOut();
	ClearList();
	Chat.clear();
	News.clear();
	hWnd = 0;
	p->State = STATE_MEETING;
	p->Dialog->StartDialog = ID_MEETING;
	p->Dialog->DialogProcessor = &MeetDlgProc;
}

void CMeeting::AppendData(string TheData)
{
	TheData += "\r\n";
	if (this->Chat.length() > 1700)
	{
		string tmp;
		tmp.append(&this->Chat[300]);
		this->Chat = tmp;
	}
	this->Chat.append(TheData);
	SetDlgItemText(this->hWnd, IDCHAT, this->Chat.c_str());

	SCROLLINFO si;
    si.cbSize = sizeof (si);
    si.fMask  = SIF_ALL;
    GetScrollInfo(this->ChatWnd, SB_VERT, &si);
	PostMessage(this->ChatWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, si.nMax), 0);
	PostMessage(this->ChatWnd, WM_PAINT, 0, 0);
}

void CMeeting::ClearList()
{
	for (int j = 0; j < MaxPlayers; j++)
	{
		this->lstPlayers[j] = -1;
	}
}

void CMeeting::AddPlayer(int Index)
{
	if (this->inUse(Index) == false)
	{
		int i = this->FreePlayer();
		if (i != -1)
		{
			this->lstPlayers[i] = Index;
			this->RedrawList();
		}
	}
}

void CMeeting::ClearPlayer(int Index)
{
	for (int j = 0; j < MaxPlayers; j++)
	{
		if (this->lstPlayers[j] == Index) 
		{
			this->lstPlayers[j] = -1;
			this->RedrawList();
			break;
		}
	}
}

void CMeeting::RedrawList()
{
	SendDlgItemMessage(p->Meeting->hWnd, IDLIST, LB_RESETCONTENT, 0, 0);
	for (int j = 0; j < MaxPlayers; j++)
	{
		if (p->Meeting->lstPlayers[j] != -1)
		{
			SendDlgItemMessage(p->Meeting->hWnd, IDLIST, LB_ADDSTRING, 0, (LPARAM)p->Player[p->Meeting->lstPlayers[j]]->Name.c_str());
		}
	}
}
int CMeeting::FreePlayer()
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (this->lstPlayers[i] == -1) return i;
	}

	return -1;
}

void CMeeting::AppendNews(string TheData)
{
	this->News.append(TheData);
	SetDlgItemText(this->hWnd, IDNEWS, this->News.c_str());
}

// Create list of available cities to be displayed in the Meeting Room
void CMeeting::AddCity(int City, int Mayor)
{
	int i = FreeCity();
	if (i != 255)
	{
		this->City[i].RealCity = City;
		this->City[i].Mayor = Mayor;

		string tmpString;
		if (p->Meeting->City[i].Mayor == 255)
		{
			tmpString = "Mayor required for city of ";
			tmpString += CityList[p->Meeting->City[i].RealCity];
			int index = (int)SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			p->Meeting->City[i].Index = index;
			tmpString = "<Double click to become mayor of this city>";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			tmpString = "----------------------------------------------------------------------------------------------";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
		}
		else
		{
			tmpString = "Mayor " + p->Player[p->Meeting->City[i].Mayor]->Name + " of ";
			tmpString += CityList[p->Meeting->City[i].RealCity];
			int index = (int)SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			p->Meeting->City[i].Index = index;
			tmpString = "<Double click to apply for a job>";			
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
			tmpString = "----------------------------------------------------------------------------------------------";
			SendDlgItemMessage(p->Meeting->hWnd, IDAPPLYLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
		}
	}

}

int CMeeting::FreeCity()
{
	for (int i = 0; i < 30; i++)
	{
		if (City[i].Mayor == -1) return i;
	}

	return 255;
}

bool CMeeting::inUse(int Index)
{
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (this->lstPlayers[i] == Index) return true;
	}

	return false;
}

int CALLBACK MyListProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CMeetPointer;
     switch(uMsg) {
		case WM_LBUTTONDBLCLK:
		{
			int index = (int)SendMessage(hDlg, LB_GETCURSEL, 0, 0);
			for (int i = 0; i < 30; i++)
			{
				if (p->Meeting->City[i].Index == index)
				{
					char packet[1];
					packet[0] = (unsigned char)p->Meeting->City[i].RealCity;
					p->Winsock->SendData(cmJobApp,packet,1);
					p->Dialog->StartDialog = 0;

					for (int i = 0; i < 30; i++)
					{
						p->Meeting->City[i].Mayor = -1;
						p->Meeting->City[i].RealCity = 0;
						p->Meeting->City[i].Index = 0;
					}

					EndDialog(p->Meeting->hWnd, 1);
					return 0;
				}
			}
			for (int i = 0; i < 30; i++)
			{
				if (p->Meeting->City[i].Index == (index - 1))
				{
					char packet[1];
					packet[0] = (unsigned char)p->Meeting->City[i].RealCity;
					p->Winsock->SendData(cmJobApp,packet,1);
					p->Dialog->StartDialog = 0;

					for (int i = 0; i < 30; i++)
					{
						p->Meeting->City[i].Mayor = -1;
						p->Meeting->City[i].RealCity = 0;
						p->Meeting->City[i].Index = 0;
					}
					EndDialog(p->Meeting->hWnd, 1);
					return 0;
				}
			}
		}
		break;
        default:
            return (int)CallWindowProc(wpOrigListProc, hDlg, uMsg, wParam, lParam);
        break;
     }
     return(0);
}
