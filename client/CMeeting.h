/*
===============================================================================

    Battle City - CMeeting header file
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
#ifndef _CMeeting
#define _CMeeting

#include "CGame.h"

class CGame;

struct Cities {
    int RealCity;
    int Mayor;
    int Index;
    int PlayerCount;
};

    
class CMeeting {

    public:
        CMeeting(CGame *game);
        ~CMeeting();
    
        HWND hWnd;
        HWND EditWnd;
        HWND ChatWnd;
        HWND CityWnd;
        HWND NewsWnd;
        string Chat;
        string News;

        void ShowMeetingDlg();
        void AppendData(string TheData);
        void AppendNews(string TheData);

        void ClearPlayer(int Index);
        void AddPlayer(int Index);
        void RedrawList();
        void ClearList();
        int FreePlayer();
        bool inUse(int Index);

        void AddCity(int City, int isMayor, int playerCount);
        int FreeCity();

        int lstPlayers[MAX_PLAYERS];

        Cities City[30];

        HBRUSH g_hbrBackground;
        HDC hdcStatic;

    private:
        CGame *p;

    protected:

};

#endif