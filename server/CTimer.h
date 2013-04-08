#ifndef _CTimer
#define _CTimer

#include "CServer.h"

class CTime {

	private:
		float dt, td, td2;
		long long       frequency;									// Timer Frequency
		float         resolution;									// Timer Resolution
		unsigned long mm_timer_start;								// Multimedia Timer Start Value
		unsigned long mm_timer_elapsed;								// Multimedia Timer Elapsed Time

	public:
		CTime();
		void Update();
		float Get_dt() const;
		float GetTime() const;
		void Initialize();
};

#endif
