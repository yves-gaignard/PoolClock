/**
 * \file PoolClockCmd.h
 * \author Yves Gaignard
 * \brief Header for the PoolClockCmdg class 
 */

#ifndef _POOL_CLOCK_CMD_H_
#define _POOL_CLOCK_CMD_H_

#include "Configuration.h"

#include <Arduino.h>
#include <esp_task_wdt.h>
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "ClockState.h"
#include "EasyButton.h"
#include "PoolClockCmd.h"

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
/** \} */

/**
* \brief Enumeration of states managed by the State machine
*/
enum State_enum {CLOCK, TIMER, SET_TIMER};
/**
* \brief Enumeration of transitions managed by the State machine
*/
enum Transitions_enum {NONE, MODE, LONG_MODE, PLAY, LONG_PLAY, PLUS, LONG_PLUS, MINUS, LONG_MINUS};
/**
* \brief Enumeration of states that a push button may have
*/
enum Button_State_enum {NOT_PRESSED, PRESSED, LONG_PRESSED, RELEASED};
/**
* \brief Enumeration of timer digits
*/
enum Timer_Digit_enum {HOUR_DIGIT, MINUTE_DIGIT};
/**
* \brief Enumeration of timer states
*/
enum Timer_State_enum {STOPPED, STARTED, PAUSED};
/**
* \brief Time in ms during a push button needs to be pressed to consider it is a LONG press
*/
#define LONG_PRESS_TIME 500

/**
 * \brief Loop function which is executed on the second core of the ESP
 *
 */
void PoolClockCmdLoopCode(void* pvParameters);

/**
 * @brief Button state variables
 * 
 */
static Button_State_enum _Mode_button_state;
static Button_State_enum _Play_button_state;
static Button_State_enum _Plus_button_state;
static Button_State_enum _Minus_button_state;

/**
 * \brief Configuration class for storing all Blynk related information
 *
 */
class PoolClockCmd
{
private:
	static PoolClockCmd* _instance;
	TaskHandle_t _PoolClockCmdLoop;
	#if AIR_TEMP_SENSOR == true
    	Sensor_AM232X* _am232x;
	#endif
	#if WATER_TEMP_SENSOR == true
		Sensor_DS18B20* _DS18B20Sensors;
	#endif

	PoolClockCmd();
public:
	/**
	 * \brief possible selection options of the segmented switch responsible for selecting which color should be
	 *        changed in the App
	 *
	 */
	enum ColorSelector {CHANGE_HOURS_COLOR = 0x01, CHANGE_MINUTES_COLOR = 0x02, CHANGE_INTERIOR_COLOR = 0x04, CHANGE_DOT_COLOR = 0x08};
	uint8_t _ColorSelection;
	CRGB    _InternalColor;
	CRGB    _HourColor;
	CRGB    _MinuteColor;
	CRGB    _DotColor;
	bool    _UIUpdateRequired;
	DisplayManager* _PoolClockDisplays;
	TimeManager*    _timeM;
	bool            _isClearAction;

	EasyButton* _ModeButton;
	EasyButton* _PlayButton;
	EasyButton* _PlusButton;
	EasyButton* _MinusButton;

	/**
	 * @brief Machine state variables
	 * 
	 */
	State_enum _current_state;
	State_enum _previous_state;

    Transitions_enum _last_transition;

	/**
	 * @brief Timer variables
	 * 
	 */
	TimeManager::TimeInfo   _TimerDuration;
	Timer_Digit_enum        _CurrentTimerDigit;
	Timer_State_enum        _TimerState;

	~PoolClockCmd();
	/**
	 * \brief Get the Instance object.
	 *        Use this to get an instance to the PoolClockCmd singleton instead of its constructor.
	 *
	 * \return PoolClockCmd* pointer to the already existing or newly created PoolClockCmd object.
	 */
	static PoolClockCmd* getInstance();

	/**
	 * \brief to be called as part of the setup function
	 *
	 * \pre WIFI connection has to be established already by calling #wifiSetup before this method
	 */
	void setup();

	/**
	 * \brief Code for the second thread running on the second core of the ESP handling all the blynk code since
	 *        all of it is coded in a blocking way and we don't want to influence the animation smoothness
	 *
	 */
	static void PoolClockCmdLoopCode(void* pvParameters);
	
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
