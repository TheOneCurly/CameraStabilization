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
        
    private:   
    int imu_num;
    // orientation/motion vars   
    int16_t ax, ay, az;     // raw values from imu
    int16_t gx, gy, gz;
    
    float angle_avg[3];
    int avg_count = 0;
    MPU6050 mpu;
    
};
#endif /* _IMU_CONTROL_H_ */

