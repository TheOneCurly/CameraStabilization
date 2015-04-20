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
#define DEBUG
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
          interruptNum = 0;
          mpu = MPU6050(0x68);
      }else{
          interruptNum = 1;
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
    
    #ifdef DEBUG
    // initialize serial communication
    //Serial.begin(115200);
    
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.
    
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    //Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    //while (!Serial.available());                 // wait for data
    //while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        /*
        if(interruptNum == 0){
            attachInterrupt(0, dmp0DataReady, RISING);
        }else{
            attachInterrupt(1, dmp1DataReady, RISING);
        }
        */
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        Serial.println(" ");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
	#else
	mpu.initialize();
	if(!mpu.testConnection()){
		return false;
	}
	devStatus = mpu.dmpInitialize();
	
	// supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
	
	// make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
        
        return true;
    } else {
        return false;
    }
	#endif
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
    ypr_count = 0;
    
    if (!dmpReady) return false;
    
    while(ypr_count < (AVERAGE_COUNT + 1)){
      if (mpuInterrupt || fifoCount >= packetSize){  
        // reset interrupt flag and get INT_STATUS byte
        mpuInterrupt = false;
        mpuIntStatus = mpu.getIntStatus();
    
        // get current FIFO count
        fifoCount = mpu.getFIFOCount();
        // check for overflow (this should never happen unless our code is too inefficient)
        if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();
            
            #ifdef DEBUG
              Serial.println(F("FIFO overflow!"));
            #endif
            
            return false;    
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
        } else if (mpuIntStatus & 0x02) {
            // wait for correct available data length, should be a VERY short wait
            while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
            
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            if (ypr_count < AVERAGE_COUNT) {
              ypr_avg[0] += ypr[0];
              ypr_avg[1] += ypr[1];
              ypr_avg[2] += ypr[2];
              ypr_count ++;
            }else{
              #ifdef DEBUG
                Serial.print(interruptNum);
                Serial.print("\t ypr\t");
                Serial.print(ypr_avg[0] * 180/(AVERAGE_COUNT*M_PI));
                Serial.print("\t");
                Serial.print(ypr_avg[1] * 180/(AVERAGE_COUNT*M_PI));
                Serial.print("\t");
                Serial.println(ypr_avg[2] * 180/(AVERAGE_COUNT*M_PI));
              #endif
  
              ypr_avg[0] = ypr_avg[0] * 180/(AVERAGE_COUNT*M_PI);
              ypr_avg[1] = ypr_avg[1] * 180/(AVERAGE_COUNT*M_PI);
              ypr_avg[2] = ypr_avg[2] * 180/(AVERAGE_COUNT*M_PI);
              
              angle_values[0] = ypr_avg[0];
              angle_values[1] = ypr_avg[1];
              angle_values[2] = ypr_avg[2];
              ypr_count = 0;
              ypr_avg[0] = 0;
              ypr_avg[1] = 0;
              ypr_avg[2] = 0;
              mpu.resetFIFO();       
              return true;          
            }
        }
        else{
          //Serial.println("bad int status");
        }
      } 
    }   
}

MPU6050* IMUController::getIMU(){
     return &mpu;   
}

