 /*
 *
 *  LCD_Controller.cpp
 *
 *  Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *  Creation Date: 14 March 2015
 *  Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *  Description: Funtionality for the LCD controller for a 3-axis camera stabilization system 
 *               menu displayed on an ST7565 LCD (128x64) with RGB backlight. Includes full 
 *               functionality for menu navigation, axis manipulation, and system configuration.
 *
 */

#include <U8glib.h>
#include "Arduino.h"
#include "LCD_Controller.h"

#define MENU_INDENT 8
#define MENU_HEIGHT 9

//------------------------------ 'BOOL' FOR INTERRUPTS ------------------------------\\

static int fwd_butt_trig = 0;

//Initialize the LCD Display
// pin 21 - Serial clock out (SCLK)
// pin 18 - Serial data out (SID)
// pin 28 - LCD chip select (CS)
// pin 32 - Data/Command select (RS or A0)
// pin 30 - LCD reset (RST)
static U8GLIB_LM6059_2X u8g(21, 18, 28, 32, 30);


void initialize_LCD(){

    //Initialize colored backlight pins
    pinMode(BACKLIGHT_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    //Set background color to default selection
    set_background_color(color_set);

    //Initialize menu navigation variables
    cur_selection = HOME_BASE;
    cur_menu_page = 0;
    cur_sel_min = HOME_BASE;
    cur_sel_max = HOME_MAX;
    next_move = -1;
  
    //Display the initialization screen
    draw();
}


void u8g_prepare(){
    u8g.setFont(u8g_font_6x10);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
}


void sys_init_complete(){
    cur_menu_page = 1;
    draw();
}


void LCD_movement_handler(){
  
    //Check for user input/what the user has done.
    joystick_handler();
    fwd_butt_handler();
    bck_butt_handler();
  
    //Handle user request  
    if(next_move > 0){
        //Triggered by FWD being selected
        handle_select( cur_selection );
    }else if (next_move == 0 && cur_menu_page > 2){
        // Triggered by BCK being selected while not 
        // on the home screen.
        cur_menu_page = 2;
        cur_selection = HOME_BASE; 
        cur_sel_min = HOME_BASE;
        cur_sel_max = HOME_MAX;
        enable_side_scroll = 0; 
    }else if (next_move == 0){
        //Triggered by being on the home screen, exit UI mode.
        cur_menu_page = 1;
        in_UI = false;
    }
  
    u8g.firstPage();  
    do {
        draw();
    } while( u8g.nextPage() );
  
    next_move = -1;
}


void draw_cursor( int cur_menu_index ){
    switch( cursor_set ){
        case 2: u8g.drawStr(0, cur_menu_index*MENU_HEIGHT, ">"); break;
        case 3: u8g.drawBitmap(0, cur_menu_index*MENU_HEIGHT, MENU_HEIGHT, paw_bitmap); break;
    }
}


void draw_init(){
    u8g.drawStr(0, MENU_HEIGHT,   "----------------------");
    u8g.drawStr(0, 2*MENU_HEIGHT, "  INITIALIZING SYSTEM ");
    u8g.drawStr(0, 3*MENU_HEIGHT, "   PLEASE HOLD STILL  ");
    u8g.drawStr(0, 4*MENU_HEIGHT, "----------------------");
}


void draw_sys(){
    u8g.drawStr(0, 0, "----- SYSTEM INFO ----");
    u8g.drawStr(0, MENU_HEIGHT, "TOP IMU    BOTTOM IMU"); 
    u8g.drawStr(0, 2*MENU_HEIGHT, concat(" X:", round(xControl)); 
    u8g.drawStr(0, 2*MENU_HEIGHT, concat("            X:", round(xError))); 

    u8g.drawStr(0, 3*MENU_HEIGHT, concat(" Y:", round(yControl))); 
    u8g.drawStr(0, 3*MENU_HEIGHT, concat("            Y:", round(yError))); 

    u8g.drawStr(0, 4*MENU_HEIGHT, concat(" Z:", zControl))); 
    u8g.drawStr(0, 4*MENU_HEIGHT, concat("            Z:", round(zError))); 

    u8g.drawStr(0, 5*MENU_HEIGHT, "PRESS FWD TO ENTER UI");
}


void draw_home(){
    draw_cursor(cur_selection + 1);
    u8g.drawStr(0, 0, "-------- HOME --------");
    u8g.drawStr(0, MENU_HEIGHT, "PRESS FWD TO SELECT");
    u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS LOCK/UNLOCK" );
    u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SETTINGS");
    u8g.drawStr(0, 0, 5*MENU_HEIGHT, "PRESS BCK TO EXIT UI");
}


void draw_axis_select(){
    draw_cursor(cur_selection - 18);
    u8g.drawStr(0, 0, "-- AXIS UNLOCK/LOCK --");
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


void draw_settings(){
    draw_cursor( cur_selection - 76);
    u8g.drawStr(0, MENU_HEIGHT,   "------ SETTINGS ------");
    u8g.drawStr(0, 2*MENU_HEIGHT, "SAVE CHANGES WITH FWD");
    u8g.drawStr(0, 3*MENU_HEIGHT, "USE L/R TO ADJUST");
    
    //Determine which color option to display.
    if ( cur_selection == 80 ){
        switch( (set_selection % 5) + color_set ){
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Blue   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Green  ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Red    ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Purple ]"); break;
            case 6: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Off    ]"); break;

        }
    }else{
        switch( color_set ){
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Blue   ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Green  ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Red    ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Purple ]"); break;
            case 6: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Off    ]"); break;

        }
    }

    //Determine which cursor option to display.
    if ( cur_selection == 81 ){
        switch( (set_selection % 2) + cursor_set ){
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [   >  ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [      ]");
                    u8g.drawBitmap( 3*MENU_INDENT, 5*MENU_HEIGHT, MENU_HEIGHT, paw_bitmap);
                    break;
        }
    }else{
        switch( cursor_set ){
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [   >  ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [      ]"); 
                    u8g.drawBitmap( 3*MENU_INDENT, 5*MENU_HEIGHT, MENU_HEIGHT, paw_bitmap);
                    break;
        }
    }

    u8g.drawStr(0, 6*MENU_HEIGHT, "PRESS BCK TO EXIT");

}


void draw(){
    u8g_prepare();
    switch( cur_menu_page ) {
        case 0: draw_init(); break;
        case 1: draw_sys(); break;
        case 2: draw_home(); break;
        case 3: draw_settings(); break;
        case 10: draw_axis_select(); break;          
  }

}


void bck_butt_check(){
    int bck = analogRead(BCK_BUTT);

    Serial.print("Back Butt\t");
    Serial.println(bck);

    if( bck >= BUTT_PUSH ){
        next_move = 0;
        while(analogRead(BCK_BUTT) >= BUTT_PUSH);
    }
}


void fwd_butt_check(){
    int fwd = analogRead(FWD_BUTT);
  
    Serial.print("Front Butt\t");
    Serial.println(fwd);
  
    if(fwd >= BUTT_PUSH){
        next_move = cur_selection;
        while(analogRead(FWD_BUTT) >= BUTT_PUSH);
    }
}


void joystick_check(){
    //Read the two potentiometer values for the joystick
    int x = analogRead(JS_X);
    int y = analogRead(JS_Y);
    
    Serial.print("JOYSTICK\t");
    Serial.print(x);
    Serial.print("\t");
    Serial.println(y);
    
    //Determine which direction was selected.
    //Left and right are only enabled on apropriate screens,
    //like Settings or Axis Adjust screens. 
  
    if( enable_side_scroll && x >= JS_RIGHT ){
        set_selection++;
        while( analogRead(JS_X) >= JS_RIGHT );

    }else if( enable_side_scroll && x <= JS_LEFT ){
        set_selection--;
        while( analogRead(JS_X) <= JS_LEFT );

    }else if( y >= JS_UP ){
        if( (cur_selection - 1) < cur_sel_min ){
            cur_selection = cur_sel_max;
        }else{
            cur_selection = cur_selection - 1;
        }

        while( analogRead(JS_Y) >= JS_UP );

    }else if( y <= JS_DOWN ){
        if( (cur_selection + 1) > cur_sel_max ){
            cur_selection = cur_sel_min;
        }else{
            cur_selection = cur_selection + 1;
        }

        while( analogRead(JS_Y) <= JS_DOWN );
    }
}


void fwd_butt_handler(){
    if(!in_UI){
        int fwd = analogRead(FWD_BUTT);
    
        Serial.print("Front Butt\t");
        Serial.println(fwd);
    
        if(fwd_butt_trig == 0 && fwd>= BUTT_PUSH){
            fwd_butt_trig = 1;
            cur_menu_page = 2;

            in_UI = true;
            draw();
        }else if( fwd_butt_trig && fwd <= BUTT_PUSH){
            fwd_butt_trig = 0;
        }
    }
}


void handle_select( int next_move ){
    switch(next_move){
//-----------------AXIS CONTROL --------------------------//  
        case 1: cur_menu_page = 10;
                cur_selection = AXIS_SEL_BASE; 
                cur_sel_min = AXIS_SEL_BASE;
                cur_sel_max = AXIS_SEL_MAX;
                break;  

//----------------- SETTINGS --------------------------//  
        case 2: cur_menu_page = 3; 
                enable_side_scroll = 1; 
                set_selection = 1;
                cur_selection = SET_BASE; 
                cur_sel_min = SET_BASE;
                cur_sel_max = SET_MAX;
                break;

//-----------------AXIS MANIPULATE --------------------------//  
        case 20: unlock_axis(1); //X-Axis Unlock
                 break;

        case 21: unlock_axis(2);
                 break;

        case 22: unlock_axis(3);
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



