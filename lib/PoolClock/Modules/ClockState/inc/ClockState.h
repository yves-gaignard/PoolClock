/**
 * \file ClockState.h
 * \author Yves Gaignard
 * \brief Header for class definition of the #ClockState which is used to keep track of values between modules
 */

#ifndef _CLOCK_STATE_H_
#define _CLOCK_STATE_H_

#include "Configuration.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "TimeManager.h"
#include "DisplayManager.h"
#include "EasyButton.h"
#if AIR_TEMP_SENSOR == true
	#include "Sensor_AM232X.h"
#endif
#if WATER_TEMP_SENSOR == true
	#include "Sensor_DS18B20.h"
#endif

/**
 * \brief Available clock modes each with a different behaviour
 */
enum ClockStates {CLOCK_MODE, TIMER_MODE, SET_TIMER, TIMER_NOTIFICATION, ALARM_NOTIFICATION};
/**
* \brief Enumeration of states managed by the State machine
enum State_enum {CLOCK, TIMER, SET_TIMER};
*/
/**
* \brief Enumeration of transitions managed by the State machine
*/
enum Transitions_enum {NONE, MODE, LONG_MODE, PLAY, LONG_PLAY, PLUS, LONG_PLUS, MINUS, LONG_MINUS};
/**
* \brief Enumeration of timer digits
*/
enum Timer_Digit_enum {HOUR_DIGIT, MINUTE_DIGIT};
/**
* \brief Enumeration of timer states
*/
enum Timer_State_enum {STOPPED, RUNNING, PAUSED, CANCELLED};
/**
* \brief Enumeration of states that a push button may have
*/
enum Button_State_enum {NOT_PRESSED, PRESSED, LONG_PRESSED, RELEASED};
/**
* \brief Time in ms during a push button needs to be pressed to consider it is a LONG press
*/
#define LONG_PRESS_TIME 500

/**
 * \brief Loop function which is executed on the second core of the ESP
 *
 */
void ClockStateLoopCode(void* pvParameters);

/**
 * @brief Button state variables
 * 
 */
static Button_State_enum _Mode_button_state;
static Button_State_enum _Play_button_state;
static Button_State_enum _Plus_button_state;
static Button_State_enum _Minus_button_state;

/**
 * \addtogroup BlynkChannels
 * \brief These are the channel definitions for Blynk
 * \{
 */
#define BLYNK_CHANNEL_BRIGHTNESS_SLIDER		V0
#define BLYNK_CHANNEL_LIGHT_GROUP_SELECTOR	V1
#define BLYNK_CHANNEL_CURRENT_COLOR_PICKER	V2
#define BLYNK_CHANNEL_TIMER_TIME_INPUT 		V3
#define BLYNK_CHANNEL_TIMER_START_BUTTON 	V4
#define BLYNK_CHANNEL_ALARM_TIME_INPUT 		V5
#define BLYNK_CHANNEL_ALARM_START_BUTTON 	V6
#define BLYNK_CHANNEL_NIGHT_MODE_TIME_INPUT	V7
#define BLYNK_CHANNEL_NIGHT_MODE_BRIGHTNESS	V8
#define BLYNK_CHANNEL_NUM_SEPARATION_DOTS	V9
#define BLYNK_CHANNEL_HOUR_COLOR_SAVE		V10
#define BLYNK_CHANNEL_MINUTE_COLOR_SAVE		V11
#define BLYNK_CHANNEL_INTERNAL_COLOR_SAVE	V12
#define BLYNK_CHANNEL_DOT_COLOR_SAVE		V13
#define BLYNK_CHANNEL_SELECTOR_HOURS		V14
#define BLYNK_CHANNEL_SELECTOR_MINUTES		V15
#define BLYNK_CHANNEL_SELECTOR_INTERIOR		V16
#define BLYNK_CHANNEL_SELECTOR_DOT			V17
/**
 * \brief The clockState is responsible to hold all the data that needs to be communicated between components.
 *        It can be imagined as kind of like an "object oriented global variable"
 */
class ClockState
{
private:
	static ClockState* _instance;
	TimeManager*       _timeM;
	DisplayManager*    _PoolClockDisplays;
    TaskHandle_t       _ClockStateLoop;
#if AIR_TEMP_SENSOR == true
    Sensor_AM232X*     _am232x;
#endif
#if WATER_TEMP_SENSOR == true
    Sensor_DS18B20*    _DS18B20Sensors;
#endif
	unsigned long      _lastDotFlash;
    //ClockStates        _MainState;
    uint16_t           _alarmToggleCount;
    unsigned long      _lastUpdateMillis;
    bool               _currentAlarmSignalState;
    bool               _isinNightMode;

	ClockState();
public:
	/**
	 * \brief possible selection options of the segmented switch responsible for selecting which color should be
	 *        changed in the App
	 *
	 */
	enum ColorSelector {CHANGE_HOURS_COLOR = 0x01, CHANGE_MINUTES_COLOR = 0x02, CHANGE_INTERIOR_COLOR = 0x04, CHANGE_DOT_COLOR = 0x08};
	uint8_t         _ColorSelection;
	CRGB            _InternalColor;
	CRGB            _HourColor;
	CRGB            _MinuteColor;
	CRGB            _DotColor;
	bool            _UIUpdateRequired;
	bool            _isClearAction;

#if PUSH_BUTTONS == true
	EasyButton* _ModeButton;
	EasyButton* _PlayButton;
	EasyButton* _PlusButton;
	EasyButton* _MinusButton;
#endif
	/**
	 * @brief Machine state variables
	 * 
	 */
	ClockStates _current_state;
	ClockStates _previous_state;

    Transitions_enum _last_transition;

	/**
	 * @brief Timer variables
	 * 
	 */
	TimeManager::TimeInfo   _TimerDuration;
	Timer_Digit_enum        _CurrentTimerDigit;
	Timer_State_enum        _TimerState;
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

    /**
	 * \brief to be called as part of the setup function
	 *
	 * \pre WIFI connection has to be established already by calling #wifiSetup before this method
	 */
	void setup();

	/**
	 * \brief Code for the second thread running on the second core of the ESP handling all the LCD code since
	 *        all of it is coded in a blocking way and we don't want to influence the animation smoothness
	 *
	 */
	static void ClockStateLoopCode(void* pvParameters);
	
	/**
	 * \brief Stop the execution of PoolClockCmd
	 *
	 */
	void stop();

	/**
	 * \brief Call this if a UI update is needed
	 *        Possible cases for this are that the timer ticked or was triggered or the alarm was triggered
	 *
	 */
	void updateUI();

	/**
	 * \brief Change the selection of the segmented switch responsible for selecting which color should be changed
	 */
	void changeSelection(ColorSelector selector, bool state);

	/**
	 * \brief Management of the state machine
	 */
	void state_machine_run(Transitions_enum transition);

	/**
	 * \brief Print to the serial the state of a button (for debug purpose)
	 */
	static void print_button_state(const char* button_name, Button_State_enum button_state);

	/**
	 * @brief Buttons read detection and transform theim into transitions
	 * 
	 * @return Transitions_enum 
	 */
	Transitions_enum read_buttons();

	/**
	 * \brief Transition action routines
	 */
	void NOPE();
	void ChgModeToTimer();
	void ChgModeToSetTimer();
	void ChgModeToClock();
	void StartPauseResumeTimer();
	void CancelTimer();
	void CancelSetTimer();
	void ValidateSetTimer();
	void MoveNextDigit();
	void IncrementDigit();
	void IncrementQuicklyDigit();
	void DecrementDigit();
	void DecrementQuicklyDigit();

	/**
	 * \brief Callbacks to manage button actions
	 */
	static void Mode_onPressed();
	static void Mode_onPressedForDuration();
	static void Play_onPressed();
	static void Play_onPressedForDuration();
	static void Plus_onPressed();
	static void Plus_onPressedForDuration();
	static void Minus_onPressed();
	static void Minus_onPressedForDuration();
};

#endif
