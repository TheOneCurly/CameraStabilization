#include <U8glib.h>

#include "LCD_Controller.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initialize_LCD();
}

void loop() {
  // put your main code here, to run repeatedly:

  LCD_movement_handler();
  delay(10);
}

