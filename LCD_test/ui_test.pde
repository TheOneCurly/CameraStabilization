#include "ST7565.h"

int ledPin =  13;    // LED connected to digital pin 13

// the LCD backlight is connected up to a pin so you can turn it on & off
#define BACKLIGHT_LED 10

// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// a bitmap of a 16x16 fruit icon
const static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00, 
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };

// The setup() method runs once, when the sketch starts
void setup()   {                
  Serial.begin(9600);
  
  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  // initialize and set the contrast to 0x18
  glcd.begin(0x18);

  glcd.display(); // show splashscreen
  delay(2000);
  glcd.clear();

  // draw a single pixel
  glcd.setpixel(10, 10, BLACK);
  glcd.display();        // show the changes to the buffer
  delay(2000);
  glcd.clear();

  // draw a black circle, 10 pixel radius, at location (32,32)
  glcd.fillcircle(32, 32, 10, BLACK);
  glcd.display();
  delay(2000);
  glcd.clear();

  // draw a string at location (0,0)
  // draw params = indent, line
  glcd.drawstring(0, 0, "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation");
  glcd.display();
  delay(2000);
  glcd.clear();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}

void draw_home(){
	glcd.clear();
	//draw stuff
	// HOME SCREEN
	// SELECT A MENU:
	// AXIS CONTROL
	// SYSTEM INFO
	glcd.display();
}

void draw_sys(){
	glcd.clear();
	//draw stuff
	// SYSTEM INFO
	// BATTERY LEFT : 50%
	// X STATUS: LOCKED 90 DEG.
	// Y STATUS: UNLOCKED
	// Z STATUS: UNLOCKED
	// IMU (running out of space here? resolution?)
	glcd.display();
}

void draw_axis_select(){
	glcd.clear();
	//draw stuff
	// AXIS CONTROL
	// SELECT AN AXIS:
	// X (ypr)
	// Y (ypr)
	// Z (ypr)
	glcd.display();
}

void draw_axis_options(){
	glcd.clear();
	//draw stuff
	// AXIS MENU
	// SELECT AN OPTION:
	// UNLOCK AXIS
	// RESET AXIS TO NEUTRAL
	// ADJUST AXIS AND LOCK
	glcd.display();
}

void draw_axis_control(){
	glcd.clear();
	//draw stuff
	// ADJUST/LOCK MENU
	// USE CONTROL STICK
	// TO ADJUST <NAME> MOTOR
	// FWD TO ACCEPT CHANGE
	// BCK TO REVERT CHANGE
	// <SHOW ANGLE> DEG.
	glcd.display();
}

}
void loop()                     
{
	//while not button pressed or stick used
	//loop?
	
	//if stick used
	//--take up/down input, adjust menu
	//--redraw
	
	//if button pressed
	//--find forward or back
	//--find menu option
	//--find any other input?
	//--call redraw with inputs, redraw screen
	
	// track array of fn pointers to draw menus
	//       current menu location
	//       button statuses
}