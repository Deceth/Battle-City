/*
===============================================================================

    Battle City - Main
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

		// Destroy window
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

			// Pass it to the game to handle
			Game.handleKey(wParam);
			break;

		// Default action: pass the key through to the window
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	int X, Y;
        
	X = (GetSystemMetrics(SM_CXSCREEN) / 2) - (Game.ResolutionX / 2);
	Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (Game.ResolutionY / 2);

	Game.Options->LoadOptions();
	if (Game.Options->fullscreen == 1) {
		hwnd = CreateWindowEx(
			0,
			g_szClassName,
			"Battle City",
			WS_POPUP,
			X, Y, Game.ResolutionX, Game.ResolutionY,
			NULL, NULL, hInstance, NULL);
	}

	else {
		Game.DDraw->XOff = 4;
		Game.DDraw->YOff = 30;
		hwnd = CreateWindowEx(
			0,
			g_szClassName,
			"Battle City",
			WS_DLGFRAME,
			X, Y, Game.ResolutionX + 7, Game.ResolutionY + 33,
			NULL, NULL, hInstance, NULL);
	}

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	Game.Init(hwnd, hInstance);

    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return (int)Msg.wParam;

}
