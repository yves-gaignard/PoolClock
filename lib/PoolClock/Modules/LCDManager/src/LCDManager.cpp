/**
 * \file LCDManager.cpp
 * \author Yves Gaignard
 * \brief Implementation of the class LCDManager which manage LCD Display
 */

#define TAG "LCDManager"

// Standard library definitions
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

//#include "PM_Pool_Manager.h"
#include <LCDManager.h>

LCDManager::LCDManager(uint8_t Device_Addr, uint8_t Cols, uint8_t Rows) {
  this->_initLCD (Device_Addr, Cols, Rows);
}

LCDManager::~LCDManager(){
  delete _lcd;
}

void LCDManager::_initLCD (uint8_t Device_Addr, uint8_t Cols, uint8_t Rows) {
  _lcd = new LiquidCrystal_I2C(Device_Addr, Cols, Rows);
  _deviceAddress= Device_Addr;
  _columnNumber=Cols;
  _rowNumber=Rows;
  _padding = "";
  for (int i=0; i < _columnNumber; i++) {
    _padding+=" ";
  }
  // Create custom character
  _lcd->createChar(PLAY,  LCDPlayChar);
  _lcd->createChar(PAUSE, LCDPauseChar);
  _lcd->createChar(STOP,  LCDStopChar);
  _isInit=true;
}
LiquidCrystal_I2C*  LCDManager::getLCD() {
  return _lcd;
}
int                 LCDManager::getColumnNumber() {
  return _columnNumber;
}
int                 LCDManager::getRowNumber() {
  return _rowNumber;
}

boolean             LCDManager::getDisplayState() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  return _displayState;
}
    
void LCDManager::init() {
  _lcd->init();
}
void LCDManager::clear() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->clear();
}
void LCDManager::home() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->home();
}
void LCDManager::display() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->display();
  _displayState = true;
}
void LCDManager::noDisplay() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->noDisplay();
  _displayState = false;
}
void LCDManager::blink() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->blink();
  _blink=true;
}
void LCDManager::noBlink() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->noBlink();
  _blink=false;
}
void LCDManager::cursor() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->cursor();
  _cursor=true;
}
void LCDManager::noCursor() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->noCursor();
  _cursor=false;
}
void LCDManager::noBacklight() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->noBacklight();
  _backlight=false;
}
void LCDManager::backlight() {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  _lcd->backlight();
  _backlight=true;
}

void LCDManager::printScreen       (std::vector<std::string>& screen) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  this->clear();
  this->display();
  this->backlight();
  this->home();
  this->noBlink();
  this->noCursor();
  std::string subLine;
  for (int row =0; row < _rowNumber; row++) {
    subLine = std::string(screen[row]+_padding).substr(0, _columnNumber);
    _lcd->setCursor(0,row);
    _lcd->print(subLine.c_str());
  }
}
void LCDManager::printScrollScreen (std::vector<std::string>& screen) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  
}
void LCDManager::printScrollScreen (std::vector<std::string>& screen, int displayTime) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  
}

void LCDManager::printLine         (uint8_t row, std::string& line) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  std::string subLine;
  subLine = std::string(line+_padding).substr(0, _columnNumber);
  _lcd->printf(subLine.c_str());
}

void LCDManager::printScrollLine   (uint8_t row, std::string& line) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  this->_printScrollLine(row, line, _scrollHorizontalDelay, _scrollDisplayTime);
}

void LCDManager::printScrollLine   (uint8_t row, std::string& line, int displayTime) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  this->_printScrollLine(row, line, _scrollHorizontalDelay, displayTime);
}

void LCDManager::printScrollLine   (uint8_t row, std::string& line, int delayTime, int displayTime) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  this->_printScrollLine(row, line, delayTime, displayTime);
}

void LCDManager::_printScrollLine   (uint8_t row, std::string& line, int scrollHorizontalDelay, int scrollDisplayTime) {
  if (_isInit == false) { LOG_E(TAG, "LCD was not initialized. Take care of coredump !!!"); }
  int time_to_display = scrollDisplayTime * 1000;
  std::string message = line + " ";
  int len = message.length();
  message = _padding + message + _padding;
  std::string sub_message;
  time_t start =millis();
  int j = start;
  while (j<start+time_to_display) {
    for (int position = 0; position < len+_columnNumber; position++) {
      _lcd->setCursor(0, row);
      sub_message=message.substr(position, _columnNumber);
      _lcd->print(sub_message.c_str());
      delay(scrollHorizontalDelay);
    }
    j=millis();
  }
}
