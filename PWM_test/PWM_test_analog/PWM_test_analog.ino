// Initial PWM test code.
// This code uses analog write, more specialized code may be needed due to interrupt usage.  
// Frequency and pin may need adjustment as well.
// Mitchell Cook

#include "customPWM.h"

// initialize pwm pin, using digital write this can be any digital out pin
int PWM_output_pin = 34;

int hall_analog_1, hall_analog_2, hall_analog_3 = 0;
float hall_voltage_1, hall_voltage_2, hall_voltage_3 = 0;

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

//        int i = 0;
//        for (i = 0; i <= 50; i++){
//            motorPin.duty(i);
//            delay(100);
//            hall_analog = analogRead(A0);
//            hall_voltage = map(hall_analog, 1, 1023, 0, 5);
//            //hall_voltage = sensorValue * (5.0 / 1023.0);
//            Serial.print("Hall voltage is currently: ");
//            Serial.println(hall_voltage);
//        }
//        
//        for (i = 50; i <= 100; i++){
//            motorPin.duty(i);
//            delay(100);
//            hall_analog = analogRead(A0);
//            hall_voltage = map(hall_analog, 1, 1023, 0, 5);
//            //hall_voltage = sensorValue * (5.0 / 1023.0);
//            Serial.print("Hall voltage is currently: ");
//            Serial.println(hall_voltage);
//        }
//        
//        for (i = 100; i >= 50; i--){
//            motorPin.duty(i);
//            delay(100);
//            hall_analog = analogRead(A0);
//            hall_voltage = map(hall_analog, 1, 1023, 0, 5);
//            //hall_voltage = sensorValue * (5.0 / 1023.0);
//            Serial.print("Hall voltage is currently: ");
//            Serial.println(hall_voltage);
//        }
//        
//        for (i = 50; i >= 0; i--){
//            motorPin.duty(i);
//            delay(100);
//            hall_analog = analogRead(A0);
//            hall_voltage = map(hall_analog, 1, 1023, 0, 5);
//            //hall_voltage = sensorValue * (5.0 / 1023.0);
//            Serial.print("Hall voltage is currently: ");
//            Serial.println(hall_voltage);
//        }

          motorPin.duty(65);
          delay(100);
          
          
          
          hall_analog_1 = analogRead(A0);
          hall_voltage_1 = hall_analog_1 * (5.0 / 1023.0);
          
          hall_analog_2 = analogRead(A1);
          hall_voltage_2 = hall_analog_2 * (5.0 / 1023.0);
          
          hall_analog_3 = analogRead(A2);
          hall_voltage_3 = hall_analog_3 * (5.0 / 1023.0);
          
          Serial.print("Hall voltage is currently: ");
          Serial.print(hall_voltage_1);
          Serial.print("          ");
          Serial.print(hall_voltage_2);
          Serial.print("          ");
          Serial.println(hall_voltage_3);
          Serial.println();
          
          motorPin.duty(50);
          delay(2000);
	
}

