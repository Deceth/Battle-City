#ifndef _CINPUT
#define _CINPUT

#include "CGame.h"

class CGame;

class CInput {

	public:
		CInput(CGame *game);
		~CInput();

		void StartDInput();
		void Cycle();
		void InfoButton();
		void PointsButton();

		char MouseOverChat;
		char MouseOverInfo;
		float lastRefresh;
		int LastMouseX;
		int LastMouseY;
		int HasFocus;

	private:
		CGame *p;

		LPDIRECTINPUT8         lpDI;
		LPDIRECTINPUTDEVICE8   m_Keyboard;
		LPDIRECTINPUTDEVICE8   m_Mouse;
		POINT Mouse;
		POINT oMouse;
		DIMOUSESTATE oMouseState;
	
		float LastShot;
		float LastTankChange;
		char Tab;
		int NeedRelease;
		float DemolishTimer;

		void ProcessKeys(char buffer[256]);
		void MouseMove(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
		void MouseUp(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
		void MouseDown(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]);
		void endMouseDown(DIMOUSESTATE mouse_state);
		void endMouseDown(DIMOUSESTATE mouse_state, bool setNeedRelease);

		void ChangeTank(int tankNumber);

	protected:

};

#endif