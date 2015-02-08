// Initial PWM test code.
// This code uses analog write, more specialized code may be needed due to interrupt usage.  
// Frequency and pin may need adjustment as well.
// Mitchell Cook

// initialize pwm pin, using digital write this can be any digital out pin
int PWM_output_pin = 11;


void setup(){
	Serial.begin(115200);
	Serial.print("Initializing PWM output on pin: ");
	Serial.println(PWM_output_pin);
	
	pinMode(PWM_output_pin, OUTPUT);
}

void loop(){
	
	Serial.println("Running at 0% duty cycle, full speed reverse");
	analogWrite(PWM_output_pin, 0);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 25% duty cycle, half speed reverse");
	analogWrite(PWM_output_pin, 64);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 50% duty cycle, idle");
	analogWrite(PWM_output_pin, 127);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 75% duty cycle, half speed forward");
	analogWrite(PWM_output_pin, 191);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 100% duty cycle, full speed forward");
	analogWrite(PWM_output_pin, 255);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 75% duty cycle, half speed forward");
	analogWrite(PWM_output_pin, 191);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 50% duty cycle, idle");
	analogWrite(PWM_output_pin, 127);
	delay(5000); // 5 seconds
	
	Serial.println("Running at 25% duty cycle, half speed reverse");
	analogWrite(PWM_output_pin, 64);
	delay(5000); // 5 seconds
	
}