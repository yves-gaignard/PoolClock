/**
 * \file DisplayConfiguration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup the PoolClock Display Configuration
 */

#ifndef _DISPLAY_CONFIGURATION_H_
#define _DISPLAY_CONFIGURATION_H_

/**
 * \brief Color of the hour segments, this will be the default color if blynk functionality is disabled
 */
#define HOUR_COLOR							CRGB::White

/**
 * \brief Color of the minute segments, this will be the default color if blynk functionality is disabled
 */
#define MINUTE_COLOR						CRGB::Azure

/**
 * \brief Color of the temperature 1 segments, this will be the default color if blynk functionality is disabled
 */
#define TEMP1_COLOR							CRGB::PaleVioletRed

/**
 * \brief Color of the temperature 2 segments, this will be the default color if blynk functionality is disabled
 */
#define TEMP2_COLOR							CRGB::DarkBlue

/**
 * \brief Color of the internal LEDs, this will be the default color if blynk functionality is disabled
 */
#define INTERNAL_COLOR						CRGB::Blue

/**
 * \brief Color of the separation dot LEDs, this will be the default color if blynk functionality is disabled
 */
#define SEPARATION_DOT_COLOR				CRGB::Blue

/**
 * \brief Color of the LEDs for the OTA update progress bar
 */
#define OTA_UPDATE_COLOR					CRGB::Orange

/**
 * \brief Color of the LEDs while searching for a WIFI network
 */
#define WIFI_CONNECTING_COLOR				CRGB::Blue

/**
 * \brief Color of the LEDs signaling a successful WIFI connection
 */
#define WIFI_CONNECTION_SUCCESSFUL_COLOR	CRGB::Red

/**
 * \brief Color of the LEDs if system is waiting for WIFI smart config
 */
#define WIFI_SMART_CONFIG_COLOR				CRGB::Yellow

/**
 * \brief Color of the LEDs signaling an error of some sort
 */
#define ERROR_COLOR							CRGB::Red


/***************************
*
* LED Configuration
*
*****************************/

/**
 * \brief Pin to which the led strip data pin is connected to
 */
#define LED_DATA_PIN				18

/**
 * \brief Total number of segments that have LEDs in the shelf
 */
#define NUM_BIG_SEGMENTS 			4*7
#define NUM_SMALL_SEGMENTS 			4*7
#define NUM_DOT_SEGMENTS 			2*1
#define NUM_SEGMENTS 				(NUM_BIG_SEGMENTS + NUM_SMALL_SEGMENTS + NUM_DOT_SEGMENTS)

/**
 * \brief Number of LEDs in each segment
 */
//#define NUM_LEDS_PER_SEGMENT	7
#define NUM_LEDS_PER_LONG_SEGMENT	7
#define NUM_LEDS_PER_SHORT_SEGMENT	3
#define NUM_LEDS_PER_DOT_SEGMENT	1

/**
 * \brief If you wired the down lighter LEDs to the end of the LED strips set this to true
 */
#define APPEND_DOWN_LIGHTERS	false

/**
 * \brief Number of LEDs For interior lights
 */
#define ADDITIONAL_LEDS			1  // ATTENTION MUST BE >= 1

/**
 * \brief Automatically calculated total number of LEDs used
 */
#if APPEND_DOWN_LIGHTERS == true
	#define NUM_LEDS 				(NUM_BIG_SEGMENTS * NUM_LEDS_PER_LONG_SEGMENT)+(NUM_SMALL_SEGMENTS * NUM_LEDS_PER_SHORT_SEGMENT)+(NUM_DOT_SEGMENTS * NUM_LEDS_PER_DOT_SEGMENT) + ADDITIONAL_LEDS
#else
	#define NUM_LEDS 				(NUM_BIG_SEGMENTS * NUM_LEDS_PER_LONG_SEGMENT)+(NUM_SMALL_SEGMENTS * NUM_LEDS_PER_SHORT_SEGMENT)+(NUM_DOT_SEGMENTS * NUM_LEDS_PER_DOT_SEGMENT)
	/**
	 * \brief Pin to which the downlight led Strip data pin is connected to
	 */
	#define DOWNLIGHT_LED_DATA_PIN			19 
#endif

/**
 * \brief Number of displays in the shelf
 */
#define NUM_DISPLAYS			9

/**
 * \brief These enum definitions are used in the code do address the different Seven segment displays.
 * 		  The numbers have to match with the place of the display in the #DisplayManager::SegmentDisplayModes array in the file \ref DisplayConfiguration.cpp
 */
enum DisplayIDs {
	HIGHER_DIGIT_TEMP1_DISPLAY 	= 0,
	LOWER_DIGIT_TEMP1_DISPLAY 	= 1,
	HIGHER_DIGIT_HOUR_DISPLAY 	= 2,
	LOWER_DIGIT_HOUR_DISPLAY 	= 3,
	DIGIT_DOT_DISPLAY			= 4,
	HIGHER_DIGIT_MINUTE_DISPLAY = 5,
	LOWER_DIGIT_MINUTE_DISPLAY 	= 6,
	HIGHER_DIGIT_TEMP2_DISPLAY 	= 7,
	LOWER_DIGIT_TEMP2_DISPLAY 	= 8
};

/**
 * \brief If set to true the display will show 0 at midnight and 12 otherwise
 */
#define DISPLAY_0_AT_MIDNIGHT 		true

/**
 * \brief If set to true the higher displays will turn off in case they would show 0
 */
#define DISPLAY_SWITCH_OFF_AT_0 	true

/**
 * \brief If set to true 24 hour format will be used. For this one additional column is needed in the shelf to display it correctly
 */
#define USE_24_HOUR_FORMAT			true

/**
 * \brief The number of segments to use for displaying a progress bar for the OTA updates
 */
#define NUM_SEGMENTS_PROGRESS		18

/**
 * \brief The time is shall take for one iteration of the loading animation
 */
#define LOADING_ANIMATION_DURATION		3000

/**
 * \brief How fast the brightness interpolation shall react to brightness changes
 */
#define BRIGHTNESS_INTERPOLATION	3000

/**
 * \brief If set to -1 the flashing middle dot is disabled, otherwise this is the index of the Display segment that should display the dot.
 */
#define DISPLAY_FOR_SEPARATION_DOT DIGIT_DOT_DISPLAY

/**
 * \brief Target Frames per second for the smoothness of animations
 */
#define ANIMATION_TARGET_FPS		60

/**
 * \brief Length of sooth animation transition from fully on to black and vice versa in percent
 * 		  NOTE: The higher this number the less obvious easing effects like bounce or elastic will be
 */
#define ANIMATION_AFTERGLOW			0.2

/**
 * \brief Length of the dot/s fading animation. One flash fades in and out
 */
#define DOT_FLASH_SPEED 1000

/**
 * \brief Intervale in which the dot/s should flash
 */
#define DOT_FLASH_INTERVAL	2000

/**
 * \brief Number of separation dots to use by default (or if no blynk functionality is available)
 * 		  allowed values are 1, 2 and 0 to turn it off
 */
#define NUM_SEPARATION_DOTS	2

/***************************
*
* Light sensor settings
*
*****************************/

/**
 * \brief Enable automatic brightness adjustments based on a light sensor
 */
#define ENABLE_LIGHT_SENSOR			true //TODOPROD true

#if ENABLE_LIGHT_SENSOR == true
	/**
	 * \brief ADC pin to which the light sensor is connected to
	 */
	#define LIGHT_SENSOR_PIN			34

	/**
	 * \brief How many measurements shall be averaged. Higher number -> smoother but slower change
	 */
	#define LIGHT_SENSOR_AVERAGE		20

	/**
	 * \brief Width of the median calculation. Higher number -> smoother change
	 * 		  Should never be higher than the LIGHT_SENSOR_AVERAGE
	 */
	#define LIGHT_SENSOR_MEDIAN_WIDTH	10

	/**
	 * \brief Time that should pass before the light sensor is read again. Higher number -> slower adjustments but also changes will be more sudden
	 */
	#define LIGHT_SENSOR_READ_DELAY		500

	/**
	 * \brief AnalogRead value if the light sensor reads complete darkness
	 */
	#define LIGHT_SENSOR_MIN			0

	/**
	 * \brief AnalogRead value if the light sensor reads the brightest
	 */
	#define LIGHT_SENSOR_MAX			4095

	/**
	 * \brief Value between 0 and 255 that determines how much the light sensor values can influence the led brightness
	 */
	#define LIGHT_SENSOR_SENSITIVITY	100

#endif

/*********************************
*
*	Misc settings:
*
**********************************/

/**
 * \brief The time it takes for one digit to morph into another
 */
#define DIGIT_ANIMATION_SPEED 900

/**
 * \brief the minimum delay between calls of FastLED.show()
 */
#define FASTLED_SAFE_DELAY_MS 20


#endif