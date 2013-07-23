/*
===============================================================================

    Battle City - CEngine
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
#include "CEngine.h"
/// <summary>   Engine thread </summary>
///
/// <param name="pParam">   [in,out] If non-null, the parameter. </param>
void _cdecl thrEngine(void * pParam);
void _cdecl thrMessageBox(void * pParam);
void _cdecl thrMessageQuit(void * pParam);
/// <summary>   Constructor. Sets default values to variables </summary>
///
/// <param name="game"> [in,out] If non-null, the game. </param>
CEngine::CEngine(CGame *game) {
	p = game;
	this->FPS = 0;
	this->FPSTick = 0;
	this->loops = 0;
	this->MsgUp = 0;
	this->MsgQueue = 0;
}
/// <summary>   Destructor. </summary>
CEngine::~CEngine() {

}
/// <summary>   Starts the engine thread </summary>
void CEngine::Init() {
	_beginthread(thrEngine,0,p);
}
/// <summary>   Thread engine code </summary>
///
/// <param name="pParam">   [in,out] If non-null, the parameter. </param>
void _cdecl thrEngine(void * pParam) {
    //  Prepare p to CGame object
	CGame * p;
    //  Assign pParam to p as CGame object
	p = (CGame *)pParam;
    //  Set character variable with twenty spaces
	char FPSString[20];
    //  Set NextFrame as zero
	float NextFrame = 0;
    //  Set last frame per second as zero
	int lastFPS = 0;
    //  Set last, last frame per second as zero
	int lastlastFPS = 0;
    //  Game loop while p->running is true
	do {
        //  Update last time interval with current time interval
		p->lastTick = p->Tick;
        //  Set current time interval with new time interval
		p->Tick = p->Timer->GetTime();
        //  Set time passed by calling the difference between current time interval and previous time interval
		p->TimePassed = p->Tick - p->lastTick;
        //  Observe the current game state
		switch (p->State) {
            //  Perform game cycle and draw debug information
			case STATE_GAME:
                //  Clear DirectDraw canvas
				p->DDraw->Clear();
                //  Perform game cycle
				p->InGame->Cycle();
                //  Upon verifying debug mode is set FPSString with debug information
				if (p->Options->debug) {
                    //  Compile string and assign to FPSString
					sprintf(FPSString, "FPS: %i", p->Engine->FPS);
                    //  Draw text at bottom right using white font color
					p->DDraw->DTextOut(p->Draw->MaxMapX + 28, 433, FPSString, COLOR_WHITE);
                    //  Compile string and assign to FPSString
					sprintf(FPSString, "Ping: %i", p->InGame->TCPPing);
                    //  Draw text at bottom right below previous text using white font color
					p->DDraw->DTextOut(p->Draw->MaxMapX + 105, 433, FPSString, COLOR_WHITE);
				}
                //  Perform bit blocking transfer
				p->DDraw->Flip();
				break;
            //  Display joining game screen
			case STATE_JOINING:
                //  Clear DirectDraw canvas
				p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2), (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2)-320, (p->ResolutionY/2), 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2)-320, (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2), (p->ResolutionY/2), 320, 240);
                //  Draw text in the middle of the screen using yellow font color
				p->DDraw->DTextOut((p->ResolutionX/2)-112, (p->ResolutionY/2)-20, "Joining Game - Please Wait", COLOR_YELLOW);
                //  Perform bit blocking transfer
				p->DDraw->Flip();
                //  Slow down refresh since joining can take time to finish
				Sleep(10);
				break;			
			case 0:
			case STATE_LOGIN:
			case STATE_RECOVER:
			case STATE_NEWACCOUNT:
			case STATE_PERSONALITY:
			case STATE_INTERVIEW:
			case STATE_MEETING:
            //  Display default screen and wait 10 milliseconds
			case STATE_VERIFY:
				p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2), (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2)-320, (p->ResolutionY/2), 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2)-320, (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2), (p->ResolutionY/2), 320, 240);
				p->DDraw->Flip();
				Sleep(10);
				break;

			default:
				break;
		}
        //  Cycle through Input object
		p->Input->Cycle();
        //  Cycle through Winsock object
		p->Winsock->Cycle();
        //  Upon verifying that current time interval is larger than the engine's FPS tick  
		if (p->Tick > p->Engine->FPSTick) {
            //  Upon verifying state is game state refresh the game area
			if (p->State == STATE_GAME) {
				p->InGame->RefreshArea();
			}
            //  Set lastlastFPS to lastFPS
			lastlastFPS = lastFPS;
            //  Set lastFPS to Engine->FPS
			lastFPS = p->Engine->FPS;
            //  Set FPS to Engine->loops
			p->Engine->FPS = p->Engine->loops;
            //  Set FPSTick to time interval plus 1000
			p->Engine->FPSTick = p->Tick + 1000;
            //  Set loops to zero
			p->Engine->loops = 0;
		}
        //  Update the 3d sound engine
		FSOUND_Update();
        //  Increase loops by one
		p->Engine->loops++;
        //  Verify limitfps is defined
		if (p->Options->limitfps) {
            //  Upon verifying engine FPS is less than 10, lastlastFPS is less than 10, 
            //  and state is STATE_GAME display message box alerting user that the system 
            //  cannot handle the game
            //  Upon verifying that FPS is greater than 30 set next frame to time interval plus 1
            //  Upon verifying that FPS is greater than 50 set next frame to time interval plus 6
            //  Upon verifying that FPS is greater than 60 set next frame to time interval plus 8
            //  Upon verifying that FPS is greater than 70 set next frame to time interval plus 10
            //  Otherwise set next time to time interval plus 12
			if (p->Engine->FPS < 10 && lastFPS < 10 && lastlastFPS < 10 && p->State == STATE_GAME) {
				MessageBox(p->hWnd, "Your computer is running too slow to play BattleCity.  Try closing some other programs or updating your video card drivers.", "BattleCity", 0);
				break;
			} else if (p->Engine->FPS < 30 && lastFPS < 30 && lastlastFPS < 30) {
				NextFrame = p->Tick + 1;
			} else if (p->Engine->FPS < 50 && lastFPS < 50) {
				NextFrame = p->Tick + 6;
			} else if (p->Engine->FPS < 60 && lastFPS < 60 && lastlastFPS < 60) {
				NextFrame = p->Tick + 8;
			} else if (p->Engine->FPS < 70 && lastFPS < 70 && lastlastFPS < 70) {
				NextFrame = p->Tick + 10;
			} else {
				NextFrame = p->Tick + 12;
			}
            //  Loop while current time interval is less than NextFrame time interval
			while (p->Timer->GetTime() < NextFrame) {
                //  Perform Winsock cycle
				p->Winsock->Cycle();
                //  Perform thread sleep
				Sleep(p->Options->sleep);
			}
		} else {
            //  Perform thread sleep
			Sleep(p->Options->sleep);
		}
	}
	while (p->running == 1);
    //  Perform thread sleep before ending
	Sleep(100);
    //  End thread
	_endthread();
}
/// <summary>   Log errors to debug.log </summary>
///
/// <param name="error">    The error. </param>
void CEngine::logerror(std::string error) {
    //  Prepare file handler
	FILE *file;
    //  Open debug.log through file handler
	file = fopen("debug.log","a");
    //  Set linebreak to be 3 characters
	char linebreak[3];
    //  Set carriage return
	linebreak[0] = 13;
    //  Set line feed
	linebreak[1] = 10;
    //  Set Null character
	linebreak[2] = 0;
    //  Set ErrorString to be 255 characters
	char ErrorString[255];
    //  Copy characters from error to ErrorString
	strcpy(ErrorString, error.c_str());
    //  Send error string to game server
	p->Winsock->SendData(cmCrash, ErrorString);
    //  Upon verifying state is STATE_GAME populate chat with error string
	if (p->State == STATE_GAME) {
		error = "WARNING:  Game Crash.  Code:  " + error;
        //  Populate chat with error string
		p->InGame->AppendChat(error.c_str(), COLOR_YELLOW);
        //  Turn sound off
		p->Options->sound = 0;
	}
    //  Append linebreak
	error = linebreak + error;
    //  Write error message to file handler
	fwrite(error.c_str(), error.size(), 1, file); 
    //  Close file handler
	fclose(file);
    //  Clear error object
	error.clear();
}

void _cdecl thrMessageBox(void * pParam) {
	CGame * p;
	p = (CGame *)pParam;

	p->Engine->MsgUp = 1;

	MessageBox(p->hWnd, p->Engine->MsgText.c_str(), "BattleCity", 0);

	p->Engine->MsgUp = 0;
	p->Engine->MsgQueue = 0;
}

void CEngine::ThreadMessage(std::string MsgText) {
	if (this->MsgUp == 1) return;
	this->MsgText = MsgText;
	_beginthread(thrMessageBox, 0, p);
}

void CEngine::ThreadMessageQuit() {
	_beginthread(thrMessageQuit, 0, p);
}

void _cdecl thrMessageQuit(void * pParam) {
	CGame * p;
	p = (CGame *)pParam;

	p->Engine->MsgUp = 1;

	int response = MessageBox(p->hWnd, "Leave BattleCity?", "BattleCity", MB_YESNO || MB_ICONQUESTION);
	if (response == 1) {
		p->State = STATE_MEETING;
		p->Meeting->ShowMeetingDlg();
	}

	p->Engine->MsgUp = 0;
}
