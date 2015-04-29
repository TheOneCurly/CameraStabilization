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
 #include "IMUController.h"
 
IMUController::IMUController (int addr){
  if(addr == 0){
	  mpu = MPU6050(0x68);
          imu_num = 0;
  }else{
	  mpu = MPU6050(0x69);
          imu_num = 1;
  }
}

bool IMUController::init(){
	// initialize quaternion
	q0 = 1.0f;
	q1 = 0.0f;
	q2 = 0.0f;
	q3 = 0.0f;
	exInt = 0.0;
	eyInt = 0.0;
	ezInt = 0.0;
	twoKp = twoKpDef;
	twoKi = twoKiDef;
	integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;
	lastUpdate = 0;
	now = 0;
	
	// get values from global variables of same name defined in calibration.h
	acc_off_x = ::acc_off_x;
	acc_off_y = ::acc_off_y;
	acc_off_z = ::acc_off_z;
	acc_scale_x = ::acc_scale_x;
	acc_scale_y = ::acc_scale_y;
	acc_scale_z = ::acc_scale_z;
        gyro_sensitivity = 14.375f;

	mpu.initialize();
        mpu.setDLPFMode(MPU6050_DLPF_BW_20); 
	mpu.setI2CMasterModeEnabled(0);
	mpu.setI2CBypassEnabled(1);
	mpu.setRate(0x13);			//Sets sample rate to 8000/1+7 = 1000Hz
	mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
	mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	gyro_sensitivity = 16.4f;
	delay(5);
	delay(30);
	Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	
	// zero gyro
	zeroGyro();
}

void IMUController::getRawValues(int16_t * raw_values) {
    mpu.getMotion6(&raw_values[0], &raw_values[1], &raw_values[2], &raw_values[3], &raw_values[4], &raw_values[5]);
}

/**
 * Populates values with calibrated readings from the sensors
*/
void IMUController::getValues(float * values) {  
    int16_t accgyroval[6];
    mpu.getMotion6(&accgyroval[0], &accgyroval[1], &accgyroval[2], &accgyroval[3], &accgyroval[4], &accgyroval[5]);

    // remove offsets from the gyroscope
    accgyroval[3] = accgyroval[3] - gyro_off_x;
    accgyroval[4] = accgyroval[4] - gyro_off_y;
    accgyroval[5] = accgyroval[5] - gyro_off_z;

    for(int i = 0; i<6; i++) {
      if(i < 3) {
        values[i] = (float) accgyroval[i];
      }
      else {
        values[i] = ((float) accgyroval[i]) / 16.4f; // NOTE: this depends on the sensitivity chosen
      }
    }
  
  
  #warning Accelerometer calibration active: have you calibrated your device?
  // remove offsets and scale accelerometer (calibration)
  values[0] = (values[0] - acc_off_x) / acc_scale_x;
  values[1] = (values[1] - acc_off_y) / acc_scale_y;
  values[2] = (values[2] - acc_off_z) / acc_scale_z;
  
}

/**
 * Computes gyro offsets
*/
void IMUController::zeroGyro() {
  const int totSamples = 3;
  int16_t raw[11];
  float tmpOffsets[] = {0,0,0};
  
  for (int i = 0; i < totSamples; i++){
    getRawValues(raw);
    tmpOffsets[0] += raw[3];
    tmpOffsets[1] += raw[4];
    tmpOffsets[2] += raw[5];
  }
  
  gyro_off_x = tmpOffsets[0] / totSamples;
  gyro_off_y = tmpOffsets[1] / totSamples;
  gyro_off_z = tmpOffsets[2] / totSamples;
}

void IMUController::AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az) {
	float recipNorm;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
	float halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
	float qa, qb, qc;

	// Auxiliary variables to avoid repeated arithmetic
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if((ax != 0.0f) && (ay != 0.0f) && (az != 0.0f)) {
		float halfvx, halfvy, halfvz;

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity
		halfvx = q1q3 - q0q2;
		halfvy = q0q1 + q2q3;
		halfvz = q0q0 - 0.5f + q3q3;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex += (ay * halfvz - az * halfvy);
		halfey += (az * halfvx - ax * halfvz);
		halfez += (ax * halfvy - ay * halfvx);
	}

	// Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
	if(halfex != 0.0f && halfey != 0.0f && halfez != 0.0f) {
		// Compute and apply integral feedback if enabled
		if(twoKi > 0.0f) {
			integralFBx += twoKi * halfex * (1.0f / sampleFreq);  // integral error scaled by Ki
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			gx += integralFBx;  // apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		}else {
			integralFBx = 0.0f; // prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

/**
 * Populates array q with a quaternion representing the IMU orientation with respect to the Earth
 * 
 * @param q the quaternion to populate
*/
void IMUController::getQ(float * q) {
	float val[9];
	getValues(val);

	now = micros();
	sampleFreq = 1.0 / ((now - lastUpdate) / 1000000.0);
	lastUpdate = now;
	// gyro values are expressed in deg/sec, the * M_PI/180 will convert it to radians/sec
	AHRSupdate(val[3] * M_PI/180, val[4] * M_PI/180, val[5] * M_PI/180, val[0], val[1], val[2]);


	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;

}

/**
 * Returns the yaw pitch and roll angles, respectively defined as the angles in radians between
 * the Earth North and the IMU X axis (yaw), the Earth ground plane and the IMU X axis (pitch)
 * and the Earth ground plane and the IMU Y axis.
 * 
 * @note This is not an Euler representation: the rotations aren't consecutive rotations but only
 * angles from Earth and the IMU. For Euler representation Yaw, Pitch and Roll see FreeIMU::getEuler
 * 
 * @param ypr three floats array which will be populated by Yaw, Pitch and Roll angles in radians
*/
void IMUController::getYawPitchRollRad(float * ypr) {
	float q[4]; // quaternion
	float gx, gy, gz; // estimated gravity direction
	getQ(q);

	gx = 2 * (q[1]*q[3] - q[0]*q[2]);
	gy = 2 * (q[0]*q[1] + q[2]*q[3]);
	gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

	ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);
	ypr[1] = atan(gx / sqrt(gy*gy + gz*gz));
	ypr[2] = atan(gy / sqrt(gx*gx + gz*gz));
}

/**
 * Returns the yaw pitch and roll angles, respectively defined as the angles in degrees between
 * the Earth North and the IMU X axis (yaw), the Earth ground plane and the IMU X axis (pitch)
 * and the Earth ground plane and the IMU Y axis.
 * 
 * @note This is not an Euler representation: the rotations aren't consecutive rotations but only
 * angles from Earth and the IMU. For Euler representation Yaw, Pitch and Roll see FreeIMU::getEuler
 * 
 * @param ypr three floats array which will be populated by Yaw, Pitch and Roll angles in degrees
*/
void IMUController::getYawPitchRoll(float * ypr) {
	getYawPitchRollRad(ypr);
	arr3_rad_to_deg(ypr);
}

bool IMUController::poll(float* angle_values){
    float data[3];
    for (int i = 0; i < 10; i++){
        getYawPitchRoll(data);;
        angle_values[0] += data[0];
        angle_values[1] += data[1];
     	angle_values[2] += data[2];
    }
    
    angle_values[0] = angle_values[0]/10;
    angle_values[1] = angle_values[1]/10;
    angle_values[2] = angle_values[2]/10;
    
    Serial.print(imu_num);
    Serial.print(F("\t angles \t"));
    Serial.print(angle_values[0]);
    Serial.print(F("\t"));
    Serial.print(angle_values[1]);
    Serial.print(F("\t"));
    Serial.println(angle_values[2]);
	
    if (angle_values)
	return true;
	
    return false;
}

/**
 * Converts a 3 elements array arr of angles expressed in radians into degrees
*/
void arr3_rad_to_deg(float * arr) {
	arr[0] *= 180/M_PI;
	arr[1] *= 180/M_PI;
	arr[2] *= 180/M_PI;
}

/**
 * Fast inverse square root implementation
 * @see http://en.wikipedia.org/wiki/Fast_inverse_square_root
*/
float invSqrt(float number) {
	volatile long i;
	volatile float x, y;
	volatile const float f = 1.5F;

	x = number * 0.5F;
	y = number;
	i = * ( long * ) &y;
	i = 0x5f375a86 - ( i >> 1 );
	y = * ( float * ) &i;
	y = y * ( f - ( x * y * y ) );
	return y;
}
