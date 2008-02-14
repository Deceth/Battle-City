#include "CGame.h"

CGame Game;

#include <windows.h>

const char g_szClassName[] = "BattleCity";

void CreateMyTooltip (HWND hwnd, HINSTANCE ghThisInstance);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
			Game.running = 0;
			Sleep(50);
            DestroyWindow(hwnd);
			break;
        case WM_DESTROY:
			Game.running = 0;
			Sleep(50);
            PostQuitMessage(0);
			break;
		case WM_SETFOCUS:
		{
			Game.Input->HasFocus = 1;
			if (Game.InGame->Minimized == 1)
			{
				Game.InGame->Minimized = 0;
				if (Game.DDraw->FullScreen == 1) Game.DDraw->DirectDraw->SetDisplayMode(Game.ResolutionX, Game.ResolutionY, 16, 0, 0);
				Game.DDraw->LoadSurfaces();
				Game.DDraw->DirectDraw->RestoreAllSurfaces();
			}
            break;
		}
		case WM_KILLFOCUS:
		{
			Game.Input->HasFocus = 0;
        }
		case WM_CHAR:
			if (Game.State == STATE_GAME)
			{
				if (Game.InGame->IsChatting == 1)
				{
					if (wParam == 13)
					{
						Game.InGame->IsChatting = 0;
						if (Game.InGame->ChatLine.length() > 0)
						{
							if (Game.Player[Game.Winsock->MyIndex]->isAdmin == 2)
							{
								std::stringstream parser(Game.InGame->ChatLine);
								std::string temp;

								parser >> temp;
								std::cout << temp << std::endl;

								if (strcmp(temp.c_str(), "/g") == 0) //Global message
								{
									string tmpString;
									char SendString[255];
									memset(SendString, 0, 255);
									Game.InGame->ChatLine.copy(SendString, tmpString.length() - 3, 3);
									Game.Winsock->SendData(cmGlobal, SendString);
									tmpString = Game.Player[Game.Winsock->MyIndex]->Name + ": " + SendString;
									Game.InGame->AppendChat(tmpString, RGB(255, 165, 0));
									Game.InGame->ChatLine.clear();
								}
								else //Chat Line
								{
									char SendString[255];
									strcpy(SendString, Game.InGame->ChatLine.c_str());
									if (Game.InGame->HasWalkie == 1)
										Game.Winsock->SendData(cmWalkie, SendString);
									else
										Game.Winsock->SendData(cmChatMessage, SendString);
									string tmpString;
									tmpString = Game.Player[Game.Winsock->MyIndex]->Name + ": " + Game.InGame->ChatLine;
									Game.InGame->AppendChat(tmpString, RGB(255, 165, 0));
									Game.InGame->ChatLine.clear();
								}
							}
							else
							{
								char SendString[255];
								strcpy(SendString, Game.InGame->ChatLine.c_str());
								if (Game.InGame->HasWalkie == 1)
									Game.Winsock->SendData(cmWalkie, SendString);
								else
									Game.Winsock->SendData(cmChatMessage, SendString);
								string tmpString;
								tmpString = Game.Player[Game.Winsock->MyIndex]->Name + ": " + Game.InGame->ChatLine;
								if (Game.Player[Game.Winsock->MyIndex]->isDead == true)
								{
									Game.InGame->AppendChat(tmpString, RGB(135, 135, 135));
								}
								else
								{
									Game.InGame->AppendChat(tmpString, RGB(0, 255, 0));
								}
								Game.InGame->ChatLine.clear();
							}
						}
					}
					else
					{
						if (wParam == 8)
						{
							if (Game.InGame->ChatLine.length() > 1)
							{
								Game.InGame->ChatLine.erase(Game.InGame->ChatLine.length() - 1, 1);
							}
							else Game.InGame->ChatLine.clear();
						}
						else
						{
							char blargh[2];
							blargh[1] = 0;
							blargh[0] = (char)wParam;
							if (Game.InGame->ChatLine.length() < 100)
							{
								Game.InGame->ChatLine.append(blargh);
							}
						}
					}
				}
				else
				{
					if (wParam == 13) Game.InGame->IsChatting = 1;

					if (wParam == 117 || wParam == 85 && Game.Player[Game.Winsock->MyIndex]->isFrozen == 0) {
						CItem *item = Game.Item->findItembyLocation(((int)Game.Player[Game.Winsock->MyIndex]->X + 24) / 48, ((int)Game.Player[Game.Winsock->MyIndex]->Y + 24) / 48);
						if (item)
						{
							sCMItem ii;
							ii.id = item->id;
							ii.active = item->active;
							ii.type = item->Type;
							Game.Winsock->SendData(cmItemUp,(char *)&ii,sizeof(ii));
						}
					}
					else if (wParam == 73 || wParam == 105) {
						Game.Input->InfoButton();
					}
					else if (wParam == 79 || wParam == 111) {
						//Drop Orb
						CItem *itm = Game.Inventory->findItembyType(5);
						if (itm)
						{
							sCMItem ii;
							ii.id = itm->id;
							ii.active = itm->active;
							ii.type = itm->Type;
							Game.Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
						}
					}
					else if (wParam == 66 || wParam == 98) {
						//Drop Bomb
						CItem *itm = Game.Inventory->findItembyType(3);
						if (itm)
						{
							sCMItem ii;
							ii.id = itm->id;
							ii.active = 1;
							ii.type = itm->Type;
							Game.Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
						}
					}
					else if (wParam == 72 || wParam == 104) {
						//Use Health Pack
						CItem *itm = Game.Inventory->findItembyType(2);
						if (itm)
						{
							Game.Winsock->SendData(cmMedKit, (char *)&itm->id, sizeof(itm->id));
						}
					}
					else if (wParam == 100 || wParam == 68) {//Drop
						Game.Inventory->Drop();
					}
					else if (wParam == 91 || wParam == 123) {//Refresh Data '['
						//Game.Player[Game.Winsock->MyIndex]->timeFrozen = Game.Timer->GetTime() + 60000;
						//Game.Player[Game.Winsock->MyIndex]->isFrozen = 1;
					}
					else if (wParam == 93 || wParam == 125) {//Reload Surfaces ']'
						if (Game.Tick > Game.Input->lastRefresh)
						{
							Game.Input->lastRefresh = Game.Tick + 3000;
							Game.DDraw->LoadSurfaces();
							Game.DDraw->DirectDraw->RestoreAllSurfaces();
							Game.InGame->AppendChat("The game's surfaces were reloaded.", RGB(255, 165, 0));
						}
					}
					else if (wParam == 77 || wParam == 109) {//'m' - Map
						Game.Draw->PlayerOffsetX = 276;
						Game.Draw->PlayerOffsetY = 276;
						Game.InGame->ShowMap = 1 - Game.InGame->ShowMap;
						Game.Player[Game.Winsock->MyIndex]->isMoving = 0;
						if (Game.InGame->ShowMap == 1) Game.Winsock->SendData(cmMiniMap, " ");
					}
					else if (wParam == 80 || wParam == 112) {//'p' - Points
						Game.Input->PointsButton();
					}
				}
			}
			break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_BC));
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO_BC), ICO_BC, 16, 16, 0);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	int X, Y;
        
	X = (GetSystemMetrics(SM_CXSCREEN) / 2) - (Game.ResolutionX / 2);
	Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (Game.ResolutionY / 2);

	Game.Options->LoadOptions();
	if (Game.Options->fullscreen == 1)
	{
		hwnd = CreateWindowEx(
			0,
			g_szClassName,
			"BattleCity Classic",
			WS_POPUP,
			X, Y, Game.ResolutionX, Game.ResolutionY,
			NULL, NULL, hInstance, NULL);
	}
	else
	{
		Game.DDraw->XOff = 4;
		Game.DDraw->YOff = 30;
		hwnd = CreateWindowEx(
			0,
			g_szClassName,
			"BattleCity Classic",
			WS_DLGFRAME,
			X, Y, Game.ResolutionX + 7, Game.ResolutionY + 33,
			NULL, NULL, hInstance, NULL);
	}

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	Game.Init(hwnd, hInstance);

    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return (int)Msg.wParam;
}