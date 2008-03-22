#include "CTimer.h"

/***************************************************************
 * Constructor
 *
 **************************************************************/
CTime::CTime() {
	this->dt = 0;
	this->td2 = 0;
	this->td = 0;
	this->frequency = 0;
	this->resolution = 0;
	this->mm_timer_start = 0;
	this->mm_timer_elapsed = 0;
	this->Initialize();
}

/***************************************************************
 * Function:	Update
 *
 **************************************************************/
void CTime::Update()  {
	this->td2 = this->GetTime();
	this->dt = (this->td2-this->td) * 0.1f; 
	this->td = this->td2;
}

/***************************************************************
 * Function:	Initialize
 *
 **************************************************************/
void CTime::Initialize() {
#ifdef WIN32
	// Use timeGetTime() To Get Current Time
	this->mm_timer_start	= timeGetTime();			

	// Set Our Timer Resolution To .001f
	resolution		= 1.0f/1000.0f;
	
	// Set Our Timer Frequency To 1000
	frequency			= 1000;					

	// Set The Elapsed Time To The Current Time
	mm_timer_elapsed	= mm_timer_start;		
#endif
}

/***************************************************************
 * Function:	Get_dt
 *
 **************************************************************/
float CTime::Get_dt() const  { 
	return this->dt; 
}

/***************************************************************
 * Function:	GetTime
 *
 **************************************************************/
float CTime::GetTime() const {
#ifdef WIN32
	// Get Time In Milliseconds
	return( (float) ( timeGetTime() - this->mm_timer_start) * this->resolution)*1000.0f;
#endif
}
