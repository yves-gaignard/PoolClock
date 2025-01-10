/**
 * \file NetworkConfiguration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup the Network (Wifi, OTA, ...) Configuration
 */

#ifndef _NETWORK_CONFIGURATION_H_
#define _NETWORK_CONFIGURATION_H_

/**
 * \brief If you want to run the system in a minimal mode to test some basic functionality or
 * 		  debug something it could be useful to disable wifi functionality completely.
 */
#define RUN_WITHOUT_WIFI 		false  // TODOPROD false

#if RUN_WITHOUT_WIFI == false
	/**
	 * \brief If you want Blynk functionality set this to true and set your authentication token. Otherwise set it to false
	 */
	#define IS_BLYNK_ACTIVE 		false // TODOPROD true

	#if IS_BLYNK_ACTIVE == true
		/**
		 * \brief If you want Blynk functionality paste your authentication token here.
		 */
		#define BLYNK_AUTH_TOKEN 	"AUTH_TOKEN_GOES_HERE"

		/**
		 * \brief Template ID for this device. If you want to use your own custom Template you will have to change this
		 */
		#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID_GOES_HERE"

		/**
		 * \brief Name of this device in the Blynk app
		 */
		#define BLYNK_DEVICE_NAME "LED Clock"

		/**
		 * \brief In case the blynk communication is not working this line causes Blynk to send debug output to the serial port.
		 * 		  If you are not worried about Blynk or have to diagnose some other issue you can comment this line out.
		 */
		#define BLYNK_PRINT 		Serial

		/**
		 * \brief Set the Blynk server address.
		 */
		#define BLYNK_SERVER		BLYNK_DEFAULT_DOMAIN

	#endif

	/**
	 * \brief If you want to use OTA upload instead or in addition to the normal cable upload set this option to true.
	 * To actually flash something via OTA you have to uncomment the OTA flash lines in the \ref platformio.ini file
	 * This is a nice addition to cable upload but it doesn't replace it completely.
	 * If the microcontroller crashes because of bad configuration you still have to use a cable
	 */
	#define ENABLE_OTA_UPLOAD	true		// TODOPROD true

	#if ENABLE_OTA_UPLOAD == true
		/**
		 * \brief The host name that shall be used for OTA updates. If you change this here it must also be changed
         *        in the \ref platformio.ini file
		 */
		#define OTA_UPDATE_HOST_NAME	"PoolClock"
		#define OTA_UPDATE_PORT         80  
		#define IS_WEB_SERIAL_ACTIVATED true
	#endif

	/**
	 * \brief The number of times the controller tries to connect to wifi before it fails and goes into
     *        smartConfig mode (if that is enabled)
	 */
	#define NUM_RETRIES 			50

	/**
	 * \brief Use the ESP smart config to setup the wifi network. If you want to set it manually set this to false
	 */
	#define USE_ESPTOUCH_SMART_CONFIG	false // TODOPROD true

	#if USE_ESPTOUCH_SMART_CONFIG == false
		/**
		 * \brief WIFI_SSID and WIFI_PASSWORD are only needed if smart setup is disabled
		 */
		#include "Secrets.h"  // To include the WIFI SSID and Password
		#define WIFI_WITHOUT_SCANNING_PHASE false // TODOPROD false
	#endif

	/**
	 * \brief Use the static IP to setup the wifi network. 
	 */
	#define USE_STATIC_IP_CONFIG	true // TODOPROD true

	#if USE_STATIC_IP_CONFIG == true
		#define MY_IP_ADDRESS         192,168,  1,24
		#define MY_IP_GATEWAY_ADDRESS 192,168,  1, 1
		#define MY_IP_SUBNET_ADDRESS  255,255,255, 0
		#define PRIMARY_DNS             8,  8,  8, 8
		#define SECONDARY_DNS           8,  8,  4, 4
	#endif

#endif

/*********************************
*
*	Misc settings:
*
**********************************/
/**
 * \brief enable for wifi less operation or to demo all the animations
 */
#define TIME_MANAGER_DEMO_MODE	false  // TODOPROD false

#endif