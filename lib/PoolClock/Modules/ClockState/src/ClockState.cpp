/**
 * \file ClockState.cpp
 * \author Yves Gaignard
 * \brief Implementation of the ClockState class member functions
 */

#define TAG "ClockState"

#include "ClockState.h"
#include "LogManager.h"
/**
 * \note if you use a different controller make sure to change the include here
 */
#include <BlynkSimpleEsp32.h>
ClockState*   ClockS  = ClockState::getInstance();

#if LCD_SCREEN == true
	/**
	 * \brief The PoolClock screens function declarations
	*/
	extern void LCDScreen_Clock_Mode(TimeManager* currentTime, float temperature1, float humidity1, float temperature2, float humidity2);
	extern void LCDScreen_Timer_Mode(TimeManager* currentTimer, Timer_State_enum timerState);
	extern void LCDScreen_Set_Timer (TimeManager* currentTimer, LCDScreen_BlinkingDigit digitCursor);
#endif

ClockState* ClockState::_instance = nullptr;

ClockState::ClockState()
{
	_current_state = CLOCK_MODE;
	_previous_state= CLOCK_MODE;
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
	_isClearAction     = false;
	_ColorSelection    = CHANGE_HOURS_COLOR;
	_UIUpdateRequired  = false;

	_ModeButton = new EasyButton(BUTTON_MODE_PIN ,   30U, false, false);
	_PlusButton = new EasyButton(BUTTON_PLUS_PIN ,   30U, false, false);
	_PlayButton = new EasyButton(BUTTON_PLAY_PIN ,   30U, false, false);
	_MinusButton= new EasyButton(BUTTON_MINUS_PIN,   30U, false, false);

	_Mode_button_state  = NOT_PRESSED;
	_Play_button_state  = NOT_PRESSED;
	_Plus_button_state  = NOT_PRESSED;
	_Minus_button_state = NOT_PRESSED;
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
    if(newState == CLOCK_MODE)
    {
        _alarmToggleCount = 0;
    }
	LOG_D(TAG, "ClockState::switchMode() ... From: %d To: %d", _current_state, newState);
	_previous_state= _current_state;
    _current_state = newState;
}

ClockStates ClockState::getMode()
{
    return _current_state;
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
		switch (_current_state)
		{
		case CLOCK_MODE:
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
		case TIMER_MODE:
			currentTime = _timeM->getRemainingTimerTime();
			LOG_D(TAG, "PoolClockDisplays->displayTimer... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
			#if LCD_SCREEN == true
				LCDScreen_Timer_Mode(_timeM, _TimerState);
			#endif
		break;
		case SET_TIMER:
			currentTime = _timeM->getRemainingTimerTime();
			LOG_D(TAG, "PoolClockDisplays->displaySetTimer... %02d:%02d:%02d",currentTime.hours, currentTime.minutes, currentTime.seconds);
			_PoolClockDisplays->displayTimer(currentTime.hours, currentTime.minutes, currentTime.seconds);
			#if LCD_SCREEN == true
				LOG_D(TAG, "ClockState from SET_TIMER - cursor on: %d - address: %p", _lcd_blinking_digit, (void*)&_lcd_blinking_digit);
				LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
			#endif
		break;
		case TIMER_NOTIFICATION:
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
				_previous_state = _current_state;
				_current_state = CLOCK_MODE;
			}
		break;
		case ALARM_NOTIFICATION:
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
				_previous_state = _current_state;
				_current_state = CLOCK_MODE;
			}
		break;
		default:
			break;
		}

	}
	LOG_V(TAG, "ClockState::handleStates() ... End");
}

/**
 * \brief Terminates the command task running on the second core
 *
 */
void ClockState::stop()
{
	vTaskDelete(_ClockStateLoop);
}

/**
 * \brief Figure out which changes have to be made in the UI to represent the correct state
 *
 */
void ClockState::changeSelection(ColorSelector selector, bool state)
{
	if(state == true)
	{
		_ColorSelection |= selector;
	}
	else
	{
		_ColorSelection &= ~selector;
	}
	if(_ColorSelection == ClockState::CHANGE_HOURS_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _HourColor.r, _HourColor.g, _HourColor.b);
	}
	if(_ColorSelection == ClockState::CHANGE_MINUTES_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _MinuteColor.r, _MinuteColor.g, _MinuteColor.b);
	}
	if(_ColorSelection == ClockState::CHANGE_INTERIOR_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _InternalColor.r, _InternalColor.g, _InternalColor.b);
	}
	if(_ColorSelection == ClockState::CHANGE_DOT_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _DotColor.r, _DotColor.g, _DotColor.b);
	}
}

/**
 * \brief Code for the second thread running on the second core of the ESP handling all the blynk code since
 *        all of it is coded in a blocking way and we don't want to influence the animation smoothness
 *
 */
void ClockState::ClockStateLoopCode(void* pvParameters)
{
	Transitions_enum transition;
	LOG_I(TAG, "Loop task running on core %d", xPortGetCoreID());
	ClockState* ClockS = ClockState::getInstance();
	esp_task_wdt_init(30, false);
	for(;;)
	{
		transition = ClockS->read_buttons();
		if ( transition != NONE && ClockS->_last_transition == NONE) {
			ClockS->_last_transition = transition;
			LOG_I(TAG, "Set _last_transition to: %d", transition);
		}
	    //state_machine_run(transition);
 
		#if IS_BLYNK_ACTIVE == true
			Blynk.run();
			if(UIUpdateRequired == true)
			{
				UIUpdateRequired = false;
				if(ClockS->getMode() == ClockState::CLOCK_MODE)
				{
					Blynk.virtualWrite(BLYNK_CHANNEL_TIMER_START_BUTTON, 0);
				}
				else if(ClockS->getMode() == ClockState::ALARM_NOTIFICATION)
				{
					Blynk.setProperty(BLYNK_CHANNEL_ALARM_START_BUTTON, "onLabel", "Clear");
				}
				else
				{
					// TimeManager::TimeInfo currentTimerValue = timeM->getCurrentTime();
					// int time = currentTimerValue.hours * 3600 + currentTimerValue.minutes * 60 + currentTimerValue.seconds;
					// Blynk.virtualWrite(BLYNK_CHANNEL_TIMER_START_BUTTON, time, 0);
				}
			}
		#endif
		esp_task_wdt_reset();
		//delay(1); //needed to serve the wdg
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}


/**
 * \brief Initialize buttons and start the separate thread on the second core
 *
 */
void ClockState::setup()
{
	// Setup for each button
	_ModeButton-> begin();
	_PlayButton-> begin();
	_PlusButton-> begin();
	_MinusButton->begin();
	
	// Default current state
    _current_state  = CLOCK_MODE; 
    _previous_state = CLOCK_MODE;
	_last_transition= NONE;

	// Default Timer Duration
	_TimerDuration.hours   = 0;
	_TimerDuration.minutes = 0;
	_TimerDuration.seconds = 0;

	// Default Timer State
	_TimerState = STOPPED;

	// Default Timer Digit
	_CurrentTimerDigit = HOUR_DIGIT;

	#if AIR_TEMP_SENSOR == true
		_am232x = Sensor_AM232X::getInstance();
	#endif
	#if WATER_TEMP_SENSOR == true
		_DS18B20Sensors = Sensor_DS18B20::getInstance();
	#endif

    // callback declaration
    _ModeButton-> onPressed(ClockState::Mode_onPressed);
    _ModeButton-> onPressedFor(LONG_PRESS_TIME, ClockState::Mode_onPressedForDuration);
    _PlayButton-> onPressed(ClockState::Play_onPressed);
    _PlayButton-> onPressedFor(LONG_PRESS_TIME, ClockState::Play_onPressedForDuration);
    _PlusButton-> onPressed(ClockState::Plus_onPressed);
    _PlusButton-> onPressedFor(LONG_PRESS_TIME, ClockState::Plus_onPressedForDuration);
    _MinusButton->onPressed(ClockState::Minus_onPressed);
    _MinusButton->onPressedFor(LONG_PRESS_TIME, ClockState::Minus_onPressedForDuration);

	LOG_I(TAG, "Starting ClockStateLoopCode on core 0...");
	//Setup the loop task on the second core
	xTaskCreatePinnedToCore(
	ClockStateLoopCode,	    // Task function.
	"ClockStateLoopCode",	// name of task.
	10000,			        // Stack size of task
	NULL,		    	    // parameter of the task
	1,				        // priority of the task
	&_ClockStateLoop,	    // Task handle to keep track of created task
	0);				        // pin task to core 0
}

/**
 * \brief Callback to manage Mode button action in case of short press
 */
void ClockState::Mode_onPressed() {
    LOG_I(TAG, "Mode button has been pressed!");
    _Mode_button_state = PRESSED;
}

/**
 * \brief Callback to manage Mode button action in case of long press
 */
void ClockState::Mode_onPressedForDuration() {
    LOG_I(TAG, "Mode button has been pressed for the given duration!");
    _Mode_button_state = LONG_PRESSED;
}

void ClockState::Play_onPressed() {
    LOG_I(TAG, "Play button has been pressed!");
    _Play_button_state = PRESSED;
}

void ClockState::Play_onPressedForDuration() {
    LOG_I(TAG, "Play button has been pressed for the given duration!");
    _Play_button_state = LONG_PRESSED;
}
void ClockState::Plus_onPressed() {
    LOG_I(TAG, "Plus button has been pressed!");
    _Plus_button_state = PRESSED;
}

void ClockState::Plus_onPressedForDuration() {
    LOG_I(TAG, "Plus button has been pressed for the given duration!");
    _Plus_button_state = LONG_PRESSED;
}
void ClockState::Minus_onPressed() {
    LOG_I(TAG, "Minus button has been pressed!");
    _Minus_button_state = PRESSED;
}

void ClockState::Minus_onPressedForDuration() {
    LOG_I(TAG, "Minus button has been pressed for the given duration!");
    _Minus_button_state = LONG_PRESSED;
}

/**
 * @brief Manage the state of the machine and launch actions depending on transitions
 * 
 * @param Transitions_enum transition
 */
void ClockState::state_machine_run(Transitions_enum transition) 
{
	LOG_I(TAG, "ClockState::state_machine_run - Transition: %d",transition);
    switch(transition)
    {
        case NONE:              LOG_V(TAG, "buttons = NONE");               break;
        case MODE:              LOG_I(TAG, "buttons = MODE");               break;
        case LONG_MODE:         LOG_I(TAG, "buttons = LONG_MODE");          break;
        case PLAY:              LOG_I(TAG, "buttons = PLAY");               break;
        case LONG_PLAY:         LOG_I(TAG, "buttons = LONG_PLAY");          break;
        case PLUS:              LOG_I(TAG, "buttons = PLUS");               break;
        case LONG_PLUS:         LOG_I(TAG, "buttons = LONG_PLUS");          break;
        case MINUS:             LOG_I(TAG, "buttons = MINUS");              break;
        case LONG_MINUS:        LOG_I(TAG, "buttons = LONG_MINUS");         break;
    }
    
    switch(_current_state)
    {
        case CLOCK_MODE:
			if(transition == MODE){
				ChgModeToTimer();
				_current_state = TIMER_MODE;
			}
			else if(transition == LONG_MODE){
				ChgModeToSetTimer();
				_current_state = SET_TIMER;
			}
			else{
				NOPE();
				_current_state = CLOCK_MODE;
			}
        	break;
        
        case TIMER_MODE:
			if(transition == MODE){
				ChgModeToClock();
				_current_state = CLOCK_MODE;
			}
			else if(transition == LONG_MODE){
				ChgModeToSetTimer();
				_current_state = SET_TIMER;
			}
			else if(transition == PLAY){
				StartPauseResumeTimer();
				_current_state = TIMER_MODE;
			}
			else if(transition == LONG_PLAY){
				CancelTimer();
				_current_state = TIMER_MODE;
			}
			else{
				NOPE();
				_current_state = TIMER_MODE;
			}
        	break;
    
        case SET_TIMER:
			if(transition == MODE){
				ValidateSetTimer();
				_current_state = TIMER_MODE;
			}
			else if(transition == LONG_MODE){
				ChgModeToClock();
				_current_state = CLOCK_MODE;
			}
			else if(transition == PLAY ){
				MoveNextDigit();
				_current_state = SET_TIMER;
			}
			else if(transition == LONG_PLAY ){
				ValidateSetTimer();
				_current_state = TIMER_MODE;
			}
			else if(transition == PLUS ){
				IncrementDigit();
				_current_state = SET_TIMER;
			}
			else if(transition == LONG_PLUS ){
				IncrementQuicklyDigit();
				_current_state = SET_TIMER;
			}
			else if(transition == MINUS ){
				DecrementDigit();
				_current_state = SET_TIMER;
			}
			else if(transition == LONG_MINUS ){
				DecrementQuicklyDigit();
				_current_state = SET_TIMER;
			}
			else{
				NOPE();
				_current_state = SET_TIMER;
			}
			break;
    }

    if (_previous_state != _current_state) 
    {
        _previous_state = _current_state;
        {
            switch(_current_state)
            {
                case CLOCK_MODE: LOG_I(TAG, "New current state = CLOCK_MODE"); break;
                case TIMER_MODE: LOG_I(TAG, "New current state = TIMER_MODE"); break;
                case SET_TIMER:  LOG_I(TAG, "New current state = SET_TIMER");  break;
            }
        }
    }
}
 
void ClockState::NOPE()
{
    LOG_V(TAG, "Action: NOPE");
}

void ClockState::ChgModeToTimer()
{
    LOG_I(TAG, "Action: Change Mode To Timer");
	_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
	switchMode(TIMER_MODE);
	_TimerState = STOPPED;
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, _TimerState);
	#endif	
}

void ClockState::ChgModeToSetTimer()
{
    LOG_I(TAG, "Action: Change Mode To Set Timer");
	_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
	switchMode(SET_TIMER);
	_timeM->setTimerDuration(_TimerDuration);
	_CurrentTimerDigit = MINUTE_DIGIT;
	_lcd_blinking_digit = LowMinute;
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from ChgModeToSetTimer - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	

}

void ClockState::ChgModeToClock()
{
    LOG_I(TAG, "Action: Change Mode To Clock");
	_PoolClockDisplays->setGlobalBrightness(_clockBrightness);
	switchMode(CLOCK_MODE);
	float temperature1=0.0;
	float humidity1=0.0;
	float temperature2=0.0;
	float humidity2=0.0;
	#if AIR_TEMP_SENSOR == true
		temperature1=_am232x->getTemperature();
		humidity1=_am232x->getHumidity();
	#endif
	#if WATER_TEMP_SENSOR == true
		temperature2=_DS18B20Sensors->getPreciseTempCByAddress(waterThermometerAddress);
	#endif
	#if LCD_SCREEN == true
		LCDScreen_Clock_Mode(_timeM, temperature1, humidity1, temperature2, humidity2);
	#endif	
}

void ClockState::StartPauseResumeTimer()
{
    LOG_I(TAG, "Action: Start Pause Resume Timer");
	if (_TimerState == STOPPED)
	{
		_timeM->startTimer();
		LOG_I(TAG, "Timer Started");
		_TimerState = RUNNING;
	}
	else if (_TimerState == RUNNING)
	{
		_timeM->stopTimer();
		LOG_I(TAG, "Timer Paused");
		_TimerState = PAUSED;
	}
	else if (_TimerState == PAUSED)
	{
		_timeM->startTimer();
		LOG_I(TAG, "Timer Resumed");
		_TimerState = RUNNING;
	}
	else if (_TimerState == CANCELLED)
	{
		_timeM->startTimer();
		LOG_I(TAG, "Timer Restarted");
		_TimerState = RUNNING;
	}
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, _TimerState);
	#endif	
}

void ClockState::CancelTimer()
{
    LOG_I(TAG, "Action: Cancel Timer");
	_timeM->disableTimer();
	LOG_I(TAG, "Timer Cancelled");
	_TimerState = CANCELLED;
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, _TimerState);
	#endif	
}

void ClockState::CancelSetTimer()
{
    LOG_I(TAG, "Action: Cancel Set Timer");
	_TimerDuration.hours   = 0;
	_TimerDuration.minutes = 0;
	_TimerDuration.seconds = 0;
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, _TimerState);
	#endif	
}

void ClockState::ValidateSetTimer()
{
    LOG_I(TAG, "Action: Validate Set Timer");
	LOG_D(TAG, "Timer Duration: %d:%d:%d", _TimerDuration.hours, _TimerDuration.minutes, _TimerDuration.seconds);
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, _TimerState);
	#endif	
}

void ClockState::MoveNextDigit()
{
	if (_CurrentTimerDigit == HOUR_DIGIT) 
	{
		_CurrentTimerDigit = MINUTE_DIGIT;
	    LOG_I(TAG, "Action: Move Digit to MINUTES");
		_lcd_blinking_digit=LowMinute;
	}
	else
	{
		_CurrentTimerDigit = HOUR_DIGIT;
	    LOG_I(TAG, "Action: Move Digit to HOURS");
		_lcd_blinking_digit=LowHour;
	}
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LOG_D(TAG, "LCDScreen_Set_Timer from MoveNextDigit - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	
}

void ClockState::IncrementDigit()
{
    LOG_I(TAG, "Action: Increment Digit");
	if (_CurrentTimerDigit == HOUR_DIGIT)  {
		_TimerDuration.hours++;
		if (_TimerDuration.hours >= 24) _TimerDuration.hours = 0;
		_lcd_blinking_digit=LowHour;
	}
	else {
		_TimerDuration.minutes++;
		if (_TimerDuration.minutes >= 60) _TimerDuration.minutes = 0;
		_lcd_blinking_digit=LowMinute;
	}
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from IncrementDigit - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	
}

void ClockState::IncrementQuicklyDigit()
{
    LOG_I(TAG, "Action: Increment Quickly Digit");
		if (_CurrentTimerDigit == HOUR_DIGIT)  {
		_TimerDuration.hours = _TimerDuration.hours + 10;
		if (_TimerDuration.hours >= 24) _TimerDuration.hours = 0;
		_lcd_blinking_digit=HighHour;
	}
	else {
		_TimerDuration.minutes = _TimerDuration.minutes + 10;
		if (_TimerDuration.minutes >= 60) _TimerDuration.minutes = 0;
		_lcd_blinking_digit=HighMinute;
	}
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from IncrementQuicklyDigit - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	
}

void ClockState::DecrementDigit()
{
    LOG_I(TAG, "Action: Decrement Digit");
	if (_CurrentTimerDigit == HOUR_DIGIT)  {
		if (_TimerDuration.hours > 0) _TimerDuration.hours--;
		_lcd_blinking_digit=LowHour;
	}
	else {
		if (_TimerDuration.minutes > 0) _TimerDuration.minutes--;
		_lcd_blinking_digit=LowMinute;
	}
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from DecrementDigit - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	
}

void ClockState::DecrementQuicklyDigit()
{
    LOG_I(TAG, "Action: Decrement Quickly Digit");
	if (_CurrentTimerDigit == HOUR_DIGIT)  {
		if (_TimerDuration.hours >= 10) _TimerDuration.hours = _TimerDuration.hours -10;
		else _TimerDuration.hours = 0;
		_lcd_blinking_digit=HighHour;
	}
	else {
		if (_TimerDuration.minutes >= 10) _TimerDuration.minutes = _TimerDuration.minutes -10;
		else _TimerDuration.minutes = 0;
		_lcd_blinking_digit=HighMinute;
	}
	_timeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from DecrementQuicklyDigit - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	
}

Transitions_enum ClockState::read_buttons()
{
    //code for reading both buttons
    _ModeButton->read();
	_PlayButton->read();
	_PlusButton->read();
	_MinusButton->read();
    
    print_button_state("MODE ", _Mode_button_state);
    if      (_Mode_button_state == PRESSED)      { _Mode_button_state = NOT_PRESSED; return MODE; }
    else if (_Mode_button_state == LONG_PRESSED) { _Mode_button_state = NOT_PRESSED; return LONG_MODE; }

    print_button_state("PLAY ", _Play_button_state);
    if      (_Play_button_state == PRESSED)      { _Play_button_state = NOT_PRESSED; return PLAY; }
    else if (_Play_button_state == LONG_PRESSED) { _Play_button_state = NOT_PRESSED; return LONG_PLAY; }

    print_button_state("PLUS ", _Plus_button_state);
    if      (_Plus_button_state == PRESSED)      { _Plus_button_state = NOT_PRESSED; return PLUS; }
    else if (_Plus_button_state == LONG_PRESSED) { _Plus_button_state = NOT_PRESSED; return LONG_PLUS; }

    print_button_state("MINUS", _Minus_button_state);
    if      (_Minus_button_state == PRESSED)      { _Minus_button_state = NOT_PRESSED; return MINUS; }
    else if (_Minus_button_state == LONG_PRESSED) { _Minus_button_state = NOT_PRESSED; return LONG_MINUS; }

    return NONE;
}

/**
 * @brief Print the state of a push button
 * 
 * @param button_name  : name of the button
 * @param button_state : state of the button
 */
void ClockState::print_button_state(const char* button_name, Button_State_enum button_state) 
{
    const char* state_not_pressed           = "NOT_PRESSED";
    const char* state_pressed               = "PRESSED";
    const char* state_long_pressed          = "LONG_PRESSED";
 
#if DEBUG == false
    switch(button_state)
    {
        case NOT_PRESSED:          LOG_V(TAG, "button = %s   status= %s", button_name, state_not_pressed);          break;
        case PRESSED:              LOG_I(TAG, "button = %s - status= %s", button_name, state_pressed);              break;
        case LONG_PRESSED:         LOG_I(TAG, "button = %s - status= %s", button_name, state_long_pressed);         break;
    }
#endif
}
/**
 * \brief Notify the Blynk thread that a UI update is needed.
 *        What exactly needs to be updated will be figured out in the thread loop itself.
 *
 */
void ClockState::updateUI()
{
	_UIUpdateRequired = true;
}

#if IS_BLYNK_ACTIVE == true

/**
 * \brief Sync all channels as soon as blynk connects to the server
 *        Setup default UI states
 *
 */
BLYNK_CONNECTED()
{
	Blynk.syncVirtual(BLYNK_CHANNEL_BRIGHTNESS_SLIDER);
	Blynk.syncVirtual(BLYNK_CHANNEL_LIGHT_GROUP_SELECTOR);
	Blynk.syncVirtual(BLYNK_CHANNEL_CURRENT_COLOR_PICKER);
	Blynk.syncVirtual(BLYNK_CHANNEL_TIMER_TIME_INPUT);
	Blynk.syncVirtual(BLYNK_CHANNEL_HOUR_COLOR_SAVE);
	Blynk.syncVirtual(BLYNK_CHANNEL_MINUTE_COLOR_SAVE);
	Blynk.syncVirtual(BLYNK_CHANNEL_INTERNAL_COLOR_SAVE);
	Blynk.syncVirtual(BLYNK_CHANNEL_DOT_COLOR_SAVE);
	Blynk.syncVirtual(BLYNK_CHANNEL_NIGHT_MODE_BRIGHTNESS);
	Blynk.syncVirtual(BLYNK_CHANNEL_NIGHT_MODE_TIME_INPUT);
	Blynk.syncVirtual(BLYNK_CHANNEL_NUM_SEPARATION_DOTS);
	Blynk.syncVirtual(BLYNK_CHANNEL_ALARM_START_BUTTON);
	Blynk.syncVirtual(BLYNK_CHANNEL_ALARM_TIME_INPUT);
	Blynk.syncVirtual(BLYNK_CHANNEL_SELECTOR_HOURS);
	Blynk.syncVirtual(BLYNK_CHANNEL_SELECTOR_MINUTES);
	Blynk.syncVirtual(BLYNK_CHANNEL_SELECTOR_INTERIOR);
	Blynk.syncVirtual(BLYNK_CHANNEL_SELECTOR_DOT);
	Blynk.virtualWrite(BLYNK_CHANNEL_TIMER_START_BUTTON, 0);
}

/**
 * \brief Handle a brightness change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_BRIGHTNESS_SLIDER)
{
	ClockState::_clockBrightness = param[0].asInt();
	ClockState::_PoolClockDisplays->setGlobalBrightness(ClockState::_clockBrightness);
}

/**
 * \brief Handle the change of the light group selector by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_LIGHT_GROUP_SELECTOR)
{
	switch (param.asInt())
	{
	case 1:
		_ColorSelection = ClockState::CHANGE_HOURS_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _HourColor.r, _HourColor.g, _HourColor.b);
		break;
	case 2:
		_ColorSelection = ClockState::CHANGE_MINUTES_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _MinuteColor.r, _MinuteColor.g, _MinuteColor.b);
		break;
	case 3:
		_ColorSelection = ClockState::CHANGE_INTERIOR_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _InternalColor.r, _InternalColor.g, _InternalColor.b);
		break;
	case 4:
		_ColorSelection = ClockState::CHANGE_DOT_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, _DotColor.r, _DotColor.g, _DotColor.b);
		break;
	}
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_HOURS)
{
	changeSelection(ClockState::CHANGE_HOURS_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_MINUTES)
{
	changeSelection(ClockState::CHANGE_MINUTES_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_INTERIOR)
{
	changeSelection(ClockState::CHANGE_INTERIOR_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_DOT)
{
	changeSelection(ClockState::CHANGE_DOT_COLOR, param.asInt());
}

/**
 * \brief Handle a color change for the currently selected light group by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_CURRENT_COLOR_PICKER)
{
	CRGB currentColor;
	currentColor.r  = param[0].asInt();
	currentColor.g  = param[1].asInt();
	currentColor.b  = param[2].asInt();
	if(_ColorSelection & ClockState::CHANGE_HOURS_COLOR)
	{
		_PoolClockDisplays->setHourSegmentColors(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_HOUR_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		_HourColor = currentColor;
	}
	if(_ColorSelection & ClockState::CHANGE_MINUTES_COLOR)
	{
		_PoolClockDisplays->setMinuteSegmentColors(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_MINUTE_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		_MinuteColor = currentColor;
	}
	if(_ColorSelection & ClockState::CHANGE_INTERIOR_COLOR)
	{
		_PoolClockDisplays->setInternalLEDColor(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_INTERNAL_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		_InternalColor = currentColor;
	}
	if(_ColorSelection & ClockState::CHANGE_DOT_COLOR)
	{
		_PoolClockDisplays->setDotLEDColor(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_DOT_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		_DotColor = currentColor;
	}
}

/**
 * \brief Use an unused virtual channel to permanently store the color for the hours light group to be able
 *        to restore it's state after a reset.
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_HOUR_COLOR_SAVE)
{
	CRGB SavedColor;
	SavedColor.r  = param[0].asInt();
	SavedColor.g  = param[1].asInt();
	SavedColor.b  = param[2].asInt();
	_PoolClockDisplays->setHourSegmentColors(SavedColor);
	_HourColor = SavedColor;
}

/**
 * \brief Use an unused virtual channel to permanently store the color for the minutes light group to be able
 *        to restore it's state after a reset.
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_MINUTE_COLOR_SAVE)
{
	CRGB SavedColor;
	SavedColor.r  = param[0].asInt();
	SavedColor.g  = param[1].asInt();
	SavedColor.b  = param[2].asInt();
	_PoolClockDisplays->setMinuteSegmentColors(SavedColor);
	_MinuteColor = SavedColor;
}

/**
 * \brief Use an unused virtual channel to permanently store the color for the internal light group to be able
 *        to restore it's state after a reset.
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_INTERNAL_COLOR_SAVE)
{
	CRGB SavedColor;
	SavedColor.r  = param[0].asInt();
	SavedColor.g  = param[1].asInt();
	SavedColor.b  = param[2].asInt();
	_PoolClockDisplays->setInternalLEDColor(SavedColor);
	_InternalColor = SavedColor;
}

/**
 * \brief Use an unused virtual channel to permanently store the color for the dot light group to be able
 *        to restore it's state after a reset.
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_DOT_COLOR_SAVE)
{
	CRGB SavedColor;
	SavedColor.r  = param[0].asInt();
	SavedColor.g  = param[1].asInt();
	SavedColor.b  = param[2].asInt();
	_PoolClockDisplays->setDotLEDColor(SavedColor);
	_DotColor = SavedColor;
}

/**
 * \brief Handle a timer duration change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_TIMER_TIME_INPUT)
{
	TimeManager::TimeInfo TimerDuration;
	TimeInputParam t(param);
	TimerDuration.hours = t.getStartHour();
	TimerDuration.minutes = t.getStartMinute();
	TimerDuration.seconds = t.getStartSecond();
	LOG_D(TAG, "StartTime: %d:%d:%d\n\r", TimerDuration.hours, TimerDuration.minutes, TimerDuration.seconds);
	TimeM->setTimerDuration(TimerDuration);
}

/**
 * \brief Handle the start of the timer by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_TIMER_START_BUTTON)
{
	if(param[0].asInt() == 1)
	{
		TimeM->startTimer();
		LOG_I(TAG, "Timer Started");
		ClockS->switchMode(ClockState::TIMER_MODE);
	}
	else
	{
		TimeM->stopTimer();
		LOG_I(TAG, "Timer Stopped");
		Blynk.syncVirtual(BLYNK_CHANNEL_TIMER_TIME_INPUT);
		_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
		ClockS->switchMode(ClockState::CLOCK_MODE);
	}
}

/**
 * \brief Handle the nightime window setting change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_NIGHT_MODE_TIME_INPUT)
{
	TimeInputParam t(param);
	ClockS->_NightModeStartTime.hours = t.getStartHour();
	ClockS->_NightModeStartTime.minutes = t.getStartMinute();
	ClockS->_NightModeStartTime.seconds = t.getStartSecond();
	ClockS->_NightModeStopTime.hours = t.getStopHour();
	ClockS->_NightModeStopTime.minutes = t.getStopMinute();
	ClockS->_NightModeStopTime.seconds = t.getStopSecond();
}

/**
 * \brief Handle the nighttime brightness setting change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_NIGHT_MODE_BRIGHTNESS)
{
	ClockS->_nightModeBrightness = param[0].asInt();
	if(TimeM->isInBetween(ClockS->_NightModeStartTime, ClockS->_NightModeStopTime))
	{
		_PoolClockDisplays->setGlobalBrightness(ClockS->_nightModeBrightness);
	}
}

/**
 * \brief Handle the change of the dots display setting by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_NUM_SEPARATION_DOTS)
{
	ClockS->_numDots = param[0].asInt() - 1;
}

/**
 * \brief Handle an alarm time change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_ALARM_TIME_INPUT)
{
	TimeInputParam t(param);
	TimeManager::TimeInfo AlarmTime
	{
		.hours 		= (uint8_t)t.getStartHour(),
		.minutes 	= (uint8_t)t.getStartMinute(),
		.seconds 	= (uint8_t)t.getStartSecond()
	};
	uint8_t activeDays = 0x00;
	for (uint8_t i = 0; i < 7; i++)
	{
		if (t.isWeekdaySelected(i + 1)) // 1 to 7 for every weekday in blynk config
		{
			activeDays |= (1 << i);
		}
	}
	TimeM->setAlarmTime(AlarmTime, (TimeManager::Weekdays)activeDays);
}

/**
 * \brief Handle the activation/deactivation/clearing of the alarm function by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_ALARM_START_BUTTON)
{
	if(ClockS->getMode() == ClockState::ALARM_NOTIFICATION)
	{
		Blynk.setProperty(BLYNK_CHANNEL_ALARM_START_BUTTON, "onLabel", "Deactivate");
		TimeM->clearAlarm();
		Blynk.virtualWrite(BLYNK_CHANNEL_ALARM_START_BUTTON, 1);
		_isClearAction = true;
	}
	else if(_isClearAction == true)
	{
		_isClearAction = false;
	}
	else
	{
		TimeM->setAlarmMode(param[0].asInt() == 1);
	}
}
#endif