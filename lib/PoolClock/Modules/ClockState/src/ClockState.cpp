#include "ClockState.h"

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
	//DBG Serial.println("ClockState::handleStates() ... Start");
	if(lastUpdateMillis + TIME_UPDATE_INTERVAL <= millis()) // update the display only in a certain intervall
	{
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
			Serial.printf("PoolClockDisplays->displayTime... %d:%d:%d\n",currentTime.hours, currentTime.minutes, currentTime.seconds);
			PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			#if DISPLAY_FOR_SEPARATION_DOT > -1
				if(numDots > 0)
				{
					if(lastDotFlash + DOT_FLASH_INTERVAL <= millis())
					{
						lastDotFlash = millis();
						//DBG Serial.printf("PoolClockDisplays->flashSeparationDot... %d\n",numDots);
						PoolClockDisplays->flashSeparationDot(numDots);
						//DBG Serial.println("PoolClockDisplays->flashSeparationDot...End");
					}
				}
			#endif
		break;
		case ClockState::TIMER_MODE:
			currentTime = timeM->getRemainingTimerTime();
			Serial.printf("PoolClockDisplays->displayTimer... %d:%d:%d\n",currentTime.hours, currentTime.minutes, currentTime.seconds);
			PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
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
			Serial.printf("PoolClockDisplays->displayAlarm... %d:%d:%d\n",currentTime.hours, currentTime.minutes, currentTime.seconds);
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
		// Display of temperature
		PoolClockDisplays->displayTemperature(34, 29);
	}
	//DBG Serial.println("ClockState::handleStates() ... End");
}
