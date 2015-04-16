 /*
 *
 *	LCD_Controller.cpp
 *
 *	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *	Creation Date: 14 March 2015
 *	Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *	Description: Funtionality for the LCD controller for a 3-axis camera stabilization system 
 *				 menu displayed on an ST7565 LCD (128x64) with RGB backlight. Includes full 
 *				 functionality for menu navigation, axis manipulation, and system configuration.
 *
 */


#include "Arduino.h"
#include "LCD_Controller.h"

#define MENU_INDENT 8
#define MENU_HEIGHT 9

#define JS_X A3
#define JS_Y A4
#define FWD_BUTT A5
#define BCK_BUTT A6

//Initialize the LCD Display
// pin 21 - Serial clock out (SCLK)
// pin 18 - Serial data out (SID)
// pin 46 - LCD chip select (CS)
// pin 50 - Data/Command select (RS or A0)
// pin 48 - LCD reset (RST)
static U8GLIB_LM6059_2X u8g(21, 18, 46, 50, 48);


void initialize_LCD(){

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

        next_menu_page = 1;
        draw(next_menu_page);
}


void toggle_interrupt_handler(){

}


void button_interrupt_handler(){

}

//TODO: check to see if this is simply repeated functionality.
void respond_to_action(){
	//draw a menu
    // OR perform some action (lock, unlock, reset)
	//based on orientation and inputs

}


void u8g_prepare(){
	u8g.setFont(u8g_font_6x10);
	u8g.setFontRefHeightExtendedText();
	u8g.setDefaultForegroundColor();
	u8g.setFontPosTop();
}


void draw_home( int cur_menu_index ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "HOME SCREEN");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT A MENU");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS CONTROL");
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SYSTEM INFO");
}


void draw_sys( int cur_menu_index ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "SYSTEM INFO");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "BATTERY LEFT: "); //needs value (ie 50%)
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y STATUS: "); //value
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z STATUS: "); //value 
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO EXIT"); //value 
	// IMU (running out of space here? resolution?)
}


void draw_axis_select( int cur_menu_index ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, "AXIS SELECTION");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT AN AXIS");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X (PAN))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y (TILT))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z (PITCH))"); //ypr value/reference - fix this
}


void draw_axis_options( int cur_menu_index, int axis ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
        switch(axis){
          case 1: u8g.drawStr(MENU_INDENT, 0, "X AXIS OPTIONS"); break;
          case 2: u8g.drawStr(MENU_INDENT, 0, "Y AXIS OPTIONS"); break;
          case 3: u8g.drawStr(MENU_INDENT, 0, "Z AXIS OPTIONS"); break;
        }
	//u8g.drawStr(MENU_INDENT, 0, header); // value for current selected axis
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SELECT AN OPTION:");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "UNLOCK AXIS");
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "RESET TO NEUTRAL");
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "ADJUST AND LOCK");
}


void draw_axis_control(int axis){
        switch(axis){
          case 1: u8g.drawStr(MENU_INDENT, 0, "X AXIS ADJUST"); Serial.println("Print X Adjust"); break;
          case 2: u8g.drawStr(MENU_INDENT, 0, "Y AXIS ADJUST"); Serial.println("Print Y Adjust"); break;
          case 3: u8g.drawStr(MENU_INDENT, 0, "Z AXIS ADJUST"); Serial.println("Print Z Adjust"); break;
        }
	//u8g.drawStr(MENU_INDENT, 0, header); // value of current selected axis
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "USE CONTROL STICK TO");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "ADJUST MOTOR"); //needs motor name/axis
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "FWD TO ACCEPT");
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO CANCEL");
	u8g.drawStr(MENU_INDENT, 6*MENU_HEIGHT, "<ANGLE> DEG."); //show angle 

}


void joystick_handler(){
    int x = analogRead(JS_X);
    int y = analogRead(JS_Y);
    int fwd = analogRead(FWD_BUTT);
    int bck = analogRead(BCK_BUTT);
    //Serial.println(fwd);
    //Serial.println(y);
  
    if(fwd >= 1020){
      Serial.println("select value:");
      Serial.println(cur_selection);
      handle_select( cur_selection );
      draw(next_menu_page);
    }else if(bck >= 1020){
      Serial.println("select value:");
      Serial.println(cur_selection);
      handle_select( cur_selection );
      draw(parent_menu_page);
    
    }else{
      if( x >= JS_RIGHT ){
          Serial.println("Right");
      }else if( x <= JS_LEFT ){
          Serial.println("Left");
      }else if( y >= JS_UP ){
          if(cur_selection - 1 < cur_sel_min){
              cur_selection = cur_sel_max;
          }else{
              cur_selection = cur_selection - 1;
          }
          Serial.println(cur_selection);
  
      }else if( y <= JS_DOWN ){
          if(cur_selection+1 > cur_sel_max){
              cur_selection = cur_sel_min;
          }else{
              cur_selection = cur_selection + 1;
          }
          Serial.println(cur_selection);
      }
    }
}

void draw(int next_page){
	  u8g_prepare();
  switch(next_page) {
    case 1: draw_home(cur_menu_index);
            Serial.println("Print Home");
            cur_selection = HOME_BASE; 
            cur_sel_min = HOME_BASE;
            cur_sel_max = HOME_MAX;
            break;
    case 2: draw_sys(cur_menu_index);
            Serial.println("Print SYS");
            cur_selection = SYS_BASE; 
            cur_sel_min = SYS_BASE;
            cur_sel_max = SYS_MAX;
            break;


    case 10: draw_axis_select(cur_menu_index);
            Serial.println("Print Axis Select");
            cur_selection = AXIS_SEL_BASE; 
            cur_sel_min = AXIS_SEL_BASE;
            cur_sel_max = AXIS_SEL_MAX;
            break;
            
   case 11: draw_axis_control(cur_axis);
            Serial.println("Print X Axis Control");
            cur_selection = X_CTRL_BASE;
            cur_sel_min = X_CTRL_BASE;
            cur_sel_max = X_CTRL_MAX;
            break;

   case 12: draw_axis_options(cur_menu_index, cur_axis);
            Serial.println("Print X axis adjust");
            cur_selection = X_ADJUST;
            cur_sel_min = X_ADJUST;
            cur_sel_max = X_ADJUST;
            break;


   case 13: draw_axis_control(cur_axis);
            Serial.println("Print Y Axis Control");
            cur_selection = Y_CTRL_BASE;
            cur_sel_min = Y_CTRL_BASE;
            cur_sel_max = Y_CTRL_MAX;
            break;

   case 14: draw_axis_options(cur_menu_index, cur_axis);
            Serial.println("Print Y axis adjust");
            cur_selection = Y_ADJUST;
            cur_sel_min = Y_ADJUST;
            cur_sel_max = Y_ADJUST;
            break;
            
            
   case 15: draw_axis_control(cur_axis);
          Serial.println("Print Z Axis Control");
          cur_selection = Z_CTRL_BASE;
          cur_sel_min = Z_CTRL_BASE;
          cur_sel_max = Z_CTRL_MAX;
          break;
          
    case 16: draw_axis_options(cur_menu_index, cur_axis);
            Serial.println("Print Z axis adjust");
            cur_selection = Z_ADJUST;
            cur_sel_min = Z_ADJUST;
            cur_sel_max = Z_ADJUST;
            break;
  }

}


void handle_select( int command ){
	switch(command){

        // select Axis Selection Screen
        case 1: next_menu_page = 10; 
                parent_menu_page = 1;
                //cur_selection = 20;
               Serial.println("Axis Selection Screen"); 
                break; 

        // select System Info Screen
        case 2: next_menu_page = 2;  
                parent_menu_page = 1;
                //cur_selection = 0; 
                Serial.println("Sys Info Screen");
                break;  

        // select Settings Screen
        case 3: next_menu_page = 3;  
                parent_menu_page = 1;
                //cur_selection = 80;
                Serial.println("Settings Screen");
                break;
        
        // Case 4 - 19 free for future additions

        // select X Axis Control Screen
        case 20: next_menu_page = 11;
                 //cur_selection = X_CTRL_BASE;
                 //cur_sel_min = X_CTRL_BASE;
                 //cur_sel_max = X_CTRL_MAX;
                 cur_axis = 1;
                 //menu_header = X_HEADER;
                 parent_menu_page = 10;
                 Serial.println("X Axis CTRL Screen");
                 break; 

        // select Y Axis Control Screen
        case 21: next_menu_page = 13;
                 //cur_selection = Y_CTRL_BASE;
                 //cur_sel_min = Y_CTRL_BASE;
                 //cur_sel_max = Y_CTRL_MAX;
                 cur_axis = 2;
                 //menu_header = Y_HEADER;
                 parent_menu_page = 10;
                 Serial.println("Y Axis CTRL Screen");
                 break;

        // select Z Axis Control Screen
        case 22: next_menu_page = 15;
                 //cur_selection = Z_CTRL_BASE;
                // cur_sel_min = Z_CTRL_BASE;
                // cur_sel_max = Z_CTRL_MAX;
                 cur_axis = 3;
                 //menu_header = Z_HEADER;
                 parent_menu_page = 10;
                 Serial.println("Z Axis CTRL Screen");
                 break;

        //case 23-29 free for future additions

//-------------------- X AXIS --------------------------//
        // Unlock Axis - Return to parent page
        case 30: next_menu_page = 0;  
                 unlock_axis(1); //X-Axis Unlock
                 Serial.println("X Axis UNLOCK");
                 break;

        // Reset Axis - Return to parent page
        case 31: next_menu_page = 0;
                 reset_axis(1); // X- Axis Reset
                 Serial.println("X Axis RESET");
                 break;

        // select Adjust X Axis Nav Screen
        case 32: next_menu_page = 12; 
                 parent_menu_page = 11;
                 Serial.println("X Axis ADJUST Screen");
                 break;

        //return to X Axis Control display
        case 39: next_menu_page = 11; 
                 adjust_axis(1, adjust_val);
                 Serial.println("X Axis ADJUST CMD");
                 break;    // X-Axis Adjust Handler

//-------------------- Y AXIS --------------------------//
        //Y-Axis Unlock
        case 40: next_menu_page = 0; 
                 unlock_axis(2);
                 Serial.println("Y Axis unlock");
                 break;

        //Y-Axis Reset
        case 41: next_menu_page = 0;
                 reset_axis(2);
                 Serial.println("Y Axis reset");
                 break;

        //Y-Axis Adjust Nav Screen"
        case 42: next_menu_page = 14;
                 parent_menu_page = 13;
                 Serial.println("Y Axis adjust screen");
                 break;

        // Y-Axis Adjust Handler
        case 49: next_menu_page = 13; 
                 adjust_axis(2, adjust_val);
                 Serial.println("Y Axis adjust cmd");
                 break;    

//-------------------- Z AXIS --------------------------//
        //Z-Axis Unlock 
        case 50: next_menu_page = 0;  
                 unlock_axis(3);
                Serial.println("Z Axis unlock"); 
                 break;

        //Z-Axis Reset
        case 51: next_menu_page = 0;  
                 reset_axis(3);
                 Serial.println("Z Axis reset"); 
                 break;

        //Z-Axis Adjust Nav Screen
        case 52: next_menu_page = 16;
                 parent_menu_page = 15;
                 Serial.println("Z Axis adjust screen"); 
                 //cur_selection = 
                 break;    
        // Z-Axis Adjust Handler
        case 59: next_menu_page = 16; 
                 adjust_axis(3, adjust_val);
                 Serial.println("Z Axis adjust cmd"); 
                 break;    

        // Case 80 - 100 will be used for adjusting settings. 

    }
}


// CONTROL FUNCTIONS
void unlock_axis( int axis ){
	    switch(axis){
        case 1: //unlock X axis
                break;
        case 2: //unlock Y axis
                break;
        case 3: //unlock Z axis
                break;
    }
}


void reset_axis( int axis ){
    switch(axis){
        case 1: //reset X axis
                break;
        case 2: //reset Y axis
                break;
        case 3: //reset Z axis
                break;
    }
}

void adjust_axis( int axis, int adjust ){
    switch(axis){
        case 1: //adjust X axis
                break;
        case 2: //adjust Y axis
                break;
        case 3: //adjust Z axis
                break;
    }
}

void set_background_color(int color){
    switch(color){
    	case 0: //no backlight
    			break;
        case 1: //blue
                break;
        case 2: //green
                break;
        case 3: //red
                break;
        case 4: //orange
        		break;
        case 5: //purple
        		break;
    }
}


