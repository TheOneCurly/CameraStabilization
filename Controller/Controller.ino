#include <Wire.h>

#include <U8glib.h>
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
#include "Variables.h"

const float ANGLE_INIT_THRESHOLD = 0.01;

// declare pins
const int PWM_pin_x = 7;
const int brake_x = 3;
const int enable_x = 2;


const int PWM_pin_y = 8;
const int brake_y = 5;
const int enable_y = 4;

const int PWM_pin_z = 9;
const int brake_z = 10;
const int enable_z = 6;

const int IMU0Interrupt = 48;
const int IMU1Interrupt = 50;

bool base_imu_flag;
bool error_imu_flag;
    
bool PWMisGood = customPWMinit(20000, 100);
customPWM motorPinx(PWM_pin_x);
customPWM motorPiny(PWM_pin_y);
customPWM motorPinz(PWM_pin_z);

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
//#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
//    #include "Wire.h"
//#endif
#include "LCD_Controller.h"
#include "IMUController.h"

IMUController imu(0);
IMUController imu_error(1);



int* duty;
float* angle_values = (float*) calloc(3,sizeof(float));
float* error_angle_values = (float*) calloc(3,sizeof(float));
float* angle_values_init = (float*) calloc(3,sizeof(float));
float* error_angle_values_init = (float*) calloc(3,sizeof(float));
float* base_angles_updated = (float*) calloc(3,sizeof(float));
float* base_error_angles_updated = (float*) calloc(3,sizeof(float));

float yaw_start, yaw_error_start, yaw_end, yaw_error_end, yaw_drift, yaw_error_drift = 0;
float pitch_start, pitch_error_start, pitch_end, pitch_error_end, pitch_drift, pitch_error_drift = 0;
float roll_start, roll_error_start, roll_end, roll_error_end, roll_drift, roll_error_drift = 0;

unsigned long start_time, end_time = 0;
unsigned long start_test, end_test = 0;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    
    Serial.begin(9600);
    
    initialize_LCD();
    Wire.begin();
    
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
    
    bool imu_ready = false;
    bool imu_error_ready = false;
    bool imu_valid = false;
    bool imu_error_valid = false;
     
    imu.init();
    imu_error.init();
    
    start_time = millis();
    while( ( millis() - start_time ) < 45000 ) {
        imu.poll(angle_values);
        imu_error.poll(error_angle_values);
    }
    
    while(!imu_ready || !imu_error_ready){
        delay(15); // needs small delay for init DO NOT REMOVE >:( 
        imu_valid = false;
        imu_error_valid = false;
        angle_values_init[0] = angle_values[0];
        angle_values_init[1] = angle_values[1];
        angle_values_init[2] = angle_values[2];
         
        error_angle_values_init[0] = error_angle_values[0];
        error_angle_values_init[1] = error_angle_values[1];
        error_angle_values_init[2] = error_angle_values[2];
          
        imu_valid = imu.poll(angle_values);
        imu_error_valid = imu_error.poll(error_angle_values);
          
        if(imu_valid && !imu_ready){
            if((/*abs(angle_values[0] - angle_values_init[0])<ANGLE_INIT_THRESHOLD &&*/ abs(angle_values[1] - angle_values_init[1])<ANGLE_INIT_THRESHOLD && abs(angle_values[2] - angle_values_init[2])<ANGLE_INIT_THRESHOLD)){
                imu_ready = true;
            }
        }
          
        if(imu_error_valid && !imu_error_ready){
            if((/*abs(error_angle_values[0] - error_angle_values_init[0])<ANGLE_INIT_THRESHOLD &&*/ abs(error_angle_values[1] - error_angle_values_init[1])<ANGLE_INIT_THRESHOLD && abs(error_angle_values[2] - error_angle_values_init[2])<ANGLE_INIT_THRESHOLD)){
                imu_error_ready = true;
            }
        }
    }
    
    // Poll to get YPR values for calculating drift
    imu.poll(angle_values);
    imu_error.poll(error_angle_values);
    
    // Set the YPR start values
    yaw_start = angle_values[0];
    pitch_start = angle_values[1];
    roll_start = angle_values[2];
    yaw_error_start =  error_angle_values[0];
    pitch_error_start = error_angle_values[1];
    roll_error_start = error_angle_values[2];
    
    Serial.println("Drift Calculations...");
    
    delay(15000);
    
    // Poll to get YPR values after 5 seconds
    imu.poll(angle_values);
    imu_error.poll(error_angle_values);
    
    // Set Base Angles
    base_angles_updated[0] = angle_values[0];
    base_angles_updated[1] = angle_values[1];
    base_angles_updated[2] = angle_values[2];
    base_error_angles_updated[0] = error_angle_values[0];
    base_error_angles_updated[1] = error_angle_values[1];
    base_error_angles_updated[2] = error_angle_values[2];
    
    setBaseAngles(angle_values,0);
    setBaseAngles(error_angle_values,1);
    
    start_time = millis();
    
    // Set the YPR end values
    yaw_end = angle_values[0];
    pitch_end = angle_values[1];
    roll_end = angle_values[2];
    yaw_error_end = error_angle_values[0];
    pitch_error_end = error_angle_values[1];
    roll_error_end = error_angle_values[2];
    
    // Calculate drift of the YPR over 5 seconds
    yaw_drift = (yaw_start - yaw_end) / 15000;
    pitch_drift = (pitch_start - pitch_end) / 15000;
    roll_drift = (roll_start - roll_end) / 15000; 
    yaw_error_drift = (yaw_error_start - yaw_error_end) / 15000;
    pitch_error_drift = (pitch_error_start - pitch_error_end) / 15000;
    roll_error_drift = (roll_error_start - roll_error_end) / 15000;
    
    end_time = millis();
    
    Serial.print("Time = ");
    Serial.println(end_time - start_time);
    Serial.print(" Yaw Drift Amounts ");
    Serial.print(yaw_drift);
    Serial.print(" \t");
    Serial.println(yaw_error_drift);   
    Serial.print(" Pitch Drift Amounts ");
    Serial.print(pitch_drift);
    Serial.print(" \t");
    Serial.println(pitch_error_drift);   
    Serial.print(" Roll Drift Amounts ");
    Serial.print(roll_drift);
    Serial.print(" \t");
    Serial.println(roll_error_drift);

    sys_init_complete();
    
    // Enable movement
    digitalWrite(enable_x, HIGH);
    digitalWrite(enable_y, HIGH);
    digitalWrite(enable_z, HIGH);
    free(angle_values_init);
    free(error_angle_values_init);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    if(in_UI == 0){
        //get angles from poll
        start_test = millis();
        
        base_imu_flag = imu.poll(angle_values);
        error_imu_flag = imu_error.poll(error_angle_values);
        
        //pid returns 3 duty cycles
        //ignore cases where there was a fifo overflow
        if( error_imu_flag && base_imu_flag ){
            end_test = millis();
              
            base_angles_updated[0] = base_angles_updated[0] + ((end_test-start_test)* yaw_drift);
            base_error_angles_updated[0] = base_error_angles_updated[0]  + ((end_test-start_test)* yaw_error_drift);
            base_angles_updated[1] = base_angles_updated[1] + ((end_test-start_test)* pitch_drift);
            base_error_angles_updated[1] = base_error_angles_updated[1]  + ((end_test-start_test)* pitch_error_drift);
            base_angles_updated[2] = base_angles_updated[2] + ((end_test-start_test)* roll_drift);
            base_error_angles_updated[2] = base_error_angles_updated[2]  + ((end_test-start_test)* roll_error_drift);
              
            setBaseAngles(base_angles_updated,0);
            setBaseAngles(base_error_angles_updated,1);  
            
//            Serial.print("Base Angles Updated: \t");
//            Serial.print(base_angles_updated[0]);
//            Serial.print("\t");
//            Serial.print(base_angles_updated[1]);
//            Serial.print("\t");
//            Serial.println(base_angles_updated[2]);
//            Serial.print("Base Error Angles Updated: \t");
//            Serial.print(base_error_angles_updated[0]);
//            Serial.print("\t");
//            Serial.print(base_error_angles_updated[1]);
//            Serial.print("\t");
//            Serial.println(base_error_angles_updated[2]);

            duty = PIDMovementCalc_withError(angle_values, error_angle_values);
              
            // debug
            Serial.print(F("duty cycles: \t"));                               
            Serial.print(duty[0]);
            Serial.print(F("\t"));
            Serial.print(duty[1]);
            Serial.print(F("\t"));
            Serial.println(duty[2]);
            Serial.println(F("")); 
              
            //set duty cycles
            motorPinx.duty(duty[0]);
            motorPiny.duty(50);
            motorPinz.duty(50);
            free(duty);
          
        }else{
            Serial.println(F("Something went wrong in retreiving IMU data"));
        }
        
        in_UI = fwd_butt_handler();
    }else{
        //set duty cycles
        motorPinx.duty(50);
        motorPiny.duty(50);
        motorPinz.duty(50);
        
        //stop using the IMU, focus on only the LCD interface.
        in_UI = LCD_movement_handler(0);
    }
}


