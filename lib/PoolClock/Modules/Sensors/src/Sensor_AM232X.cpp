/**
 * \file Sensor_AM232X.cpp
 * \author Yves Gaignard
 * \brief Implementation of the class Sensor_AM232X
 */

#include "Sensor_AM232X.h"

/**
 * \brief The Sensor_AM232X is responsible to get temperature and humidity
 *        This sensor is using I2C protocol to communicate
 */

Sensor_AM232X* Sensor_AM232X::_instance = nullptr;

Sensor_AM232X::Sensor_AM232X()
{
    _sda_pin    = 0;
    _scl_pin    = 0;
    _frequency  = 100000ul;
    _is_init    = false;
	_temperature= AM232X_TEMPERATURE_OUT_OF_RANGE;
	_humidity   = AM232X_HUMIDITY_OUT_OF_RANGE;
    _read_status= -1;
    _last_read  = 0;
    _read_frequency = 500;


	_I2CDHT = new TwoWire(1);   //  I2C bus
	_AM232X = new AM232X(_I2CDHT);
}


Sensor_AM232X* Sensor_AM232X::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new Sensor_AM232X();
	}
	return _instance;
}

Sensor_AM232X::~Sensor_AM232X()
{
	_instance = nullptr;
}

bool Sensor_AM232X::init(int sda_pin, int scl_pin, uint32_t read_frequency)
{
    _sda_pin   = sda_pin;
    _scl_pin   = scl_pin;
    _read_frequency = read_frequency;

    Serial.println("Initialization of AM232X Sensor");

    if (! _I2CDHT->begin(_sda_pin, _scl_pin, _frequency) ) 
    {
        Serial.println("Cannot initialize I2C AM232X Sensor");
        return false;
    } 

	if (! _AM232X->begin() )
    {
        Serial.println("Sensor AM232X not found");
        return false;
    }

	if (! _AM232X->wakeUp() )
    {
        Serial.println("Sensor AM232X not connected");
        return false;
    }
    _is_init = true;
    Serial.println("Initialization OK");
    return true;
}

float Sensor_AM232X::getTemperature()
{
    return _temperature;
}

float Sensor_AM232X::getHumidity()
{
    return _humidity;
}

void Sensor_AM232X::handle()
{
    if (_is_init)
    {
    	uint64_t currentMillis = millis();
        if (_last_read + _read_frequency < currentMillis)
        {
            int status = _AM232X->read();
            switch (status)
            {
                case AM232X_OK:
                    //Serial.println("AM232X sensor read status = OK");
                break;
                default:
                    Serial.print("AM232X sensor read status = ");
                    Serial.println(status);
                break;
            }
            float humidity = _AM232X->getHumidity();
            if (humidity != AM232X_INVALID_VALUE ) { _humidity = humidity; }
            float temperature = _AM232X->getTemperature();
            if (temperature != AM232X_INVALID_VALUE ) { _temperature = temperature; }

            //Serial.print("AM232X sensor temperature = ");
            //Serial.println(_temperature);
            //Serial.print("AM232X sensor humidity = ");
            //Serial.println(_humidity);

            _last_read = millis();
        }
    }

}
