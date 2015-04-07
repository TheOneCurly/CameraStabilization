/*============================================================================================


	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
	Creation Date: 2 April 2015
	Project: RIT Senior Design 2014-2015 -- Camera Stabilization

	Description: Wrapper functions for the MPU6050.
                        Provides initialization and polling.

============================================================================================ */
#ifndef _IMU_CONTROL_H_
#define _IMU_CONTROL_H_

#include "MPU6050_6Axis_MotionApps20.h"


class IMUController{
    public:
        IMUController(int);
        bool init();
        bool poll(float*);
        MPU6050* getIMU();
        volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
        
    private:
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    bool dmpReady = false;  // set true if DMP init was successful
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer
    
    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    
    float ypr_avg[3];
    int ypr_count = 0;
    int interruptNum;
    MPU6050 mpu;
    
};
#endif /* _IMU_CONTROL_H_ */

