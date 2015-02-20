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
const int PWM_pin_x = 11;
const int PWM_pin_y = 3;
const int PWM_pin_z = 5;

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
 * ========================================================================= */

/* =========================================================================
   NOTE: Arduino v1.0.1 with the Leonardo board generates a compile error
   when using Serial.write(buf, len). The Teapot output uses this method.
   The solution requires a modification to the Arduino USBAPI.h file, which
   is fortunately simple, but annoying. This will be fixed in the next IDE
   release. For more info, see these links:

   http://arduino.cc/forum/index.php/topic,109987.0.html
   http://code.google.com/p/arduino/issues/detail?id=958
 * ========================================================================= */

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

float euler_avg[3];
float euler_count = 0;

float ypr_avg[3];
float ypr_count = 0;

volatile int stopx, stopy, stopz;

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

volatile int timerCnt = 0;
//TC1 ch 0
void TC3_Handler()
{
       TC_GetStatus(TC1, 0);
       if (stopx != 0 && stopy != 0 && stopz != 0){
           timerCnt ++;
           
           if (timerCnt >= stopx){
               analogWrite(PWM_pin_x, 128); // Stop
               Serial.println("StopX");
               stopx = 0;
           }
           
           if (timerCnt >= stopy){
               analogWrite(PWM_pin_y, 128); // Stop
               Serial.println("StopY");
               stopy = 0;
           }
           
           if (timerCnt >= stopz){
               analogWrite(PWM_pin_z, 128); // Stop
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
    pinMode(PWM_pin_x, OUTPUT);
    pinMode(PWM_pin_y, OUTPUT);
    pinMode(PWM_pin_z, OUTPUT);
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        // TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
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
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

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
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
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
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    
    digitalWrite(13, false);
    
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
          Serial.println(F("FIFO overflow!"));
  
      // otherwise, check for DMP data ready interrupt (this should happen frequently)
      } else if (mpuIntStatus & 0x02) {
          // wait for correct available data length, should be a VERY short wait
          while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
          // read a packet from FIFO
          mpu.getFIFOBytes(fifoBuffer, packetSize);
          
          // track FIFO count here in case there is > 1 packet available
          // (this lets us immediately read more without waiting for an interrupt)
          fifoCount -= packetSize;
  
          #ifdef OUTPUT_READABLE_EULER
              // display Euler angles in degrees
              mpu.dmpGetQuaternion(&q, fifoBuffer);
              mpu.dmpGetEuler(euler, &q);
              if(euler_count < 10) {
                euler_avg[0] += euler[0];
                euler_avg[1] += euler[1];
                euler_avg[2] += euler[2];
                euler_count ++;
              } else {
                Serial.print("euler\t");
                Serial.print(euler_avg[0] * 18/M_PI);
                Serial.print("\t");
                Serial.print(euler_avg[1] * 18/M_PI);
                Serial.print("\t");
                Serial.println(euler_avg[2] * 18/M_PI);
                euler_count = 0;
                euler_avg[0] = 0;
                euler_avg[1] = 0;
                euler_avg[2] = 0;
                
                /*
                accept angle, adjust motor
                analogWrite(PWM_output_pin, 191);
	        delay(5000); // 5 seconds
                */
              }
          #endif
  
          #ifdef OUTPUT_READABLE_YAWPITCHROLL
              // display Euler angles in degrees
              mpu.dmpGetQuaternion(&q, fifoBuffer);
              mpu.dmpGetGravity(&gravity, &q);
              mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
              if (ypr_count < 10) {
                ypr_avg[0] += ypr[0];
                ypr_avg[1] += ypr[1];
                ypr_avg[2] += ypr[2];
                ypr_count ++;
              }else{
                Serial.print("ypr\t");
                Serial.print(ypr_avg[0] * 18/M_PI);
                Serial.print("\t");
                Serial.print(ypr_avg[2] * 18/M_PI);
                Serial.print("\t");
                Serial.println(ypr_avg[1] * 18/M_PI);

                ypr_avg[0] = ypr_avg[0] * 18/M_PI;
                ypr_avg[1] = ypr_avg[1] * 18/M_PI;
                ypr_avg[2] = ypr_avg[2] * 18/M_PI;
                
                
                if (ypr_avg[0] < -10 || ypr_avg[0] > 10) {
                    if (ypr_avg[0] < -10){
                        analogWrite(PWM_pin_x, 255); // Full Forward
                        Serial.println("StartX_Forward");
                    }else if (ypr_avg[0] > 10){
                        analogWrite(PWM_pin_x, 0); // Full Reverse
                        Serial.println("StartX_Reverse");
                    }
                    stopx = 10;
                }else{
                    stopx = 0;
                    Serial.println("StartX_Stop");
                }
                
                if (ypr_avg[1] < -10 || ypr_avg[1] > 10) {
                    if (ypr_avg[1] < -10){
                        analogWrite(PWM_pin_y, 255); // Full Forward
                        Serial.println("StartY_Forward");
                    }else if (ypr_avg[1] > 10){
                        analogWrite(PWM_pin_y, 0); // Full Reverse
                        Serial.println("StartY_Reverse");
                    }
                    stopy = 10;
                }else{
                    stopy = 0;
                    Serial.println("StartY_Stop");
                }
                
                if (ypr_avg[2] < -10 || ypr_avg[2] > 10) {
                    if (ypr_avg[2] < -10){
                        analogWrite(PWM_pin_z, 255); // Full Forward
                        Serial.println("StartZ_Forward");
                    }else if (ypr_avg[2] > 10){
                        analogWrite(PWM_pin_z, 0); // Full Reverse
                        Serial.println("StartZ_Reverse");
                    }
                    stopz = 10;
                }else{
                    stopz = 0;
                    Serial.println("StartZ_Stop");
                }
                
                ypr_count = 0;
                ypr_avg[0] = 0;
                ypr_avg[1] = 0;
                ypr_avg[2] = 0;
                
              }
          #endif
      }
    }
}

