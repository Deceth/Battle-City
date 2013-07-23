/*
===============================================================================

    Battle City - CSound
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
#include "CSound.h"
/// <summary>   Thread to handle sound </summary>
///
/// <param name="pParam">   [in,out] If non-null, the parameter. </param>
void _cdecl thrSound(void * pParam);
/// <summary>   Constructor. </summary>
///
/// <param name="game"> [in,out] If non-null, the game. </param>
CSound::CSound(CGame *game)
{
    //  Reference CGame
	p = game;
    //  Set MIDIndex to zero
	MIDIndex = 0;
}
/// <summary>   Destructor. </summary>
CSound::~CSound()
{
    //  Safely close FSound
	FSOUND_Close();
}
/// <summary>   Initializes this object. </summary>
void CSound::Init()
{
    //  Upon verifying FSOUND is not initialized using 44100 mix rate, 32 channels, and zero flags; do nothing
    //  Otherwise, proceed with loading sounds and music into memory
	if (!FSOUND_Init(44100,32,0))
	{

	}
	else
	{
        //  Upon verifying that sounds did not load report an error message and indicate sound has not loaded
		if (!LoadSounds())
		{
            //  Display message box informing user that sounds did not load
			MessageBox(p->hWnd, "Failed to load sounds!", 0, 0); 
            //  Set sound indicator to zero
			p->Options->sound = 0;
            //  Save options
			p->Options->SaveOptions();
		}
        //  Upon verifying that music did not load report an error message and indicate music has not loaded
		if (!LoadMusic())
		{
            //  Display message box informing user that music did not load
			MessageBox(p->hWnd, "Failed to load music!", 0, 0); 
            //  Set music indicator to zero
			p->Options->music = 0;
            //  Save options
			p->Options->SaveOptions();
		}
	}
    //  Set random starting point based upon current game time interval
    //  http://msdn.microsoft.com/en-us/library/f0d4wb4t(v=vs.71).aspx
	srand((unsigned int)p->Tick);
    //  Set the index to be zero plus the remainder of rand() divided by six
	this->MIDIndex = 0 + (rand()%6);
    //  Play the music associated to the index
	PlaYMID(MIDIndex);
    //  Start the Sound thread
	_beginthread(thrSound,0,p);
}

/// <summary>   Loads the sounds. </summary>
///
/// <returns>   The sounds. </returns>
int CSound::LoadSounds()
{
	s_engine = FSOUND_Sample_Load(FSOUND_FREE, "wav/engine.wav", FSOUND_LOOP_NORMAL,0,0);
	s_tanklaser = FSOUND_Sample_Load(FSOUND_FREE, "wav/laser.wav", FSOUND_NORMAL,0,0);
	s_fire = FSOUND_Sample_Load(FSOUND_FREE, "wav/fire.wav", FSOUND_NORMAL,0,0);
	s_build = FSOUND_Sample_Load(FSOUND_FREE, "wav/build.wav", FSOUND_NORMAL,0,0);
	s_die = FSOUND_Sample_Load(FSOUND_FREE, "wav/die.wav", FSOUND_NORMAL,0,0);
	s_eXplode = FSOUND_Sample_Load(FSOUND_FREE, "wav/eXplode.wav", FSOUND_NORMAL,0,0);
	s_turret = FSOUND_Sample_Load(FSOUND_FREE, "wav/turretfire.wav", FSOUND_NORMAL,0,0);
	s_buzz = FSOUND_Sample_Load(FSOUND_FREE, "wav/buzz.wav", FSOUND_NORMAL,0,0);
	s_click = FSOUND_Sample_Load(FSOUND_FREE, "wav/click.wav", FSOUND_NORMAL,0,0);
	s_bigturret = FSOUND_Sample_Load(FSOUND_FREE, "wav/bturr.wav", FSOUND_NORMAL,0,0);
	s_demolish = FSOUND_Sample_Load(FSOUND_FREE, "wav/demo.wav", FSOUND_NORMAL,0,0);
	s_screech = FSOUND_Sample_Load(FSOUND_FREE, "wav/screech.wav", FSOUND_NORMAL,0,0);
	s_hit = FSOUND_Sample_Load(FSOUND_FREE, "wav/hit.wav", FSOUND_NORMAL,0,0);
	s_cloak = FSOUND_Sample_Load(FSOUND_FREE, "cloak.wav", FSOUND_NORMAL,0,0);
	s_flare = FSOUND_Sample_Load(FSOUND_FREE, "flare.wav", FSOUND_NORMAL,0,0);
	if (!s_tanklaser || !s_fire || !s_engine || !s_build || !s_die || !s_eXplode || !s_turret || !s_buzz || !s_click || !s_bigturret || !s_demolish || !s_screech || !s_hit || !s_cloak || !s_flare)
		return 0;  
	return 1;
}

/// <summary>   Loads the music. </summary>
///
/// <returns>   The music. </returns>
int CSound::LoadMusic()
{
	m_BC1 = FMUSIC_LoadSong("midi/BC1.mid");
	m_BC2 = FMUSIC_LoadSong("midi/BC2.mid");
	m_BC3 = FMUSIC_LoadSong("midi/BC3.mid");
	m_BC5 = FMUSIC_LoadSong("midi/BC5.mid");
	m_BC6 = FMUSIC_LoadSong("midi/BC6.mid");
	m_BC8 = FMUSIC_LoadSong("midi/BC8.mid");
	m_BC9 = FMUSIC_LoadSong("midi/BC9.mid");
	if (!m_BC1 || !m_BC2 || !m_BC3 || !m_BC5 || !m_BC6 || !m_BC8 || !m_BC9)
		return 0;
	return 1;
}

/// <summary>   Play WAV. </summary>
///
/// <param name="Index">    Zero-based index of the. </param>
/// <param name="channel">  The channel. </param>
void CSound::PlayWav(int Index, int channel)
{
	try
	{
		if (p->Options->sound == 1)
		{
			//Channel -1 is FSOUND_FREE (will play on first available channel)
			FSOUND_SetSFXMasterVolume(100);
				switch(Index)
				{
				case 0:
					FSOUND_PlaySound(channel, s_tanklaser);
					break;
				case 1:
					FSOUND_PlaySound(channel, s_fire);
					break;
				case 2:
					FSOUND_PlaySound(channel, s_engine);
					break;
				case 3:
					FSOUND_PlaySound(channel, s_build);
					break;
				case 4:
					FSOUND_PlaySound(channel, s_die);
					break;
				case 5:
					FSOUND_PlaySound(channel, s_eXplode);
					break;
				case 6:
					FSOUND_PlaySound(channel, s_turret);
					break;
				case 7:
					FSOUND_PlaySound(channel, s_buzz);
					break;
				case 8:
					FSOUND_PlaySound(channel, s_click);
					break;
				case 9:
					FSOUND_PlaySound(channel, s_bigturret);
					break;
				case 10:
					FSOUND_PlaySound(channel, s_demolish);
					break;
				case 11:
					FSOUND_PlaySound(channel, s_screech);
					break;
				case 12:
					FSOUND_PlaySound(channel, s_hit);
					break;
				case 13:
					FSOUND_PlaySound(channel, s_cloak);
					break;
				case 14:
					FSOUND_PlaySound(channel, s_flare);
					break;
				//default:
				}    
		}
	}

	catch (...) {p->Options->sound = 0; p->Options->SaveOptions(); p->Engine->logerror("Crashed playing sound!");}
}

/// <summary>   Plays music </summary>
///
/// <param name="Index">    Music index </param>
void CSound::PlaYMID(int Index)
{
    //  Upon verifying the user has music enabled start playing song
	if (p->Options->music == 1)
	{
        //  Select the music based upon index
		switch(Index)
		{
		case 0:
            //  Sets a songs master volume
			FMUSIC_SetMasterVolume(m_BC1, 75);
            //  Starts a song playing
			FMUSIC_PlaySong(m_BC1);
			break;
		case 1:
			FMUSIC_SetMasterVolume(m_BC2, 75);
			FMUSIC_PlaySong(m_BC2);
			break;
		case 2:
			FMUSIC_SetMasterVolume(m_BC3, 75);
			FMUSIC_PlaySong(m_BC3);
			break;
		case 3:
			FMUSIC_SetMasterVolume(m_BC5, 75);
			FMUSIC_PlaySong(m_BC5);
			break;
		case 4:
			FMUSIC_SetMasterVolume(m_BC6, 75);
			FMUSIC_PlaySong(m_BC6);
			break;
		case 5:
			FMUSIC_SetMasterVolume(m_BC8, 75);
			FMUSIC_PlaySong(m_BC8);
			break;
		case 6:
			FMUSIC_SetMasterVolume(m_BC9, 75);
			FMUSIC_PlaySong(m_BC9);
			break;
		}
	}
}

void CSound::StopWav(int Index)
{
	//-3 is FSOUND_ALL, stops all sounds
		FSOUND_StopSound(Index);  
}

void CSound::StopMID(int all, int Index)
{
	if (all)
	{	
		FMUSIC_StopAllSongs();
	}
	else
	{
		switch(Index)
		{
		case 0:
			FMUSIC_StopSong(m_BC1);
			break;
		case 1:
			FMUSIC_StopSong(m_BC2);
			break;
		case 2:
			FMUSIC_StopSong(m_BC3);
			break;
		case 3:
			FMUSIC_StopSong(m_BC5);
			break;
		case 4:
			FMUSIC_StopSong(m_BC6);
			break;
		case 5:
			FMUSIC_StopSong(m_BC8);
			break;
		case 6:
			FMUSIC_StopSong(m_BC9);
			break;
		}
	}   

}

int CSound::musicCycle()
{
	signed char isDone = 0;
	switch(MIDIndex)
		{
		case 0:
			isDone = FMUSIC_IsFinished(m_BC1);
			break;
		case 1:
			isDone = FMUSIC_IsFinished(m_BC2);
			break;
		case 2:
			isDone = FMUSIC_IsFinished(m_BC3);
			break;
		case 3:
			isDone = FMUSIC_IsFinished(m_BC5);
			break;
		case 4:
			isDone = FMUSIC_IsFinished(m_BC6);
			break;
		case 5:
			isDone = FMUSIC_IsFinished(m_BC8);
			break;
		case 6:
			isDone = FMUSIC_IsFinished(m_BC9);
			break;  
		case -1:
			isDone = 0;
			break;
		}
		if (isDone)
		{
			MIDIndex++;
			if (MIDIndex > 6)
				MIDIndex = 0;
			PlaYMID(MIDIndex);
		}
		return 0;
}

void _cdecl thrSound(void * pParam)
{
	CGame * p;
	p = (CGame *)pParam;

	do
	{
		p->Sound->musicCycle();
		Sleep(1000);
	} while (p->running);

	_endthread();
}

void CSound::Play3dSound(FSOUND_SAMPLE *sound, int volume, float x, float y)
{
	if (p->Options->sound == 1)
	{
		float xPos = (p->Player[p->Winsock->MyIndex]->X - x) / 2400.0f;
		float yPos = (p->Player[p->Winsock->MyIndex]->Y - y) / 2400.0f;
		if (xPos >= -1.0f && xPos <= 1.0f && yPos >= -1.0f && yPos <= 1.0f) //Within hearing range?
		{
			float soundPos[3] = {xPos, yPos, 0.0f};
			int returnedChannel = FSOUND_PlaySound(FSOUND_FREE, sound);
			FSOUND_SetVolume(returnedChannel, volume);
			FSOUND_3D_SetAttributes(returnedChannel, soundPos, NULL);
			FSOUND_3D_SetMinMaxDistance(returnedChannel, 0.15f, 1.0f);
			FSOUND_Update();
		}
	}
}