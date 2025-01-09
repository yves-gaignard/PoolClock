/**
 * \file ClockState.h
 * \author Yves Gaignard
 * \brief Header for class definition of the #ClockState which is used to keep track of values between modules
 */

#ifndef _CLOCK_STATE_H_
#define _CLOCK_STATE_H_

#include "Configuration.h"
#include <Arduino.h>
#include "TimeManager.h"
#include "DisplayManager.h"

#if AIR_TEMP_SENSOR == true
	#include "Sensor_AM232X.h"
#endif
#if WATER_TEMP_SENSOR == true
	#include "Sensor_DS18B20.h"
#endif


/**
 * \brief The clockState is responsible to hold all the data that needs to be communicated between components.
 *        It can be imagined as kind of like an "object oriented global variable"
 */
class ClockState
{
public:
    /**
     * \brief Available clock modes each with a different behaviour
     */
	enum ClockStates {CLOCK_MODE, TIMER_MODE, SET_TIMER, TIMER_NOTIFICATION, ALARM_NOTIFICATION};
private:
	TimeManager*       _timeM;
	DisplayManager*    _PoolClockDisplays;
#if AIR_TEMP_SENSOR == true
    Sensor_AM232X*     _am232x;
#endif
#if WATER_TEMP_SENSOR == true
    Sensor_DS18B20*    _DS18B20Sensors;
#endif
	static ClockState* _instance;
	unsigned long      _lastDotFlash;
    ClockStates        _MainState;
    uint16_t           _alarmToggleCount;
    unsigned long      _lastUpdateMillis;
    bool               _currentAlarmSignalState;
    bool               _isinNightMode;

	ClockState();
public:

    /**
     * \brief Base brightness of the clock. The actual brightness can still change if a light sensor is used
     */
	uint8_t _clockBrightness;

    /**
     * \brief Brightness of the clock driing nighttime hours define by #ClockState::NightModeStartTime and
     *        #ClockState::NightModeStopTime
     */
    uint8_t _nightModeBrightness;

    /**
     * \brief Any time after will be considered nighttime as long as it is still lower than #ClockState::NightModeStopTime
     */
	TimeManager::TimeInfo _NightModeStartTime;

    /**
     * \brief Any time before will be considered nighttime as long as it is still higher than #ClockState::NightModeStartTime
     */
	TimeManager::TimeInfo _NightModeStopTime;

    /**
     * \brief defines the number of dots.
     * \range 0 -> no dot; 1 -> one dot; 2 -> two dots; other-> one dot
     */
	uint8_t _numDots;

    /**
     * \brief Get the instance of the Clock object or create it if it was not yet instantiated.
     *
     * \return ClockState* returns the address to the ClockState object
     */
    static ClockState* getInstance();

    /**
     * \brief Destroys the ClockState object and cause #ClockState::getInstance to create a new object the next time it is called
     */
	~ClockState();

    /**
     * \brief Switch the current mode of the clock
     */
    void switchMode(ClockStates newState);

    /**
     * \brief Returns the current mode of the clock
     */
    ClockStates getMode();

    /**
     * \brief Has to be called periodically to update the screen and process state transitions within the state machine.
     *
     */
	void handleStates();
};

#endif
