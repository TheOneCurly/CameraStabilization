// Initial PWM test code.
// This code uses analog write, more specialized code may be needed due to interrupt usage.  
// Frequency and pin may need adjustment as well.
// Mitchell Cook

#include "customPWM.h"

// initialize pwm pin, using digital write this can be any digital out pin
int PWM_output_pin = 34;
const int brake_x = 6;
const int enable_x = 9;

bool isGood = customPWMinit(20000, 100);
customPWM motorPin(PWM_output_pin);

void setup(){
	Serial.begin(115200);


	Serial.print("Initializing PWM output on pin: ");
	Serial.println(PWM_output_pin);
	pinMode(brake_x, OUTPUT);
        pinMode(enable_x, OUTPUT);
	//pinMode(PWM_output_pin, OUTPUT);
        digitalWrite(brake_x, HIGH);
        digitalWrite(enable_x, HIGH);
}

void loop(){
  //digitalWrite(PWM_output_pin, HIGH);
  
  //360 deg
//  motorPin.duty(75);
//  delay(350);
//  motorPin.duty(50);
//  delay(2000);
//  motorPin.duty(25);
//  delay(350);
//  motorPin.duty(50);
//  delay(2000);
  
  //not strong enough to turn 360 deg
//  motorPin.duty(60);
//  delay(1000);
//  motorPin.duty(50);
//  delay(2000);
//  motorPin.duty(40);
//  delay(1000);
//  motorPin.duty(50);
//  delay(2000);

     //360 deg
//    motorPin.duty(100);
//    delay(170);
//    motorPin.duty(50);
//    delay(2000);
//    motorPin.duty(0);
//    delay(170);
//    motorPin.duty(50);
//    delay(2000);

//motorPin.duty(75);
//delay(350);
//motorPin.duty(25);
//delay(350);
//motorPin.duty(100);
//delay(170);
//motorPin.duty(0);
//delay(170);


  motorPin.duty(75);
  delay(1000);
  motorPin.duty(50);
  delay(1000);
  motorPin.duty(25);
  delay(1000);
  motorPin.duty(50);
  delay(1000);
//        int i = 0;
//        for (i = 0; i <= 50; i++){
//            motorPin.duty(i);
//            delay(100);
//        }
//        
//        for (i = 50; i <= 100; i++){
//            motorPin.duty(i);
//            delay(100);
//        }
//        
//        for (i = 100; i >= 50; i--){
//            motorPin.duty(i);
//            delay(100);
//        }
//        
//        for (i = 50; i >= 0; i--){
//            motorPin.duty(i);
//            delay(100);
//        }

}

