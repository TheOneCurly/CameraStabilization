/*
 *
 *	PID.c
 *
 *	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *	Creation Date: 14 March 2015
 *	Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *	Description: Funtionality for the PID controller for a 3-axis camera stabilization system.
 *				 Includes code to adjust the speed and movement direction of 3-phase BLDC motors
 *				 based on the input from a 6050 MPU.
 *
 */

/***************************** Include Files *********************************/

#include "PID.h"

/************************** Constant Definitions *****************************/
/**************************** Type Definitions *******************************/
/***************** Macros (Inline Functions) Definitions *********************/
/************************** Function Prototypes ******************************/
/************************** Variable Definitions *****************************/
//For each of the following enables:
//	True means the axis is stabilized
//	False means the axis is free floating. 
static bool X_control_en = true;
static bool Y_control_en = true;
static bool Z_control_en = true;


/**
 *
 * Calculates the movement of each of the 3 three-phase BLDC motors as they 
 * stabilize the platform. Movement is based on a single 6050MPU IMU that 
 * senses the user's movement. This process does not use any error correction. 
 *
 * @param	
 *
 * @return	Array of ints that correlate to the PWM signal needed for each of the
 *			three motors. 
 *
 *
 ******************************************************************************/
int* PIDMovementCalc(){

}


/*******************************************************************************
 *
 * Calculates the movement of each of the 3 three-phase BLDC motors as they 
 * stabilize the platform. Movement is based on a single 6050MPU IMU that 
 * senses the user's movement. This process does use error correction.
 *
 * @param	
 *
 * @return	Array of ints that correlate to the PWM signal needed for each of the
 *			three motors. 
 *
 *
 ******************************************************************************/
int* PIDMovementCalc_withError(){

}




/****************************PID ADJUSTMENT FUNCS*****************************/



/*******************************************************************************
 *
 * Adjusts the amount of movement required on the output of the PID based on the
 * weight of the camera being stabilized. Less force is required for lighter cameras,
 * and more force is required for heavier cameras
 *
 * @param	weight - the current weight of the item held by the system in ounces
 *
 * @return	Whether or not the weight adjustement was successful.
 *
 ******************************************************************************/
static bool AdjustWeight(int weight){

}


/*******************************************************************************
 *
 * Adjusts the amount of movement required on the output of the PID based on the
 * remaining charge of the battery. Lower charege requires additional force be used
 * to maintain desired movement. 
 *
 * @param	batteryCharge - amount of charge remaining
 *
 * @return	Whether or not the power adjustement was successful.
 *
 *
 ******************************************************************************/
static bool AdjustPower(int batteryCharge){

}


/*******************************************************************************
 *
 * Adjusts the amount of movement based on the performance desired by the user,
 * which will allow the user to introduce their own level of 'shake' into the system.
 *
 * @param	adjustValue - 0-100 - percentage of stabilization to perform.
 *
 * @return	Whether or not the performance adjustement was successful.
 *
 *
 ******************************************************************************/
static bool AdjustPerformance(int adjustValue){

}


/******************************************************************************
 *
 * stabilize the X Axis of the Gimbal
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool holdXAxis(){

	X_control_en = true;

	return X_control_en;

}


/******************************************************************************
 *
 * stabilize the Y Axis of the Gimbal
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool holdYAxis(){

	Y_control_en = true;

	return Y_control_en;
}


/******************************************************************************
/**
 *
 * stabilize the Z Axis of the Gimbal. 
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool holdZAxis(){

	Z_control_en = true;

	return Z_control_en;

}


/******************************************************************************

 *
 * Do not stabilize the X Axis, allow it to freely move with the user's movement.
 *
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool freeXAxis(){

	X_control_en = false;

	return X_control_en;

}


/******************************************************************************
 *
 * Do not stabilize the Y Axis, allow it to freely move with the user's movement.
 *
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool freeYAxis(){

	Y_control_en = false;

	return Y_control_en;

}

/******************************************************************************
 *
 * Do not stabilize the Z Axis, allow it to freely move with the user's movement.
 *
 *
 * @return	current state of holding - true is stablized, false is free movement.
 *
 *
 ******************************************************************************/
static bool freeZAxis(){

	Z_control_en = false;

	return Z_control_en;
}
