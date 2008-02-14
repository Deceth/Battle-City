#include "CManage.h"

void *CManagePointer;
WNDPROC wpCommandoListProc;
int CALLBACK CommandoListProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int CALLBACK ManageDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CGame *p = (CGame *)CManagePointer;
    switch(Message)
    {
        case WM_INITDIALOG:
			p->Manage->hWnd = hwnd;
			p->Manage->CommandoWnd = GetDlgItem(hwnd, IDLIST);
			wpCommandoListProc = (WNDPROC)SetWindowLong(p->Manage->CommandoWnd, GWL_WNDPROC, (LONG)&CommandoListProc);
			p->Manage->DisplayCommandos();
			return 1;
			break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case 1:
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 0);
					break;
				case 2: //Fire
					{
						char packet[2];
						packet[1] = 0;
						int Index = (int)SendMessage(p->Manage->CommandoWnd, LB_GETCURSEL, 0, 0);
						for (int i = 0; i < 4; i++)
						{
							if (p->Manage->Commando[i].Index == Index)
							{
								char packet[2];
								packet[0] = (unsigned char)p->Manage->Commando[i].TheCommando;
								packet[1] = 0;
								p->Winsock->SendData(cmFired,packet,1);
								p->Dialog->StartDialog = 0;
								EndDialog(p->Manage->hWnd, 1);
								return 0;
							}
						}
						p->Dialog->StartDialog = 0;
						EndDialog(hwnd, 0);
					}
					break;
				case 4: //Successor
					{
						char packet[2];
						packet[1] = 0;
						int Index = (int)SendMessage(p->Manage->CommandoWnd, LB_GETCURSEL, 0, 0);
						for (int i = 0; i < 4; i++)
						{
							if (p->Manage->Commando[i].Index == Index)
							{
								char packet[2];
								packet[0] = (unsigned char)p->Manage->Commando[i].TheCommando;
								packet[1] = 0;
								p->InGame->Successor = p->Manage->Commando[i].TheCommando;
								p->Winsock->SendData(cmSuccessor,packet,1);
								p->Manage->DisplayCommandos();
								return 0;
							}
						}
					}
					break;
				case 5: // Set Mayor
					{
						char packet[2];
						packet[1] = 0;
						int Index = (int)SendMessage(p->Manage->CommandoWnd, LB_GETCURSEL, 0, 0);
						for (int i = 0; i < 4; i++)
						{
							if (p->Manage->Commando[i].Index == Index)
							{
								char packet[2];
								packet[0] = (unsigned char)p->Manage->Commando[i].TheCommando;
								packet[1] = 0;
								p->Winsock->SendData(cmSetMayor,packet,1);
								p->Dialog->StartDialog = 0;
								EndDialog(p->Manage->hWnd, 1);
								return 0;
							}
						}
					}
            }
			break;
        default:
            return 0;
    }
    return 1;
}

int CALLBACK CommandoListProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CGame *p = (CGame *)CManagePointer;
     switch(uMsg) {
		case WM_LBUTTONDBLCLK:
		{
			int Index = (int)SendMessage(hDlg, LB_GETCURSEL, 0, 0);
			for (int i = 0; i < 4; i++)
			{
				if (p->Manage->Commando[i].Index == Index)
				{
					p->Manage->Commando[i].IsSuccessor = 1;
					char packet[2];
					packet[0] = (unsigned char)p->Manage->Commando[i].TheCommando;
					packet[1] = 0;
					p->InGame->Successor = p->Manage->Commando[i].TheCommando;
					p->Winsock->SendData(cmSuccessor,packet,1);
					p->Manage->DisplayCommandos();
					return 0;
				}
			}
		}
		break;
        default:
            return (int)CallWindowProc(wpCommandoListProc, hDlg, uMsg, wParam, lParam);
        break;
     }
     return(0);
}

CManage::CManage(CGame *game)
{
	p = game;
	CManagePointer = game;
	hWnd = 0;
	for (int i = 0; i < 4; i++)
	{
		Commando[i].TheCommando = 0;
		Commando[i].Index = 0;
		Commando[i].IsSuccessor = 0;
	}
}

CManage::~CManage()
{

}

void CManage::ShowManageDlg()
{
	p->Dialog->StartDialog = ID_ORGANIZE;
	p->Dialog->DialogProcessor = &ManageDlgProc;
}

void CManage::AddCommando(int Commando, int Successor)
{
	int i = FreeCommando();
	if (i != 255)
	{
		this->Commando[i].TheCommando = Commando;
		this->Commando[i].IsSuccessor = Successor;

		string tmpString;
		tmpString = p->Player[Commando]->Name;
		if (Successor == 1)
			tmpString += " (*)";
		int Index = (int)SendDlgItemMessage(p->Manage->hWnd, IDLIST, LB_ADDSTRING, 0, (LPARAM)tmpString.c_str());
		p->Manage->Commando[i].Index = Index;
	}

}

int CManage::FreeCommando()
{
	for (int i = 0; i < 4; i++)
	{
		if (Commando[i].TheCommando == 0) return i;
	}

	return 255;
}

void CManage::DisplayCommandos()
{
	for (int i = 0; i < 4; i++)
	{
		Commando[i].TheCommando = 0;
		Commando[i].Index = 0;
		Commando[i].IsSuccessor = 0;
	}
	SendDlgItemMessage(p->Manage->hWnd, IDLIST, LB_RESETCONTENT, 0, 0);
	for (int i = 0; i < MaxPlayers; i++)
	{
		if (i != p->Winsock->MyIndex)
		{
			if (p->InGame->Successor == i)
			{
				if (p->Player[i]->City == p->Player[p->Winsock->MyIndex]->City) p->Manage->AddCommando(i, 1);
			}
			else
			{
				if (p->Player[i]->City == p->Player[p->Winsock->MyIndex]->City) p->Manage->AddCommando(i, 0);
			}
		}
	}

}