/**
 * \file Sensor_DS18B20.cpp
 * \author Yves Gaignard
 * \brief The Sensor_DS18B20 is responsible to get temperature
 *        This sensor uses one-wire protocol to communicate
 */

#define TAG "Sensor_DS18B20"

// Standard library definitions
#include <Arduino.h>

#include "Sensor_DS18B20.h"
#include "LogManager.h"
#include "Utilities.h"

Sensor_DS18B20* Sensor_DS18B20::_instance = nullptr;

Sensor_DS18B20::Sensor_DS18B20() {
}

Sensor_DS18B20* Sensor_DS18B20::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new Sensor_DS18B20();
	}
	return _instance;
}

Sensor_DS18B20::~Sensor_DS18B20(){
}

void Sensor_DS18B20::init (DallasTemperature & sensors) {
  _sensors = sensors;
  _sensors.begin(); // begin method needs to be called twice if you use OneWire library on platformio for versions above 2.3.2 (excluded) 
  _sensors.begin(); // (the by pass is reported on this issue https://github.com/platformio/platform-espressif32/issues/253 )

  _isInit=true;
  _deviceNameArray.reserve(3);
  _deviceAddressArray.reserve(3);
}

int Sensor_DS18B20::getDeviceCount() {
  if (_isInit) {
    _sensorNumber = _sensors.getDeviceCount();
    // LOG_D(TAG, "%d temperature sensors found", _sensorNumber);
    return _sensorNumber;
  } 
  else{
    LOG_E(TAG, "TempManager object not initialized");
    return -1;
  }
}

std::string Sensor_DS18B20::getDeviceAddress(int deviceIndex) {
  std::string   deviceAddressStr;
  DeviceAddress deviceAddress;
  if (_isInit) {
    if (!_sensors.getAddress(deviceAddress, deviceIndex)) {
      LOG_E(TAG, "Device: %d not found", deviceIndex);
    } 
    deviceAddressStr = deviceAddressToString(deviceAddress);
  } 
  else{
    LOG_E(TAG, "TempManager object not initialized");
  }

  // LOG_D(TAG, "Device address : %s", deviceAddressStr.c_str());
  return deviceAddressStr;
}

int Sensor_DS18B20::addDevice(std::string deviceName, std::string deviceAddress) {
  if (_isInit) {
    if (std::find(_deviceNameArray.begin(), _deviceNameArray.end(), deviceName) != _deviceNameArray.end() ) {
      LOG_E(TAG, "%s temperature sensor name already exist on the table of sensor name", deviceName.c_str());
      return 2;
    }
    else { 
      if (std::find(_deviceAddressArray.begin(), _deviceAddressArray.end(), deviceAddress) != _deviceAddressArray.end() ) {
        LOG_E(TAG, "%s temperature sensor address already exist on the table of sensor addr", deviceAddress.c_str());
        return 3;
      }
      else {
        LOG_D(TAG, "Add temperature sensor: %s with the address: %s", deviceName.c_str(), deviceAddress.c_str());
        _deviceNameArray.push_back(deviceName);
        _deviceAddressArray.push_back(deviceAddress); 
      }
    }
  } 
  else {
    LOG_E(TAG, "TempManager object not initialized");
    return 1;
  }
  return 0;
}

// Get the device name by index
std::string Sensor_DS18B20::getDeviceNameByIndex(int idx) {
  std::string deviceName;
  if (_isInit) {
    if (idx >= 0 && idx <= _sensorNumber) {
      deviceName = _deviceNameArray[idx];
    } 
    else {
      LOG_E(TAG, "Bad index %d to get the device name", idx);
    }
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
  return deviceName;
}


DallasTemperature& Sensor_DS18B20::getSensors() {
  return _sensors;
}

// sends command for all devices on the bus to perform a temperature conversion
void Sensor_DS18B20::requestTemperatures() {
  if (_isInit) {
    _request = _sensors.requestTemperatures();
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
}

// Request temperature (float precision) for the sensor through its index in the array
float Sensor_DS18B20::getPreciseTempCByIndex(int idx) {
  float temperatureC;
  if (_isInit) {
    if (idx < 0 || idx >= _sensorNumber) {
      LOG_E(TAG, "Cannot get temperature for unknown sensor index: %d ", idx);
    }
    temperatureC = -127.0; // retry in case of failure to get temperature
    int max_try = 5; 
    int nb_try = 0;
    while (temperatureC == -127.0 && nb_try < max_try) {
      _sensors.setResolution(TEMPERATURE_PRECISION);
      temperatureC = _sensors.getTempCByIndex(idx);
      LOG_D(TAG, "Get temperature for index %d: %f ", idx, temperatureC);
      nb_try ++;
    }
    return temperatureC;
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
    temperatureC = -128.0;
    return temperatureC;
  }
}

// Request temperature (int precision) for the sensor through its index in the array
int Sensor_DS18B20::getTempCByIndex(int idx) {
  int temperatureC;
  float tempPreciseC;
  if (_isInit) {
    tempPreciseC= getPreciseTempCByIndex(idx);
    tempPreciseC= tempPreciseC + 0.5 - (tempPreciseC<0); 
    temperatureC = (int) tempPreciseC;
    return temperatureC;
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
    temperatureC = -128;
    return temperatureC;
  }
}

// Request temperature (float precision) for the sensor through its address
float Sensor_DS18B20::getPreciseTempCByAddress(std::string deviceAddress) {
  float temperatureC = -128.0;
  if (_isInit) {
    ptrdiff_t pos = find(_deviceAddressArray.begin(), _deviceAddressArray.end(), deviceAddress) - _deviceAddressArray.begin();
    if (pos >= _sensorNumber) {
      LOG_E(TAG, "Cannot get temperature for unknown sensor address: %s ", deviceAddress.c_str());
    }
    else {
      temperatureC = this->getPreciseTempCByIndex(pos);
      LOG_D(TAG, "Get temperature for name %s: %f ", deviceAddress.c_str(), temperatureC);
    }
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
  return temperatureC;
}

// Request temperature (int precision) for the sensor through its address
int Sensor_DS18B20::getTempCByAddress(std::string deviceAddress) {
  int temperatureC = -128;
  if (_isInit) {
    float tempPreciseC= getPreciseTempCByAddress(deviceAddress);
    tempPreciseC= tempPreciseC + 0.5 - (tempPreciseC<0); 
    temperatureC = (int) tempPreciseC;
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
  return temperatureC;
}

// Request temperature (float precision) for the sensor through its name
float Sensor_DS18B20::getPreciseTempCByName(std::string deviceName) {
  float temperatureC = -128.0;
  if (_isInit) {
    ptrdiff_t pos = find(_deviceNameArray.begin(), _deviceNameArray.end(), deviceName) - _deviceNameArray.begin();
    if (pos >= _sensorNumber) {
      LOG_E(TAG, "Cannot get temperature for unknown sensor name: %s ", deviceName.c_str());
    }
    else {
      temperatureC = this->getPreciseTempCByIndex(pos);
      LOG_D(TAG, "Get temperature for name %s: %f ", deviceName.c_str(), temperatureC);
    }
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
  return temperatureC;
}

// Request temperature (int precision) for the sensor through its name
int Sensor_DS18B20::getTempCByName(std::string deviceName) {
  int temperatureC = -128;
  if (_isInit) {
    float tempPreciseC= getPreciseTempCByName(deviceName);
    tempPreciseC= tempPreciseC + 0.5 - (tempPreciseC<0); 
    temperatureC = (int) tempPreciseC;
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
  return temperatureC;
}

// function to convert a device address to a string
std::string Sensor_DS18B20::deviceAddressToString(DeviceAddress deviceAddress) {
  std::string deviceAddressString;
  for (int i = 0; i < 8; i++)
  {  
    LOG_D(TAG, "deviceAddress[%d] = %d", i,  deviceAddress[i]);
    // zero pad the address if necessary
    deviceAddressString += n2hexstr(deviceAddress[i]);
    LOG_D(TAG, "deviceAddressString = %s", deviceAddressString.c_str());
  }
  if (deviceAddressString.size() != 16) {
    LOG_E(TAG, "deviceAddressString = %s length is not 16", deviceAddressString.c_str());
    return "";
  };
  return deviceAddressString;
}

// function to convert a string to a device address
template <typename T> 
T Sensor_DS18B20::stringToDeviceAddress(std::string deviceAddressStr) {
  DeviceAddress deviceAddress;
  for (int i = 0; i < 8; i++)
  { 
    long n = hexstr2n(deviceAddressStr.substr(i*2, 2));
    deviceAddress[i]=n;
    LOG_D(TAG, "deviceAddress[%d] = %d", i,  deviceAddress[i]);
  }
  return deviceAddress;
}
