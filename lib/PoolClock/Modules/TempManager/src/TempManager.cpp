/**
 * \file TempManager.cpp
 * \author Yves Gaignard
 * \brief Temperature sensor management
 */
#define TAG "TempManager"

// Standard library definitions
#include <Arduino.h>

#include <TempManager.h>           // Management of temperature sensor
#include <LogManager.h>
#include <Utilities.h>

TempManager::TempManager() {
}

TempManager::~TempManager(){
}

void TempManager::init (DallasTemperature & sensors) {
  _sensors = sensors;
  _sensors.begin(); // begin method needs to be called twice if you use OneWire library on platformio for versions above 2.3.2 (excluded) 
  _sensors.begin(); // (the by pass is reported on this issue https://github.com/platformio/platform-espressif32/issues/253 )

  _isInit=true;
  _deviceNameArray.reserve(3);
  _deviceAddressArray.reserve(3);
}

int TempManager::getDeviceCount() {
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

std::string TempManager::getDeviceAddress(int deviceIndex) {
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

int TempManager::addDevice(std::string deviceName, std::string deviceAddress) {
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
std::string TempManager::getDeviceNameByIndex(int idx) {
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


DallasTemperature& TempManager::getSensors() {
  return _sensors;
}

// sends command for all devices on the bus to perform a temperature conversion
void TempManager::requestTemperatures() {
  if (_isInit) {
    _request = _sensors.requestTemperatures();
  }
  else {
    LOG_E(TAG, "TempManager object not initialized");
  }
}

// Request temperature (float precision) for the sensor through its index in the array
float TempManager::getPreciseTempCByIndex(int idx) {
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
int TempManager::getTempCByIndex(int idx) {
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
float TempManager::getPreciseTempCByAddress(std::string deviceAddress) {
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
int TempManager::getTempCByAddress(std::string deviceAddress) {
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
float TempManager::getPreciseTempCByName(std::string deviceName) {
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
int TempManager::getTempCByName(std::string deviceName) {
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
std::string TempManager::deviceAddressToString(DeviceAddress deviceAddress) {
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
T TempManager::stringToDeviceAddress(std::string deviceAddressStr) {
  DeviceAddress deviceAddress;
  for (int i = 0; i < 8; i++)
  { 
    long n = hexstr2n(deviceAddressStr.substr(i*2, 2));
    deviceAddress[i]=n;
    LOG_D(TAG, "deviceAddress[%d] = %d", i,  deviceAddress[i]);
  }
  return deviceAddress;
}
/*
  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC);
  Serial.println();
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}


//   Main function, calls the temperatures in a loop.
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  printData(insideThermometer);
  printData(outsideThermometer);
}

*/

