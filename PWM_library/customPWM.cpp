/*
  customPWM.cpp - A cpp file for custom PWM on the Arduino Due
  Created by Joshua Milas, Febuary 15th 2014
  Updated by Joshua Milas, March 14th 2014
*/
#include "Arduino.h"
#include "customPWM.h"
#include "assert.h"


//define some constants, taken from reverse engineering the arduino code
Pio* port[] = {PIOC, PIOC, PIOC, PIOC, PIOC, PIOC, PIOC, PIOC};  //Obtained from the pinout diagram

EPioType type[] = {PIO_PERIPH_B, PIO_PERIPH_B, PIO_PERIPH_B, PIO_PERIPH_B, 
                   PIO_PERIPH_B, PIO_PERIPH_B, PIO_PERIPH_B, PIO_PERIPH_B};  //Guess and check
                   
unsigned long pin[] = {PIO_PC2, PIO_PC4, PIO_PC6, PIO_PC8, PIO_PC21B_PWML4, 
                       PIO_PC22B_PWML5, PIO_PC23B_PWML6, PIO_PC24B_PWML7};  //Obtained from variant.cpp
                       //pins 34, 36, 38, 40, 9, 8, 7, 6
                       
unsigned long conf[] = {PIO_DEFAULT, PIO_DEFAULT, PIO_DEFAULT, PIO_DEFAULT, 
                        PIO_DEFAULT, PIO_DEFAULT, PIO_DEFAULT, PIO_DEFAULT};  //guess and check

//Set some global variables
bool isEnabled = false;
unsigned long globFrequency = FREQUENCY;
unsigned long globPeriod = PERIOD;




//Setup the hardware for the PWM with a specific frequency and period
bool customPWMinit(unsigned long freq, unsigned long per){
   if(!isEnabled)
  {
    globFrequency = freq;
	globPeriod = per;
	
    //Turn on the PWM peripheral
    pmc_enable_periph_clk(PWM_INTERFACE_ID);

    //Configure the clocks
    PWMC_ConfigureClocks(freq*per, 0, VARIANT_MCK);  //VARIANT_MCK is the master clock frequency

    isEnabled = true;
	return 0;
  }
  else
  {
  //The hardware is already set, and you are trying to set it again
    return 1;
  }
}
	

	
//Set up the PWM on the pin specified with the alignment and polarity specified
customPWM::customPWM(int channel, bool alignment, bool polarity)
{
  period = globPeriod;
  _lowFreq = period/3;	//Calculate the low duty cycle setting, for the motor controller
  currentDuty = DUTY;
  motorDuty = DUTY;
  
  //Get the appropriate number if a pin is given
  switch(channel)
  {
    case 34:
      _channel = 0;
      break;
    case 36:
      _channel = 1;
      break;
    case 38:
      _channel = 2;
      break;
    case 40:
      _channel = 3;
      break;
    case 9:
      _channel = 4;
      break;
    case 8:
      _channel = 5;
      break;
    case 7:
      _channel = 6;
      break;
    case 6:
      _channel = 7;
      break;
    default:
      assert(0); //You didnt give it a correct pin
  }

  //If its not enabled yet, then enable it with the default settings
  if(!isEnabled)
  {
    customPWMinit(globFrequency, globPeriod);
  }
  
  //Arduino code to set the correct settings on the pins
  PIO_SetPeripheral(port[_channel], type[_channel], pin[_channel]);	//Set the peripheral of the pin and remove control from PIO
  PIO_DisableInterrupt(port[_channel], pin[_channel]);	//Disable the interupt on the pin, not needed since it is disabled in SetPeripheral
  PIO_PullUp(port[_channel], pin[_channel],  (conf[_channel] & PIO_PULLUP));	//Enables the pullup resistor on the pin
  
  //Configure the channels
  //Needed to access the registers because the Arduino code disables the changing of alignment or polarity
  unsigned int holder = (0xBu & 0xF) | (polarity<<9) | (alignment<<8);
  point->PWM_CH_NUM[_channel].PWM_CMR = holder;
  
  //Set the period (max value)
  PWMC_SetPeriod(point, _channel, period);
  
  //Set the default duty cycle
  PWMC_SetDutyCycle(point, _channel, DUTY);
  
  //Enable the pin and make it go
  PWMC_EnableChannel(point, _channel);
}

/*//Setup the PWM on the pin specified
customPWM::customPWM(int channel)
{
   customPWM holder(channel, ALIGNMENT, POLARITY);
   return holder;
}*/

//Set the duty cycle of the PWM pin
void customPWM::duty(unsigned long dy)
{
  currentDuty = dy;
  PWMC_SetDutyCycle(point, _channel, dy);
}

void customPWM::setMotorDuty(unsigned long dy)
{
  motorDuty = dy;
  duty(_lowFreq + (dy/3));
}
