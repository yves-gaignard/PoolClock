/**
 * \file LogManager.cpp
 * \author Yves Gaignard
 * \brief Log management. This class is inspired from Arduino_DebugUtils.
 */

#define TAG "LogManager"

#include <Arduino.h>
#include <ESPPerfectTime.h>

#include "LogManager.h"

#ifdef WEB_SERIAL_LITE
  #include <WebSerialLite.h>
#endif

// Default values
static int const DEFAULT_LOG_LEVEL   = LOG_WARNING;
static Stream *  DEFAULT_OUTPUT_STREAM = &Serial;

// Constructor
LogManager::LogManager() {
  timestampOn();
  newlineOn();
  logLabelOn();
  formatTimestampOn();
  setLogLevel(DEFAULT_LOG_LEVEL);
  setLogOutputStream(DEFAULT_OUTPUT_STREAM);
  setWebSerialOff();
}

// Public member functions
bool LogManager::isValidLogLevel(int const log_level) {
  return (log_level >= LOG_ERROR) && (log_level <= LOG_VERBOSE);
}

void LogManager::setLogLevel(int const log_level) {
  if (isValidLogLevel(log_level)) _log_level = log_level;
}

int LogManager::getLogLevel() const {
  return _log_level;
}

void LogManager::setLogOutputStream(Stream * stream) {
  _log_output_stream = stream;
}

void LogManager::setTag(const char * tag, const int log_level) {
  if (isValidLogLevel(log_level)) {
    // if tag == "*", then set the default log_level
    if (strcmp(tag, "*") == 0 ) {
      setLogLevel(log_level);
      //Serial.println("setTag cas *");
    }
    else {
      if (_tags.find(tag) != _tags.end()) {
        // a same tag already exist, change its value with the new one
        _tags[tag] = log_level;
        //Serial.println("setTag cas replace tag");
      }
      else {
        // add the new tag
        _tags.insert( {tag, log_level});
        //Serial.println("setTag cas insert tag");
      }
    }
  }
}

void LogManager::setTag(const char * tag, const char * log_level) {
  String _log_level = log_level;
  if (_log_level.equalsIgnoreCase("error")) {
    setTag(tag, LOG_ERROR);
  }
  else if (_log_level.equalsIgnoreCase("warning")) {
    setTag(tag, LOG_WARNING);
  }
  else if (_log_level.equalsIgnoreCase("info")) {
    setTag(tag, LOG_INFO);
  }
  else if (_log_level.equalsIgnoreCase("debug")) {
    setTag(tag, LOG_DEBUG);
  }
  else if (_log_level.equalsIgnoreCase("verbose")) {
    setTag(tag, LOG_VERBOSE);
  }
}

void LogManager::newlineOn() {
  _newline_on = true;
}

void LogManager::newlineOff() {
  _newline_on = false;
}

void LogManager::logLabelOn() {
  _print_log_label = true;
}

void LogManager::logLabelOff() {
  _print_log_label = false;
}

void LogManager::formatTimestampOn() {
  _format_timestamp_on = true;
}

void LogManager::formatTimestampOff() {
  _format_timestamp_on = false;
}

void LogManager::timestampOn() {
  _timestamp_on = true;
}

void LogManager::timestampOff() {
  _timestamp_on = false;
}

void LogManager::setWebSerialOn() {
  _is_web_serial = true;
}

void LogManager::setWebSerialOff() {
  _is_web_serial = false;
}

void LogManager::print(const char * tag, int const log_level, const char * fmt, ...) {
  if (!shouldPrint(tag, log_level))
    return;

  if (_timestamp_on)
    printTimestamp();

  if (_print_log_label)
    printLogLabel(log_level);

  va_list args;
  va_start(args, fmt);
  vPrint(fmt, args);
  va_end(args);
}

void LogManager::print(const char * tag, int const log_level, const __FlashStringHelper * fmt, ...) {
  if (!shouldPrint(tag, log_level))
    return;

  if (_timestamp_on)
    printTimestamp();

  if (_print_log_label)
    printLogLabel(log_level);

  String fmt_str(fmt);

  va_list args;
  va_start(args, fmt);
  vPrint(fmt_str.c_str(), args);
  va_end(args);
}


// Private member functions
void LogManager::vPrint(char const * fmt, va_list args) {
  // calculate required buffer length
  int msg_buf_size = vsnprintf(nullptr, 0, fmt, args) + 1; // add one for null terminator
#if __STDC_NO_VLA__ == 1
  // in the rare case where VLA is not allowed by compiler, fall back on heap-allocated memory
  char * msg_buf = new char[msg_buf_size];
#else
  char msg_buf[msg_buf_size];
#endif

  vsnprintf(msg_buf, msg_buf_size, fmt, args);

  if (_newline_on) {
    _log_output_stream->println(msg_buf);
#ifdef WEB_SERIAL_LITE
    if (_is_web_serial == true) { WebSerial.println(msg_buf); }
#endif
  } 
  else {
    _log_output_stream->print(msg_buf);
#ifdef WEB_SERIAL_LITE
    if (_is_web_serial == true) { WebSerial.print(msg_buf); }
#endif

  }

#if __STDC_NO_VLA__ == 1
  // remember to clean up memory
  delete[] msg_buf;
#endif
}

void LogManager::printTimestamp()
{
  struct tm *tm ;
  suseconds_t usec;
  tm = pftime::localtime(nullptr, &usec);
  long int msec = usec / 1000;

  char timestamp[32];

  if (_format_timestamp_on) {
    // "prints" formatted output to a char array (string)
    snprintf(timestamp, sizeof(timestamp), "[%02d:%02d:%02d.%03ld]", tm->tm_hour, tm->tm_min, tm->tm_sec, msec );
  }
  else {
    // "prints" time in milliseconds from the beginning of the day
    unsigned long time_msec = ((tm->tm_hour * 3600) + (tm->tm_min * 60) + (tm->tm_sec))*1000 + msec;
    snprintf(timestamp, sizeof(timestamp), "[%8lu] ", time_msec);
  }

  _log_output_stream->print(timestamp);
#ifdef WEB_SERIAL_LITE
  if (_is_web_serial == true) { WebSerial.print(timestamp); }
#endif

}

void LogManager::printLogLabel(int const log_level)
{
  static char const * LOG_MODE_STRING[5] = { "[E]", "[W]", "[I]", "[D]", "[V]" };

  if ( ! isValidLogLevel(log_level))
    return;

  _log_output_stream->print(LOG_MODE_STRING[log_level]);
#ifdef WEB_SERIAL_LITE
  if (_is_web_serial == true) { WebSerial.print(LOG_MODE_STRING[log_level]); }
#endif
}

bool LogManager::shouldPrint(const char * tag, int const log_level) {
  // int numTag = _tags.size();
  // Serial.print("Number of Tag =");
  // Serial.println(numTag);

  // check if the tag is present in the tag list
  if (_tags.find(tag) != _tags.end()) {
    // yes, it exist, so compare with it
    int tag_log_level = _tags[tag];
    /* Serial.print("case 1: ");
    Serial.print(" log_level=");
    Serial.print(log_level);
    Serial.print(" tag_log_level=");
    Serial.print(tag_log_level);
    Serial.print(" Comparaison=");
    Serial.println((log_level <= tag_log_level)) ;*/
    return (log_level <= tag_log_level);
  } 
  else { 
    // no, so compare with the default log level
    /* Serial.print("case 2: ");
    Serial.print(" log_level=");
    Serial.print(log_level);
    Serial.print(" _log_level=");
    Serial.print(_log_level);
    Serial.print(" Comparaison=");
    Serial.println(( isValidLogLevel(log_level) && (log_level <= _log_level))) ;*/
    return ( isValidLogLevel(log_level) && (log_level <= _log_level));
  }
}

// CLASS INSTANTIATION
// =====================
LogManager Log;
