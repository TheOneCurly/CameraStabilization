#include <U8glib.h>
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

// Define Joystick Pins
#define JS_X A3
#define JS_Y A4

//Used to determine which page should be drawn
uint8_t draw_state = 0;
uint8_t button_toggle = 0;

int cur_menu_page, cur_menu_index = 0;

#define MENU_INDENT 8
#define MENU_HEIGHT 9

// The setup() method runs once, when the sketch starts
void setup()   {      
  Serial.begin(115200);

  //Initialize Colored Backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  //Set Background Color 
  //-- Set the opposite of what you want HIGH 
  //--- (ie. if you want green set blue and red high, and green low.
  digitalWrite(BACKLIGHT_LED, HIGH);
  digitalWrite(BLUE_LED, LOW); //Blue
  digitalWrite(GREEN_LED, LOW); //Green
  digitalWrite(RED_LED, HIGH); //Red
}

void respond_to_action(){
	//draw a menu
    // OR perform some action (lock, unlock, reset)
	//based on orientation and inputs
}

void u8g_prepare(void) {
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

//things to pass to these - current menu option, axis for axis specific menus
void draw_home(int cur_menu_index){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "HOME SCREEN");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT A MENU");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS CONTROL");
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SYSTEM INFO");
}

void draw_sys(int cur_menu_index){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "SYSTEM INFO");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "BATTERY LEFT: "); //needs value (ie 50%)
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y STATUS: "); //value
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z STATUS: "); //value 
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO EXIT"); //value 
	// IMU (running out of space here? resolution?)
}

void draw_axis_select(int cur_menu_index){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "AXIS SELECTION");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT AN AXIS");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X (YAW))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y (PITCH))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z (ROLL))"); //ypr value/reference - fix this
}

void draw_axis_options(int cur_menu_index){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "AXIS OPTIONS"); // value for current selected axis
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT AN OPTION:");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "UNLOCK AXIS");
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "RESET TO NEUTRAL");
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "ADJUST AND LOCK");
}

void draw_axis_control(){
	u8g.drawStr(MENU_INDENT, 0, "AXIS/LOCK MENU"); // value of current selected axis
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "USE CONTROL STICK TO");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "ADJUST <NAME> MOTOR"); //needs motor name/axis
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "FWD TO ACCEPT");
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO CANCEL");
	u8g.drawStr(MENU_INDENT, 6*MENU_HEIGHT, "<ANGLE> DEG."); //show angle 
}


void draw(void) {
  u8g_prepare();
  switch(draw_state >> 3) {
    case 0: draw_home(cur_menu_index); break;
    case 1: draw_sys(cur_menu_index); break;
    case 2: draw_axis_select(cur_menu_index); break;
    case 3: draw_axis_options(cur_menu_index); break;
    case 4: draw_axis_control(); break;
  }
}


void loop()                     
{  
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  Serial.println(analogRead(JS_X));
  Serial.println(analogRead(JS_Y));
  
  if(analogRead(JS_X) > 600){
    draw_state++;
    cur_menu_index = 0;
    if ( draw_state >= 5*8 )
      draw_state = 0;
  }
  else if(analogRead(JS_X) < 400){
    draw_state--;
    cur_menu_index = 0;
    if ( draw_state >= 5*8 )
      draw_state = 0;
  }
  else if(analogRead(JS_Y) < 400){
    if(cur_menu_index < 5){
      cur_menu_index++;
    }
    delay(200);
  }
  else if(analogRead(JS_Y) > 600){
    if(cur_menu_index > 0){
      cur_menu_index--;
    }
    delay(200);
  }
  else{
    delay(100);
  }
   
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
