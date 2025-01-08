/**
 * \file Sensor_AM232X.h
 * \author Yves Gaignard
 * \brief Header for class definition of the AM23X temperature and humidity sensor 
 */

#ifndef _SENSOR_AM232X_H_
#define _SENSOR_AM232X_H_

#include "AM232X.h"

/**
 * \brief The Sensor_AM232X is responsible to get temperature and humidity
 *        This sensor is using I2C protocol to communicate
 */
class Sensor_AM232X
{
private:
    static   Sensor_AM232X* _instance;

	TwoWire* _I2CDHT;   //  I2C bus
	AM232X*  _AM232X;
    int      _sda_pin;
    int      _scl_pin;
    uint32_t _frequency;
    bool     _is_init = false;
	float    _temperature;
	float    _humidity;
    int      _read_status;
    uint64_t _last_read;
    uint32_t _read_frequency;


	Sensor_AM232X();
public:

    /**
     * \brief Get the instance of the Sensor_AM232X object or create it if it was not yet instantiated.
     *
     * \return Sensor_AM232X* returns the address to the Sensor_AM232X object
     */
    static Sensor_AM232X* getInstance();

    /**
     * \brief Destroys the Sensor_AM232X object and cause #Sensor_AM232X::getInstance to create a new object the next time it is called
     */
	~Sensor_AM232X();

    /**
     * \brief initialize AM232X sensor
     * \param sda_pin   : Pin number of SDA
     * \param scl_pin   : Pin number of SCL
     * \param frequency : frequency of the measure in milliseconds 
     */
    bool init(int sda_pin, int scl_pin, uint32_t frequency = 5000ul);

    /**
     * \brief Returns the current temperature
     */
    float getTemperature();

    /**
     * \brief Returns the current humidity
     */
    float getHumidity();

    /**
     * \brief Has to be called periodically to read the sensor
     *
     */
	void handle();
};

#endif
