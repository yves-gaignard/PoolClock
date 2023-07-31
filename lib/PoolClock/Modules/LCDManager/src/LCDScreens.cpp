/*
  Yves Gaignard
  
  Pool manager screens management
*/

#define TAG "LCDScreens"

// Standard library definitions
#include <Arduino.h>

#include "LCDScreens.h"
#include "LogManager.h"

LCDScreens::LCDScreens(int inactivityMaxTime, int duration) {
  _inactivityMaxTime = inactivityMaxTime;
  _screenDuration = duration;
  time(&_currentScreenStart);
  LOG_D(TAG, "constructor: inactivityMaxTime=%d, duration=%d, time =%d",_inactivityMaxTime, _screenDuration, _currentScreenStart);
}
void LCDScreens::addScreen(std::string screenName) {
  if (std::find(_screenNames.begin(), _screenNames.end(), screenName) == _screenNames.end()) {
    _screenNames.push_back(screenName);
  }
}
void LCDScreens::addOrReplaceScreen(std::string screenName) {
  auto it = std::find(_screenNames.begin(), _screenNames.end(), screenName);
  if (it == _screenNames.end()) 
  {
    _screenNames.push_back(screenName);
  }
  else
  {
    int index = it - _screenNames.begin();
    _screenNames.at(index) = screenName;
  }
}
void LCDScreens::removeScreen(std::string screenName) {
  _screenNames.erase(std::remove(_screenNames.begin(), _screenNames.end(), screenName), _screenNames.end());
}
void LCDScreens::setInactivityMaxTime(int maxSeconds) {
  _inactivityMaxTime = maxSeconds;
}
void LCDScreens::setScreenSwitchTime(int seconds) {
  _screenDuration = seconds;
}
void LCDScreens::setInactivityTimeOutReset() {
  //LOG_D(TAG, "before _lastScreenActivationStart: time =%d",_currentScreenStart);
  time(&_lastScreenActivationStart);
  //LOG_D(TAG, "_lastScreenActivationStart: time =%d",_currentScreenStart);
}
void LCDScreens::setCurrentScreen(int screenIndex) {
  _currentScreenIndex = screenIndex;
  //LOG_D(TAG, "before setCurrentTime: time =%d",_currentScreenStart);
  time(&_currentScreenStart);
  //LOG_D(TAG, "setCurrentScreen: time =%d",_currentScreenStart);
}
int LCDScreens::getScreenNumber() {
  return _screenNames.size();
}
int  LCDScreens::getCurrentScreenIndex() {
  return _currentScreenIndex;
}
time_t LCDScreens::getDisplayStart(){
  return _lastScreenActivationStart;
}
int LCDScreens::getInactivityMaxTime() {
  return _inactivityMaxTime;
}
int LCDScreens::getLastScreenStartTime() {
  return _currentScreenStart;
}
int LCDScreens::getScreenSwitchTime() {
  return _screenDuration;
}