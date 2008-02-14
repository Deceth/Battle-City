#include "CTimer.h"

void CTime::Update() 
{
	td2 = GetTime();
	dt = (td2-td) * 0.1f; 
	td = td2;
}

CTime::CTime()
{
	dt = 0;
	td2 = 0;
	td = 0;
	frequency = 0;
	resolution = 0;
	mm_timer_start = 0;
	mm_timer_elapsed = 0;
	Initialize();
}

void CTime::Initialize()
{
#ifdef WIN32
	mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
	resolution		= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
	frequency			= 1000;						// Set Our Timer Frequency To 1000
	mm_timer_elapsed	= mm_timer_start;		// Set The Elapsed Time To The Current Time'
#endif
}

float CTime::Get_dt() const 
{ 
	return dt; 
}

float CTime::GetTime() const						// Get Time In Milliseconds
{
#ifdef WIN32
	return( (float) ( timeGetTime() - mm_timer_start) * resolution)*1000.0f;
#endif
}
