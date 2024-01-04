/**
 * \file ClockState.cpp
 * \author Yves Gaignard
 * \brief Implementation of the ClockState class member functions
 */

#define TAG "ClockState"

#include "ClockState.h"
#include "LogManager.h"

ClockState* ClockState::instance = nullptr;

ClockState::ClockState()
{
	MainState = CLOCK_MODE;
	clockBrightness = DEFAULT_CLOCK_BRIGHTNESS;
	alarmToggleCount = 0;
	nightModeBrightness = DEFAULT_NIGHT_MODE_BRIGHTNESS;
	numDots = NUM_SEPARATION_DOTS;

	NightModeStartTime = TimeManager::TimeInfo {DEFAULT_NIGHT_MODE_START_HOUR, DEFAULT_NIGHT_MODE_START_MINUTE, 0};
	NightModeStopTime = TimeManager::TimeInfo {DEFAULT_NIGHT_MODE_END_HOUR, DEFAULT_NIGHT_MODE_END_MINUTE, 0};

	lastUpdateMillis = millis();
	lastDotFlash = millis();
	currentAlarmSignalState = false;
	isinNightMode = false;
	timeM = TimeManager::getInstance();
	PoolClockDisplays = DisplayManager::getInstance();
#if AIR_TEMP_SENSOR == true
	am232x = Sensor_AM232X::getInstance();
#endif
#if WATER_TEMP_SENSOR == true
	DS18B20Sensors = Sensor_DS18B20::getInstance();
#endif
}

ClockState::~ClockState()
{
	instance = nullptr;
}

ClockState* ClockState::getInstance()
{
	if(instance == nullptr)
	{
		instance = new ClockState();
	}
	return instance;
}

void ClockState::switchMode(ClockStates newState)
{
    if(newState == ClockState::CLOCK_MODE)
    {
        alarmToggleCount = 0;
    }
    MainState = newState;
}

ClockState::ClockStates ClockState::getMode()
{
    return MainState;
}

void ClockState::handleStates()
{
	LOG_D(TAG, "ClockState::handleStates() ... Start");
	if(lastUpdateMillis + TIME_UPDATE_INTERVAL <= millis()) // update the display only in a certain intervall
	{
		// ========================
		// Display of temperature
		// ========================
		float temperature1=0.0;
		float humidity1=0.0;
		float temperature2=0.0;
		float humidity2=0.0;
		#if AIR_TEMP_SENSOR == true
			temperature1=am232x->getTemperature();
			humidity1=am232x->getHumidity();
		#endif
		#if WATER_TEMP_SENSOR == true
			temperature2=DS18B20Sensors->getPreciseTempCByAddress(waterThermometerAddress);
		#endif
		PoolClockDisplays->displayTemperature(temperature1, humidity1, temperature2, humidity2);

		// =================================
		// Display of clock, timer or alarm
		// =================================
		lastUpdateMillis = millis();
		TimeManager::TimeInfo currentTime;
		currentTime = timeM->getCurrentTime();
		switch (MainState)
		{
		case ClockState::CLOCK_MODE:
			#if USE_NIGHT_MODE == true
				if(timeM->isInBetween(NightModeStartTime, NightModeStopTime))
				{
					if(isinNightMode == false)
					{
						isinNightMode = true;
						PoolClockDisplays->setGlobalBrightness(nightModeBrightness);
					}
				}
				else
				{
					if(isinNightMode == true)
					{
						isinNightMode = false;
						PoolClockDisplays->setGlobalBrightness(clockBrightness);
					}
				}
			#endif
			LOG_D(TAG, "PoolClockDisplays->displayTime... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			#if DISPLAY_FOR_SEPARATION_DOT > -1
				if(numDots > 0)
				{
					if(lastDotFlash + DOT_FLASH_INTERVAL <= millis())
					{
						lastDotFlash = millis();
						LOG_D(TAG, "PoolClockDisplays->flashSeparationDot... %d",numDots);
						PoolClockDisplays->flashSeparationDot(numDots);
						LOG_D(TAG, "PoolClockDisplays->flashSeparationDot...End");
					}
				}
			#endif
			#if LCD_SCREEN == true
				LCDScreen_Clock_Mode(timeM, temperature1, humidity1, temperature2, humidity2);
			#endif
		break;
		case ClockState::TIMER_MODE:
			currentTime = timeM->getRemainingTimerTime();
			LOG_I(TAG, "PoolClockDisplays->displayTimer... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
			#if LCD_SCREEN == true
				LCDScreen_Timer_Mode(timeM, true);
			#endif
		break;
		case ClockState::TIMER_NOTIFICATION:
			if(currentAlarmSignalState == true)
			{
				PoolClockDisplays->setGlobalBrightness(NOTIFICATION_BRIGHTNESS, false);
			}
			else
			{
				PoolClockDisplays->setGlobalBrightness(0, false);
			}
			currentAlarmSignalState = !currentAlarmSignalState;
			alarmToggleCount++;
			#if TIMER_FLASH_TIME == true
				PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			#else
				PoolClockDisplays->displayTime(0, 0);
			#endif
			if(alarmToggleCount >= TIMER_FLASH_COUNT)
			{
				PoolClockDisplays->setGlobalBrightness(clockBrightness);
				PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
				alarmToggleCount = 0;
				MainState = ClockState::CLOCK_MODE;
			}
		break;
		case ClockState::ALARM_NOTIFICATION:
			if(currentAlarmSignalState == true)
			{
				PoolClockDisplays->setGlobalBrightness(NOTIFICATION_BRIGHTNESS, false);
			}
			else
			{
				PoolClockDisplays->setGlobalBrightness(0, false);
			}
			currentAlarmSignalState = !currentAlarmSignalState;
			LOG_I(TAG, "PoolClockDisplays->displayAlarm... %d:%d:%d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			if(!timeM->isAlarmActive())
			{
				PoolClockDisplays->setGlobalBrightness(clockBrightness);
				MainState = ClockState::CLOCK_MODE;
			}
		break;
		default:
			break;
		}

	}
	LOG_D(TAG, "ClockState::handleStates() ... End");
}
