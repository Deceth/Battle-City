/*
===============================================================================

    Battle City - COptions header file
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
#ifndef _COptions
#define _COptions

#include "CGame.h"
#include <vector>

class CGame;

class COptions
{
public:
    COptions(CGame *game);
    ~COptions();
    HWND hWnd;
    void ShowOptionsDialog();
    char music;
    char sound;
    char tanksound;
    char fullscreen;
    char newbietips;
    char sleep;
    char debug;
    char names;
    char limitfps;
    char resolution1024;
    string gameServerAddress;
    std::vector<string> pastGameServers;
    void addGameServerToHistory(char *gameServerAddress);
    void LoadOptions();
    void SaveOptions();
private:
    CGame *p;
protected:

};

#endif