/**
 * \file Sensor_LDR.h
 * \author Yves Gaignard
 * \brief Header for class definition of the Sensor_LDR Light Dependent Resistance sensor 
 */

#ifndef _Sensor_LDR_H_
#define _Sensor_LDR_H_

#include <Arduino.h>
/**
 * \brief The Sensor LDR is a Light Dependent Resistance sensor 
 * LDR (Light Dependent Resistor) as the name states is a special type of resistor that works on the photoconductivity principle means 
 * that resistance changes according to the intensity of light. Its resistance decreases with an increase in the intensity of light.
 * It is often used as a light sensor, light meter, Automatic street light, and in areas where we need to have light sensitivity. 
 * LDR is also known as a Light Sensor which can sense and measure light around it. 
 * It has two outputs: a digital output that can be either LOW or HIGH, and an analog output.
 * 
 * How It Works ? 
 * Regarding the DO pin:
 * - The LDR light sensor module has a potentiometer that allows you to adjust the sensitivity or threshold for detecting light.
 * - When the light intensity in the surrounding environment is above the set threshold (considered as light), the output of the sensor on the DO pin becomes LOW, and the DO-LED turns on.
 * - When the light intensity in the surrounding environment is below the set threshold (considered as dark), the output of the sensor on the DO pin becomes HIGH, and the DO-LED turns off.
 * 
 * Regarding the AO pin:
 * - The value read from the AO pin is inversely proportional to the light intensity in the surrounding environment. In other words, as the light intensity increases (brighter), the value on the AO pin decreases.
 * - Similarly, as the light intensity decreases (darker), the value on the AO pin increases.
 * 
 * It's important to note that adjusting the potentiometer does not affect the value on the AO pin.
 */
class Sensor_LDR
{
private:
    static        Sensor_LDR* _instance;
    // Digital Output data
    bool          _is_do_init            = false;
    int           _ldr_do_pin            = 0;   // Digital Output pin 
    int           _ldr_do_state          = LOW; // Current DO state (HIGH or LOW)
    unsigned long _do_time_delay         = 0;   // time delay before read again DO (in millis)
    unsigned long _last_do_state_HIGH    = 0;   // last time the DO was stated HIGH (in millis)
    unsigned long _last_do_state_LOW     = 0;   // last time the DO was stated LOW (in millis)
    unsigned long _last_do_changed_state = 0;   // last time the DO state changed (in millis)
    // Digital Output data
    bool          _is_ao_init            = false;
    int           _ldr_ao_pin            = 0;   // Analog Output pin 
    int           _ldr_ao_value          = 0;   // Current AO value
    unsigned long _ao_time_delay         = 0;   // time delay before read again AO (in millis)
    unsigned long _last_ao_value         = 0;   // last time the AO was red (in millis)

	Sensor_LDR();
public:

    /**
     * \brief Get the instance of the Sensor_LDR object or create it if it was not yet instantiated.
     *
     * \return Sensor_LDR* returns the address to the Sensor_LDR object
     */
    static Sensor_LDR* getInstance();

    /**
     * \brief Destroys the Sensor_LDR object and cause Sensor_LDR::getInstance to create a new object the next time it is called
     */
	~Sensor_LDR();

    /**
     * \brief initialize Sensor_LDR sensor with its Digital Output pin
     */
    bool initDO(int ldr_do_pin, unsigned long do_time_delay);

    /**
     * \brief Returns the state of DO sensor (HIGH or LOW)
     */
    int getDOState();

    /**
     * \brief Returns the last time in milliseconds the state of LDR DO sensor changed 
     *        from HIGH to LOW or LOW to HIGH
     */
    unsigned long getDOLastChangedState();

    /**
     * \brief Returns the true if light is detected in the last time delay provided at init from DO
     */
    bool isLightDetected();

    /**
     * \brief initialize Sensor_LDR sensor with its Analog Output pin
     */
    bool initAO(int ldr_ao_pin, unsigned long ao_time_delay);

    /**
     * \brief Returns the value of AO sensor
     */
    int getAOValue();

    /**
     * \brief Returns the last time in milliseconds the state of LDR AO sensor was red
     */
    unsigned long getAOLastChangedValue();

};

#endif
