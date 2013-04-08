/*
===============================================================================

    Battle City - CAdminEdit header file
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
#ifndef _CAdminEdit
#define _CAdminEdit

#include "CGame.h"
#include "NetMessages.h"

class CGame;

class CAdminEdit
{
public:
    CAdminEdit(CGame *game);
    ~CAdminEdit();

    HWND hWnd;

    char IsOpen; 

    void ShowAdminEditDlg();

    char user[15];
    char pass[15];
    char email[50];
    char fullname[20];
    char town[15];
    char state[15];

    int points;
    int monthlyTop20;
    int orbs;
    int deaths;
    int assists;
    int playerType;
    char member;
    char tank;
    char tank2;
    char tank3;
    char tank4;
    char tank5;
    char tank6;
    char tank7;
    char tank8;
    char tank9;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    int rentalcity;

private:
    CGame *p;
protected:

};

#endif