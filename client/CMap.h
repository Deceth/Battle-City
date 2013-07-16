/*
===============================================================================

    Battle City - CMap header file
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
//  Upon verifying __MAP constant is not defined, define __MAP and class
#ifndef __MAP
#define __MAP
//  Includes CGame header file
#include "CGame.h"
//  Initialize CGame class
class CGame;
//  Define CMap class
class CMap
{
//  Public methods, pointers, and variables
public:
    /// <summary>   Constructor. </summary>
    ///
    /// <param name="Game"> [in,out] If non-null, the game. </param>
    CMap(CGame *Game);
    /// <summary>   Destructor. </summary>
    ~CMap();
    /// <summary>   Calculates the tiles. </summary>
    void CalculateTiles();
    /// <summary>   Loads the map. </summary>
    void LoadMap();
    /// <summary>   The map[ 512][512]. </summary>
    unsigned char map[512][512];
    /// <summary>   The tiles[ 512][512]. </summary>
    short tiles[512][512];
private:
    /// <summary>   The CGame* to process. </summary>
    CGame *p;
protected:
};

#endif