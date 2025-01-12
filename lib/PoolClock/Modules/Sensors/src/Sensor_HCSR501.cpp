/**
 * \file Sensor_HCSR501.cpp
 * \author Yves Gaignard
 * \brief The Sensor HC-SR501 is a Passive Infrared (PIR) sensor 
 *        The PIR sensor allows you to detect when a person or animal moves in or out of sensor range.
 */
#define TAG "Sensor_HCSR501"

#include "Sensor_HCSR501.h"
#include "LogManager.h"

Sensor_HCSR501* Sensor_HCSR501::_instance = nullptr;

Sensor_HCSR501::Sensor_HCSR501()
{
    _pir_pin            = 0;
    _is_init            = false;
    _pir_state          = LOW;
    _time_delay         = 0;
    _last_state_HIGH    = 0;
    _last_state_LOW     = millis();
    _last_changed_state = _last_state_LOW;
}

Sensor_HCSR501* Sensor_HCSR501::getInstance() {
	if(_instance == nullptr)
	{
		_instance = new Sensor_HCSR501();
	}
	return _instance;
}

Sensor_HCSR501::~Sensor_HCSR501() {
	_instance = nullptr;
}

bool Sensor_HCSR501::init(int pir_pin, unsigned long time_delay) {
    _pir_pin = pir_pin;
    pinMode(_pir_pin, INPUT);     // declare sensor as input

    _time_delay = time_delay;
    LOG_D(TAG, "_time_delay is %lu", _time_delay);
    //_last_state_HIGH = - time_delay;
    _last_state_HIGH = 0;
    _is_init = true;
    this->getPIRState();

    return true;
}

int Sensor_HCSR501::getPIRState() {
    int val = digitalRead(_pir_pin);  // read input value
 
    if (val == HIGH)	// check if the input is HIGH
    {   
        _last_state_HIGH = millis(); 
        //LOG_V(TAG, "Val = HIGH");
        if (_pir_state == LOW) 
        { 
            _pir_state = HIGH;
            _last_changed_state = _last_state_HIGH;
            LOG_V(TAG, "Motion is detected at %lu", _last_changed_state);
        }         
    } 
    else 
    {
        _last_state_LOW = millis(); 
        //LOG_V(TAG, "Val = LOW");
        if (_pir_state == HIGH) 
        { 
            _pir_state = LOW;
            _last_changed_state = _last_state_LOW;
            LOG_V(TAG, "No more motion from %lu", _last_changed_state);
        }         
    }
    return _pir_state;
}

unsigned long Sensor_HCSR501::getLastChangedState() {
    return _last_changed_state;
}

bool Sensor_HCSR501::isMotionDetected() {
    this->getPIRState();
    unsigned long currentMillis = millis();
    if ( _pir_state || (currentMillis - _last_state_HIGH < _time_delay ) ) {
        unsigned long endStateHigh = _last_changed_state+_time_delay;
        LOG_V(TAG, "Motion is detected until %lu and it is only %lu", endStateHigh, currentMillis);
        return true;
    }
    else
        return false;
}