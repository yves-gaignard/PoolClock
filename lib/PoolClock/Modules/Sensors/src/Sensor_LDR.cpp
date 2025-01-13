/**
 * \file Sensor_LDR.cpp
 * \author Yves Gaignard
 * \brief The Sensor LDR is a Light Dependent Resistance sensor
 *        LDR is also known as a Light Sensor which can sense and measure light around it.
 */
#define TAG "Sensor_LDR"

#include "Sensor_LDR.h"
#include "LogManager.h"

Sensor_LDR* Sensor_LDR::_instance = nullptr;

Sensor_LDR::Sensor_LDR()
{
    // Digital Output data
    _is_do_init            = false;
    _ldr_do_pin            = 0;
    _ldr_do_state          = LOW;
    _do_time_delay         = 0;
    _last_do_state_LOW     = 0;
    _last_do_state_HIGH    = millis();
    _last_do_changed_state = _last_do_state_HIGH;
    // Digital Output data
    _is_ao_init            = false;
    _ldr_ao_pin            = 0;
    _ldr_ao_value          = 0;
    _ao_time_delay         = 0;
    _last_ao_value         = millis();
}

Sensor_LDR* Sensor_LDR::getInstance() {
	if(_instance == nullptr)
	{
		_instance = new Sensor_LDR();
	}
	return _instance;
}

Sensor_LDR::~Sensor_LDR() {
	_instance = nullptr;
}

bool Sensor_LDR::initDO(int ldr_do_pin, unsigned long do_time_delay) {
    _ldr_do_pin = ldr_do_pin;
    pinMode(ldr_do_pin, INPUT);     // declare sensor as input

    _do_time_delay = do_time_delay;
    LOG_D(TAG, "_do_time_delay is %lu", _do_time_delay);
    _last_do_state_HIGH = 0;
    _is_do_init = true;
    this->getDOState();

    return true;
}

int Sensor_LDR::getDOState() {
    int val = digitalRead(_ldr_do_pin);  // read input value
    // if value is LOW , it means that there is SOME light
    // if value is HIGH, it means that there is NO   light
 
    if (val == LOW)	// it means that there is some light
    {   
        LOG_V(TAG, "LDR value is LOW");
        _last_do_state_LOW = millis(); 
        if (_ldr_do_state == HIGH) 
        { 
            _ldr_do_state = LOW;
            _last_do_changed_state = _last_do_state_LOW;
            LOG_V(TAG, "LDR state has been changed to LOW at %lu", _last_do_changed_state);
        }         
    } 
    else // it means that there is no light
    {
        LOG_V(TAG, "LDR value is HIGH");
        _last_do_state_HIGH = millis(); 
        if (_ldr_do_state == LOW) 
        { 
            _ldr_do_state = HIGH;
            _last_do_changed_state = _last_do_state_HIGH;
            LOG_V(TAG, "LDR state has been changed to HIGH at %lu", _last_do_changed_state);
        }         
    }
    return _ldr_do_state;
}

unsigned long Sensor_LDR::getDOLastChangedState() {
    return _ldr_do_state;
}

bool Sensor_LDR::isLightDetected() {
    this->getDOState();
    unsigned long currentMillis = millis();
    if ( _ldr_do_state == LOW || (currentMillis - _last_do_state_LOW < _do_time_delay ) ) {
        unsigned long endStateLow = _last_do_changed_state+_do_time_delay;
        LOG_V(TAG, "LDR DO is HIGH until %lu and it is only %lu", endStateLow, currentMillis);
        return true;
    }
    else
        return false;
}

bool Sensor_LDR::initAO(int ldr_ao_pin, unsigned long ao_time_delay) {
    _ldr_ao_pin = ldr_ao_pin;

    _ao_time_delay = ao_time_delay;
    LOG_D(TAG, "_ao_time_delay is %lu", _ao_time_delay);
    _last_ao_value = millis();
    _is_ao_init = true;
    this->getAOValue();

    return true;

}

int Sensor_LDR::getAOValue() {
    int ldrValue =0;
    if ( _is_ao_init ) {
        ldrValue = analogRead(_ldr_ao_pin); // Reading Analog Output value
        _last_ao_value = millis(); 
    }
    return ldrValue;
}

unsigned long Sensor_LDR::getAOLastChangedValue() {
    return _last_ao_value;
}
