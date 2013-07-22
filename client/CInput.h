/*
===============================================================================

    Battle City - CInput header file
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
#ifndef _CINPUT
#define _CINPUT

#include "CGame.h"

class CGame;

class CInput {

    public:
        CInput(CGame *game);
        ~CInput();
        void ReleaseDInput();
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