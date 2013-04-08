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