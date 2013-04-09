/*
===============================================================================

    Battle City - CNew header file
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
#ifndef _CNew
#define _CNew

#include "CGame.h"

class CGame;

class CNew
{
public:
    CNew(CGame *game);
    ~CNew();

    HWND hWnd;
    
    char user[15];
    char pass[15];
    char email[50];
    char fullname[20];
    char town[15];
    char state[15];

    char Editing;

    void ShowNewDlg();
    void ShowEditDlg();
    void Refill();
private:
    CGame *p;
protected:

};

#endif