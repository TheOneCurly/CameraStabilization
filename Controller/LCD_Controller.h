/*
 *
 *	LCD_Controller.h
 *
 *	Team: Marit Brocker, Mitch Cook, Alex Wilson, Caleb Disbrow
 *	Creation Date: 07 April 2015
 *	Project: RIT Senior Design 2014-2015 -- Camera Stabilization
 *
 *	Description: Funtionality for the LCD controller for a 3-axis camera stabilization system 
 *				 menu displayed on an ST7565 LCD (128x64) with RGB backlight. Includes constants, 
 *				 variable, and function header information for navigation and command selection 
 *				 for a 3-axis Camera Stabalization system. 
 *
 */

#ifndef _H_LCD_CONTROLLER_H_
#define _H_LCD_CONTROLLER_H_

#include <U8glib.h>
#include "Arduino.h"
#include "Variables.h"

//Initialize the LCD Display
// pin 21 - Serial clock out (SCLK)
// pin 18 - Serial data out (SID)
// pin 28 - LCD chip select (CS)
// pin 32 - Data/Command select (RS or A0)
// pin 30 - LCD reset (RST)
static U8GLIB_LM6059_2X u8g(21, 18, 28, 32, 30);


#define JS_X A3
#define JS_Y A4
#define FWD_BUTT A5
#define BCK_BUTT A6



//--------------------------------- LED PIN VALUES ---------------------------------\\
// -- All are connected to digital pins so you can turn them off and on
#define BACKLIGHT_LED 36
#define BLUE_LED 40
#define GREEN_LED 38
#define RED_LED 34

//-------------------------------- MENU INDEX VALUES --------------------------------\\
    
//home page
#define HOME_BASE 1
#define HOME_MAX 2

#define SET_BASE 80
#define SET_MAX 81

//Axis Control
#define AXIS_SEL_BASE 20
#define AXIS_SEL_MAX 22

//--------------------------------- JOYSTICK VALUES ---------------------------------\\

// Y
#define JS_UP 1000
#define JS_DOWN 200

//X
#define JS_LEFT 10
#define JS_RIGHT 1023

//---------------------------------- BUTTON VALUES ----------------------------------\\

#define BUTT_PUSH 1000
//---------------------------- Menu Navigation Values -------------------------------\\

static int cur_menu_page = 0;
static int cur_selection, cur_sel_max, cur_sel_min = 0;

static int next_move = -1;
static int enable_side_scroll = 0;
static int set_selection = 1;

// System Settings Defaults 
static int color_set = 2; //Green
static int cursor_set = 2; //Right Waka (>)



const uint8_t paw_bitmap[] PROGMEM = {
  0x00,         // 0 0 0 0 0 0 0 0 
  0x28,         // 0 0 1 0 1 0 0 0
  0x28,         // 0 0 1 0 1 0 0 0
  0x82,         // 1 0 0 0 0 0 1 0
  0x92,         // 1 0 0 1 0 0 1 0
  0x38,         // 0 0 1 1 1 0 0 0
  0x38,         // 0 0 1 1 1 0 0 0 
  0x28          // 0 0 1 0 1 0 0 0
};

//-----------------------------------------------------------------------------------\\

//-----------------------------  LCD DISPLAY FUNCTIONS ------------------------------\\

void initialize_LCD();
void u8g_prepare();
void sys_init_complete();
void draw_cursor( int cur_menu_index );
void draw_init();
void draw_sys();
void draw_home();
void draw_axis_select();
void draw_settings();
void draw();

//------------------------------ NAVIGATION FUNCTIONS -------------------------------\\

bool LCD_movement_handler(int special);
void handle_select( int command );
void joystick_check();
void fwd_butt_check();
void bck_butt_check();
bool fwd_butt_handler();

//-------------------------- SYSTEM MANIPULATION FUNCTIONS --------------------------\\

void unlock_axis( int axis );

void set_background_color(int color);

#endif
