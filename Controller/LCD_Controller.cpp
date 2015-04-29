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
#include "String.h"

#define MENU_INDENT 8
#define MENU_HEIGHT 9

//------------------------------ 'BOOL' FOR INTERRUPTS ------------------------------\\

static int fwd_butt_trig = 0;




/*******************************************************************************
 * initialize_LCD
 *
 * Enables background LEDs for the display, sets initial navigation variable 
 * values. Also draws the initialization screen.
 *
 ******************************************************************************/
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
    //draw();
    LCD_movement_handler( 1 );
}


/*******************************************************************************
 * u8g_prepare
 *
 * Sets up LCD display variables for printing, including font, font height
 * and other display characteristics.
 *
 ******************************************************************************/
void u8g_prepare(){
    u8g.setFont(u8g_font_6x10);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
}


/*******************************************************************************
 * sys_init_complete
 *
 * Only called by the main controller program when system initialization is 
 * complete. Changes menu to display system info.
 *
 ******************************************************************************/
void sys_init_complete(){
    cur_menu_page = 1;

    LCD_movement_handler( 1 );

}


/*******************************************************************************
 * LCD_movement_handler
 *
 * Once UI navigation is enabled, this function is used to check for any joystick
 * or button input, then handles the resulting action desired. If no action is
 * requested, the screen just refreshes and updates.
 *
 ******************************************************************************/
bool LCD_movement_handler(int special){
  
    bool return_val = true;
    
    if(special == 0){
        //Check for user input/what the user has done.
        joystick_check();
        fwd_butt_check();
        bck_butt_check();
    }
  
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
        return_val = false;
    }
  
    u8g.firstPage();  
    do {
        draw();
    } while( u8g.nextPage() );
  
    next_move = -1;
    
    return return_val;
}


/*******************************************************************************
 * draw_cursor
 *
 * Uses the index provided to draw the cursor next to the currently selected
 * menu option. 
 *
 * @param  cur_menu_index - the current item selected in the menu, set by menu
 *                          calling the function. 
 *
 ******************************************************************************/
void draw_cursor( int cur_menu_index ){
    switch( cursor_set ){
        case 1: u8g.drawStr(0, cur_menu_index*MENU_HEIGHT, ">"); break;
        case 2: u8g.drawBitmap(0, cur_menu_index*MENU_HEIGHT, 1,  8, paw_bitmap); break;
    }
}


/*******************************************************************************
 * draw_init
 * 
 * Draws the notice that the system is initializing to the LCD.
 *
 ******************************************************************************/
void draw_init(){
    u8g.drawStr(0, 2*MENU_HEIGHT, "----------------------");
    u8g.drawStr(0, 3*MENU_HEIGHT, " INITIALIZING SYSTEM ");
    u8g.drawStr(0, 4*MENU_HEIGHT, "   PLEASE HOLD STILL  ");
    u8g.drawStr(0, 5*MENU_HEIGHT, "----------------------");
}


/*******************************************************************************
 * draw_sys
 *
 * Draws the system info display, uses values provided by the control algorithm
 * through Variables.h to show the current angles of the upper and lower IMUs.
 *
 ******************************************************************************/
void draw_sys(){
    char buf[5];
    u8g.drawStr(0, 0, "----- SYSTEM INFO ----");
    u8g.drawStr(0, MENU_HEIGHT, "TOP IMU    BOTTOM IMU");

    //X
    u8g.drawStr(0, 2*MENU_HEIGHT, " X:" );
    u8g.drawStr(3*MENU_INDENT, 2*MENU_HEIGHT, itoa(round(xControl), buf, 10)); 

    u8g.drawStr(9*MENU_INDENT, 2*MENU_HEIGHT, "X:" );
    u8g.drawStr(11*MENU_INDENT, 2*MENU_HEIGHT, itoa(round(xError), buf, 10));

    //Y
    u8g.drawStr(0, 3*MENU_HEIGHT, " Y:" );
    u8g.drawStr(3*MENU_INDENT, 3*MENU_HEIGHT, itoa(round(yControl), buf, 10)); 

    u8g.drawStr(9*MENU_INDENT, 3*MENU_HEIGHT, "Y:" );
    u8g.drawStr(11*MENU_INDENT, 3*MENU_HEIGHT, itoa(round(yError), buf, 10)); 

    //Z
    u8g.drawStr(0, 4*MENU_HEIGHT, " Z:" );
    u8g.drawStr(3*MENU_INDENT, 4*MENU_HEIGHT, itoa(round(zControl), buf, 10)); 

    u8g.drawStr(9*MENU_INDENT, 4*MENU_HEIGHT, "Z:" );
    u8g.drawStr(11*MENU_INDENT, 4*MENU_HEIGHT, itoa(round(zError), buf, 10)); 
    
    u8g.drawStr(0, 6*MENU_HEIGHT, "PRESS FWD TO ENTER UI");
}


/*******************************************************************************
 * draw_home
 * 
 * Draws the home screen that will allow the user to navigate to the axis
 * lock/unlock screens, system settings, and back to system info display.
 *
 ******************************************************************************/
void draw_home(){
    draw_cursor(cur_selection + 1);
    u8g.drawStr(0, 0, "-------- HOME --------");
    u8g.drawStr(0, MENU_HEIGHT, "PRESS FWD TO SELECT");
    u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "AXIS LOCK/UNLOCK" );
    u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "SETTINGS");
    u8g.drawStr(0, 5*MENU_HEIGHT, "PRESS BCK TO EXIT UI");
}


/*******************************************************************************
 * draw_axis_select
 * 
 * Draws the display that allows the user to lock or unlock an axis. Display 
 * changes based on the current state of the axis. Axis status is pulled from 
 * control program via Variables.h.
 *
 * Lock - Stabilized
 * Unlock - free floating
 *
 ******************************************************************************/
void draw_axis_select(){
    draw_cursor(cur_selection - 17);
    u8g.drawStr(0, 0, "-- AXIS UNLOCK/LOCK --");
    u8g.drawStr(0, MENU_HEIGHT, "NOTE:");
    u8g.drawStr(MENU_INDENT, 2*MENU_HEIGHT, "  Lock is Stabilized");

    if(X_control_en){ //true = stabilized = lock
        u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Unlock X Axis (Pan)");
    }else{
        u8g.drawStr(MENU_INDENT, 3*MENU_HEIGHT, "Lock X Axis (Pan)");
    }


    if(Y_control_en){ //true = stabilized = lock
        u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Unlock Y Axis (Tilt)");
    }else{
        u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Lock Y Axis (Tilt)");
    }


    if(Z_control_en){ //true = stabilized = lock
        u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Unlock Z Axis (Pitch)");
    }else{
        u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Lock Z Axis (Pitch)");
    }
}


/*******************************************************************************
 * draw_settings
 * 
 * Draws the UI settings menu. Modifies display as user manipulates the joystick.
 *
 ******************************************************************************/
void draw_settings(){
    draw_cursor( cur_selection - 76);
    u8g.drawStr(0, MENU_HEIGHT,   "------ SETTINGS ------");
    u8g.drawStr(0, 2*MENU_HEIGHT, "SAVE CHANGES WITH FWD");
    u8g.drawStr(0, 3*MENU_HEIGHT, "USE L/R TO ADJUST");
    
    //Determine which color option to display.
    if ( cur_selection == 80 ){
        switch( (set_selection % 5)){
            case 1: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Blue   ]"); break;
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Green  ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Red    ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Purple ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Off    ]"); break;

        }
    }else{
        switch( color_set ){
            case 1: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Blue   ]"); break;
            case 2: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Green  ]"); break;
            case 3: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Red    ]"); break;
            case 4: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Purple ]"); break;
            case 5: u8g.drawStr(MENU_INDENT, 4*MENU_HEIGHT, "Color [ Off    ]"); break;

        }
    }

    //Determine which cursor option to display.
    if ( cur_selection == 81 ){
        switch( (set_selection % 2)){
            case 1: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [   >  ]"); break;
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [      ]");
                    u8g.drawBitmap( 9*MENU_INDENT, 5*MENU_HEIGHT, 1, 8, paw_bitmap);
                    break;
        }
    }else{
        switch( cursor_set ){
            case 1: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [   >  ]"); break;
            case 2: u8g.drawStr(MENU_INDENT, 5*MENU_HEIGHT, "Cursor  [      ]"); 
                    u8g.drawBitmap( 9*MENU_INDENT, 5*MENU_HEIGHT, 1, 8, paw_bitmap);
                    break;
        }
    }

    u8g.drawStr(0, 6*MENU_HEIGHT, "PRESS BCK TO EXIT");

}


/*******************************************************************************
 * draw
 * 
 * Handles redrawing the display based on a global variable cur_menu_page.
 *
 ******************************************************************************/
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


/*******************************************************************************
 * bck_butt_check
 * 
 * Checks to see if the back button is pressed by determining is the button line
 * has gone high. If this is the case, the system is notified that the menu should
 * navigate up a page.
 *
 ******************************************************************************/
void bck_butt_check(){
    int bck = analogRead(BCK_BUTT);

   // Serial.print("Back Butt\t");
    //Serial.println(bck);

    if( bck >= BUTT_PUSH ){
        next_move = 0;
        while(analogRead(BCK_BUTT) >= BUTT_PUSH);
    }
}


/*******************************************************************************
 * fwd_butt_check
 * 
 * Checks to see if the forward button is pressed by determining is the button line
 * has gone high. If this is the case, the system is notified that the menu should
 * navigate down a page.
 *
 ******************************************************************************/
void fwd_butt_check(){
    int fwd = analogRead(FWD_BUTT);
  
    //Serial.print("Front Butt\t");
   // Serial.println(fwd);
  
    if(fwd == 0){
        next_move = cur_selection;
        while(analogRead(FWD_BUTT) == 0);
    }
}


/*******************************************************************************
 * joystick_check
 * 
 * Checks to see the current state of the joystick. If the joystick has moved away
 * from the center position, the movement is recorded and the cursor is moved or 
 * highlighted settings option is changed.
 *
 ******************************************************************************/
void joystick_check(){
    //Read the two potentiometer values for the joystick
    int x = analogRead(JS_X);
    int y = analogRead(JS_Y);
    
    //Serial.print("JOYSTICK\t");
    //Serial.print(x);
    //Serial.print("\t");
    //Serial.println(y);
    
    //Determine which direction was selected.
    //Left and right are only enabled on apropriate screens,
    //like Settings or Axis Adjust screens. 
  
    if( enable_side_scroll && x >= JS_RIGHT ){
        set_selection--;
        if(set_selection < 1){
            set_selection = 1;
        }
        while( analogRead(JS_X) >= JS_RIGHT );

    }else if( enable_side_scroll && x <= JS_LEFT ){
        
        set_selection++;
        if(set_selection < 1){
            set_selection = 1;
        }
        while( analogRead(JS_X) <= JS_LEFT );

    }else if( y >= JS_UP ){
        if( (cur_selection + 1) > cur_sel_max ){
            cur_selection = cur_sel_min;
        }else{
            cur_selection = cur_selection + 1;
        }
        set_selection = 1;
        while( analogRead(JS_Y) >= JS_UP );

    }else if( y <= JS_DOWN ){
        if( (cur_selection - 1) < cur_sel_min ){
            cur_selection = cur_sel_max;
        }else{
            cur_selection = cur_selection - 1;
        }
        set_selection = 1;
        while( analogRead(JS_Y) <= JS_DOWN );
    }
}


/*******************************************************************************
 * fwd_butt_handler
 *
 * Special forward button handler for the interrupt flag that will enter the user
 * into being able to manipulate the UI. Should only be enabled and called when 
 * the user is not in the UI.
 *
 ******************************************************************************/
bool fwd_butt_handler(){
    bool return_val = false;
    int fwd = analogRead(FWD_BUTT);

    if(fwd == 0){
        //Serial.print("Front Butt\t");
        //Serial.println(fwd);
        cur_menu_page = 2;
        return_val = true;
        LCD_movement_handler(1);
    }
    return return_val;
}


/*******************************************************************************
 * handle_select
 * 
 * Takes in the menu option selected, updates navigation variables that will 
 * control/limit the UI.
 * 
 * @param next_move - selected menu option
 *
 *
 ******************************************************************************/
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
        case 80: color_set = (set_selection % 5) ;
                 set_background_color(color_set);
                 set_selection = 1;
                 break;

        case 81: cursor_set = (set_selection % 2);
                 set_selection = 1;
                 break;
    }
}


/*******************************************************************************
 * set_background_color
 * 
 * Changes the display background color based on what the user selects in the 
 * settings menu
 *
 * @param color - desired color selected in the UI 
 *
 ******************************************************************************/
void set_background_color( int color ){
  //-- Set the opposite of what you want HIGH 
  //--- (ie. if you want green set blue and red high, and green low.
    switch( color ){
      //--------------------  BLUE  --------------------------//
      case 1: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, LOW); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
      //-------------------- GREEN --------------------------//
      case 2: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, LOW); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
      //--------------------  RED  --------------------------//
      case 3: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, LOW); //Red
              break;
      //------------------- PURPLE -------------------------//
      case 4: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, LOW); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, LOW); //Red
              break;
      //-------------------- OFF --------------------------//
      case 5: digitalWrite(BACKLIGHT_LED, HIGH);
              digitalWrite(BLUE_LED, HIGH); //Blue
              digitalWrite(GREEN_LED, HIGH); //Green
              digitalWrite(RED_LED, HIGH); //Red
              break;
    }
}


/*******************************************************************************
 * unlock_axis
 * 
 * Locks or unlocks an axis by manipulating shared variables in Variables.h that
 * will be checked by the controller algorithm
 *
 * Lock - Stabilized
 * Unlock - free floating
 *
 * @param  axis - determines which axis to lock/unlock.
 *                1 - X Axis
 *                2 - Y Axis
 *                3 - Z Axis
 *
 ******************************************************************************/
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



