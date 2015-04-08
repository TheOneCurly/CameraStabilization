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



#include "LCD_Controller.h"

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


void draw_axis_options( int cur_menu_index, char[] header ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
	u8g.drawStr(MENU_INDENT, 0, header); // value for current selected axis
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


void draw(){
	  u8g_prepare();
  switch(draw_state >> 3) {
    case 1: draw_home(cur_menu_index); 
            cur_sel_min = HOME_BASE;
            cur_sel_max = HOME_MAX;
            break;
    case 2: draw_sys(cur_menu_index); 
            cur_sel_min = SYS_BASE;
            cur_sel_max = SYS_MAX;
            break;

    case 3: draw_axis_control(); 
            cur_sel_min = SYS_BASE;
            cur_sel_max = SYS_MAX;
            break;
    case 4: draw_axis_select(cur_menu_index);
            cur_sel_min = SYS_BASE;
            cur_sel_max = SYS_MAX;
            break;
    case 5: draw_axis_options(cur_menu_index, menu_header); 
            break;
  }

}


void handle_select( int command ){
	switch(cur_menu_index){
        case 1: next_menu_page = 10; 
                parent_menu_page = 1;  
                break; // select Axis Selection Screen
        case 2: next_menu_page = 2;  
                parent_menu_page = 1;  
                break;  // select System Info Screen
        case 3: next_menu_page = 3;  
                parent_menu_page = 1;  
                break;
        
        // Case 4 - 19 free for future additions

        case 20: next_menu_page = 11;
                 cur_sel_min = X_CTRL_BASE;
                 cur_sel_max = X_CTRL_MAX;
                 menu_header = X_HEADER;
                 parent_menu_page = 10;
                 break; 

        case 21: next_menu_page = 13; 
                 cur_sel_min = Y_CTRL_BASE;
                 cur_sel_max = Y_CTRL_MAX;
                 menu_header = Y_HEADER;
                 parent_menu_page = 10;
                 break;

        case 22: next_menu_page = 15;
                 cur_sel_min = Z_CTRL_BASE;
                 cur_sel_max = Z_CTRL_MAX;
                 menu_header = Z_HEADER;
                 parent_menu_page = 10;
                 break;

        //case 23-29 free for future additions

       
        case 30: next_menu_page = 0;  
                 unlock_axis(1); //X-Axis Unlock 
                 break;
        case 31: next_menu_page = 0;
                 reset_axis(1); // X- Axis Reset 
                 break;
        case 32: next_menu_page = 12; 
                 parent_menu_page = 11;
                 break;    //X-Axis Adjust Nav Screen
        case 39: next_menu_page = 11; //return to X Axis Control display

                 break;    // X-Axis Adjust Handler



        case 40: next_menu_page = 0; 
                 unlock_axis(2); //Y-Axis Unlock
                 break;
        case 41: next_menu_page = 0;
                 reset_axis(2); //Y-Axis Reset
                 break;
        case 42: next_menu_page = 14; 
                 break;    //Y-Axis Adjust Nav Screen
        case 49: next_menu_page = 13; 
                 break;    // Y-Axis Adjust Handler



        case 50: next_menu_page = 0;  
                 unlock_axis(3); //Z-Axis Unlock 
                 break;
        case 51: next_menu_page = 0;  
                 reset_axis(3); //Z-Axis Reset
                 break;
        case 52: next_menu_page = 16; 
                 break;    //Z-Axis Adjust Nav Screen
        case 59: next_menu_page = 16; break;    // Z-Axis Adjust Handler

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
