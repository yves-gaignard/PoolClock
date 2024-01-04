/**
 * \file Sensor_AM232X.cpp
 * \author Yves Gaignard
 * \brief The Sensor_AM232X is responsible to mesure temperature and humidity 
 *        This sensor uses I2C protocol to communicate
 */
#define TAG "Sensor_AM232X"

#include "Sensor_AM232X.h"
#include "LogManager.h"

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
    _read_frequency = 1000;  // in ms


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

    LOG_I(TAG, "Initialization of AM232X Sensor");

    if (! _I2CDHT->begin(_sda_pin, _scl_pin, _frequency) ) 
    {
        LOG_E(TAG, "Cannot initialize I2C AM232X Sensor");
        return false;
    } 

	if (! _AM232X->begin() )
    {
        LOG_E(TAG, "Sensor AM232X not found");
        return false;
    }

	if (! _AM232X->wakeUp() )
    {
        LOG_E(TAG, "Sensor AM232X not connected");
        return false;
    }
    _is_init = true;
    LOG_I(TAG, "Initialization OK");
    LOG_I(TAG, "Sensor AM232X SDA PIN = %d",_sda_pin);
    LOG_I(TAG, "Sensor AM232X SCL PIN = %d",_scl_pin);
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
                  LOG_D(TAG, "AM232X sensor read status = OK");
                  break;
                default:
                  LOG_E(TAG, "AM232X sensor read status = %d", status);
                  break;
            }
            float humidity = _AM232X->getHumidity();
            if (humidity != AM232X_INVALID_VALUE ) { _humidity = humidity; }
            float temperature = _AM232X->getTemperature();
            if (temperature != AM232X_INVALID_VALUE ) { _temperature = temperature; }

            LOG_D(TAG, "AM232X sensor temperature = %4.2f", _temperature);
            LOG_D(TAG, "AM232X sensor humidity    = %4.2f", _humidity);

            _last_read = millis();
        }
    }

}
