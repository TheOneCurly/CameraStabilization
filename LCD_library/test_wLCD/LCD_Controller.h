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
//#include "U8glib.h"
#include "Arduino.h"


//--------------------------------- LED PIN VALUES ---------------------------------\\
// -- All are connected to digital pins so you can turn them off and on
#define BACKLIGHT_LED 52
#define BLUE_LED 51
#define GREEN_LED 49
#define RED_LED 47

//-------------------------------- MENU INDEX VALUES --------------------------------\\
    
//home page
#define HOME_BASE 1
#define HOME_MAX 3
//#define HOME_HEADER "HOME                "

#define SYS_BASE 10
#define SYS_MAX 11
//#define SYS_HEADER "SYSTEM INFO         "

//Axis Control
#define AXIS_SEL_BASE 20
#define AXIS_SEL_MAX 22
//#define AXIS_HEADER "AXIS SELECTION      "

//X Axis Contril Menu
#define X_CTRL_BASE 30
#define X_CTRL_MAX 32
#define X_ADJUST 39
//#define X_HEADER "X AXIS OPTIONS      "

//Y Axis Contril Menu
#define Y_CTRL_BASE 40
#define Y_CTRL_MAX 42
#define Y_ADJUST 49
///#define Y_HEADER "Y AXIS OPTIONS      "

//Z Axis Contril Menu
#define Z_CTRL_BASE 50
#define Z_CTRL_MAX 51
#define Z_ADJUST 59
//#define Z_HEADER "Z AXIS OPTIONS      "


//--------------------------------- JOYSTICK VALUES ---------------------------------\\
//#define JS_X A3
//#define JS_Y A4

// Y
#define JS_UP 1000
#define JS_DOWN 200

//X
#define JS_LEFT 10
#define JS_RIGHT 1023

//-------------------------------- MENU CONST VALUES --------------------------------\\
//#define MENU_INDENT 8
//#define MENU_HEIGHT 9


//---------------------------- Menu Navigation Values -------------------------------\\

static int parent_menu_page;
static int cur_menu_page, cur_menu_index = 0;
static int next_menu_page = 0;
static int cur_selection, cur_sel_max, cur_sel_min = 0;

static int adjust_val = 0;
static int cur_axis = 0;
static int next_move = 0;
static int back_button_pushed = 0;
//static char menu_header[] = "                    ";

//Used to determine which page should be drawn
//uint8_t draw_state = 1;
//uint8_t button_toggle = 0;

//-----------------------------------------------------------------------------------\\

void initialize_LCD();
void LCD_movement_handler();
void toggle_interrupt_handler();
void button_interrupt_handler();
void respond_to_action();
void u8g_prepare();
void draw_home( int cur_menu_index );
void draw_sys( int cur_menu_index );
void draw_axis_select( int cur_menu_index );
void draw_axis_options( int cur_menu_index, int axis);
void draw_axis_control( int axis );
void draw();

void joystick_handler();

void handle_select( int command );

// CONTROL FUNCTIONS
void unlock_axis( int axis );
void reset_axis( int axis );
void adjust_axis( int axis, int adjust );

void set_background_color(int color);

#endif
