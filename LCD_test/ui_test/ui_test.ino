#include "U8glib.h"

int ledPin =  13;    // LED connected to digital pin 13

//Initialize the LCD Display
// pin 21 - Serial clock out (SCLK)
// pin 18 - Serial data out (SID)
// pin 46 - LCD chip select (CS)
// pin 50 - Data/Command select (RS or A0)
// pin 48 - LCD reset (RST)
U8GLIB_LM6059_2X u8g(21, 18, 46, 50, 48);

// Define the backlight LED locations 
// -- All are connected to digital pins so you can turn them off and on
#define BACKLIGHT_LED 52
#define BLUE_LED 51
#define GREEN_LED 49
#define RED_LED 47

//Used to determine which page should be drawn
uint8_t draw_state = 0;
uint8_t button_toggle = 0;

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// a bitmap of a 16x16 fruit icon
const static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x30, 0xf0, 0xf0, 0xf0, 0xf0, 0x30, 0xf8, 0xbe, 0x9f, 0xff, 0xf8, 0xc0, 0xc0, 0xc0, 0x80, 0x00, 
0x20, 0x3c, 0x3f, 0x3f, 0x1f, 0x19, 0x1f, 0x7b, 0xfb, 0xfe, 0xfe, 0x07, 0x07, 0x07, 0x03, 0x00, };

// The setup() method runs once, when the sketch starts
void setup()   {                

  //Initialize Colored Backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  //Set Background Color 
  //-- Set the opposite of what you want HIGH 
  //--- (ie. if you want green set blue and red high, and green low.
  digitalWrite(BACKLIGHT_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH); //Blue
  digitalWrite(GREEN_LED, LOW); //Green
  digitalWrite(RED_LED, HIGH); //Red

//TODO: The remainder of this needs to be adjusted. LCD should already be setup
//      using the object instatiation above.
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

void respond_to_action(){
	//draw a menu
    // OR perform some action (lock, unlock, reset)
	//based on orientation and inputs
}

// Menu indexes
// 0 - home screen
// 1 - system info
// 2 - axis selection - x/y/z
// 3 - axis options   - unlock/lock/reset
// 4 - axis adjust    - +/- deg to set position
void draw_menu(int current_menu, int current_position){
	switch (current_menu){
		case 0:
			draw_home(current_position);
			break;
		case 1:
			draw_sys(current_position);
			break;
		case 2:
			draw_axis_select(current_position);
			break;
		case 3:
			draw_axis_options(current_position);
			break;
		case 4:
			draw_axis_control();
			break;
	}
}

//things to pass to these - current menu option, axis for axis specific menus
void draw_home(int cur_menu_index){
	glcd.clear();
	glcd.drawstring(1, cur_menu_index+2, ">");
	glcd.drawstring(3, 0, "HOME SCREEN");
	glcd.drawstring(3, 1, "SELECT A MENU");
	glcd.drawstring(3, 2, "AXIS CONTROL");
	glcd.drawstring(3, 3, "SYSTEM INFO");
	glcd.display();
}

void draw_sys(int cur_menu_index){
	glcd.clear();
	glcd.drawstring(1, cur_menu_index+2, ">");
	glcd.drawstring(3, 0, "SYSTEM INFO");
	glcd.drawstring(3, 1, "BATTERY LEFT: "); //needs value (ie 50%)
	glcd.drawstring(3, 2, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	glcd.drawstring(3, 3, "Y STATUS: "); //value
	glcd.drawstring(3, 4, "Z STATUS: "); //value 
	glcd.drawstring(3, 5, "BCK TO EXIT"); //value 
	// IMU (running out of space here? resolution?)
	glcd.display();
}

void draw_axis_select(int cur_menu_index){
	glcd.clear();
	glcd.drawstring(1, cur_menu_index+2, ">");
	glcd.drawstring(3, 0, "AXIS SELECTION");
	glcd.drawstring(3, 1, "SELECT AN AXIS");
	glcd.drawstring(3, 2, "X (YAW))"); //ypr value/reference - fix this
	glcd.drawstring(3, 3, "Y (PITCH))"); //ypr value/reference - fix this
	glcd.drawstring(3, 4, "Z (ROLL))"); //ypr value/reference - fix this
	glcd.display();
}

void draw_axis_options(int cur_menu_index){
	glcd.clear();
	glcd.drawstring(1, cur_menu_index+2, ">");
	glcd.drawstring(3, 0, "AXIS OPTIONS"); // value for current selected axis
	glcd.drawstring(3, 1, "SELECT AN OPTION:");
	glcd.drawstring(3, 2, "UNLOCK AXIS");
	glcd.drawstring(3, 3, "RESET AXIS TO NEUTRAL");
	glcd.drawstring(3, 4, "ADJUST AXIS AND LOCK");
	glcd.display();
}

void draw_axis_control(){
	glcd.clear();
	glcd.drawstring(3, 0, "AXIS/LOCK MENU"); // value of current selected axis
	glcd.drawstring(3, 1, "USE CONTROL STICK");
	glcd.drawstring(3, 2, "TO ADJUST <NAME> MOTOR"); //needs motor name/axis
	glcd.drawstring(3, 4, "FWD TO ACCEPT CHANGE");
	glcd.drawstring(3, 5, "BCK TO REVERT CHANGE");
	glcd.drawstring(3, 6, "<ANGLE> DEG."); //show angle 
	glcd.display();
}


void draw(void) {
  u8g_prepare();
  switch(draw_state >> 3) {
    case 0: draw_home(draw_state&7); break;
    case 1: draw_sys(draw_state&7); break;
    case 2: draw_axis_select(draw_state&7); break;
    case 3: draw_axis_options(draw_state&7); break;
    case 4: draw_axis_control(draw_state&7); break;
    //case 5: u8g_triangle(draw_state&7); break;
    //case 6: u8g_ascii_1(); break;
    //case 7: u8g_ascii_2(); break;
    //case 8: u8g_extra_page(draw_state&7); break;
  }
}



void loop()                     
{
  u8g.firstPage();
  do {
     draw();
     delay(500);
  }while( button_toggle );
  
    draw_state++;
    
    
    delay(200);
  
  
	// notes: screen size is about 8 lines x 21 characters
	//        need a way of signifying current axis
	
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
