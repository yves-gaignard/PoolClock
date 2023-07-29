/**
 * \file Sensor_HCSR501.cpp
 * \author Yves Gaignard
 * \brief Implementation of the class Sensor_HCSR501
 */

#include "Sensor_HCSR501.h"

/**
 * \brief The Sensor HC-SR501 is a Passive Infrared (PIR) sensor 
 *        The PIR sensor allows you to detect when a person or animal moves in or out of sensor range.
 */
Sensor_HCSR501* Sensor_HCSR501::_instance = nullptr;

Sensor_HCSR501::Sensor_HCSR501()
{
    _pir_pin    = 0;
    _is_init    = false;
    _pir_state  = false;
    _time_delay = 0;
    _last_state_true=0;
    _last_state_false=millis();
}


Sensor_HCSR501* Sensor_HCSR501::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new Sensor_HCSR501();
	}
	return _instance;
}

Sensor_HCSR501::~Sensor_HCSR501()
{
	_instance = nullptr;
}

bool Sensor_HCSR501::init(int pir_pin, uint64_t time_delay)
{
    _pir_pin = pir_pin;
    pinMode(_pir_pin, INPUT);     // declare sensor as input

    _time_delay = time_delay;
    _last_state_true = - time_delay;
    _is_init = true;
    this->getPIRState();

    return true;
}

bool Sensor_HCSR501::getPIRState()
{
    int val = digitalRead(_pir_pin);  // read input value
 
    if (val == HIGH)	// check if the input is HIGH
    {   
        if (! _pir_state) 
        { 
            _last_state_true=millis(); 
            _pir_state = true;
            //Serial.printf("Motion is detected at %u\n", _last_state_true);
        }         
    } 
    else 
    {
        if (_pir_state) 
        { 
            _last_state_false=millis(); 
            _pir_state = false;
        }         
    }
    return _pir_state;
}

bool Sensor_HCSR501::isMotionDetected()
{
    this->getPIRState();
    uint64_t currentMillis = millis();
    if ( _pir_state || (currentMillis - _last_state_true < _time_delay ) )
        return true;
    else
        return false;
}