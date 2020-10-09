#include "LCD.h"


void init(LCD *lcd)
{
    if (lcd->bits == fourbitmode) //fourbitmode
      lcd->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    else 
      lcd->_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    begin(lcd, LCD_5x8DOTS);   
}

void begin(LCD *lcd, uint8_t dotsize) 
{
  int loop=0;
  
  if (lcd->lines > 1) {
    lcd->_displayfunction |= LCD_2LINE;
  }
  lcd->_numlines = lcd->lines;

  setRowOffsets(lcd, 0x00, 0x40, 0x00 + lcd->cols, 0x40 + lcd->cols);  

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != LCD_5x8DOTS) && (lcd->lines == 1)) 
  {
    lcd->_displayfunction |= LCD_5x10DOTS;
  }

  //pinMode(_rs_pin, OUTPUT);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) { 
    //pinMode(_rw_pin, OUTPUT);
  }
  //pinMode(_enable_pin, OUTPUT);
  
  // Do these once, instead of every time a character is drawn for speed reasons.
  for (loop=0; loop<((lcd->_displayfunction & LCD_8BITMODE) ? 8 : 4); ++loop)
  {
    //pinMode(_data_pins[loop], OUTPUT);
   } 

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  DelayUS(50000); 
  // Now we pull both RS and R/W low to begin commands
  digitalWrite(_rs_pin, LOW);
  digitalWrite(_enable_pin, LOW);
  
  if (_rw_pin != 255) 
  { 
    digitalWrite(_rw_pin, LOW);
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (lcd->_displayfunction & LCD_8BITMODE)) 
  {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    DelayUS(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    DelayUS(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    DelayUS(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
    DelayUS(4500);  // wait more than 4.1ms

    // second try
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
    DelayUS(150);

    // third go
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);  

  // turn the display on with no cursor or blinking default
  lcd->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display(lcd);

  // clear it off
  clear(lcd);

  // Initialize to default text direction (for romance languages)
  lcd->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);

}

void setRowOffsets(LCD *lcd, int row0, int row1, int row2, int row3)
{
  lcd->_row_offsets[0] = row0;
  lcd->_row_offsets[1] = row1;
  lcd->_row_offsets[2] = row2;
  lcd->_row_offsets[3] = row3;
}

void clear(LCD *lcd)
{
  command(lcd, LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  DelayUS(2000);  // this command takes a long time!
}

void home(LCD *lcd)
{
  command(lcd, LCD_RETURNHOME);  // set cursor position to zero
  DelayUS(2000);  // this command takes a long time!
}

void setCursor(LCD *lcd, uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(lcd->_row_offsets) / sizeof(*lcd->_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= lcd->_numlines ) {
    row = lcd->_numlines - 1;    // we count rows starting w/0
  }
  
  command(lcd, LCD_SETDDRAMADDR | (col + lcd->_row_offsets[row]));
}

void noDisplay(LCD *lcd) 
{
  lcd->_displaycontrol &= ~LCD_DISPLAYON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void display(LCD *lcd) 
{
  lcd->_displaycontrol |= LCD_DISPLAYON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void noBlink(LCD *lcd) 
{
  lcd->_displaycontrol &= ~LCD_BLINKON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void blink(LCD *lcd) 
{
  lcd->_displaycontrol |= LCD_BLINKON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void scrollDisplayLeft(LCD *lcd) 
{
  command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(LCD *lcd) 
{
  command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void leftToRight(LCD *lcd) 
{
  lcd->_displaymode |= LCD_ENTRYLEFT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(LCD *lcd) 
{
  lcd->_displaymode &= ~LCD_ENTRYLEFT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(LCD *lcd) 
{
  lcd->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(LCD *lcd) 
{
  lcd->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}
void noCursor(LCD *lcd) 
{
  lcd->_displaycontrol &= ~LCD_CURSORON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void cursor(LCD *lcd) 
{
  lcd->_displaycontrol |= LCD_CURSORON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void lcdWrite(LCD *lcd, char text[])
{
    int loop = 0;
    
    for(loop = 0; loop < strlen(text); loop++)
    {
        write(lcd, text[loop]);
    }
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(LCD *lcd, uint8_t location, uint8_t charmap[]) 
{
  int loop=0;
  location &= 0x7; // we only have 8 locations 0-7
  command(lcd, LCD_SETCGRAMADDR | (location << 3));
  for (loop=0; loop<8; loop++) 
  {
    write(lcd, charmap[loop]);
  }
}

void command(LCD *lcd, uint8_t value) 
{
  send(lcd, value, LOW);
}

void write(LCD *lcd, uint8_t value) 
{
  send(lcd, value, HIGH);
}

void send(LCD *lcd, uint8_t value, uint8_t mode) 
{
  digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, LOW);
  }
  
  if (lcd->_displayfunction & LCD_8BITMODE) {
    write8bits(value); 
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
}

void pulseEnable(void) 
{
  digitalWrite(_enable_pin, LOW);
  DelayUS(1);    
  digitalWrite(_enable_pin, HIGH);
  DelayUS(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  DelayUS(100);   // commands need > 37us to settle
}

void write4bits(uint8_t value) 
{
  int loop = 0;  
  for (loop = 0; loop < 4; loop++) 
  {
    digitalWrite(loop+4, (value >> loop) & 0x01);
  }

  pulseEnable();
}

void write8bits(uint8_t value) 
{
  int loop = 0;
  for (loop = 0; loop < 8; loop++) 
  {
    digitalWrite(loop, (value >> loop) & 0x01);
  }
  
  pulseEnable();
}

void DelayUS(uint32_t time)
{
    int loop = 0;
    for (loop = 0; loop < time; loop++)
    {
        CORETIMER_DelayUs (1);
    }
}

uint8_t digitalWrite(PINS pin, PINSTATES state)
{
 if(state == HIGH)
 {
    switch(pin)
    {
        case _data_pins0:
            D0_Set();
            break;
            
        case _data_pins1:
            D1_Set();
            break;
            
        case _data_pins2:
            D2_Set();
            break;
            
        case _data_pins3:
            D3_Set();
            break;
            
        case _data_pins4:
            D4_Set();
            break;
            
        case _data_pins5:
            D5_Set();
            break;
            
        case _data_pins6:
            D6_Set();
            break;
        
        case _data_pins7:
            D7_Set();
            break;
            
        case _rs_pin:
            RS_Set();
            break;
            
        case _rw_pin:
            RW_Set();
            break;
            
        case _enable_pin:
            E_Set();
            break;
    }
     return 0;
 }
 else if(state == LOW)
 {
     switch(pin)
    {
        case _data_pins0:
            D0_Clear();
            break;
            
        case _data_pins1:
            D1_Clear();
            break;
            
        case _data_pins2:
            D2_Clear();
            break;
            
        case _data_pins3:
            D3_Clear();
            break;
            
        case _data_pins4:
            D4_Clear();
            break;
            
        case _data_pins5:
            D5_Clear();
            break;
            
        case _data_pins6:
            D6_Clear();
            break;
        
        case _data_pins7:
            D7_Clear();
            break;
            
        case _rs_pin:
            RS_Clear();
            break;
            
        case _rw_pin:
            RW_Clear();
            break;
            
        case _enable_pin:
            E_Clear();
            break;
    }
     return 0;
 }
 else // Invalid state
 {
    return 1; 
 }
}