#include <customPWM.h>

//Start the PWM hardware with a frequency of 1000 hz
//and make the period from 0-100
bool isGood = customPWMinit(1000, 100);

//set pin 6 to be a PWM pin that is center wave thats 
//starts out low
customPWM ledPin(6, 1, 0);

void setup() {
  // put your setup code here, to run once:
  ledPin.duty(ledPin.period/2); //set it to half the maximum
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
