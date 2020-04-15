/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display when the display is fitted
 with an I2C backpack. The LiquidCrystal library works with all LCD
 displays that are compatible with the Hitachi HD44780 driver. 

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi
modified by Dave Cherry in 2018 to demo I2C backpack support.
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystalIO.h>
#include <IoAbstractionWire.h>
#include <Wire.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to, there are two
// common possibilities

// set this according to your backpack. If it has RS on pin 0 set to true, otherwise false.
// if unsure, try both! If it's really non standard, you can set the pins as needed!
#define RS_RW_EN true

#if RS_RW_EN == true
// if the backpack you have has the RS pin on 0 and the EN pin on 2.
const int rs = 0, rw = 1, en = 2, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
#else
// if the backpack you have has the RS pin on 2 and the EN pin on 0.
const int rs = 2, en = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
#endif
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7, ioFrom8574(0x20));

void setup() {
  // most backpacks have the backlight on pin 3.
  lcd.configureBacklightPin(3);
  lcd.backlight();
  
  // for i2c variants, this must be called first.
  Wire.begin();

  // set up the LCD's number of columns and rows, must be called.
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello over i2c!");

  //
  // when using this version of liquid crystal, it interacts (fairly) nicely with task manager. 
  // instead of doing stuff in loop, we can schedule things to be done. But just be aware than
  // only one task can draw to the display. Never draw to the display in two tasks.
  //
  // You don't have to use the library with task manager like this, it's an option.
  //
  taskManager.scheduleFixedRate(250, [] {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    float secondsFraction =  millis() / 1000.0F;
    lcd.print(secondsFraction);
  });
}

void loop() {
    taskManager.runLoop();
}


