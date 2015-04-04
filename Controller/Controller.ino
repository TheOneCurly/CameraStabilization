// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added seamless Fastwire support
//                 - added note about gyro calibration
//      2012-06-21 - added note about Arduino 1.0.1 + Leonardo compatibility error
//      2012-06-20 - improved FIFO overflow handling and simplified read process
//      2012-06-19 - completely rearranged DMP initialization code and simplification
//      2012-06-13 - pull gyro and accel data from FIFO packet instead of reading directly
//      2012-06-09 - fix broken FIFO read sequence and change interrupt detection to RISING
//      2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                 - add 3D math helper file to DMP6 example sketch
//                 - add Euler output and Yaw/Pitch/Roll output formats
//      2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//      2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//      2012-05-30 - basic DMP initialization working

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

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

// declare PWM pins
const int PWM_pin_x = 34;
const int PWM_pin_y = 3;
const int PWM_pin_z = 5;

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "IMUController.h"
IMUController imu;

volatile int stopx, stopy, stopz;
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
//
volatile int timerCnt = 0;
//TC1 ch 0
void TC3_Handler()
{
       TC_GetStatus(TC1, 0);
       if (stopx != 0 && stopy != 0 && stopz != 0){
           timerCnt ++;
           
           if (timerCnt >= stopx){
               //analogWrite(PWM_pin_x, 128); // Stop
               Serial.println("StopX");
               stopx = 0;
           }
           
           if (timerCnt >= stopy){
               //analogWrite(PWM_pin_y, 128); // Stop
               Serial.println("StopY");
               stopy = 0;
           }
           
           if (timerCnt >= stopz){
               //analogWrite(PWM_pin_z, 128); // Stop
               Serial.println("StopZ");
               stopz = 0;
           }
       }else{
           timerCnt = 0;  
       }
}
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
       pmc_set_writeprotect(false);
       pmc_enable_periph_clk((uint32_t)irq);
       TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
       uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
       TC_SetRA(tc, channel, rc/2); //50% high, 50% low
       TC_SetRC(tc, channel, rc);
       TC_Start(tc, channel);
       tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
       tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
       NVIC_EnableIRQ(irq);
}

void setup() {
    startTimer(TC1, 0, TC3_IRQn, 4); //TC1 channel 0, the IRQ for that channel and the desired frequency
    

    // initialize PWM pins
    //pinMode(PWM_pin_x, OUTPUT);
    //pinMode(PWM_pin_y, OUTPUT);
    //pinMode(PWM_pin_z, OUTPUT);
    
    imu.init();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    imu.poll();
}

