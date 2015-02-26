/*
  customPWM.h - A header file for custom PWM on the Arduino Due
  Created by Joshua Milas, Febuary 15th 2014
  Updated by Joshua Milas, March 14th 2014
*/
#ifndef customPWM_h
#define customPWM_h

#include "Arduino.h"

#define point PWM	//A pointer to the device interface
#define FREQUENCY 333  //The default frequency of the PWM in Hz
#define PERIOD 255  //The default MAX of the PWM, or the max range (0-PERIOD)
#define DUTY 0      //The default initial duty cycle, turned off for safety
#define ALIGNMENT 0  //default alignment. 0 is left aligned, 1 is center
#define POLARITY 0  //default polarity, 0 is LOW on start, 1 is HIGH on start

bool customPWMinit(unsigned long freq, unsigned long per);

class customPWM
{
  public:
	customPWM(int channel, bool alignment = ALIGNMENT, bool polarity = POLARITY);
    void duty(unsigned long dy);
    void setMotorDuty(unsigned long dy);
	
	unsigned long period;
    unsigned long currentDuty;
    unsigned long motorDuty;
  private:
    unsigned int _channel;
    unsigned long _lowFreq;
};

#endif

