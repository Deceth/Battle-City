#include "CSound.h"
void _cdecl thrSound(void * pParam);

CSound::CSound(CGame *game)
{
	p = game;
	MIDIndex = 0;
}

CSound::~CSound()
{
	FSOUND_Close();
}

void CSound::Init()
{
	if (!FSOUND_Init(44100,32,0))
	{

	}
	else
	{
		if (!LoadSounds())
		{
			MessageBox(p->hWnd, "Failed to load sounds!", 0, 0); 
			p->Options->sound = 0;
			p->Options->SaveOptions();
		}
		if (!LoadMusic())
		{
			MessageBox(p->hWnd, "Failed to load music!", 0, 0); 
			p->Options->music = 0;
			p->Options->SaveOptions();
		}
	}
	srand((unsigned int)p->Tick);
	this->MIDIndex = 0 + (rand()%6);
	PlaYMID(MIDIndex);
	_beginthread(thrSound,0,p);
}

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

void CSound::PlaYMID(int Index)
{
	if (p->Options->music == 1)
	{
		switch(Index)
		{
		case 0:
			FMUSIC_SetMasterVolume(m_BC1, 75);
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