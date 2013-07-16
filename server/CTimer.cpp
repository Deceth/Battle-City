/*
===============================================================================

    Battle City - CTimer file
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
#include "CTimer.h"


/// <summary>
/// Initializes a new instance of the <see cref="CTime" /> class.
/// </summary>
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

/// <summary>
/// Updates the Timer
/// </summary>
void CTime::Update()  {
	this->td2 = this->GetTime();
	this->dt = (this->td2-this->td) * 0.1f; 
	this->td = this->td2;
}

/// <summary>
/// Initializes this instance.
/// </summary>
void CTime::Initialize() {
#ifdef WIN32
	// Use timeGetTime() To Get Current Time
	this->mm_timer_start = timeGetTime();			

	// Set Our Timer Resolution To .001f
	resolution = 1.0f/1000.0f;
	
	// Set Our Timer Frequency To 1000
	frequency = 1000;					

	// Set The Elapsed Time To The Current Time
	mm_timer_elapsed	= mm_timer_start;		
#endif
}

/// <summary>
/// Get_dts this instance.
/// </summary>
/// <returns></returns>
float CTime::Get_dt() const  { 
	return this->dt; 
}


float CTime::GetTime() const {
#ifdef WIN32
	// Get Time In Milliseconds
	return( (float) ( timeGetTime() - this->mm_timer_start) * this->resolution)*1000.0f;
#endif
}
