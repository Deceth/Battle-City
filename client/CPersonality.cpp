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