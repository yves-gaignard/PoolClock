/**
 * \file ClockStateConfiguration.cpp
 * \author Yves Gaignard
 * \brief Configuration for the whole Clock State setup.
 *
 */

#ifndef _CLOCKSTATE_CONFIGURATION_H_
#define _CLOCKSTATE_CONFIGURATION_H_

/**
 * \brief Timer default Value
 */
#define TIMER_DEFAULT_HOUR     0
#define TIMER_DEFAULT_MINUTE  30
#define TIMER_DEFAULT_SECOND   0

/**
 * \brief Flash the current time in case a timer is expired instead of flashing 00:00
 */
#define TIMER_FLASH_TIME false

/**
 * \brief Number of flashes until an alarm is considered complete and the system goes back to normal
 */
#define TIMER_FLASH_COUNT 10

/**
 * \brief How bright the clock should blink when an alarm or timer was triggered
 * \range: 0 - 255
 */
#define NOTIFICATION_BRIGHTNESS 125

/**
 * \brief How often the time is checked and the displays are updated
 */
#define TIME_UPDATE_INTERVAL	250 // milliseconds

/**
 * \brief Default brightness of the display. If you are using blynk you may ignore this setting.
 */
#define DEFAULT_CLOCK_BRIGHTNESS 192

/**
 * \brief Whether to activate night mode or not. If you want the clock to reduce brightness/switch off during certain hours set this to true.
 * If you are using Blynk to control the settings of your clock you may ignore the default settings as they can be changed dynamically during runtime in that case
 */
#define USE_NIGHT_MODE true

#if USE_NIGHT_MODE == true

	/**
	 * \brief Start hour for the night mode
	 */
	#define DEFAULT_NIGHT_MODE_START_HOUR 22

	/**
	 * \brief Start minute for the night mode
	 */
	#define DEFAULT_NIGHT_MODE_START_MINUTE 0

	/**
	 * \brief End hour for the night mode
	 */
	#define DEFAULT_NIGHT_MODE_END_HOUR 7

	/**
	 * \brief End minute for the night mode
	 */
	#define DEFAULT_NIGHT_MODE_END_MINUTE 0

	/**
	 * \brief Brightness that the clock should be set to while night mode is active
	 */
	#define DEFAULT_NIGHT_MODE_BRIGHTNESS 64

#endif

#endif