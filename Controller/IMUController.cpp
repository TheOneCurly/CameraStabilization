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

const int AVERAGE_COUNT = 6;

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
        mpu = MPU6050(0x68);
    }else{
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
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
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
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);
    
//    if (ypr_count < AVERAGE_COUNT) {
//      ypr_avg[0] += ypr[0];
//      ypr_avg[1] += ypr[1];
//      ypr_avg[2] += ypr[2];
//      ypr_count ++;
//    }else{
//      #ifdef DEBUG
//        Serial.print(interruptNum);
//        Serial.print(F("\t ypr\t"));
//        Serial.print(ypr_avg[0] * 180/(AVERAGE_COUNT*M_PI));
//        Serial.print(F("\t"));
//        Serial.print(ypr_avg[1] * 180/(AVERAGE_COUNT*M_PI));
//        Serial.print(F("\t"));
//        Serial.println(ypr_avg[2] * 180/(AVERAGE_COUNT*M_PI));
//      #endif
//
//      ypr_avg[0] = ypr_avg[0] * 180/(AVERAGE_COUNT*M_PI);
//      ypr_avg[1] = ypr_avg[1] * 180/(AVERAGE_COUNT*M_PI);
//      ypr_avg[2] = ypr_avg[2] * 180/(AVERAGE_COUNT*M_PI);
//      
//      angle_values[0] = ypr_avg[0];
//      angle_values[1] = ypr_avg[1];
//      angle_values[2] = ypr_avg[2];
//      ypr_count = 0;
//      ypr_avg[0] = 0;
//      ypr_avg[1] = 0;
//      ypr_avg[2] = 0;
//      mpu.resetFIFO();       
//      return true;   

    // this is where the conditioning of the raw values will occur. 
    // this should return 3 float angles
    angle_values[0] = 50;
    angle_values[1] = 50;
    angle_values[2] = 50;
}

MPU6050* IMUController::getIMU(){
     return &mpu;   
}

