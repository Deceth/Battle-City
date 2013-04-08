/*
===============================================================================

    Battle City - CDialog
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
#include "CDialog.h"

void _cdecl thrDialog(void * pParam);

CDialog::CDialog(CGame *game)
{
	p = game;
	this->StartDialog = 0;
	this->DialogProcessor = 0;
}

CDialog::~CDialog()
{

}

void CDialog::Start()
{
	_beginthread(thrDialog,0,p);
}

void _cdecl thrDialog(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	while (1)
	{
		if (p->Dialog->StartDialog > 0)
		{
			DialogBox(p->hInst, MAKEINTRESOURCE(p->Dialog->StartDialog), p->hWnd, p->Dialog->DialogProcessor);
		}
		Sleep(5);
	}

	_endthread();
}