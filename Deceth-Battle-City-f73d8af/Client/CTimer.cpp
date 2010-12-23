#include "CTimer.h"

void CTime::Update() 
{

}

CTime::CTime()
{
	Initialize();
}

void CTime::Initialize()
{
	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &frequency))
	{
		// No Performace Counter Available
		performance_timer	= 0;				// Set Performance Timer To FALSE
		mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		resolution	= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		frequency		= 1000;					// Set Our Timer Frequency To 1000
		mm_timer_elapsed	= mm_timer_start;			// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &performance_timer_start);
		performance_timer		= 1;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		resolution		= (float) (((double)1.0f)/((double)frequency));
		// Set The Elapsed Time To The Current Time
		performance_timer_elapsed	= performance_timer_start;
	}
}

float CTime::GetTime() const						// Get Time In Milliseconds
{
	__int64 time;								// time Will Hold A 64 Bit Integer

	if (performance_timer)						// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);		// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (float) ( time - performance_timer_start) * resolution)*1000.0f;
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (float) ( timeGetTime() - mm_timer_start) * resolution)*1000.0f;
	}
}