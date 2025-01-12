/**
 * \file DeviceConfiguration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup the Devices' Configuration
 */

#ifndef _DEVICE_CONFIGURATION_H_
#define _DEVICE_CONFIGURATION_H_

/***************************
*
* PIR sensor settings
*
*****************************/
/**
 * \brief Enable automatic light on when a person is detected by the PIR sensor
 */
#define PIR_SENSOR			true //TODOPROD true

#if PIR_SENSOR == true
	/**
	 * \brief ADC pin to which the PIR sensor is connected to
	 */
	#define PIR_SENSOR_PIN			5
	#define PIR_SENSOR_DELAY        30*1000ul //5*60*1000ul
#endif

/***************************
*
* Push Buttons settings
*
*****************************/
/**
 * \brief Push button definition
 */
#define PUSH_BUTTONS			true //TODOPROD true

#if PUSH_BUTTONS == true
/**
	 * \brief ADC pin to push button is connected to
	 */
	#define BUTTON_MODE_PIN			12
	#define BUTTON_PLUS_PIN			14
	#define BUTTON_PLAY_PIN			27
	#define BUTTON_MINUS_PIN		26
#endif

/***************************
*
* Buzzer settings
*
*****************************/
/**
 * \brief Buzzer definition
 */
#define USE_BUZZER		true //TODOPROD true
#if USE_BUZZER == true
/**
	 * \brief Buzzer pin is connected to
	 */
	#define BUZZER_PIN				17
#endif

/***************************
*
* Water Temperature settings
*
*****************************/
/**
 * \brief Water Temperature definition
 */
#define WATER_TEMP_SENSOR			true //TODOPROD true

#if WATER_TEMP_SENSOR == true
/**
	 * \brief ADC pin to water temperature sensor is connected to
	 */
	#define WATER_TEMP_PIN			4
	#define waterThermometerName    "Water Thermometer"
	#define waterThermometerAddress "28881B94970E03DA"
		/**
	 * \brief Frequency to read the wter temperature in ms
	 */
	#define WATER_TEMP_READ_FREQUENCY	30000
#endif

/***************************
*
* I2C sensor settings
*
*****************************/
/**
 * \brief I2C sensor definition
 */
#define I2C_SENSOR			true //TODOPROD true

#if I2C_SENSOR == true
/**
	 * \brief I2C pins 
	 */
	#define I2C_SDA_PIN			21
	#define I2C_SCL_PIN			22
#endif

/***************************
*
* Air temperature sensor ID settings
*
*****************************/
/**
 * \brief Air temperature sensor ID definition
 */
#define AIR_TEMP_SENSOR			true //TODOPROD true

#if AIR_TEMP_SENSOR == true
	/**
	 * \brief Frequency to read the air temperature in ms
	 */
	#define AIR_TEMP_READ_FREQUENCY	30000
#endif

#endif