/**
 * \file TimeManagerConfiguration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup the TimeManager Configuration
 */

#ifndef _TIMEMANAGER_CONFIGURATION_H_
#define _TIMEMANAGER_CONFIGURATION_H_

    #include <Arduino.h>

    /**
     * \brief For how long the Display should flash when an alarm was fired in seconds
     */
    #define ALARM_NOTIFICATION_PERIOD 600

	/**
	 * \brief Server for the time
	 */
	#define NTP_SERVER "pool.ntp.org"

	/**
	 * \brief Enter the string for your timezone according to this webpage: https://remotemonitoringsystems.ca/time-zone-abbreviations.php
	 */
	#define TIMEZONE_INFO "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

	/**
	 * \brief Time in seconds for the interval in which the time should be synchronized with the time server
	 */
	#define TIME_SYNC_INTERVAL 1800

#endif