#include "CEngine.h"
void _cdecl thrEngine(void * pParam);
void _cdecl thrMessageBox(void * pParam);
void _cdecl thrMessageQuit(void * pParam);

CEngine::CEngine(CGame *game) {
	p = game;
	this->FPS = 0;
	this->FPSTick = 0;
	this->loops = 0;
	this->MsgUp = 0;
	this->MsgQueue = 0;
}

CEngine::~CEngine() {

}

void CEngine::Init() {
	_beginthread(thrEngine,0,p);
}

void _cdecl thrEngine(void * pParam) {
	CGame * p;
	p = (CGame *)pParam;

	char FPSString[20];
	float NextFrame = 0;
	int lastFPS = 0;
	int lastlastFPS = 0;

	do {
		p->lastTick = p->Tick;
		p->Tick = p->Timer->GetTime();
		p->TimePassed = p->Tick - p->lastTick;

		switch (p->State) {

			case 0:

			case STATE_LOGIN:

			case STATE_RECOVER:

			case STATE_NEWACCOUNT:

			case STATE_PERSONALITY:

			case STATE_INTERVIEW:

			case STATE_MEETING:

			case STATE_VERIFY:
				p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2), (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2)-320, (p->ResolutionY/2), 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2)-320, (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2), (p->ResolutionY/2), 320, 240);
				p->DDraw->Flip();
				Sleep(10);
				break;

			case STATE_JOINING:
				p->DDraw->Clear();
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2), (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgCompany, (p->ResolutionX/2)-320, (p->ResolutionY/2), 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2)-320, (p->ResolutionY/2)-240, 320, 240);
				p->DDraw->Draw(p->DDraw->imgBCLogo, (p->ResolutionX/2), (p->ResolutionY/2), 320, 240);
				p->DDraw->DTextOut((p->ResolutionX/2)-112, (p->ResolutionY/2)-20, "Joining Game - Please Wait", COLOR_YELLOW);
				p->DDraw->Flip();
				Sleep(10);
				break;

			case STATE_GAME:
				p->DDraw->Clear();
				p->InGame->Cycle();
				if (p->Options->debug) {
					sprintf(FPSString, "FPS: %i", p->Engine->FPS);
					p->DDraw->DTextOut(p->Draw->MaxMapX + 28, 433, FPSString, COLOR_WHITE);
					sprintf(FPSString, "Ping: %i", p->InGame->TCPPing);
					p->DDraw->DTextOut(p->Draw->MaxMapX + 105, 433, FPSString, COLOR_WHITE);
				}
				p->DDraw->Flip();
		}

		p->Input->Cycle();
		p->Winsock->Cycle();

		if (p->Tick > p->Engine->FPSTick) {			
			if (p->State == STATE_GAME) {
				p->InGame->RefreshArea();
			}
			lastlastFPS = lastFPS;
			lastFPS = p->Engine->FPS;
			p->Engine->FPS = p->Engine->loops;
			p->Engine->FPSTick = p->Tick + 1000;
			p->Engine->loops = 0;
		}

		FSOUND_Update();

		p->Engine->loops++;

		if (p->Options->limitfps) {
			if (p->Engine->FPS < 10 && lastFPS < 10 && lastlastFPS < 10 && p->State == STATE_GAME) {
				MessageBox(p->hWnd, "Your computer is running too slow to play BattleCity.  Try closing some other programs or updating your video card drivers.", "BattleCity", 0);
				break;
			}
			else if (p->Engine->FPS < 30 && lastFPS < 30 && lastlastFPS < 30) {
				NextFrame = p->Tick + 1;
			}
			else if (p->Engine->FPS < 50 && lastFPS < 50) {
				NextFrame = p->Tick + 6;
			}
			else if (p->Engine->FPS < 60 && lastFPS < 60 && lastlastFPS < 60) {
				NextFrame = p->Tick + 8;
			}
			else if (p->Engine->FPS < 70 && lastFPS < 70 && lastlastFPS < 70) {
				NextFrame = p->Tick + 10;
			}
			else {
				NextFrame = p->Tick + 12;
			}

			while (p->Timer->GetTime() < NextFrame) {
				p->Winsock->Cycle();
				Sleep(p->Options->sleep);
			}
		}
		else {
			Sleep(p->Options->sleep);
		}

	}
	while (p->running == 1);

	Sleep(100);

	_endthread();
}

void CEngine::logerror(std::string error) {
	FILE *file;
	file = fopen("debug.log","a");
	char linebreak[3];
	linebreak[0] = 13;
	linebreak[1] = 10;
	linebreak[2] = 0;

	char ErrorString[255];
	strcpy(ErrorString, error.c_str());
	p->Winsock->SendData(cmCrash, ErrorString);

	if (p->State == STATE_GAME) {
		error = "WARNING:  Game Crash.  Code:  " + error;
		p->InGame->AppendChat(error.c_str(), COLOR_YELLOW);
		p->Options->sound = 0;
	}

	error = linebreak + error;

	fwrite(error.c_str(), error.size(), 1, file); 

	fclose(file);

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
