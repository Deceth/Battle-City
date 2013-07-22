/*
===============================================================================

    Battle City - Main
    Copyright (C) 2005-2013  battlecity.org

    Contains the main application function. 

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

/** @file: Main.cpp Contains functions related to starting Battle City */

#include "CGame.h"
/// <summary>   The game. </summary>
CGame Game;

#include <windows.h>

const char g_szClassName[] = "BattleCity";

void CreateMyTooltip (HWND hwnd, HINSTANCE ghThisInstance);

/// <summary>   Method will handle incoming messages from the specified window handler.
/// 			http://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx
/// </summary>
///
/// <param name="hwnd">     Pointer to window handler</param>
/// <param name="msg">      The message </param>
/// <param name="wParam">   Additional message parameters </param>
/// <param name="lParam">   Additional message parameters</param>
///
/// <returns>   . </returns>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//	Observe the message value
    switch(msg) {
		//	Closing the client
        case WM_CLOSE:
			//	Set running to false
            Game.running = 0;
			//	Perform thread sleep for 50 milliseconds
            Sleep(50);
			//	Destory the window handler by sending WM_DESTROY
            DestroyWindow(hwnd);
            break;
        //	Destroys the window handler, flushing thread, and other clean-up operations
        //	http://msdn.microsoft.com/en-us/library/windows/desktop/ms632682(v=vs.85).aspx
        case WM_DESTROY:
			//	Set running to false
            Game.running = 0;
			//	Perform thread sleep for 50 milliseconds
            Sleep(50);
			//	Send the WM_QUIT message
            PostQuitMessage(0);
            break;

        //	Keyboard has been focused on window
        //	http://msdn.microsoft.com/en-us/library/windows/desktop/ms646283(v=vs.85).aspx
        case WM_SETFOCUS:
			//	Set HasFocus to true that device input is on the window
            Game.Input->HasFocus = 1;
			//	Upon verifying that minimized is true
            if (Game.InGame->Minimized == 1) {
				//	Set miminized to false since we have input focus
                Game.InGame->Minimized = 0;
				//	Upon verifying that game is set to full screen
                if (Game.DDraw->FullScreen == 1) {
					//	Set display mode for direct draw specifying width, height, bits per pixel, refresh rate, and flags
					//	http://msdn.microsoft.com/en-us/library/windows/desktop/gg426156(v=vs.85).aspx
                    Game.DDraw->DirectDraw->SetDisplayMode(Game.ResolutionX, Game.ResolutionY, 16, 0, 0);
                }
				//	Load bitmap resources into DirectDraw surface objects
                Game.DDraw->LoadSurfaces();
				//	Restore surfaces in the order loaded
				//	http://msdn.microsoft.com/en-us/library/aa911217.aspx
                Game.DDraw->DirectDraw->RestoreAllSurfaces();
            }
            break;
		//	Keyboard focus has been removed from window
		//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms646282(v=vs.85).aspx
        case WM_KILLFOCUS:
			//	Set HasFocus to false
            Game.Input->HasFocus = 0;
            break;
		//	Incoming keyboard character
		//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms646276(v=vs.85).aspx
        case WM_CHAR:
            //	Pass character to the Game
            Game.handleKey(wParam);
            break;
		// Default windows procedure for handling all other messages not handled.
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
	//	Zero indicates message has been processed
    return 0;
}

/// <summary>   Entry point for the Windows-based application.
/// 			http://msdn.microsoft.com/en-us/library/windows/desktop/ms633559(v=vs.85).aspx
/// </summary>
///
/// <param name="hInstance">        Handle for current instance </param>
/// <param name="hPrevInstance">    Previous handle instance </param>
/// <param name="lpCmdLine">        Command line provided in application launch </param>
/// <param name="nCmdShow">         Control how the window is to be shown </param>
///
/// <returns>   . </returns>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//	Set window class information structure
    WNDCLASSEX wc;
	//	Set window handler structure
    HWND hwnd;
	//	Set message structure
    MSG Msg;
	//	Build the window structure
	//	Set the size of the structure
    wc.cbSize        = sizeof(WNDCLASSEX);
	//	Set the window class style
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
    wc.style         = 0;
	//	Define the window procedure
    wc.lpfnWndProc   = WndProc;
	//	Set the number of extra bytes to allocate following the windows class structure
    wc.cbClsExtra    = 0;
	// Set the number of extra bytes to allocate following the window instance
    wc.cbWndExtra    = 0;
	//	Define the instance handler for the window procedure
    wc.hInstance     = hInstance;
	//	Load and set the window icon
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_BC));
	//	Load and set the default windows cursor
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	//	Load the COLOR_WINDOW HBrush then add one to the pointer
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//	Set the menu pointer
    wc.lpszMenuName  = NULL;
	//	Set the class name for the menu
    wc.lpszClassName = g_szClassName;
	//	Set the small icon handler
    wc.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO_BC), ICO_BC, 16, 16, 0);
	//	Upon verifying that the window class registration failed
    if(!RegisterClassEx(&wc)) {
		//	Display message box alerting the end-user
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	//	Initialize X and Y variables
    int X, Y;
    //	Set the screen width by retrieving the primary screen's width, dividing by two then subtracting the value of game's width resolution divided by two
    X = (GetSystemMetrics(SM_CXSCREEN) / 2) - (Game.ResolutionX / 2);
	//	Set the screen height by retrieving the primary screen's height, dividing by two then subtracting the value of game's height resolution divided by two
    Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (Game.ResolutionY / 2);
	//	Load the game options
    Game.Options->LoadOptions();
	//	Upon verifying the game is set to fullscreen
    if (Game.Options->fullscreen == 1) {
		//	Create Window with extended window style
		//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx
        hwnd = CreateWindowEx(
            0,
            g_szClassName,
            "Battle City",
            WS_POPUP,
            X, Y, Game.ResolutionX, Game.ResolutionY,
            NULL, NULL, hInstance, NULL);
    } else {
		//	Otherwise, create Window with extended window style 
		//	with width and height offset for window mode
		//
		//	Set DirectDraw width and height offset
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
	//	Upon verifying window handler is null
    if(hwnd == NULL) {
		//	Display message box to alert user window creation failed
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	//	Sets the window's show state
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms633548(v=vs.85).aspx
    ShowWindow(hwnd, nCmdShow);
	//	Sends the window handler the WM_PAINT message
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/dd145167(v=vs.85).aspx
    UpdateWindow(hwnd);
	//	
	//	Window has been created; therefore, it is time to initialize the Game
	//	
	//	Pass window handler and instance handler to Game object
    Game.Init(hwnd, hInstance);
	//	Initialize "game loop" by continuing to receive messages from WinMain
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		//	Add the received message to the thread's message queue
		//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms644955(v=vs.85).aspx
        TranslateMessage(&Msg);
		//	Dispatches the message to the window procedure
		//	http://msdn.microsoft.com/en-us/library/windows/desktop/ms644934(v=vs.85).aspx
        DispatchMessage(&Msg);
    }
	//	WM_QUIT will terminate the application; otherwise, continue running
    return (int)Msg.wParam;
}
