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
    parent_menu_page = 1;
    cur_selection = HOME_BASE;
    cur_menu_page = 1;
    cur_sel_min = HOME_BASE;
    cur_sel_max = HOME_MAX;
    next_move = -1;
}

void LCD_movement_handler(){
  
  joystick_handler();
  fwd_butt_handler();
  bck_butt_handler();
  if(next_move > 0){
    handle_select( cur_selection );
    //cur_menu_page = next_menu_page;
  }else if (next_move == 0){
    //cur_menu_page = parent_menu_page;
    handle_menu_context( );
  }
  
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  next_move = -1;

}


void u8g_prepare(){
	u8g.setFont(u8g_font_6x10);
	u8g.setFontRefHeightExtendedText();
	u8g.setDefaultForegroundColor();
	u8g.setFontPosTop();
}


void draw_home( int cur_menu_index ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+1), ">");
	u8g.drawStr(0, 0, "HOME SCREEN");
	u8g.drawStr(0, MENU_HEIGHT, "SELECT A MENU");
        u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS CONTROL");
        u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SYSTEM INFO");
        u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "SETTINGS");
}


void draw_sys( int cur_menu_index ){
	u8g.drawStr(MENU_INDENT, 0, "SYSTEM INFO");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "BATTERY LEFT: "); //needs value (ie 50%)
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y STATUS: "); //value
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z STATUS: "); //value 
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO EXIT"); //value 
	// IMU (running out of space here? resolution?)
}


void draw_axis_select( int cur_menu_index ){
	u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index-18), ">");
	u8g.drawStr(0, 0, "AXIS SELECTION");
	u8g.drawStr(0, MENU_HEIGHT, "SELECT AN AXIS");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X (PAN))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y (TILT))"); //ypr value/reference - fix this
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z (PITCH))"); //ypr value/reference - fix this
}


void draw_axis_options( int cur_menu_index, int axis ){
	//u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+2), ">");
        switch(axis){
          case 1: u8g.drawStr(0, 0, "X AXIS OPTIONS"); u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index-28), ">"); break;
          case 2: u8g.drawStr(0, 0, "Y AXIS OPTIONS"); u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index-38), ">"); break;
          case 3: u8g.drawStr(0, 0, "Z AXIS OPTIONS"); u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index-58), ">"); break;
        }
	//u8g.drawStr(MENU_INDENT, 0, header); // value for current selected axis
	u8g.drawStr(0, MENU_HEIGHT, "SELECT AN OPTION:");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "UNLOCK AXIS");
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "RESET TO NEUTRAL");
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "ADJUST AND LOCK");
}


void draw_axis_control(int axis){
        switch(axis){
          case 1: u8g.drawStr(MENU_INDENT, 0, "X AXIS ADJUST"); break;
          case 2: u8g.drawStr(MENU_INDENT, 0, "Y AXIS ADJUST"); break;
          case 3: u8g.drawStr(MENU_INDENT, 0, "Z AXIS ADJUST"); break;
        }
	//u8g.drawStr(MENU_INDENT, 0, header); // value of current selected axis
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "USE CONTROL STICK TO");
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "ADJUST MOTOR"); //needs motor name/axis
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "FWD TO ACCEPT");
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO CANCEL");
	u8g.drawStr(MENU_INDENT, 6*MENU_HEIGHT, "<ANGLE> DEG."); //show angle 

}

void draw_settings(){
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "SETTINGS HOLDER");
}

void fwd_butt_handler(){
    int fwd = analogRead(FWD_BUTT);
   // Serial.println(fwd);
    if(fwd >= 1000){
      next_move = cur_selection;
      Serial.println("FWD");
      while(analogRead(FWD_BUTT) >= 1020);
    }
}


void bck_butt_handler(){
    int bck = analogRead(BCK_BUTT);

    if(bck >= 1000){
      next_move = 0;
      Serial.println("BCK");
      while(analogRead(BCK_BUTT) >= 1020);
    }
}



void joystick_handler(){
    int x = analogRead(JS_X);
    int y = analogRead(JS_Y);
    //int fwd = analogRead(FWD_BUTT);
    //int bck = analogRead(BCK_BUTT);
    //Serial.println(y);
    
    // if(fwd >= 1020){
    //   next_move = cur_selection;
    //   Serial.println("FWD");
    // }else if(bck >= 1020){
    //   next_move = 0;
    //   Serial.println("BCK");
    // }else{
      if( x >= JS_RIGHT ){
         // Serial.println("Right");
      }else if( x <= JS_LEFT ){
          //Serial.println("Left");
      }else if( y >= JS_UP ){
          if(cur_selection - 1 < cur_sel_min){
              cur_selection = cur_sel_max;
          }else{
              cur_selection = cur_selection - 1;
          }
      
        while( analogRead(JS_Y) >= JS_UP );
      }else if( y <= JS_DOWN ){
          if(cur_selection+1 > cur_sel_max){
              cur_selection = cur_sel_min;
          }else{
              cur_selection = cur_selection + 1;
          }
          while( analogRead(JS_Y) <= JS_DOWN );
      }
   // }
    Serial.println(cur_selection);
    
    
}

void draw(){
  u8g_prepare();
  switch( cur_menu_page ) {
    case 1: draw_home(cur_selection); break;
            
    case 2: draw_sys(1); break;
    
    case 3: draw_settings(); break;

    case 10: draw_axis_select(cur_selection); break;
    
    case 11: draw_axis_options(cur_selection, cur_axis); break;
    
    case 12: draw_axis_control(cur_axis); break;

    case 13: draw_axis_options(cur_selection, cur_axis); break;
  
    case 14: draw_axis_control(cur_axis); break;
    
    case 15:  draw_axis_options(cur_selection, cur_axis); break;
        
    case 16: draw_axis_control(cur_axis); break;
          
    
  }

}

void handle_menu_context(){
switch( parent_menu_page ) {
    case 1: cur_menu_page = parent_menu_page;
            parent_menu_page = 1;
            cur_selection = HOME_BASE; 
            cur_sel_min = HOME_BASE;
            cur_sel_max = HOME_MAX;
            break;
            
    case 2: cur_menu_page = parent_menu_page;
            parent_menu_page = 1;
            cur_selection = SYS_BASE; 
            cur_sel_min = SYS_BASE;
            cur_sel_max = SYS_MAX;
            break;


    case 10:cur_menu_page = parent_menu_page;
            parent_menu_page = 1;
            cur_selection = AXIS_SEL_BASE; 
            cur_sel_min = AXIS_SEL_BASE;
            cur_sel_max = AXIS_SEL_MAX;
            break;
            
   case 11: cur_menu_page = parent_menu_page;
            parent_menu_page = 10;
            cur_selection = AXIS_SEL_BASE; 
            cur_sel_min = AXIS_SEL_BASE;
            cur_sel_max = AXIS_SEL_MAX;
            break;

   case 12: cur_menu_page = parent_menu_page;
            parent_menu_page = 11;
            cur_selection = X_CTRL_BASE;
            cur_sel_min = X_CTRL_BASE;
            cur_sel_max = X_CTRL_MAX;
            break;


   case 13: cur_menu_page = parent_menu_page;
            parent_menu_page = 10;
            cur_selection = AXIS_SEL_BASE; 
            cur_sel_min = AXIS_SEL_BASE;
            cur_sel_max = AXIS_SEL_MAX;
            break;

   case 14: cur_menu_page = parent_menu_page;
            parent_menu_page = 13;
            cur_selection = Y_CTRL_BASE;
            cur_sel_min = Y_CTRL_BASE;
            cur_sel_max = Y_CTRL_MAX;
            break;
            
            
   case 15: cur_menu_page = parent_menu_page;
            parent_menu_page = 10;
            cur_selection = AXIS_SEL_BASE; 
            cur_sel_min = AXIS_SEL_BASE;
            cur_sel_max = AXIS_SEL_MAX;
            break;
          
    case 16: cur_menu_page = parent_menu_page;
             parent_menu_page = 15;
             cur_selection = Z_CTRL_BASE;
             cur_sel_min = Z_CTRL_BASE;
             cur_sel_max = Z_CTRL_MAX;
             break;
  }

}



void handle_select( int next_move ){
	switch(next_move){

        // select Axis Selection Screen
        case 1: //next_menu_page = 10;
                cur_menu_page = 10;
                parent_menu_page = 1;
                cur_selection = AXIS_SEL_BASE; 
                cur_sel_min = AXIS_SEL_BASE;
                cur_sel_max = AXIS_SEL_MAX;
                break; 

        // select System Info Screen
        case 2: cur_menu_page = 2;  
                parent_menu_page = 1;
                cur_selection = SYS_BASE; 
                cur_sel_min = SYS_BASE;
                cur_sel_max = SYS_MAX;
                break;  

        // select Settings Screen
        case 3: cur_menu_page = 3;  
                parent_menu_page = 1;
                break;
        
        // Case 4 - 19 free for future additions

        // select X Axis Control Screen
        case 20: cur_menu_page = 11;
                 cur_axis = 1;
                 parent_menu_page = 10;
                 cur_selection = X_CTRL_BASE;
                 cur_sel_min = X_CTRL_BASE;
                 cur_sel_max = X_CTRL_MAX;
                 break; 

        // select Y Axis Control Screen
        case 21: cur_menu_page = 13;
                 cur_axis = 2;
                 parent_menu_page = 10;
                 cur_selection = Y_CTRL_BASE;
                 cur_sel_min = Y_CTRL_BASE;
                 cur_sel_max = Y_CTRL_MAX;
                 break;

        // select Z Axis Control Screen
        case 22: cur_menu_page = 15;
                 cur_axis = 3;
                 parent_menu_page = 10;
                 cur_selection = Z_CTRL_BASE;
                 cur_sel_min = Z_CTRL_BASE;
                 cur_sel_max = Z_CTRL_MAX;
                 break;

        //case 23-29 free for future additions

//-------------------- X AXIS --------------------------//
        // Unlock Axis - Return to parent page
        case 30: next_menu_page = 0;  
                 unlock_axis(1); //X-Axis Unlock
                 break;

        // Reset Axis - Return to parent page
        case 31: next_menu_page = 0;
                 reset_axis(1); // X- Axis Reset
                 break;

        // select Adjust X Axis Nav Screen
        case 32: cur_menu_page = 12; 
                 parent_menu_page = 11;
                 cur_selection = X_ADJUST;
                 cur_sel_min = X_ADJUST;
                 cur_sel_max = X_ADJUST;
                 break;

        //return to X Axis Control display
        case 39: next_menu_page = 11; 
                 adjust_axis(1, adjust_val);
                 break;    // X-Axis Adjust Handler

//-------------------- Y AXIS --------------------------//
        //Y-Axis Unlock
        case 40: next_menu_page = 0; 
                 unlock_axis(2);
                 break;

        //Y-Axis Reset
        case 41: next_menu_page = 0;
                 reset_axis(2);
                 break;

        //Y-Axis Adjust Nav Screen"
        case 42: cur_menu_page = 14;
                 parent_menu_page = 13;
                 cur_selection = Y_ADJUST;
                 cur_sel_min = Y_ADJUST;
                 cur_sel_max = Y_ADJUST;
                 break;

        // Y-Axis Adjust Handler
        case 49: next_menu_page = 13; 
                 adjust_axis(2, adjust_val);
                 break;    

//-------------------- Z AXIS --------------------------//
        //Z-Axis Unlock 
        case 50: next_menu_page = 0;  
                 unlock_axis(3);
                 break;

        //Z-Axis Reset
        case 51: next_menu_page = 0;  
                 reset_axis(3);
                 break;

        //Z-Axis Adjust Nav Screen
        case 52: cur_menu_page = 16;
                 parent_menu_page = 15;
                 cur_selection = Z_ADJUST;
                 cur_sel_min = Z_ADJUST;
                 cur_sel_max = Z_ADJUST;
                 break;    
        // Z-Axis Adjust Handler
        case 59: next_menu_page = 16; 
                 adjust_axis(3, adjust_val);
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


