// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "helper_3dmath.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "IMUController.h";

IMUController imu1(0);
IMUController imu2(1);

float *data = (float*) calloc(3,sizeof(float));
float *angle_avg = (float*) calloc(3,sizeof(float));

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(115200);
    
    imu1.init();
    imu2.init();
}

void loop() {
    for (int i = 0; i < 10; i++){
        imu1.poll(data);
        angle_avg[0] += data[0];
        angle_avg[1] += data[1];
     	angle_avg[2] += data[2];
    }
    
    angle_avg[0] = angle_avg[0]/10;
    angle_avg[1] = angle_avg[1]/10;
    angle_avg[2] = angle_avg[2]/10;

    Serial.print(" Yaw1: ");
    Serial.print(angle_avg[0]);
    Serial.print(" \t ");
    
    Serial.print(" Pitch1: ");
    Serial.print(angle_avg[1]);
    Serial.print(" \t ");
    
    Serial.print(" Roll1: ");
    Serial.println(angle_avg[2]);
    
    angle_avg[0] = 0;
    angle_avg[1] = 0;
    angle_avg[2] = 0;
    
    for (int i = 0; i < 10; i++){
        imu2.poll(data);
        angle_avg[0] += data[0];
        angle_avg[1] += data[1];
        angle_avg[2] += data[2];
    }
    
    angle_avg[0] = angle_avg[0]/10;
    angle_avg[1] = angle_avg[1]/10;
    angle_avg[2] = angle_avg[2]/10;

    Serial.print(" Yaw2: ");
    Serial.print(angle_avg[0]);
    Serial.print(" \t ");
    
    Serial.print(" Pitch2: ");
    Serial.print(angle_avg[1]);
    Serial.print(" \t ");
    
    Serial.print(" Roll2: ");
    Serial.println(angle_avg[2]);
    
    angle_avg[0] = 0;
    angle_avg[1] = 0;
    angle_avg[2] = 0;
}
