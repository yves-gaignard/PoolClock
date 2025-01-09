/**
 * \file ClockState.cpp
 * \author Yves Gaignard
 * \brief Implementation of the ClockState class member functions
 */

#define TAG "ClockState"

#include "ClockState.h"
#include "LogManager.h"

#if LCD_SCREEN == true
	/**
	 * \brief The PoolClock screens function declarations
	*/
	extern void LCDScreen_Clock_Mode(TimeManager* currentTime, float temperature1, float humidity1, float temperature2, float humidity2);
	extern void LCDScreen_Timer_Mode(TimeManager* currentTimer, bool isTimerStarted);
	extern void LCDScreen_Set_Timer (TimeManager* currentTimer, LCDScreen_BlinkingDigit digitCursor);
#endif

ClockState* ClockState::_instance = nullptr;

ClockState::ClockState()
{
	_MainState = CLOCK_MODE;
	_clockBrightness = DEFAULT_CLOCK_BRIGHTNESS;
	_alarmToggleCount = 0;
	_nightModeBrightness = DEFAULT_NIGHT_MODE_BRIGHTNESS;
	_numDots = NUM_SEPARATION_DOTS;

	_NightModeStartTime = TimeManager::TimeInfo {DEFAULT_NIGHT_MODE_START_HOUR, DEFAULT_NIGHT_MODE_START_MINUTE, 0};
	_NightModeStopTime = TimeManager::TimeInfo {DEFAULT_NIGHT_MODE_END_HOUR, DEFAULT_NIGHT_MODE_END_MINUTE, 0};

	_lastUpdateMillis = millis();
	_lastDotFlash = millis();
	_currentAlarmSignalState = false;
	_isinNightMode = false;
	_timeM = TimeManager::getInstance();
	_PoolClockDisplays = DisplayManager::getInstance();
#if AIR_TEMP_SENSOR == true
	_am232x = Sensor_AM232X::getInstance();
#endif
#if WATER_TEMP_SENSOR == true
	_DS18B20Sensors = Sensor_DS18B20::getInstance();
#endif
}

ClockState::~ClockState()
{
	_instance = nullptr;
}

ClockState* ClockState::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new ClockState();
	}
	return _instance;
}

void ClockState::switchMode(ClockStates newState)
{
    if(newState == ClockState::CLOCK_MODE)
    {
        _alarmToggleCount = 0;
    }
	LOG_D(TAG, "ClockState::switchMode() ... From: %d To: %d", _MainState, newState);
    _MainState = newState;
}

ClockState::ClockStates ClockState::getMode()
{
    return _MainState;
}

void ClockState::handleStates()
{
	LOG_V(TAG, "ClockState::handleStates() ... Start");
	if(_lastUpdateMillis + TIME_UPDATE_INTERVAL <= millis()) // update the display only in a certain interval
	{
		// ========================
		// Display of temperature
		// ========================
		float temperature1=0.0;
		float humidity1=0.0;
		float temperature2=0.0;
		float humidity2=0.0;
	
		#if AIR_TEMP_SENSOR == true
			_am232x->handle();
			temperature1=_am232x->getTemperature();
			humidity1=_am232x->getHumidity();
		#endif
		#if WATER_TEMP_SENSOR == true
			temperature2=_DS18B20Sensors->getPreciseTempCByAddress(waterThermometerAddress);
		#endif
		_PoolClockDisplays->displayTemperature(temperature1, humidity1, temperature2, humidity2);

		// =================================
		// Display of clock, timer or alarm
		// =================================
		_lastUpdateMillis = millis();
		TimeManager::TimeInfo currentTime;
		currentTime = _timeM->getCurrentTime();
		switch (_MainState)
		{
		case ClockState::CLOCK_MODE:
			#if USE_NIGHT_MODE == true
				if(_timeM->isInBetween(_NightModeStartTime, _NightModeStopTime))
				{
					if(_isinNightMode == false)
					{
						_isinNightMode = true;
						_PoolClockDisplays->setGlobalBrightness(_nightModeBrightness);
					}
				}
				else
				{
					if(_isinNightMode == true)
					{
						_isinNightMode = false;
						_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
					}
				}
			#endif
			LOG_D(TAG, "PoolClockDisplays->displayTime... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			#if DISPLAY_FOR_SEPARATION_DOT > -1
				if(_numDots > 0)
				{
					if(_lastDotFlash + DOT_FLASH_INTERVAL <= millis())
					{
						_lastDotFlash = millis();
						LOG_D(TAG, "PoolClockDisplays->flashSeparationDot... %d",_numDots);
						_PoolClockDisplays->flashSeparationDot(_numDots);
						LOG_D(TAG, "PoolClockDisplays->flashSeparationDot...End");
					}
				}
			#endif
			#if LCD_SCREEN == true
				LCDScreen_Clock_Mode(_timeM, temperature1, humidity1, temperature2, humidity2);
			#endif
		break;
		case ClockState::TIMER_MODE:
			currentTime = _timeM->getRemainingTimerTime();
			LOG_D(TAG, "PoolClockDisplays->displayTimer... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
			#if LCD_SCREEN == true
				LCDScreen_Timer_Mode(_timeM, true);
			#endif
		break;
		case ClockState::SET_TIMER:
			currentTime = _timeM->getRemainingTimerTime();
			LOG_D(TAG, "PoolClockDisplays->displaySetTimer... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
			#if LCD_SCREEN == true
				LOG_D(TAG, "ClockState from SET_TIMER - cursor on: %d - address: %p", _lcd_blinking_digit, (void*)&_lcd_blinking_digit);
				LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
			#endif
		break;
		case ClockState::TIMER_NOTIFICATION:
			if(_currentAlarmSignalState == true)
			{
				_PoolClockDisplays->setGlobalBrightness(NOTIFICATION_BRIGHTNESS, false);
			}
			else
			{
				_PoolClockDisplays->setGlobalBrightness(0, false);
			}
			_currentAlarmSignalState = !_currentAlarmSignalState;
			_alarmToggleCount++;
			#if TIMER_FLASH_TIME == true
				_PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			#else
				_PoolClockDisplays->displayTime(0, 0);
			#endif
			if(_alarmToggleCount >= TIMER_FLASH_COUNT)
			{
				_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
				_PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
				_alarmToggleCount = 0;
				_MainState = ClockState::CLOCK_MODE;
			}
		break;
		case ClockState::ALARM_NOTIFICATION:
			if(_currentAlarmSignalState == true)
			{
				_PoolClockDisplays->setGlobalBrightness(NOTIFICATION_BRIGHTNESS, false);
			}
			else
			{
				_PoolClockDisplays->setGlobalBrightness(0, false);
			}
			_currentAlarmSignalState = !_currentAlarmSignalState;
			LOG_I(TAG, "PoolClockDisplays->displayAlarm... %d:%d:%d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTime(currentTime.hours, currentTime.minutes);
			if(!_timeM->isAlarmActive())
			{
				_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
				_MainState = ClockState::CLOCK_MODE;
			}
		break;
		default:
			break;
		}

	}
	LOG_V(TAG, "ClockState::handleStates() ... End");
}
