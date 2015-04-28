/*
 *
 *	IMUController.cpp
 *
 *	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *	Creation Date: 2 April 2015
 *	Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *	Description: Wrapper functions for the MPU6050.
 *                        Provides initialization and polling.
 *
 */
 /***************************** Preprocessor Flags ****************************/
//#define DEBUG

//possible implementation of debug code
//#ifdef DEBUG
// #define DEBUG_PRINT(x)     Serial.print (x)
// #define DEBUG_PRINTDEC(x)     Serial.print (x, DEC)
// #define DEBUG_PRINTLN(x)  Serial.println (x)
//#else
// #define DEBUG_PRINT(x)
// #define DEBUG_PRINTDEC(x)
// #define DEBUG_PRINTLN(x) 
//#endif

 /***************************** Include Files *********************************/
#include "IMUController.h"

/***************************** Variables *********************************/

const int AVERAGE_MAX_COUNT = 6;

/***************************** ISR *********************************/


/**
 *
 * Creates a new MPU6050 object and initializes the I2C connection  
 *
 * @param
 *
 * @return	Boolean indicating status of MPU connection. 
 *                True - success
 *                False - Failure
 ******************************************************************************/
IMUController::IMUController (int addr){
    if(addr == 0){
        imu_num = 0;
        mpu = MPU6050(0x68);
    }else{
        imu_num = 1;
        mpu = MPU6050(0x69);
    }
}
 
bool IMUController::init(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
       // TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
   
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
  
    // verify connection
    Serial.println(F("Testing device connections..."));
    bool connection = mpu.testConnection();
    
    if(connection){
      Serial.println(F("MPU6050 connection successful") );
      return true;
    }
    else{
      Serial.println(F("MPU6050 connection failed"));
      return false;
    }
  
    //    // supply your own gyro offsets here, scaled for min sensitivity
    //    mpu.setXGyroOffset(220);
    //    mpu.setYGyroOffset(76);
    //    mpu.setZGyroOffset(-85);
    //    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
}

/**
 *
 * Averages 10 values from the initialized IMU and places them in the
 * provided float array.
 *
 * @param    3-length float array pointer for the output values
 *
 * @return	Boolean indicating success of data pull 
 *                True - success
 *                False - Failure
 ******************************************************************************/
bool IMUController::poll(float* angle_values){
    avg_count = 0;
  
//    while(avg_count < (AVERAGE_MAX_COUNT + 1)){
//      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
//      Serial.print("a/g:\t");
//      Serial.print(ax); Serial.print("\t");
//      Serial.print(ay); Serial.print("\t");
//      Serial.print(az); Serial.print("\t");
//      Serial.print(gx); Serial.print("\t");
//      Serial.print(gy); Serial.print("\t");
//      Serial.println(gz);
//      
//      // do calculations to make the raw values into angles
//      //raw_converted_angle[0] = ...;
//      
//      if( avg_count < AVERAGE_MAX_COUNT){
//        angle_avg[0] += raw_converted_angle[0];
//        angle_avg[1] += raw_converted_angle[1];
//        angle_avg[2] += raw_converted_angle[2];
//        avg_count++;
//      }
//      else{
//        angle_avg[0] = angle_avg[0]/AVERAGE_MAX_COUNT;
//        angle_avg[1] = angle_avg[1]/AVERAGE_MAX_COUNT;
//        angle_avg[2] = angle_avg[2]/AVERAGE_MAX_COUNT;
//        
//        Serial.print(imu_num);
//        Serial.print(F("\t angles \t"));
//        Serial.print(angle_avg[0]);
//        Serial.print(F("\t"));
//        Serial.print(angle_avg[1]);
//        Serial.print(F("\t"));
//        Serial.println(angle_avg[2]);
//        
//        angle_values[0] = angle_avg[0];
//        angle_values[1] = angle_avg[1];
//        angle_values[2] = angle_avg[2];
//        avg_count = 0;
//        return true;
//      }  
//    }  
    
    angle_values[0] = 50;
    angle_values[1] = 50;
    angle_values[2] = 50;
    
    Serial.print(F(imu_num));
    Serial.print(F("\t imu angles \t"));
    Serial.print(angle_values[0]);
    Serial.print(F("\t"));
    Serial.print(angle_values[1]);
    Serial.print(F("\t"));
    Serial.println(angle_values[2]);
    
    return true;
}

MPU6050* IMUController::getIMU(){
     return &mpu;   
}

