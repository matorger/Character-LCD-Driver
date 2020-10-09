/* 
 * File:   LCD.h
 * Author: Mason Torgerson
 *
 * Created on September 30, 2020, 8:28 PM
 * https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 */

#ifndef LCD_H
#define	LCD_H

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>
#include "definitions.h"
#include <string.h>


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

typedef enum
{
    fourbitmode,
    eightbitmode
}LCDINTERFACE;

typedef enum
{
    _data_pins0,
    _data_pins1,
    _data_pins2,
    _data_pins3,
    _data_pins4,
    _data_pins5,
    _data_pins6,
    _data_pins7,
    _rs_pin,
    _rw_pin,
    _enable_pin
}PINS;

typedef enum
{
    LOW = 0,
    HIGH
}PINSTATES;

typedef struct
{
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    uint8_t _row_offsets[4];
    LCDINTERFACE bits;
    uint8_t cols;
    uint8_t lines;
}LCD;


void init(LCD *lcd);
void begin(LCD *lcd, uint8_t dotsize);
void setRowOffsets(LCD *lcd, int row0, int row1, int row2, int row3);
void clear(LCD *lcd);
void home(LCD *lcd);
void noDisplay(LCD *lcd);
void display(LCD *lcd);
void noBlink(LCD *lcd);
void blink(LCD *lcd);
void scrollDisplayLeft(LCD *lcd);
void scrollDisplayRight(LCD *lcd);
void leftToRight(LCD *lcd);
void rightToLeft(LCD *lcd);
void autoscroll(LCD *lcd);
void noAutoscroll(LCD *lcd);
void noCursor(LCD *lcd);
void cursor(LCD *lcd);
void lcdWrite(LCD *lcd, char text[]);
void command(LCD *lcd, uint8_t value);
void write(LCD *lcd, uint8_t value);
void send(LCD *lcd, uint8_t value, uint8_t mode);
void pulseEnable(void);
void setCursor(LCD *lcd, uint8_t col, uint8_t row);
void write4bits(uint8_t value);
void write8bits(uint8_t value);
void DelayUS(uint32_t time);
uint8_t digitalWrite(PINS pin, PINSTATES state);


#endif	/* LCD_H */

