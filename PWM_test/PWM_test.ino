// Initial PWM test code.
// This code uses digital write, more specialized code may be needed due to interrupt usage.  
// Frequency and pin may need adjustment as well.
// Mitchell Cook

// initialize pwm pin, using digital write this can be any digital out pin
int PWM_output_pin = 11;

// this should be the length of a full duty cycle.  1khz = 1000?
int FULL_DUTY_CYCLE = 1000;

void setup(){
	Serial.begin(115200);
	Serial.print("Initializing PWM output on pin: ");
	Serial.println(PWM_output_pin);
	
	pinMode(PWM_output_pin, OUTPUT);
}

// write pwm to the initialized output pin
// stays high for the percentage specified
// writes one single pwm cycle
void sendSinglePWMCycle( int duty_cycle_percentage){
	int input_duty_cycle = FULL_DUTY_CYCLE * duty_cycle_percentage/100;
	
	digitalWrite(PWM_output_pin, HIGH);
	delay(input_duty_cycle);
	digitalWrite(PWM_output_pin, LOW);
	delay(FULL_DUTY_CYCLE - input_duty_cycle);

}

// send x pwm cycles and print to serial io
void sendMultiplePWMCycles( int num_cycles, int duty_cycle_percentage){
	Serial.print("Running at ");
	Serial.print(duty_cycle_percentage);
	Serial.print("% duty cycle for ");
	Serial.print(num_cycles);
	Serial.println(" cycles.");
	
	for( int x1 = 0; x1 < num_cycles; x1++ ){
		sendPWMCycle(duty_cycle_percentage);
	}
}

void loop(){
	int num_cycles = 20;
	
	// 0% duty cycle, full speed reverse
	sendMultiplePWMCycles(num_cycles, 0);
	
	// 25% duty cycle, half speed reverse
	sendMultiplePWMCycles(num_cycles, 25);

	// 50% duty cycle, no movement
	sendMultiplePWMCycles(num_cycles, 50);

	// 75% duty cycle, half speed forward
	sendMultiplePWMCycles(num_cycles, 75);
	
	// 100% duty cycle, full speed forward
	sendMultiplePWMCycles(num_cycles, 100);
	
	// 75% duty cycle, half speed forward
	sendMultiplePWMCycles(num_cycles, 75);
	
	// 50% duty cycle, no movement
	sendMultiplePWMCycles(num_cycles, 50);
	
	// 25% duty cycle, half speed reverse
	sendMultiplePWMCycles(num_cycles, 25);

}