/**
 * \file Configuration.h
 * \author Yves Gaignard
 * \brief This is the place to tweak and setup everything. Individual features can be turned on or off here
 *        as well as many other configuration parameters.
 * 		  This configuration is for a fully loaded 24h display with intermediate segments.
 */

#ifndef _CONFIGURATIONS_H_
#define _CONFIGURATIONS_H_

/**
 * \addtogroup MainConfiguration
 * \brief Main configuration settings
 */
#include "DeviceConfiguration.h"        // Configuration of the devices (sensors, ...)
#include "LCDConfiguration.h"			// Configuration of the LCD screen
#include "DisplayConfiguration.h"       // Configuration of the Display Manager Component
#include "ClockStateConfiguration.h"    // Configuration of the Clock State Component
#include "NetworkConfiguration.h"		// Configuration of the Network composnents (Wifi, OTA, ...)

/**
 * \brief Definition of the default LOG Level chosen for the application
 *        Possible levels : LOG_NONE/LOG_ERROR/LOG_WARNING/LOG_INFO/LOG_DEBUG/LOG_VERBOSE
 */
#define DEFAULT_LOG_LEVEL LOG_INFO

#endif
