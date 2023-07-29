/**
 * \file Sensor_DS18B20.h
 * \author Yves Gaignard
 * \brief Header for class definition of the DS18B20 temperature sensor 
 */

#ifndef _SENSOR_DS18B20_H_
#define _SENSOR_DS18B20_H_

#include <Arduino.h>
#include <vector>
#include <DallasTemperature.h>

// Precision of temperature sensors
//----------------------------------
#define TEMPERATURE_PRECISION 10

class Sensor_DS18B20 {

  private: 
    static                        Sensor_DS18B20* _instance;
    DallasTemperature            _sensors;
    DallasTemperature::request_t _request;
    boolean                      _isInit=false;
    int                          _sensorNumber=0;
    std::vector<std::string>     _deviceAddressArray;
    std::vector<std::string>     _deviceNameArray;

    // Constructor
    Sensor_DS18B20();

  public:
    /**
    * \brief Get the instance of the Sensor_DS18B20 object or create it if it was not yet instantiated.
    *
    * \return Sensor_DS18B20* returns the address to the Sensor_DS18B20 object
    */
    static Sensor_DS18B20* getInstance();

    // Destructor
    ~Sensor_DS18B20();

    // operator ==
    //friend bool operator== (const DeviceAddr& a, const DeviceAddr& b);

    // initialization of the object (MANDATORY)
    void init(DallasTemperature& sensors);

    // return the number of devices found
    int getDeviceCount();

    // return a string containing the hexadecimal address of a device
    std::string getDeviceAddress(int deviceIndex);

    // Add a device in the array of the devices through its name (you can choose it but it must be unique) and its address
    int addDevice(std::string deviceName, std::string deviceAddress);

    // Get the device name by index
    std::string getDeviceNameByIndex(int idx);

    // Get the handle of the DallasTemperature object
    DallasTemperature& getSensors();

    // sends command for all devices on the bus to perform a temperature conversion
    void requestTemperatures();

    // Request temperature (float precision) for the sensor through its index in the array
    float getPreciseTempCByIndex(int idx);

    // Request temperature (int precision) for the sensor through its index in the array
    int getTempCByIndex(int idx);

    // Request temperature (float precision) for the sensor through its address
    float getPreciseTempCByAddress(std::string deviceAddress);

    // Request temperature (int precision) for the sensor through its address
    int getTempCByAddress(std::string deviceAddress);

    // Request temperature (float precision) for the sensor through its name
    float getPreciseTempCByName(std::string deviceName);

    // Request temperature (int precision) for the sensor through its name
    int getTempCByName(std::string deviceName);

    // function to convert a device address to a string
    static std::string deviceAddressToString(DeviceAddress deviceAddress);

    // function to convert a string to a device address
    template <typename T> 
    static T stringToDeviceAddress(std::string deviceAddressStr);
   
};

#endif
