/**
 * \file Sensor_HCSR501.h
 * \author Yves Gaignard
 * \brief Header for class definition of the Sensor_HC_SR501 PIR Motion sensor 
 */

#ifndef _SENSOR_HCSR501_H_
#define _SENSOR_HCSR501_H_

#include <Arduino.h>
/**
 * \brief The Sensor HC-SR501 is a Passive Infrared (PIR) sensor 
 *        The PIR sensor allows you to detect when a person or animal moves in or out of sensor range.
 */
class Sensor_HCSR501
{
private:
    static   Sensor_HCSR501* _instance;
    int      _pir_pin = 0;
    bool     _is_init = false;
    bool     _pir_state = false;
    uint64_t _time_delay = 0;
    uint64_t _last_state_true = 0;
    uint64_t _last_state_false = 0;

	Sensor_HCSR501();
public:

    /**
     * \brief Get the instance of the Sensor_HCSR501 object or create it if it was not yet instantiated.
     *
     * \return Sensor_HCSR501* returns the address to the Sensor_AM232X object
     */
    static Sensor_HCSR501* getInstance();

    /**
     * \brief Destroys the Sensor_AM232X object and cause #Sensor_AM232X::getInstance to create a new object the next time it is called
     */
	~Sensor_HCSR501();

    /**
     * \brief initialize HC-SR501 sensor
     */
    bool init(int pir_pin, uint64_t time_delay);

    /**
     * \brief Returns the state of PIR sensor
     */
    bool getPIRState();

    /**
     * \brief Returns the true if motion is detected in the last time delay provided at init
     */
    bool isMotionDetected();

};

#endif
