#include <customPWM.h>

customPWM test(38);  //Start the PWM on pin 38

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  //Cycle from 0 to the max period...
  for(int i = 0; i <= test.period; ++i){
    test.duty(i);
    delay(5);
  }
  //...then go back down to zero
  for(int i = test.period; i >= 0; --i){
    test.duty(i);
    delay(5);
  }
}
