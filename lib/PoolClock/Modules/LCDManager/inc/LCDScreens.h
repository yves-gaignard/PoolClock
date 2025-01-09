/*
  Copyright 2022 - Yves Gaignard
  
  Functions to manage the differents screens
*/
#ifndef _LCD_SCREENS_H_
#define _LCD_SCREENS_H_

#include <Arduino.h>
#include <vector>

class LCDScreens {
  public:
    LCDScreens(int inactivityMaxTime = 30, int duration = 5); // time in seconds
    void   addLCDScreen(std::string screenName);
    void   addOrReplaceLCDScreen(std::string screenName);
    void   removeLCDScreen(std::string screenName);
    void   setInactivityMaxTime(int maxSeconds);
    void   setScreenSwitchTime(int seconds);
    void   setInactivityTimeOutReset();
    void   setCurrentLCDScreen(int screenIndex);

    int    getLCDScreenNumber();
    int    getCurrentLCDScreenIndex();
    int    getLCDScreenIndex(std::vector<std::string> screenName);
    time_t getDisplayStart();
    int    getInactivityMaxTime();
    int    getLastLCDScreenStartTime();
    int    getLCDScreenSwitchTime();

  private:
    std::vector<std::string> _screenNames; // vector of screens names
    int     _inactivityMaxTime;            // time out to shutdown the screen light
    int     _screenDuration;               // duration in seconds of a screen display before switching to the next time
    time_t  _lastScreenActivationStart;    // time of the last display activation without any user interaction
    time_t  _currentScreenStart;           // time of the current screen start
    int     _currentScreenIndex;           // Current displayed screen index 
};

#endif