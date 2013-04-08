#ifndef _CSound
#define _CSound

#include "CGame.h"
#include "../bc-libraries/fmod/inc/fmod.h"
#include "../bc-libraries/fmod/inc/fmod_errors.h"    

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