/*
===============================================================================

    Battle City - CManage header file
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
#ifndef _CManage
#define _CManage

#include "CGame.h"

class CManage;

struct Commando
{
    int IsSuccessor;
    int Index;
    int TheCommando;
};

class CManage
{
public:
    CManage(CGame *game);
    ~CManage();

    HWND CommandoWnd;
    HWND hWnd;

    void ShowManageDlg();
    void AddCommando(int Commando, int Sucessor);
    int FreeCommando();
    void DisplayCommandos();

    Commando Commando[4];
private:
    CGame *p;
protected:

};

#endif