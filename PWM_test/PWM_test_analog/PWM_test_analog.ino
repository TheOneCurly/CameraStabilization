// Initial PWM test code.
// This code uses analog write, more specialized code may be needed due to interrupt usage.  
// Frequency and pin may need adjustment as well.
// Mitchell Cook

#include "customPWM.h"

// initialize pwm pin, using digital write this can be any digital out pin
int PWM_output_pin = 34;

bool isGood = customPWMinit(20000, 100);
customPWM motorPin(PWM_output_pin);

void setup(){
	Serial.begin(115200);
	Serial.print("Initializing PWM output on pin: ");
	Serial.println(PWM_output_pin);
	
	//pinMode(PWM_output_pin, OUTPUT);
}

void loop(){
//	Serial.println("Running at 0% duty cycle, full speed reverse");
//	//analogWrite(PWM_output_pin, 0);
//        motorPin.duty(0);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 25% duty cycle, half speed reverse");
//	//analogWrite(PWM_output_pin, 64);
//        motorPin.duty(25);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 50% duty cycle, idle");
//	//analogWrite(PWM_output_pin, 127);
//        motorPin.duty(50);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 75% duty cycle, half speed forward");
//	//analogWrite(PWM_output_pin, 191);
//        motorPin.duty(75);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 100% duty cycle, full speed forward");
//	//analogWrite(PWM_output_pin, 255);
//        motorPin.duty(100);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 75% duty cycle, half speed forward");
//	//analogWrite(PWM_output_pin, 191);
//        motorPin.duty(75);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 50% duty cycle, idle");
//	//analogWrite(PWM_output_pin, 127);
//        motorPin.duty(50);
//	delay(5000); // 5 seconds
//	
//	Serial.println("Running at 25% duty cycle, half speed reverse");
//	//analogWrite(PWM_output_pin, 64);
//        motorPin.duty(25);
//	delay(5000); // 5 seconds

        int i = 0;
        for (i = 0; i <= 50; i++){
            motorPin.duty(i);
            delay(100);
        }
        
        for (i = 50; i <= 100; i++){
            motorPin.duty(i);
            delay(100);
        }
        
        for (i = 100; i >= 50; i--){
            motorPin.duty(i);
            delay(100);
        }
        
        for (i = 50; i >= 0; i--){
            motorPin.duty(i);
            delay(100);
        }
	
}

