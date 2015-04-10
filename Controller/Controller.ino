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

const float ANGLE_INIT_THRESHOLD = 0.2;

// declare pins
const int PWM_pin_x = 34;
const int brake_x = 6;
const int enable_x = 9;

const int PWM_pin_y = 3;
const int brake_y = 7;
const int enable_y = 10;

const int PWM_pin_z = 5;
const int brake_z = 8;
const int enable_z = 11;

bool isGood = customPWMinit(20000, 100);
customPWM motorPinx(PWM_pin_x);
//customPWM motorPiny(PWM_pin_y);
//customPWM motorPinz(PWM_pin_z);

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "IMUController.h"
IMUController imu(0);
IMUController imu_error(1);

int* duty;
float* angle_values = (float*) malloc(3*sizeof(float));
float* error_angle_values = (float*) malloc(3*sizeof(float));
float* angle_values_init = (float*) malloc(3*sizeof(float));
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
//
void dmp0DataReady() {
    imu.mpuInterrupt = true;
}

void dmp1DataReady() {
    imu_error.mpuInterrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // Set pin modes
    pinMode(brake_x, OUTPUT);
    pinMode(brake_y, OUTPUT);
    pinMode(brake_z, OUTPUT);
    
    pinMode(enable_x, OUTPUT);
    pinMode(enable_y, OUTPUT);
    pinMode(enable_z, OUTPUT);
    
    // Release brakes
    digitalWrite(brake_x, HIGH);
    digitalWrite(brake_y, HIGH);
    digitalWrite(brake_z, HIGH);
    
    // Disable movement
    digitalWrite(enable_x, LOW);
    digitalWrite(enable_y, LOW);
    digitalWrite(enable_z, LOW);
    
    attachInterrupt(0, dmp0DataReady, RISING);
    attachInterrupt(1, dmp1DataReady, RISING);
    
    // Set up IMU connection///////////////////////////////////////////////////clean this shit
    imu.init();
    while(!imu.poll(angle_values));
    //setBaseAngles(angle_values,0);
    //if(imu.poll(angle_values))  
    do{
      angle_values_init[0] = angle_values[0];
      angle_values_init[1] = angle_values[1];
      angle_values_init[2] = angle_values[2];
      imu.poll(angle_values);
      Serial.println("init...");
    }while(abs(angle_values[0] - angle_values_init[0])>ANGLE_INIT_THRESHOLD || abs(angle_values[1] - angle_values_init[1])>ANGLE_INIT_THRESHOLD || abs(angle_values[2] - angle_values_init[2])>ANGLE_INIT_THRESHOLD);
    setBaseAngles(angle_values,0);
    
    imu_error.init();
    while(!imu_error.poll(error_angle_values)){
      imu.poll(angle_values);
    }
    //setBaseAngles(error_angle_values,1);
    do{
      angle_values_init[0] = error_angle_values[0];
      angle_values_init[1] = error_angle_values[1];
      angle_values_init[2] = error_angle_values[2];
      imu_error.poll(error_angle_values);
      imu.poll(angle_values);
      Serial.println("init...");
    }while(abs(error_angle_values[0] - angle_values_init[0])>ANGLE_INIT_THRESHOLD || abs(error_angle_values[1] - angle_values_init[1])>ANGLE_INIT_THRESHOLD || abs(error_angle_values[2] - angle_values_init[2])>ANGLE_INIT_THRESHOLD);
    setBaseAngles(error_angle_values,1);
    //////////////////////////////////////////////////////////////////////
    
    // Enable movement
    digitalWrite(enable_x, HIGH);
    digitalWrite(enable_y, HIGH);
    digitalWrite(enable_z, HIGH);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    bool angles_flag;
    //get angles from poll
    imu_error.poll(error_angle_values);
    angles_flag = imu.poll(angle_values);
        
    Serial.println(" ");
//    Serial.println(angles_flag);
//    Serial.println(angle_values[0]);
//    Serial.println(angle_values[1]);
//    Serial.println(angle_values[2]);
//    pid returns duty cycles
    duty = PIDMovementCalc(angle_values);
    //Serial.println("x axis duty cycle");
    //Serial.println(duty[0]);
//    Serial.println(duty[1]);
//    Serial.println(duty[2]);

    //set duty cycles
    motorPinx.duty(duty[0]);
    //repeat
}


