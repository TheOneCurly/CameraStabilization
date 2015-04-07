/*
 *
 *	Controller.ino
 *
 *	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *	Creation Date: 30 March 2015
 *	Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *	Description: Main loop for the camera stabilization system
 *
 */
 /***************************** Preprocessor Flags ****************************/
 
#include "PID.h"
#include "customPWM.h"
// declare PWM pins
const int PWM_pin_x = 34;
const int PWM_pin_y = 3;
const int PWM_pin_z = 5;
const float ANGLE_INIT_THRESHOLD = 0.2;

bool isGood = customPWMinit(20000, 100);
customPWM motorPin(PWM_pin_x);
//y pin
//z pin

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "IMUController.h"
IMUController imu;

int* duty;
float* angle_values = (float*) malloc(3*sizeof(float));
float* angle_values_init = (float*) malloc(3*sizeof(float));
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
//


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // Set up IMU connection
    imu.init();
    
    // Grab initial values
    if(imu.poll(angle_values))       
        do{
          angle_values_init[0] = angle_values[0];
          angle_values_init[1] = angle_values[2];
          angle_values_init[2] = angle_values[2];
          imu.poll(angle_values);
          Serial.println("init...");
        }while(abs(angle_values[0] - angle_values_init[0])>ANGLE_INIT_THRESHOLD || abs(angle_values[1] - angle_values_init[1])>ANGLE_INIT_THRESHOLD || abs(angle_values[2] - angle_values_init[2])>ANGLE_INIT_THRESHOLD);
        
        setBaseAngles(angle_values);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    bool angles_flag;
    //get angles from poll
    angles_flag = imu.poll(angle_values);
//    Serial.println(angles_flag);
//    Serial.println(angle_values[0]);
//    Serial.println(angle_values[1]);
//    Serial.println(angle_values[2]);
    //pid returns duty cycles
    duty = PIDMovementCalc(angle_values);
    Serial.print("x axis duty cycle: ");
    Serial.println(duty[0]);
//    Serial.println(duty[1]);
//    Serial.println(duty[2]);
    //set duty cycles
    motorPin.duty(duty[0]);
    //repeat
}


