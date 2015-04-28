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

#include <U8glib.h>
#include "Arduino.h"
#include "LCD_Controller.h"

#define MENU_INDENT 8
#define MENU_HEIGHT 9

//------------------------------ 'BOOL' FOR INTERRUPTS ------------------------------\\

static int bck_butt_trig = 0;
static int js_trig_x = 0;
static int js_trig_y = 0;
static int fwd_butt_trig = 0;
//Initialize the LCD Display
// pin 21 - Serial clock out (SCLK)
// pin 18 - Serial data out (SID)
// pin 28 - LCD chip select (CS)
// pin 32 - Data/Command select (RS or A0)
// pin 30 - LCD reset (RST)
static U8GLIB_LM6059_2X u8g(21, 18, 28, 32, 30);


void initialize_LCD(){

	//Initialize Colored Backlight
	pinMode(BACKLIGHT_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);

	//Set Background Color to default selection
	set_background_color(color_set);

  next_menu_page = 1;
  parent_menu_page = 1;
  cur_selection = HOME_BASE;
  cur_menu_page = 1;
  cur_sel_min = HOME_BASE;
  cur_sel_max = HOME_MAX;
  next_move = -1;
}

void LCD_movement_handler(){
  
  //joystick_handler();
  //fwd_butt_handler();
  //bck_butt_handler();
  if(next_move > 0){
    handle_select( cur_selection );

  }else if (next_move == 0){
    cur_menu_page = parent_menu_page;
    parent_menu_page = 1;
    cur_selection = HOME_BASE; 
    cur_sel_min = HOME_BASE;
    cur_sel_max = HOME_MAX;
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

void draw_cursor(int cur_menu_index){
  switch( cursor_set ){
            case 2: u8g.drawStr(0, cur_menu_index*MENU_HEIGHT, ">"); break;
            case 3: u8g.drawBitmapP(0, cur_menu_index*MENU_HEIGHT, 1,  MENU_HEIGHT, paw_bitmap); break;
        }
}

void draw_home( int cur_menu_index ){
  draw_cursor(cur_menu_index + 1);
	//u8g.drawStr(0, MENU_HEIGHT*(cur_menu_index+1), ">");
	u8g.drawStr(0, 0, "HOME SCREEN");
	u8g.drawStr(0, MENU_HEIGHT, "SELECT A MENU");
        u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS LOCK/UNLOCK");
        u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SYSTEM INFO");
        u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "SETTINGS");
}


void draw_sys(){
	u8g.drawStr(MENU_INDENT, 0, "SYSTEM INFO");
	u8g.drawStr(MENU_INDENT, MENU_HEIGHT, "BATTERY LEFT: "); //needs value (ie 50%)
	u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "X STATUS: "); //value (ie LOCKED, 90 DEG, or UNLOCKED)
	u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Y STATUS: "); //value
	u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Z STATUS: "); //value 
	u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "BCK TO EXIT"); //value 
}

// Yanked to do main screen for axis locking and unlocking because
// reset/adjust have been eliminated
void draw_axis_select( int cur_menu_index ){
  draw_cursor(cur_menu_index - 18);
	u8g.drawStr(0, 0, "AXIS UNLOCK/LOCK");
  u8g.drawStr(0, MENU_HEIGHT, "NOTE: Lock is Stabilized");

  if(X_control_en){ //true = stabilized = lock
    u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "Unlock X Axis (Pan)");
  }else{
    u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "Lock X Axis (Pan)");
  }


  if(Y_control_en){ //true = stabilized = lock
    u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Unlock Y Axis (Tilt)");
  }else{
    u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Lock Y Axis (Tilt)");
  }


  if(Z_control_en){ //true = stabilized = lock
    u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Unlock Z Axis (Pitch)");
  }else{
    u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Lock Z Axis (Pitch)");
  }
}

void draw_settings( int cur_menu_index ){
    draw_cursor( cur_menu_index - 77);
	  u8g.drawStr(0, MENU_HEIGHT, "SETTINGS HOLDER");
    u8g.drawStr(0, 2*MENU_HEIGHT, "Save Each Change with FWD");
    u8g.drawStr(0, 3*MENU_HEIGHT, "Use L/R to adjust");
    
    //Determine which color option to display.
    if ( cur_selection == 80 ){
        switch( (set_selection % 5) + color_set ){
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Blue   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Green  ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Red    ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Purple ]"); break;
            case 6: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Off    ]"); break;

        }
    }else{
        switch( color_set ){
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Blue   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Green  ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Red    ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Purple ]"); break;
            case 6: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Display Color [ Off    ]"); break;

        }
    }

    //Determine which cursor option to display.
    if ( cur_selection == 81 ){
        switch( (set_selection % 2) + cursor_set ){
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor        [    >   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor        [        ]");
                    u8g.drawBitmapP( 3*MENU_INDENT, 5*MENU_HEIGHT, 1, MENU_HEIGHT, paw_bitmap);
                    break;
        }
    }else{
        switch( cursor_set ){
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor        [    >   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor        [        ]"); 
                    u8g.drawBitmapP( 3*MENU_INDENT, 5*MENU_HEIGHT, 1, MENU_HEIGHT, paw_bitmap);
                    break;
        }
    }

}


void draw(){
  u8g_prepare();
  switch( cur_menu_page ) {
    case 1: draw_home(cur_selection); break;
            
    case 2: draw_sys(); break;
    
    case 3: draw_settings(cur_selection); break;

    case 10: draw_axis_select(cur_selection); break;          
    
  }

}


void bck_butt_handler(){
  int bck = analogRead(BCK_BUTT);

  if( 0 == bck_butt_trig && bck >= 1000 ){
    bck_butt_trig = 1;
    next_move = 0;
    LCD_movement_handler();
  }else if( bck_butt_trig && bck <= 1000 ){
    bck_butt_trig = 0;
  }
}

void fwd_butt_handler(){
  int fwd = analogRead(FWD_BUTT);
  
  if(fwd_butt_trig == 0 && fwd>= 1000){
    fwd_butt_trig = 1;
    next_move = cur_selection;
    LCD_movement_handler();
  }else if( fwd_butt_trig && fwd <= 1000){
    fwd_butt_trig = 0;
  }
}




void joystick_handler(){
    //Read the two potentiometer values for the joystick
    int x = analogRead(JS_X);
    int y = analogRead(JS_Y);
    
    //Determine which direction was selected.
    //Left and right are only enabled on apropriate screens,
    //like Settings or Axis Adjust screens. 
  if ( 0 == js_trig_x && (x >= JS_RIGHT || x <= JS_LEFT) ){
      if( enable_side_scroll && x >= JS_RIGHT ){
           set_selection++;
      }else if( enable_side_scroll && x <= JS_LEFT ){
          set_selection--;
      }

      LCD_movement_handler();
  }else if( js_trig_x && (x <= JS_RIGHT || x >= JS_LEFT) ){
      js_trig_x = 0;
  }

  if ( 0 == js_trig_y && (y >= JS_UP || y<= JS_DOWN) ){
    if( y >= JS_UP ){
        if(cur_selection - 1 < cur_sel_min){
            cur_selection = cur_sel_max;
        }else{
            cur_selection = cur_selection - 1;
        }

    }else if( y <= JS_DOWN ){
        if(cur_selection+1 > cur_sel_max){
            cur_selection = cur_sel_min;
        }else{
            cur_selection = cur_selection + 1;
        }
    }

    LCD_movement_handler();

  }else if( js_trig_y && (y <= JS_UP || y >= JS_DOWN) ){
      js_trig_y = 0;
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
                set_selection = 1;
                break;

//-------------------- X AXIS --------------------------//
        // Unlock Axis - Return to parent page
        case 20: next_menu_page = 0;  
                 unlock_axis(1); //X-Axis Unlock
                 break;

//-------------------- Y AXIS --------------------------//
        //Y-Axis Unlock
        case 21: next_menu_page = 0; 
                 unlock_axis(2);
                 break;
//-------------------- Z AXIS --------------------------//
        //Z-Axis Unlock 
        case 22: next_menu_page = 0;  
                 unlock_axis(3);
                 break;

//-----------------UI SETTINGS --------------------------//       
        case 80: set_background_color((set_selection % 5) + color_set);
                 set_selection = 1;
                 break;

        case 81: cursor_set = (set_selection % 2) + color_set;
                 set_selection = 1;
                 break;
    }
}


void set_background_color( int color ){
  //-- Set the opposite of what you want HIGH 
  //--- (ie. if you want green set blue and red high, and green low.
    switch( color ){
      //--------------------  BLUE  --------------------------//
      case 2: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, LOW); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
      //-------------------- GREEN --------------------------//
      case 3: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, LOW); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
      //--------------------  RED  --------------------------//
      case 4: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, LOW); //Red
              break;
      //------------------- PURPLE -------------------------//
      case 5: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, LOW); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, LOW); //Red
              break;
      //-------------------- OFF --------------------------//
      case 6: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
    }
}

// CONTROL FUNCTIONS
void unlock_axis( int axis ){
	    switch(axis){
        case 1: X_control_en = !X_control_en;
                break;
        case 2: Y_control_en = !Y_control_en;
                break;
        case 3: Z_control_en = !Z_control_en;
                break;
    }
}



