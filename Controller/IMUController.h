#include <Wire.h>

/*============================================================================================


	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
	Creation Date: 2 April 2015
	Project: RIT Senior Design 2014-2015 -- Camera Stabilization

	Description: Wrapper functions for the MPU6050.
                        Provides initialization and polling.

============================================================================================ */
#ifndef _IMU_CONTROL_H_
#define _IMU_CONTROL_H_

//#include "MPU6050_6Axis_MotionApps20.h"
#include "Arduino.h"
#include "calibration.h"
//#include <EEPROM.h>

#include "I2Cdev.h"
#include "MPU6050.h"

#define twoKpDef  (2.0f * 0.5f) // 2 * proportional gain
#define twoKiDef  (2.0f * 0.1f) // 2 * integral gain

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

class IMUController{
    public:
		IMUController(int);
		bool init();
		bool poll(float*);
                void calLoad();
		int16_t* raw_acc, raw_gyro;
		int16_t gyro_off_x, gyro_off_y, gyro_off_z;
		int16_t acc_off_x, acc_off_y, acc_off_z;
		float acc_scale_x, acc_scale_y, acc_scale_z;
                float gyro_sensitivity;
                int imu_num;
	private:
		void zeroGyro();
		void getRawValues(int16_t * raw_values);
		void getValues(float * values);
		void getQ(float * q);
		void getYawPitchRollRad(float * ypr);
		void getYawPitchRoll(float * ypr);
		void gravityCompensateAcc(float * acc, float * q);
		void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az);
		MPU6050 mpu;
		float iq0, iq1, iq2, iq3;
		float exInt, eyInt, ezInt;  // scaled integral error
		volatile float twoKp;      // 2 * proportional gain (Kp)
		volatile float twoKi;      // 2 * integral gain (Ki)
		volatile float q0, q1, q2, q3; // quaternion of sensor frame relative to auxiliary frame
		volatile float integralFBx,  integralFBy, integralFBz;
		unsigned long lastUpdate, now; // sample period expressed in milliseconds
		float sampleFreq; // half the sample period expressed in seconds
};

float invSqrt(float number);
void arr3_rad_to_deg(float * arr);
#endif /* _IMU_CONTROL_H_ */

