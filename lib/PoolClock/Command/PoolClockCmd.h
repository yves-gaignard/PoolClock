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
 * \brief (in ms) duration of press to consider it is a long press 
 */
#define LONG_PRESS_TIME 800

/**
 * \brief Loop function which is executed on the second core of the ESP
 *
 */
void PoolClockCmdLoopCode(void* pvParameters);

/**
 * \brief Configuration class for storing all Blynk related information
 *
 */
class PoolClockCmd
{
private:
	static PoolClockCmd* instance;
	TaskHandle_t PoolClockCmdLoop;

	PoolClockCmd();
public:
	/**
	 * \brief possible selection options of the segmented switch responsible for selecting which color should be
	 *        changed in the App
	 *
	 */
	enum ColorSelector {CHANGE_HOURS_COLOR = 0x01, CHANGE_MINUTES_COLOR = 0x02, CHANGE_INTERIOR_COLOR = 0x04, CHANGE_DOT_COLOR = 0x08};
	uint8_t ColorSelection;
	CRGB InternalColor;
	CRGB HourColor;
	CRGB MinuteColor;
	CRGB DotColor;
	bool UIUpdateRequired;
	DisplayManager* PoolClockDisplays;
	bool isClearAction;

	EasyButton* ModeButton;
	EasyButton* PlayButton;
	EasyButton* PlusButton;
	EasyButton* MinusButton;


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
	 * \brief Callbacks to manage button actions
	 */
	static void ModeButtonPressed();
	static void ModeButtonPressedFor();

	static void PlayButtonPressed();
	static void PlayButtonPressedFor();

	static void PlusButtonPressed();
	static void PlusButtonPressedFor();

	static void MinusButtonPressed();
	static void MinusButtonPressedFor();
};

#endif
