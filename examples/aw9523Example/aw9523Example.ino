/*
  LiquidCrystalIO Library - Example with LCD on AW9523 device, including using the LCD controller for backlight.
  As there is no defined backpack standard for AW9523 I'm aware of we generally just manually wire the device to
  pins as needed. The example below shows how.
*/

#include <Arduino.h>
#include <LiquidCrystalIO.h>

// When using the I2C version, these two extra includes are always needed. Doing this reduces the memory slightly for
// users that are not using I2C.
#include <IoAbstraction.h>
#include <IoAbstractionWire.h>
#include <Wire.h>
#include <TextUtilities.h>

// First we create the actual I2C device handler
const int interruptPin = 15;
AW9523IoAbstraction aw9523IoAbstraction(0x58, 15);
// Then we create an analog device from it to be used for the LED controller backlight
AW9523AnalogAbstraction awLedController(aw9523IoAbstraction);

//
// Encoder connections
//
const int encoderButtonPin = 0;
const int encoderAPin = 1;
const int encoderBPin = 2;

//
// LCD connections
//
const int rsPin = 8;
const int rwPin = 9;
const int enPin = 10;
const int data4Pin = 11;
const int data5Pin = 12;
const int data6Pin = 13;
const int data7Pin = 14;
const int backLightPin = 15;

// Now we create the LCD itself, in this case the wiring is:
// 8      - RS
// 9      - RW
// 10     - EN
// 11..14 - D4..7
// 15     - K - backlight low side
LiquidCrystal lcd(rsPin, rwPin, enPin, data4Pin, data5Pin, data6Pin, data7Pin, LiquidCrystal::BACKLIGHT_PWM, asIoRef(aw9523IoAbstraction));

enum MySwitchStates { MY_SW_RELEASED, MY_SW_PRESSED, MY_SW_HELD };
MySwitchStates switchState = MY_SW_RELEASED;

void switchWasPressed(pinid_t which, bool held) {
    switchState = held ? MY_SW_HELD : MY_SW_PRESSED;
}

void switchWasReleased(pinid_t which, bool held) {
    switchState = MY_SW_RELEASED;
}

void encoderHasChanged(int value) {
    lcd.setBacklight(value);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting LCD example");

    // for i2c variants, this must be called first.
    Wire.setSDA(12);
    Wire.setSCL(13);
    Wire.begin();

    Serial.println("Configure LCD");

    // set up the LCD's number of columns and rows, must be called.
    lcd.begin(20, 4);

    aw9523IoAbstraction.writeGlobalControl(true, AW9523IoAbstraction::CURRENT_HALF);

    // most backpacks have the backlight on pin 3.
    lcd.configureAnalogBacklight(&awLedController, backLightPin);

    // Print a message to the LCD.
    lcd.print("hello over i2c!");

    switches.init(asIoRef(aw9523IoAbstraction), SWITCHES_NO_POLLING, true);
    switches.addSwitch(encoderButtonPin, switchWasPressed, NO_REPEAT);
    switches.onRelease(encoderButtonPin, switchWasReleased);
    setupRotaryEncoderWithInterrupt(encoderAPin, encoderBPin, encoderHasChanged);
    switches.changeEncoderPrecision(255, 128);

    //
    // when using this version of liquid crystal, it interacts (fairly) nicely with task manager.
    // instead of doing stuff in loop, we can schedule things to be done. But just be aware that
    // only one task can draw to the display. Never draw to the display in two tasks.
    //
    // You don't have to use the library with task manager like this, it's an option.
    //
    taskManager.scheduleFixedRate(100, [] {
        Serial.println("Set cursor & print");

        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.setCursor(0, 1);
        // print the number of seconds since reset:
        float secondsFraction = millis() / 1000.0F;
        lcd.print(secondsFraction);

        lcd.setCursor(0, 2);
        lcd.print("Switch ");
        switch (switchState) {
            case MY_SW_RELEASED:
                lcd.print("OFF ");
                break;
            case MY_SW_PRESSED:
                lcd.print("PUSH");
                break;
            case MY_SW_HELD:
                lcd.print("HOLD");
                break;
        }

        lcd.setCursor(0, 3);
        lcd.print("Encoder ");
        char sz[10];
        ltoaClrBuff(sz, switches.getEncoder()->getCurrentReading(), 4, '0', sizeof sz);
        lcd.print(sz);

    });
}

void loop() {
    taskManager.runLoop();
}


