/*
===============================================================================

    Battle City - COptions
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
#include "COptions.h"

void *COptionsPointer;

int CALLBACK OptionsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	int i = 0;
	CGame *p = (CGame *)COptionsPointer;
    switch(Message) {
        case WM_INITDIALOG:
			p->Options->hWnd = hwnd;
			CheckDlgButton(hwnd, 1100, p->Options->sound);
			CheckDlgButton(hwnd, 1101, p->Options->tanksound);
			CheckDlgButton(hwnd, 1102, p->Options->music);
			CheckDlgButton(hwnd, 1103, p->Options->fullscreen);
			CheckDlgButton(hwnd, 1104, p->Options->newbietips);
			CheckDlgButton(hwnd, 1105, p->Options->sleep);
			CheckDlgButton(hwnd, 1106, p->Options->debug);
			CheckDlgButton(hwnd, 1107, p->Options->names);
			CheckDlgButton(hwnd, 1108, p->Options->limitfps);
			CheckDlgButton(hwnd, 1109, p->Options->resolution1024);
        return 1;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case 1:
					//Save Options
					p->Options->sound = IsDlgButtonChecked(hwnd, 1100);
					p->Options->tanksound = IsDlgButtonChecked(hwnd, 1101);
					p->Options->music = IsDlgButtonChecked(hwnd, 1102);
					p->Options->fullscreen = IsDlgButtonChecked(hwnd, 1103);
					p->Options->newbietips = IsDlgButtonChecked(hwnd, 1104);
					p->Options->sleep = IsDlgButtonChecked(hwnd, 1105);
					p->Options->debug = IsDlgButtonChecked(hwnd, 1106);
					p->Options->names = IsDlgButtonChecked(hwnd, 1107);
					p->Options->limitfps = IsDlgButtonChecked(hwnd, 1108);
					p->Options->resolution1024 = IsDlgButtonChecked(hwnd, 1109);
					if (p->Options->music == 0) {
						p->Sound->StopMID(1, 0);
					}
					p->Options->SaveOptions();
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 1);
                break;
				case 2:
					p->Dialog->StartDialog = 0;
					EndDialog(hwnd, 2);
				break;
            }
        break;
        default:
            return 0;
    }
    return 1;
}

COptions::COptions(CGame *game)
{
	p = game;
	COptionsPointer = game;
}

COptions::~COptions()
{

}

void COptions::ShowOptionsDialog()
{
	p->Dialog->StartDialog = ID_OPTIONS;
	p->Dialog->DialogProcessor = &OptionsDlgProc;

}

void COptions::LoadOptions()
{
	char buffer[1024];

	GetCurrentDirectory(1024,buffer);
	strcat(buffer, "\\options.ini");

	int flag = 0;
	fstream fin;
	fin.open(buffer,ios::in);
	if( fin.is_open() )
	{
		flag = 1;
	}
	fin.close();

	if (flag == 1)
	{
		this->music = GetPrivateProfileInt("Options", "Music", 1, buffer);
		this->sound = GetPrivateProfileInt("Options", "Sound", 1, buffer);
		this->tanksound = GetPrivateProfileInt("Options", "TankSound", 1, buffer);
		this->fullscreen = GetPrivateProfileInt("Options", "Fullscreen", 1, buffer);
		this->newbietips = GetPrivateProfileInt("Options", "NewbieTips", 1, buffer);
		this->sleep = GetPrivateProfileInt("Options", "Sleep", 1, buffer);
		this->debug = GetPrivateProfileInt("Options", "Debug", 0, buffer);
		this->names = GetPrivateProfileInt("Options", "Names", 1, buffer);
		this->limitfps = GetPrivateProfileInt("Options", "LimitFPS", 1, buffer);
		this->resolution1024 = GetPrivateProfileInt("Options", "Resolution1024", 1, buffer);
	}
	else
	{
		this->music = 1;
		this->sound = 1;
		this->tanksound = 1;
		this->fullscreen = 1;
		this->newbietips = 1;
		this->sleep = 1;
		this->debug = 0;
		this->names = 1;
		this->limitfps = 1;
		this->resolution1024 = 1;
		SaveOptions();
	}
}

void COptions::SaveOptions()
{
	char buffer[1024];

	GetCurrentDirectory(1024,buffer);
	strcat(buffer, "\\options.ini");

	char sdf[2];
	memset(sdf, 0, 2);
	itoa(this->music, sdf, 10);
	WritePrivateProfileString("Options", "Music", sdf, buffer);

	itoa(this->sound, sdf, 10);
	WritePrivateProfileString("Options", "Sound", sdf, buffer);

	itoa(this->tanksound, sdf, 10);
	WritePrivateProfileString("Options", "TankSound", sdf, buffer);

	itoa(this->fullscreen, sdf, 10);
	WritePrivateProfileString("Options", "Fullscreen", sdf, buffer);

	itoa(this->newbietips, sdf, 10);
	WritePrivateProfileString("Options", "NewbieTips", sdf, buffer);

	itoa(this->sleep, sdf, 10);
	WritePrivateProfileString("Options", "Sleep", sdf, buffer);

	itoa(this->debug, sdf, 10);
	WritePrivateProfileString("Options", "Debug", sdf, buffer);

	itoa(this->names, sdf, 10);
	WritePrivateProfileString("Options", "Names", sdf, buffer);

	itoa(this->limitfps, sdf, 10);
	WritePrivateProfileString("Options", "LimitFPS", sdf, buffer);

	itoa(this->resolution1024, sdf, 10);
	WritePrivateProfileString("Options", "Resolution1024", sdf, buffer);
}