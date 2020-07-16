#ifndef LiquidCrystal_h
#define LiquidCrystal_h

#include <IoAbstraction.h>
#include <inttypes.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#ifndef IOA_USE_ARDUINO
#ifndef HEX
#define HEX 2
#define DEC 1
#endif
class LiquidCrystal {
#else

#include "Print.h"

class LiquidCrystal : public Print {
#endif
public:
    enum BackLightPinMode { BACKLIGHT_NORMAL, BACKLIGHT_INVERTED, BACKLIGHT_PWM };

    LiquidCrystal(uint8_t rs, uint8_t enable,
                  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                  uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, BasicIoAbstraction *ioMethod = NULL);

    LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                  uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, BasicIoAbstraction *ioMethod = NULL);

    LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, BasicIoAbstraction *ioMethod = NULL);

    LiquidCrystal(uint8_t rs, uint8_t enable,
                  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, BasicIoAbstraction *ioMethod = NULL);

    void init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
              uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
              uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, BasicIoAbstraction *ioMethod);

    /**
     * Sets the device that will be used, IE where the LCD pins are connected to.
     * @param ioRef the reference to an IO device previously created.
     */
    void setIoAbstraction(IoAbstractionRef ioRef) {
        _io_method = ioRef;
    }

    /**
     * Configure a backlight pin that is used to control the backlight using the setBacklight(level) function.
     * Depending on the type of display you have this may be either:
     *  * LiquidCrystal::BACKLIGHT_NORMAL - logic high for backlight on
     *  * LiquidCrystal::BACKLIGHT_INVERTED - logic low for backlight on,
     *  * LiquidCrystal::BACKLIGHT_PWN backlight pin in connected to a PWM pin and can dim.
     * @param backlightPin the pin on which the backlight is connected
     * @param mode one of the modes specified above
     */
    void configureBacklightPin(uint8_t backlightPin, BackLightPinMode mode = LiquidCrystal::BACKLIGHT_NORMAL);

    /**
     * Sets the delay for a given command, at this moment, this can only set the settle time of a standard command.
     * @param command the command to set the delay for, presently only the settle time.
     * @param settleTime the new value of the delay in microseconds.
     */
    void setDelayTime(uint8_t command, uint8_t settleTime) { _delayTime = (settleTime > 1) ? settleTime : 1; }

    /**
     * Unlike the regular display where this is option, in this variant you absolutely must call this function
     * before use.
     * @param cols the number of colums
     * @param rows the number of rows
     * @param charsize the character size, defaulted to 5x8
     */
    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

    /**
     * Clears the display completely using the clear command on the device, warning that this is a very slow
     * call taking at least 2000 micros.
     */
    void clear();

    /**
     * Moves the display back to the home position, calling setCursor(0,0) is much faster.
     */
    void home();

    void backlight() { setBacklight(HIGH); }

    void noBacklight() { setBacklight(LOW); }

    void setBacklight(uint8_t state);

    void noDisplay();

    void display();

    void noBlink();

    void blink();

    void noCursor();

    void cursor();

    void scrollDisplayLeft();

    void scrollDisplayRight();

    void leftToRight();

    void rightToLeft();

    void autoscroll();

    void noAutoscroll();

    void setRowOffsets(int row1, int row2, int row3, int row4);

    void createChar(uint8_t, const uint8_t[]);

    void createCharPgm(uint8_t, const uint8_t[]);

    void setCursor(uint8_t, uint8_t);

    virtual size_t write(uint8_t);

    void command(uint8_t);

#ifdef IOA_USE_MBED
    void print(const char* data);
    void print(char data);
    void print(int data, int mode = DEC);
    void print(double dbl);
#else
    using Print::write;
#endif

private:
    void send(uint8_t, uint8_t);

    void write4bits(uint8_t);

    void write8bits(uint8_t);

    void pulseEnable();

    uint8_t _rs_pin; // LOW: command.  HIGH: character.
    uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
    uint8_t _enable_pin; // activated by a HIGH pulse.
    uint8_t _data_pins[8];

    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;

    uint8_t _delayTime;
    BackLightPinMode _backlightMode;
    uint8_t _backlightPin;

    uint8_t _numlines;
    uint8_t _row_offsets[4];

    BasicIoAbstraction *_io_method;
};

#endif
