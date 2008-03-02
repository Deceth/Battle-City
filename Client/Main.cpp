#include "CGame.h"

CGame Game;

#include <windows.h>

const char g_szClassName[] = "BattleCity";

void CreateMyTooltip (HWND hwnd, HINSTANCE ghThisInstance);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
		// Close window
        case WM_CLOSE:
			Game.running = 0;
			Sleep(50);
            DestroyWindow(hwnd);
			break;
		// Destory window
        case WM_DESTROY:
			Game.running = 0;
			Sleep(50);
            PostQuitMessage(0);
			break;
		// Focus on window
		case WM_SETFOCUS:
			Game.Input->HasFocus = 1;
			if (Game.InGame->Minimized == 1) {
				Game.InGame->Minimized = 0;
				if (Game.DDraw->FullScreen == 1) {
					Game.DDraw->DirectDraw->SetDisplayMode(Game.ResolutionX, Game.ResolutionY, 16, 0, 0);
				}
				Game.DDraw->LoadSurfaces();
				Game.DDraw->DirectDraw->RestoreAllSurfaces();
			}
            break;
		// Remove focus from window
		case WM_KILLFOCUS:
			Game.Input->HasFocus = 0;
            break;
		// Any keyboard character
		case WM_CHAR:

			// If the player is in game,
			if (Game.State == STATE_GAME) {
				
				// If the player is chatting,
				if (Game.InGame->IsChatting == 1) {
				
					// Key: ENTER
					if (wParam == 13) {

						// Clear out of chat
						Game.InGame->IsChatting = 0;

						// If there was text to send,
						if (Game.InGame->ChatLine.length() > 0) {

							// If GLOBAL and ADMIN,
							if ((Game.InGame->ChatLine.find("/g",0) == 0) && (Game.Player[Game.Winsock->MyIndex]->isAdmin == 2)) {
								Game.Send->SendGlobal();
							}

							// Else, if PM,
							else if (Game.InGame->ChatLine.find("/pm",0) == 0) {
								Game.Send->SendWhisper();
							}

							// Else, NORMAL,
							else {
								Game.Send->SendMessage();
							}
							// Clear the chat line
							Game.InGame->ChatLine.clear();
						}
					}


					// KEY: backspace
					else if (wParam == 8) {
						// If there is more than one char, delete the last
						if (Game.InGame->ChatLine.length() > 1) {
							Game.InGame->ChatLine.erase(Game.InGame->ChatLine.length() - 1, 1);
						}
						// Else, clear the only char in the chatline
						else {
							Game.InGame->ChatLine.clear();
						}
					}

					// Key: ESCAPE
					else if (wParam == 27) {
						// Clear and close the chat
						Game.InGame->IsChatting = 0;
						Game.InGame->ChatLine.clear();
					}

					// KEY: any
					else {
						// Append the key to the chat line
						char blargh[2];
						blargh[1] = 0;
						blargh[0] = (char)wParam;

						// TODO: test that the char is alphanumeric

						if (Game.InGame->ChatLine.length() < 100) {
							Game.InGame->ChatLine.append(blargh);
						}
					}
				}
				
				// Else, user is not chatting,
				else {

					// KEY: Enter
					if (wParam == 13) {
						// Open chat
						Game.InGame->IsChatting = 1;
					}

					// KEY: u or F6 (and user isn't frozen)
					if (((wParam == 85) || (wParam == 117)) && (Game.Player[Game.Winsock->MyIndex]->isFrozen == 0)) {
						// Get the item under the user's current position
						CItem *item = Game.Item->findItembyLocation(((int)Game.Player[Game.Winsock->MyIndex]->X + 24) / 48, ((int)Game.Player[Game.Winsock->MyIndex]->Y + 24) / 48);
						// If found, send a command to pick it up
						if (item) {
							sCMItem ii;
							ii.id = item->id;
							ii.active = item->active;
							ii.type = item->Type;
							Game.Winsock->SendData(cmItemUp,(char *)&ii,sizeof(ii));
						}
					}
					// KEY: i or NUMPAD9
					else if ((wParam == 73) || (wParam == 105)) {
						// Trigger the info button
						Game.Input->InfoButton();
					}
					// KEY: o or NUMPAD/
					else if ((wParam == 79) || (wParam == 111)) {
						// Get an orb from the user's inventory
						CItem *itm = Game.Inventory->findItembyType(5);
						// If found, send a command to drop it 
						if (itm) {
							sCMItem ii;
							ii.id = itm->id;
							ii.active = itm->active;
							ii.type = itm->Type;
							Game.Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
						}
					}
					// KEY: b or NUMPAD2
					else if ((wParam == 66) || (wParam == 98)) {
						// Get a bomb from the user's inventory
						CItem *itm = Game.Inventory->findItembyType(3);
						// If found, activate it then send a command to drop it
						if (itm) {
							sCMItem ii;
							ii.id = itm->id;
							ii.active = 1;
							ii.type = itm->Type;
							Game.Winsock->SendData(cmItemDrop,(char *)&ii,sizeof(ii));
						}
					}
					// KEY: h or NUMPAD8
					else if ((wParam == 72) || (wParam == 104) ) {
						// Get a health pack from the user's inventory
						CItem *itm = Game.Inventory->findItembyType(2);
						// If found, send a command to use it
						if (itm) {
							Game.Winsock->SendData(cmMedKit, (char *)&itm->id, sizeof(itm->id));
						}
					}
					// KEY: b or NUMPAD4 
					else if ((wParam == 68) || (wParam == 100)) {
						// Drop the selected item
						Game.Inventory->Drop();
					}
					// KEY: [
					else if ((wParam == 123) || (wParam == 91)) {
						//Refresh Data
						//Game.Player[Game.Winsock->MyIndex]->timeFrozen = Game.Timer->GetTime() + 60000;
						//Game.Player[Game.Winsock->MyIndex]->isFrozen = 1;
					}
					// KEY: ]
					else if ((wParam == 93) || wParam == 125) {
						// Reload Surfaces
						if (Game.Tick > Game.Input->lastRefresh) {
							Game.Input->lastRefresh = Game.Tick + TIMER_RELOAD_SURFACES;
							Game.DDraw->LoadSurfaces();
							Game.DDraw->DirectDraw->RestoreAllSurfaces();
							Game.InGame->AppendChat("The game's surfaces were reloaded.", RGB(255, 165, 0));
						}
					}
					// KEY: m or NUMPAD-
					else if ((wParam == 77) || (wParam == 109)) {
						// Center the player, freeze the player
						Game.Draw->PlayerOffsetX = 276;
						Game.Draw->PlayerOffsetY = 276;
						Game.Player[Game.Winsock->MyIndex]->isMoving = 0;

						// Toggle the minimap, and request it if it is now open
						Game.InGame->ShowMap = 1 - Game.InGame->ShowMap;
						if (Game.InGame->ShowMap == 1) {
							Game.Winsock->SendData(cmMiniMap, " ");
						}
					}
					// KEY: p or F1
					else if ((wParam == 80) || (wParam == 112)) {
						// Trigger the points button
						Game.Input->PointsButton();
					}
				}
			}
			break;
		// Default action: pass the key through to the window
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