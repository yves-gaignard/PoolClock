/**
 * \file PoolClockCmd.cpp
 * \author Yves Gaignard
 * \brief Implementation of member function of PoolClockCmd 
 */

#define TAG "PoolClockCmd"

#include "PoolClockCmd.h"
#include "LogManager.h"

/**
 * \note if you use a different controller make sure to change the include here
 */
#include <BlynkSimpleEsp32.h>

PoolClockCmd* PoolClockCmd::_instance = nullptr;
PoolClockCmd* ClockUI = PoolClockCmd::getInstance();
ClockState*   ClockS  = ClockState::getInstance();
TimeManager*  TimeM   = TimeManager::getInstance();

#if LCD_SCREEN == true
	/**
	 * \brief The PoolClock screens function declarations
	*/
	extern void LCDScreen_Clock_Mode(TimeManager* currentTime, float temperature1, float humidity1, float temperature2, float humidity2);
	extern void LCDScreen_Timer_Mode(TimeManager* currentTimer, bool isTimerStarted);
	extern void LCDScreen_Set_Timer (TimeManager* currentTimer, LCDScreen_BlinkingDigit digitCursor);
#endif

/**
 * \brief Construct a new Blynk Config object. Also populate all variables of the class with meaningful values
 *
 */
PoolClockCmd::PoolClockCmd()
{
	_PoolClockDisplays = DisplayManager::getInstance();
	_timeM             = TimeManager::getInstance();
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

/**
 * \brief Destroy the Blynk Config object. Ensure proper deletion
 *
 */
PoolClockCmd::~PoolClockCmd()
{
	_instance = nullptr;
}

/**
 * \brief Either instantiate a new PoolClockCmd object by calling the private constructor and return it's address
 *        or it an instance of it already exists just return that.
 *
 * \return PoolClockCmd* address of the new/already existing blynk config object
 */
PoolClockCmd* PoolClockCmd::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new PoolClockCmd();
	}
	return _instance;
}

/**
 * \brief Terminates the command task running on the second core
 *
 */
void PoolClockCmd::stop()
{
	vTaskDelete(_PoolClockCmdLoop);
}

/**
 * \brief Figure out which changes have to be made in the UI to represent the correct state
 *
 */
void PoolClockCmd::changeSelection(ColorSelector selector, bool state)
{
	if(state == true)
	{
		_ColorSelection |= selector;
	}
	else
	{
		_ColorSelection &= ~selector;
	}
	if(ClockUI->_ColorSelection == PoolClockCmd::CHANGE_HOURS_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_HourColor.r, ClockUI->_HourColor.g, ClockUI->_HourColor.b);
	}
	if(ClockUI->_ColorSelection == PoolClockCmd::CHANGE_MINUTES_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_MinuteColor.r, ClockUI->_MinuteColor.g, ClockUI->_MinuteColor.b);
	}
	if(ClockUI->_ColorSelection == PoolClockCmd::CHANGE_INTERIOR_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_InternalColor.r, ClockUI->_InternalColor.g, ClockUI->_InternalColor.b);
	}
	if(ClockUI->_ColorSelection == PoolClockCmd::CHANGE_DOT_COLOR)
	{
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_DotColor.r, ClockUI->_DotColor.g, ClockUI->_DotColor.b);
	}
}

/**
 * \brief Code for the second thread running on the second core of the ESP handling all the blynk code since
 *        all of it is coded in a blocking way and we don't want to influence the animation smoothness
 *
 */
void PoolClockCmd::PoolClockCmdLoopCode(void* pvParameters)
{
	Transitions_enum transition;
	LOG_I(TAG, "Loop task running on core %d", xPortGetCoreID());
	PoolClockCmd* ClockUI = PoolClockCmd::getInstance();
	esp_task_wdt_init(30, false);
	for(;;)
	{
		transition = ClockUI->read_buttons();
		if ( transition != NONE && ClockUI->_last_transition == NONE) {
			ClockUI->_last_transition = transition;
			LOG_I(TAG, "Set _last_transition to: %d", transition);
		}
	    //ClockUI->state_machine_run(transition);
 
		#if IS_BLYNK_ACTIVE == true
			Blynk.run();
			if(ClockUI->UIUpdateRequired == true)
			{
				ClockUI->UIUpdateRequired = false;
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
void PoolClockCmd::setup()
{
	// Setup for each button
	_ModeButton-> begin();
	_PlayButton-> begin();
	_PlusButton-> begin();
	_MinusButton->begin();
	
	// Default current state
    _current_state  = CLOCK; 
    _previous_state = CLOCK;
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
    _ModeButton-> onPressed(PoolClockCmd::Mode_onPressed);
    _ModeButton-> onPressedFor(LONG_PRESS_TIME, PoolClockCmd::Mode_onPressedForDuration);
    _PlayButton-> onPressed(PoolClockCmd::Play_onPressed);
    _PlayButton-> onPressedFor(LONG_PRESS_TIME, PoolClockCmd::Play_onPressedForDuration);
    _PlusButton-> onPressed(PoolClockCmd::Plus_onPressed);
    _PlusButton-> onPressedFor(LONG_PRESS_TIME, PoolClockCmd::Plus_onPressedForDuration);
    _MinusButton->onPressed(PoolClockCmd::Minus_onPressed);
    _MinusButton->onPressedFor(LONG_PRESS_TIME, PoolClockCmd::Minus_onPressedForDuration);

	LOG_I(TAG, "Starting PoolClockCmd on core 0...");
	//Setup the loop task on the second core
	xTaskCreatePinnedToCore(
	PoolClockCmdLoopCode,	// Task function.
	"PoolClockCmdLoopCode",	// name of task.
	10000,			        // Stack size of task
	NULL,		    	    // parameter of the task
	1,				        // priority of the task
	&_PoolClockCmdLoop,	    // Task handle to keep track of created task
	0);				        // pin task to core 0
}

/**
 * \brief Callback to manage Mode button action in case of short press
 */
void PoolClockCmd::Mode_onPressed() {
    LOG_I(TAG, "Mode button has been pressed!");
    _Mode_button_state = PRESSED;
}

/**
 * \brief Callback to manage Mode button action in case of long press
 */
void PoolClockCmd::Mode_onPressedForDuration() {
    LOG_I(TAG, "Mode button has been pressed for the given duration!");
    _Mode_button_state = LONG_PRESSED;
}

void PoolClockCmd::Play_onPressed() {
    LOG_I(TAG, "Play button has been pressed!");
    _Play_button_state = PRESSED;
}

void PoolClockCmd::Play_onPressedForDuration() {
    LOG_I(TAG, "Play button has been pressed for the given duration!");
    _Play_button_state = LONG_PRESSED;
}
void PoolClockCmd::Plus_onPressed() {
    LOG_I(TAG, "Plus button has been pressed!");
    _Plus_button_state = PRESSED;
}

void PoolClockCmd::Plus_onPressedForDuration() {
    LOG_I(TAG, "Plus button has been pressed for the given duration!");
    _Plus_button_state = LONG_PRESSED;
}
void PoolClockCmd::Minus_onPressed() {
    LOG_I(TAG, "Minus button has been pressed!");
    _Minus_button_state = PRESSED;
}

void PoolClockCmd::Minus_onPressedForDuration() {
    LOG_I(TAG, "Minus button has been pressed for the given duration!");
    _Minus_button_state = LONG_PRESSED;
}

/**
 * @brief Manage the state of the machine and launch actions depending on transitions
 * 
 * @param Transitions_enum transition
 */
void PoolClockCmd::state_machine_run(Transitions_enum transition) 
{
	LOG_I(TAG, "PoolClockCmd::state_machine_run - Transition: %d",transition);
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
        case CLOCK:
			if(transition == MODE){
				ChgModeToTimer();
				_current_state = TIMER;
			}
			else if(transition == LONG_MODE){
				ChgModeToSetTimer();
				_current_state = SET_TIMER;
			}
			else{
				NOPE();
				_current_state = CLOCK;
			}
        	break;
        
        case TIMER:
			if(transition == MODE){
				ChgModeToClock();
				_current_state = CLOCK;
			}
			else if(transition == LONG_MODE){
				ChgModeToSetTimer();
				_current_state = SET_TIMER;
			}
			else if(transition == PLAY){
				StartPauseResumeTimer();
				_current_state = TIMER;
			}
			else if(transition == LONG_PLAY){
				CancelTimer();
				_current_state = TIMER;
			}
			else{
				NOPE();
				_current_state = TIMER;
			}
        	break;
    
        case SET_TIMER:
			if(transition == MODE){
				ValidateSetTimer();
				_current_state = TIMER;
			}
			else if(transition == LONG_MODE){
				ChgModeToClock();
				_current_state = CLOCK;
			}
			else if(transition == PLAY ){
				MoveNextDigit();
				_current_state = SET_TIMER;
			}
			else if(transition == LONG_PLAY ){
				ValidateSetTimer();
				_current_state = TIMER;
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
                case CLOCK:     LOG_I(TAG, "New current state = CLOCK");               break;
                case TIMER:     LOG_I(TAG, "New current state = TIMER");               break;
                case SET_TIMER: LOG_I(TAG, "New current state = SET_TIMER");           break;
            }
        }
    }
}
 
void PoolClockCmd::NOPE()
{
    LOG_V(TAG, "Action: NOPE");
}

void PoolClockCmd::ChgModeToTimer()
{
    LOG_I(TAG, "Action: Change Mode To Timer");
	ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
	ClockS->switchMode(ClockState::TIMER_MODE);
	_TimerState = STOPPED;
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, false);
	#endif	
}

void PoolClockCmd::ChgModeToSetTimer()
{
    LOG_I(TAG, "Action: Change Mode To Set Timer");
	ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
	ClockS->switchMode(ClockState::SET_TIMER);
	_timeM->setTimerDuration(_TimerDuration);
	_CurrentTimerDigit = MINUTE_DIGIT;
	_lcd_blinking_digit = LowMinute;
	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen_Set_Timer from ChgModeToSetTimer - cursor on: %d", _lcd_blinking_digit);
		LCDScreen_Set_Timer(_timeM, _lcd_blinking_digit);  
	#endif	

}

void PoolClockCmd::ChgModeToClock()
{
    LOG_I(TAG, "Action: Change Mode To Clock");
	ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
	ClockS->switchMode(ClockState::CLOCK_MODE);
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

void PoolClockCmd::StartPauseResumeTimer()
{
    LOG_I(TAG, "Action: Start Pause Resume Timer");
	bool isTimerStarted = false;
	if (_TimerState == STOPPED)
	{
		TimeM->startTimer();
		LOG_I(TAG, "Timer Started");
		_TimerState = STARTED;
		isTimerStarted = true;
	}
	else if (_TimerState == STARTED)
	{
		TimeM->stopTimer();
		LOG_I(TAG, "Timer Paused");
		_TimerState = PAUSED;
		isTimerStarted = false;
	}
	else if (_TimerState == PAUSED)
	{
		TimeM->startTimer();
		LOG_I(TAG, "Timer Resumed");
		_TimerState = STARTED;
		isTimerStarted = true;
	}
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, isTimerStarted);
	#endif	
}

void PoolClockCmd::CancelTimer()
{
    LOG_I(TAG, "Action: Cancel Timer");
    //delay(500);
	if (_TimerState == STARTED)
	{
		TimeM->stopTimer();
		LOG_I(TAG, "Timer Stopped");
	}
	_TimerState = STOPPED;
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, false);
	#endif	
}

void PoolClockCmd::CancelSetTimer()
{
    LOG_I(TAG, "Action: Cancel Set Timer");
	_TimerDuration.hours   = 0;
	_TimerDuration.minutes = 0;
	_TimerDuration.seconds = 0;
	TimeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, false);
	#endif	
}

void PoolClockCmd::ValidateSetTimer()
{
    LOG_I(TAG, "Action: Validate Set Timer");
	LOG_D(TAG, "Timer Duration: %d:%d:%d", _TimerDuration.hours, _TimerDuration.minutes, _TimerDuration.seconds);
	TimeM->setTimerDuration(_TimerDuration);
	#if LCD_SCREEN == true
		LCDScreen_Timer_Mode(_timeM, false);
	#endif	
}

void PoolClockCmd::MoveNextDigit()
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

void PoolClockCmd::IncrementDigit()
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

void PoolClockCmd::IncrementQuicklyDigit()
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

void PoolClockCmd::DecrementDigit()
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

void PoolClockCmd::DecrementQuicklyDigit()
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

Transitions_enum PoolClockCmd::read_buttons()
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
void PoolClockCmd::print_button_state(const char* button_name, Button_State_enum button_state) 
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
void PoolClockCmd::updateUI()
{
	_UIUpdateRequired = true;
}

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
 * \brief Handle a brigness change by the user through the blynk app
 *
 */
BLYNK_WRITE(BLYNK_CHANNEL_BRIGHTNESS_SLIDER)
{
	ClockS->_clockBrightness = param[0].asInt();
	ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
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
		ClockUI->_ColorSelection = PoolClockCmd::CHANGE_HOURS_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_HourColor.r, ClockUI->_HourColor.g, ClockUI->_HourColor.b);
		break;
	case 2:
		ClockUI->_ColorSelection = PoolClockCmd::CHANGE_MINUTES_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_MinuteColor.r, ClockUI->_MinuteColor.g, ClockUI->_MinuteColor.b);
		break;
	case 3:
		ClockUI->_ColorSelection = PoolClockCmd::CHANGE_INTERIOR_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_InternalColor.r, ClockUI->_InternalColor.g, ClockUI->_InternalColor.b);
		break;
	case 4:
		ClockUI->_ColorSelection = PoolClockCmd::CHANGE_DOT_COLOR;
		Blynk.virtualWrite(BLYNK_CHANNEL_CURRENT_COLOR_PICKER, ClockUI->_DotColor.r, ClockUI->_DotColor.g, ClockUI->_DotColor.b);
		break;
	}
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_HOURS)
{
	ClockUI->changeSelection(PoolClockCmd::CHANGE_HOURS_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_MINUTES)
{
	ClockUI->changeSelection(PoolClockCmd::CHANGE_MINUTES_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_INTERIOR)
{
	ClockUI->changeSelection(PoolClockCmd::CHANGE_INTERIOR_COLOR, param.asInt());
}

BLYNK_WRITE(BLYNK_CHANNEL_SELECTOR_DOT)
{
	ClockUI->changeSelection(PoolClockCmd::CHANGE_DOT_COLOR, param.asInt());
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
	if(ClockUI->_ColorSelection & PoolClockCmd::CHANGE_HOURS_COLOR)
	{
		ClockUI->_PoolClockDisplays->setHourSegmentColors(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_HOUR_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		ClockUI->_HourColor = currentColor;
	}
	if(ClockUI->_ColorSelection & PoolClockCmd::CHANGE_MINUTES_COLOR)
	{
		ClockUI->_PoolClockDisplays->setMinuteSegmentColors(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_MINUTE_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		ClockUI->_MinuteColor = currentColor;
	}
	if(ClockUI->_ColorSelection & PoolClockCmd::CHANGE_INTERIOR_COLOR)
	{
		ClockUI->_PoolClockDisplays->setInternalLEDColor(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_INTERNAL_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		ClockUI->_InternalColor = currentColor;
	}
	if(ClockUI->_ColorSelection & PoolClockCmd::CHANGE_DOT_COLOR)
	{
		ClockUI->_PoolClockDisplays->setDotLEDColor(currentColor);
		Blynk.virtualWrite(BLYNK_CHANNEL_DOT_COLOR_SAVE, currentColor.r, currentColor.g, currentColor.b);
		ClockUI->_DotColor = currentColor;
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
	ClockUI->_PoolClockDisplays->setHourSegmentColors(SavedColor);
	ClockUI->_HourColor = SavedColor;
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
	ClockUI->_PoolClockDisplays->setMinuteSegmentColors(SavedColor);
	ClockUI->_MinuteColor = SavedColor;
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
	ClockUI->_PoolClockDisplays->setInternalLEDColor(SavedColor);
	ClockUI->_InternalColor = SavedColor;
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
	ClockUI->_PoolClockDisplays->setDotLEDColor(SavedColor);
	ClockUI->_DotColor = SavedColor;
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
		ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_clockBrightness);
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
		ClockUI->_PoolClockDisplays->setGlobalBrightness(ClockS->_nightModeBrightness);
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
		ClockUI->_isClearAction = true;
	}
	else if(ClockUI->_isClearAction == true)
	{
		ClockUI->_isClearAction = false;
	}
	else
	{
		TimeM->setAlarmMode(param[0].asInt() == 1);
	}
}
















 






