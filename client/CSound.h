/*
===============================================================================

    Battle City - CSound header file
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
#ifndef _CSound
#define _CSound

#include "CGame.h"
#include "../external/fmod-3.75/api/inc/fmod.h"
#include "../external/fmod-3.75/api/inc/fmod_errors.h"    

class CGame;

class CSound
{
public:
	CSound(CGame *game);
	~CSound();

	FSOUND_SAMPLE *s_tanklaser, *s_fire, *s_engine, *s_build, *s_die, *s_eXplode, *s_turret, *s_buzz, *s_click, *s_bigturret, *s_demolish, *s_screech, *s_hit, *s_cloak, *s_flare;	
	FMUSIC_MODULE *m_BC1;
	FMUSIC_MODULE *m_BC2;
	FMUSIC_MODULE *m_BC3;
	FMUSIC_MODULE *m_BC5;
	FMUSIC_MODULE *m_BC6;
	FMUSIC_MODULE *m_BC8;
	FMUSIC_MODULE *m_BC9;

	void Init();
	void PlayWav(int Index, int channel); //0-12 valid (13)
	void PlaYMID(int Index);
	void StopWav(int Index);
	void StopMID(int all, int Index);

	void Play3dSound(FSOUND_SAMPLE *sound, int volume, float x, float y);
	
	void Shutdown();

	int MIDIndex;
	int musicCycle();
private:
	CGame *p;
	int LoadSounds();
	int LoadMusic();
protected:

};

#endif