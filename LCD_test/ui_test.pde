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

void draw_menu(){
	//based on current menu, current position, button pressed
	//call to function pointer to one of the menu fns
}

//things to pass to these - current menu option, axis for axis specific menus
void draw_home(){
	glcd.clear();
	//draw stuff
	//need highlighting/coloration to show current menu option
	glcd.drawstring(0, 0, "HOME SCREEN");
	glcd.drawstring(0, 1, "SELECT A MENU");
	glcd.drawstring(0, 2, "AXIS CONTROL");
	glcd.drawstring(0, 3, "SYSTEM INFO");
	glcd.display();
}

void draw_sys(){
	glcd.clear();
	//draw stuff
	//need highlighting/coloration to show current menu option
	glcd.drawstring(0, 0, "SYSTEM INFO");
	glcd.drawstring(1, 0, "BATTERY LEFT"); //needs value (ie 50%)
	glcd.drawstring(2, 0, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	glcd.drawstring(3, 0, "Y STATUS: "); //value
	glcd.drawstring(4, 0, "Z STATUS: "); //value 
	// IMU (running out of space here? resolution?)
	glcd.display();
}

void draw_axis_select(){
	glcd.clear();
	//draw stuff
	//need highlighting/coloration to show current menu option
	glcd.drawstring(0, 0, "AXIS SELECTION");
	glcd.drawstring(0, 1, "SELECT AN AXIS");
	glcd.drawstring(0, 2, "X ())"); //ypr value/reference
	glcd.drawstring(0, 3, "Y ())"); //ypr value/reference
	glcd.drawstring(0, 4, "Z ())"); //ypr value/reference
	glcd.display();
}

void draw_axis_options(){
	glcd.clear();
	//draw stuff
	//need highlighting/coloration to show current menu option
	glcd.drawstring(0, 0, "AXIS OPTIONS");
	glcd.drawstring(0, 1, "SELECT AN OPTION:");
	glcd.drawstring(0, 2, "UNLOCK AXIS");
	glcd.drawstring(0, 3, "RESET AXIS TO NEUTRAL");
	glcd.drawstring(0, 4, "ADJUST AXIS AND LOCK");
	glcd.display();
}

void draw_axis_control(){
	glcd.clear();
	//draw stuff
	//need highlighting/coloration to show current menu option
	glcd.drawstring(0, 0, "AXIS/LOCK MENU");
	glcd.drawstring(0, 1, "USE CONTROL STICK");
	glcd.drawstring(0, 2, "TO ADJUST <NAME> MOTOR"); //needs motor name/axis
	glcd.drawstring(0, 3, "RESET AXIS TO NEUTRAL");
	glcd.drawstring(0, 4, "ADJUST AXIS AND LOCK");
	glcd.drawstring(0, 5, "FWD TO ACCEPT CHANGE");
	glcd.drawstring(0, 6, "BCK TO REVERT CHANGE");
	glcd.drawstring(0, 7, "<ANGLE> DEG."); //show angle 
	glcd.display();
}

}
void loop()                     
{
	// notes: screen size is about 8 lines x 21 characters
	
	//while not button pressed or stick used
	//loop?
	
	//if stick used - acts as pot, analog read
	//--take up/down input, adjust menu
	//--redraw
	
	//if button pressed - analog read button
	//--find forward or back
	//--find menu option
	//--find any other input?
	//--call redraw with inputs, redraw screen
	
	// track array of fn pointers to draw menus
	//       current menu location
	//       button statuses
}