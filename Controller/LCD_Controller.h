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
#define HOME_MAX 3

#define SYS_BASE 10
#define SYS_MAX 11

//Axis Control
#define AXIS_SEL_BASE 20
#define AXIS_SEL_MAX 22

//X Axis Contril Menu
#define X_CTRL_BASE 30
#define X_CTRL_MAX 32
#define X_ADJUST 39

//Y Axis Contril Menu
#define Y_CTRL_BASE 40
#define Y_CTRL_MAX 42
#define Y_ADJUST 49

//Z Axis Contril Menu
#define Z_CTRL_BASE 50
#define Z_CTRL_MAX 51
#define Z_ADJUST 59


//--------------------------------- JOYSTICK VALUES ---------------------------------\\

// Y
#define JS_UP 1000
#define JS_DOWN 200

//X
#define JS_LEFT 10
#define JS_RIGHT 1023

//---------------------------- Menu Navigation Values -------------------------------\\

static int parent_menu_page;
static int cur_menu_page, cur_menu_index = 0;
static int next_menu_page = 0;
static int cur_selection, cur_sel_max, cur_sel_min = 0;

static int adjust_val = 0;
static int cur_axis = 0;
static int next_move = 0;

static int enable_side_scroll = 0;
static int set_selection = 1;

// System Settings Defaults 
static int color_set = 2; //Green
static int cursor_set = 2;


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

void initialize_LCD();
void LCD_movement_handler();
void sys_init_complete();
void u8g_prepare();

void draw_cursor(int cur_menu_index);
void draw_init();
void draw_home( int cur_menu_index );
void draw_sys();
void draw_axis_select( int cur_menu_index );
void draw_settings( int cur_menu_index );
void draw();
void joystick_handler();
void fwd_butt_handler();
void bck_butt_handler();

void handle_select( int command );

// CONTROL FUNCTIONS
void unlock_axis( int axis );

void set_background_color(int color);

#endif
