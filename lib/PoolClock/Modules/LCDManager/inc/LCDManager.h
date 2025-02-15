/**
 * \file LCDManager.h
 * \author Yves Gaignard
 * \brief Definition of the class LCDManager which manage LCD Display
 */
#ifndef _LCD_MANAGER_H_
#define _LCD_MANAGER_H_

#include <Arduino.h>
#include <vector>
#include <LiquidCrystal_I2C.h>

#include <LogManager.h>

class LCDManager {
  private:
    LiquidCrystal_I2C*       _lcd;                         // LiquidCrystal_I2C handle 
    uint8_t                  _deviceAddress;               // Adress of the LCD device
    uint8_t                  _columnNumber;                // Column Number of the device
    uint8_t                  _rowNumber;                   // Row Number of the device
    boolean                  _isInit      = false;         // State of the LCD to store if the init function has been done 
    boolean                  _displayState= true;          // the LCD is on in case of true and is off in case of false
    boolean                  _backlight   = true;          // backlight state
    boolean                  _blink       = false;         // blink state 
    boolean                  _cursor      = false;         // cursor state
    boolean                  _scrollState = false;         // screen scroll state
    int                      _scrollHorizontalDelay = 250; // time in milliseconds between one column shift of the scroll 
    int                      _scrollVerticalDelay   = 250; // time in milliseconds between one row shift of the scroll
    int                      _scrollDisplayTime     = 10;  // number of seconds to display the row or the screen
    std::string              _padding;                     // string containing a blank line
    std::vector<uint8_t>     _custom_chars;                // vector of customized characters (8 maximum)
    std::vector<std::string> _screenCache;                 // vector of strings. each string represent a line. The number of strings can be greater than the row number.
                                                           // if the scrollState == false, string vector index > max row will ignore 


    void _clearCache();

    void _updateLCDRow(int row, std::string& line);

    void _printScrollLine   (uint8_t row, std::string& line, int scrollHorizontalDelay, int scrollDisplayTime);


  public:
    /**
     * \brief Definition of custom characters
     */

      // Constructors
    LCDManager(uint8_t Device_Addr, uint8_t Cols, uint8_t Rows);
    ~LCDManager();
    
    LiquidCrystal_I2C*  getLCD();
    int                 getColumnNumber();
    int                 getRowNumber();

    boolean             getDisplayState();

    void initLCDManager ();
    void init();
    void createChar(uint8_t location, const char *charmap);
    void clear();
    void home();
    void display();
    void noDisplay();
    void blink();
    void noBlink();
    void cursor();
    void noCursor();
    void setCursor(int row, int col);
    void noBacklight();
    void backlight();

    void printScreen       (std::vector<std::string>& screen);
    void printScrollScreen (std::vector<std::string>& screen);
    void printScrollScreen (std::vector<std::string>& screen, int displayTime);

    void printLine         (uint8_t row, std::string& line);
    void printScrollLine   (uint8_t row, std::string& line);
    void printScrollLine   (uint8_t row, std::string& line, int displayTime);
    void printScrollLine   (uint8_t row, std::string& line, int delayTime, int displayTime);
};

#endif