/*============================================================================================


	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
	Creation Date: 27 April 2015
	Project: RIT Senior Design 2014-2015 -- Camera Stabilization

	Description: Holds variables

  ============================================================================================ */


#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "Arduino.h"

        static bool IMUReady = false;

	static bool X_control_en = true;
	static bool Y_control_en = true;
	static bool Z_control_en = true;

	static float xError, yError, zError;
	static float xControl, yControl, zControl;
	static float xDuty, yDuty, zDuty;
#endif 
