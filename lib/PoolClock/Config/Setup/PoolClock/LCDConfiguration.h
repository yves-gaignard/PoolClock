/**
 * \file LCDConfiguration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup the PoolClock LCD Configuration
 */

#ifndef _LCD_CONFIGURATION_H_
#define _LCD_CONFIGURATION_H_

#include <Arduino.h>

/***************************
*
* PoolClock LCD screen settings
*
*****************************/
/**
 * \brief LCD Screen definition
 */
#define LCD_SCREEN			true //TODOPROD true

#if LCD_SCREEN == true
    /**
	 * \brief LCD Screen parameters
	 */
	#define LCD_ID			0x27
	#define LCD_COLS		20
	#define LCD_LINES		4

    /**
     * \brief PoolClock Custom characters
     */
    static const char LCDPlayChar[]  = { B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000 };
    static const char LCDPauseChar[] = { B01010, B01010, B01010, B01010, B01010, B01010, B01010, B01010 };
    static const char LCDStopChar[]  = { B00000, B00000, B01111, B01111, B01111, B01111, B00000, B00000 };

    static const uint8_t CHAR_PLAY = 1;
    static const uint8_t CHAR_PAUSE= 2;
    static const uint8_t CHAR_STOP = 3;

    /**
     * \brief The LCD blinking digit possibilities and static value
     */
    enum LCDScreen_BlinkingDigit { HighHour=0, LowHour=1, HighMinute=2, LowMinute=3, HighSecond=4, LowSecond=5 };
    extern LCDScreen_BlinkingDigit _lcd_blinking_digit;

#endif

#endif