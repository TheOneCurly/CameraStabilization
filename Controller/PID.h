/*============================================================================================


	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
	Creation Date: 14 March 2015
	Project: RIT Senior Design 2014-2015 -- Camera Stabilization

	Description: Funtionality for the PID controller for a 3-axis camera stabilization system.
				 Includes code to adjust the speed and movement direction of 3-phase BLDC motors
				 based on the input from a 6050 MPU.

  ============================================================================================ */


#ifndef _PID_CONTROL_H_
#define _PID_CONTROL_H_


    //Movement Calculation Functions 
//TODO: highly likely that one of these control functions will be eliminated in the end, but may be useful 
//      to have a backup function if the error feedback line breaks down. 
    int* PIDMovementCalc(float*);
    int* PIDMovementCalc_withError(float*);


    //The following functions will affect the PID movement calculations as they will change
    //internal variables to reflect new conditions and user desires. 
    //Update control variables based on battery/weight/other conditions
    static bool AdjustWeight(int weight);
    static bool AdjustPower(int batteryCharge);
    static bool AdjustPerformance(int adjustValue);

    //Update controller based on user set inputs (hold axis/float axis/etc.)
    static bool holdXAxis();
    static bool holdYAxis();
    static bool holdZAxis();

    static bool freeXAxis();
    static bool freeYAxis();
    static bool freeZAxis();

#endif 
