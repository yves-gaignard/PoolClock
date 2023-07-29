/**
 * \file main.cpp
 * \author Yves Gaignard
 * \brief Glue logic to bring together all the modules and some additional stuff
 */

#include "Configuration.h"
#include <Arduino.h>
#include "DisplayManager.h"
#include "ClockState.h"

#if RUN_WITHOUT_WIFI == false
	#include "WiFi.h"
#endif
#if ENABLE_OTA_UPLOAD == true
	#include <ArduinoOTA.h>
#endif
#if IS_BLYNK_ACTIVE == true
	#include "BlynkConfig.h"
#endif

#if AIR_TEMP_SENSOR == true
	#include "Sensor_AM232X.h"
	Sensor_AM232X* am232x = Sensor_AM232X::getInstance();
#endif

#if WATER_TEMP_SENSOR == true
	#include "Sensor_DS18B20.h"
	OneWire oneWire(WATER_TEMP_PIN); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	DallasTemperature dallasWaterTemp(&oneWire); // Pass our oneWire reference to Dallas Temperature.
	Sensor_DS18B20* DS18B20Sensors = Sensor_DS18B20::getInstance();
#endif

#if PIR_SENSOR == true
	#include "Sensor_HCSR501.h"
	Sensor_HCSR501* PIRSensor = Sensor_HCSR501::getInstance();
	bool isDetected = false;
	bool previousIsDetected = false;
#endif

DisplayManager* PoolClockDisplays = DisplayManager::getInstance();
TimeManager* timeM = TimeManager::getInstance();
ClockState* states = ClockState::getInstance();

#if IS_BLYNK_ACTIVE == true
	BlynkConfig* BlynkConfiguration = BlynkConfig::getInstance();
#endif


#if ENABLE_OTA_UPLOAD == true
	void setupOTA();
#endif
#if RUN_WITHOUT_WIFI == false
	void wifiSetup();
#endif

void TimerTick();
void TimerDone();
void AlarmTriggered();

void startupAnimation()
{
	uint8_t currHourH = 0;
	uint8_t currHourL = 0;
	uint8_t currMinH = 0;
	uint8_t currMinL = 0;
	uint8_t targetHourH = 0;
	uint8_t targetHourL = 0;
	uint8_t targetMinH = 0;
	uint8_t targetMinL = 0;

	TimeManager::TimeInfo currentTime;
	currentTime = timeM->getCurrentTime();
	targetHourH = currentTime.hours / 10;
	targetHourL = currentTime.hours % 10;
	targetMinH = currentTime.minutes / 10;
	targetMinL = currentTime.minutes % 10;

	PoolClockDisplays->displayTime(0, 0);
	PoolClockDisplays->delay(DIGIT_ANIMATION_SPEED + 10);

	while (currHourH != targetHourH || currHourL != targetHourL || currMinH != targetMinH || currMinL != targetMinL)
	{
		if(currHourH < targetHourH)
		{
			currHourH++;
		}
		if(currHourL < targetHourL)
		{
			currHourL++;
		}
		if(currMinH < targetMinH)
		{
			currMinH++;
		}
		if(currMinL < targetMinL)
		{
			currMinL++;
		}
		PoolClockDisplays->displayTime(currHourH * 10 + currHourL, currMinH * 10 + currMinL);
		PoolClockDisplays->delay(DIGIT_ANIMATION_SPEED + 100);
	}
}

void setup()
{
	Serial.begin(115200);
	delay(100);

	Serial.println("Init Segment...");
	PoolClockDisplays->InitSegments(0, WIFI_CONNECTING_COLOR, 50);

	Serial.println("setHourSegmentColors ...");
	PoolClockDisplays->setHourSegmentColors(HOUR_COLOR);
	Serial.println("setMinuteSegmentColors ...");
	PoolClockDisplays->setMinuteSegmentColors(MINUTE_COLOR);
	Serial.println("setTemp1SegmentColors ...");
	PoolClockDisplays->setTemp1SegmentColors(TEMP1_COLOR);
	Serial.println("setTemp2SegmentColors ...");
	PoolClockDisplays->setTemp2SegmentColors(TEMP2_COLOR);
	Serial.println("setInternalLEDColor ...");
	PoolClockDisplays->setInternalLEDColor(INTERNAL_COLOR);
	Serial.println("setDotLEDColor ...");
	PoolClockDisplays->setDotLEDColor(SEPARATION_DOT_COLOR);

	#if RUN_WITHOUT_WIFI == false
	    Serial.println("wifi setup ...");
		wifiSetup();
	#endif
	#if ENABLE_OTA_UPLOAD == true
	    Serial.println("OTA setup ...");
		setupOTA();
	#endif
	#if ENABLE_OTA_UPLOAD == true
		ArduinoOTA.handle(); //give ota the opportunity to update before the main loop starts in case we have a crash in there
	#endif

	#if IS_BLYNK_ACTIVE == true
	    Serial.println("Blynk configuration setup ...");
		BlynkConfiguration->setup();
	#endif

	Serial.println("Fetching time from NTP server...");
	if(timeM->init() == false)
	{
		Serial.printf("[E]: TimeManager failed to synchronize for the first time with the NTP server. Retrying in %d seconds\n", TIME_SYNC_INTERVAL);
	}
	String sCurrentTime = timeM->getCurrentTimeString();
	Serial.printf("Current time : %s\n", sCurrentTime);
	timeM->setTimerTickCallback(TimerTick);
	timeM->setTimerDoneCallback(TimerDone);
	timeM->setAlarmCallback(AlarmTriggered);

	#if AIR_TEMP_SENSOR == true
		Serial.println("initialize air temperature sensor ...");
		if (! am232x->init(I2C_SDA_PIN, I2C_SCL_PIN, AIR_TEMP_READ_FREQUENCY) ) 
		{
			Serial.println("[E] Initialization FAILED !!");
		}
	#endif

	#if WATER_TEMP_SENSOR == true
  		DS18B20Sensors->init(dallasWaterTemp);

  		int DS18B20SensorsNumber = DS18B20Sensors->getDeviceCount();
  		Serial.printf("%d DS18B20 sensors found\n",DS18B20SensorsNumber);

		for (int i = 0; i< DS18B20SensorsNumber; i++){
			std::string deviceAddrStr = DS18B20Sensors->getDeviceAddress(i);
			Serial.printf("sensor address [%d] : %s\n",i , deviceAddrStr.c_str() );
			if (deviceAddrStr == waterThermometerAddress) {
				DS18B20Sensors->addDevice(waterThermometerName, waterThermometerAddress);
			} else {
				Serial.printf("Unknown temperature sensor found. Its address is: %s\n",deviceAddrStr.c_str());
			}
		} 
	#endif

	#if PIR_SENSOR == true
		Serial.println("PIR Motion Sensor Initialization ...");
  		if (PIRSensor->init(PIR_SENSOR_PIN, PIR_SENSOR_DELAY))   // consider a delay of 5 minutes when the PIR sensor detects a motion
		{
		    Serial.println("Initialization OK");
		}
		else
		{
		    Serial.println("Initialization FAILED");
		}
	#endif

	Serial.println("Displaying startup animation...");
	startupAnimation();
	Serial.println("Setup done...");
}

void loop()
{
	//DBG Serial.println("Begin of Main Loop...");
	#if ENABLE_OTA_UPLOAD == true
	    //DBG Serial.println("ArduinoOTA.handle()...");
		ArduinoOTA.handle();
	#endif
	//DBG Serial.println("states->handleStates()...");
	states->handleStates(); //updates display states, switches between modes etc.

	// Test code:
	// if((millis()-last)>= 1500)
	// {
	// 	PoolClockDisplays->test();
	// 	last = millis();
	// }
	//DBG Serial.println("PoolClockDisplays->handle()...");
    PoolClockDisplays->handle();
	//DBG Serial.println("timeM->handle()...");
	timeM->handle();

	#if AIR_TEMP_SENSOR == true
		am232x->handle();
	#endif

	#if WATER_TEMP_SENSOR == true
		DS18B20Sensors->requestTemperatures();
	#endif

	#if PIR_SENSOR == true
	    isDetected = PIRSensor->isMotionDetected();
	
		if (isDetected)
		{
			if (previousIsDetected != isDetected)
			{
				Serial.println("Motion is detected");
				previousIsDetected = isDetected;
			}
		}
		else
		{
			if (previousIsDetected != isDetected)
			{
				Serial.println("No Motion detected");
				previousIsDetected = isDetected;
			}
		}
	#endif

}

void AlarmTriggered()
{
    states->switchMode(ClockState::ALARM_NOTIFICATION);
	#if IS_BLYNK_ACTIVE == true
		BlynkConfiguration->updateUI();
	#endif
}

void TimerTick()
{
	#if IS_BLYNK_ACTIVE == true
		BlynkConfiguration->updateUI();
	#endif
}

void TimerDone()
{
    states->switchMode(ClockState::TIMER_NOTIFICATION);
	#if IS_BLYNK_ACTIVE == true
		BlynkConfiguration->updateUI();
	#endif
}

#if RUN_WITHOUT_WIFI == false
	void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
	{
		Serial.print("WiFi lost connection. Reason: ");
		Serial.println(info.wifi_sta_disconnected.reason);
		Serial.println("Trying to Reconnect");
		WiFi.disconnect();
		WiFi.reconnect();
	}

	void wifiSetup()
	{
		#if USE_ESPTOUCH_SMART_CONFIG == true
			WiFi.reconnect(); //try to reconnect
			Serial.println("Trying to reconnect to previous wifi network");
		#else

		  #if USE_STATIC_IP_CONFIG == true
		    Serial.println("WIFI config ...");
		    // Configures static IP address
		    IPAddress local_IP(MY_IP_ADDRESS);
		    IPAddress gateway(MY_IP_GATEWAY_ADDRESS);
		    IPAddress subnet(MY_IP_SUBNET_ADDRESS);
		    IPAddress primaryDNS(PRIMARY_DNS);
		    IPAddress secondaryDNS(SECONDARY_DNS);
  		    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
		      Serial.println("STA Failed to configure");
  		    }
		  #endif
		  
		  Serial.println("WIFI begin ...");
		  #if WIFI_WITHOUT_SCANNING_PHASE == true
			WiFi.begin(WIFI_SSID, WIFI_PW, 6 );   // specify the WiFi channel number (6) when calling WiFi.begin(). This skips the WiFi scanning phase and saves about 4 seconds when connecting to the WiFi.
		  #else
			WiFi.begin(WIFI_SSID, WIFI_PW);
		  #endif
		#endif
		if(WiFi.status() == WL_CONNECTED) 
		  Serial.println("WIFI Connection successful");
		else
		  Serial.println("WIFI Connection failed");

		Serial.println("setAllSegmentColors ...");
		PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTING_COLOR);
		Serial.println("showLoadingAnimation ...");
		PoolClockDisplays->showLoadingAnimation();
		for (int i = 0; i < NUM_RETRIES; i++)
		{
			Serial.printf("Wifi connection retry number = %d\n", i);
			//Serial.print(".");
			#if USE_ESPTOUCH_SMART_CONFIG == true
				if(WiFi.begin() == WL_CONNECTED)
			#else
				if(WiFi.status() == WL_CONNECTED)
			#endif
			{
				Serial.println("Reconnect successful");
        		Serial.println("setAllSegmentColors ...");
				PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTION_SUCCESSFUL_COLOR);
				break;
			}
				else {
					Serial.println("WIFI Connection failed");
				}
			//delay(500);
			PoolClockDisplays->delay(500);
			Serial.println("WIFI Connection after Delay");
		}

		if(WiFi.status() != WL_CONNECTED)
		{
			#if USE_ESPTOUCH_SMART_CONFIG == true
				Serial.println("Reconnect failed. starting smart config");
				WiFi.mode(WIFI_AP_STA);
				// start SmartConfig
				WiFi.beginSmartConfig();

				// Wait for SmartConfig packet from mobile
				Serial.println("Waiting for SmartConfig.");
				PoolClockDisplays->setAllSegmentColors(WIFI_SMART_CONFIG_COLOR);
				while (!WiFi.smartConfigDone())
				{
					Serial.print(".");
					PoolClockDisplays->delay(500);
				}
				PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTING_COLOR);
				Serial.println("");
				Serial.println("SmartConfig done.");

				// Wait for WiFi to connect to AP
				Serial.println("Waiting for WiFi");
				while (WiFi.status() != WL_CONNECTED)
				{
					Serial.print(".");
					PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTION_SUCCESSFUL_COLOR);
					PoolClockDisplays->delay(500);
				}
				Serial.println("WiFi Connected.");
				Serial.print("IP Address: ");
				Serial.println(WiFi.localIP());
			#else
				Serial.println("WIFI connection failed");
				PoolClockDisplays->setAllSegmentColors(ERROR_COLOR);
			#endif
			if(WiFi.status() != WL_CONNECTED)
			{
				Serial.println("WIFI connection failed. Aborting execution.");
				abort();
			}
		}
		WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
		PoolClockDisplays->stopLoadingAnimation();
		Serial.println("Waiting for loading animation to finish...");
		PoolClockDisplays->waitForLoadingAnimationFinish();
		PoolClockDisplays->turnAllSegmentsOff();
	}
#endif

#if ENABLE_OTA_UPLOAD == true
	bool progressFirstStep = true;
	void setupOTA()
	{
		// Port defaults to 3232
		ArduinoOTA.setPort(OTA_UPDATE_PORT);


		// Hostname defaults to esp3232-[MAC]
		ArduinoOTA.setHostname(OTA_UPDATE_HOST_NAME);

		// No authentication by default
		//ArduinoOTA.setPassword("admin");

		// Password can be set with it's md5 value as well
		// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
		// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

		ArduinoOTA.onStart([]() {
			String type;
			if (ArduinoOTA.getCommand() == U_FLASH)
			{
				type = "sketch";
			}
			else // U_SPIFFS
			{
				type = "filesystem";
			}
			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			Serial.println("Start updating " + type);
			timeM->disableTimer();
			#if IS_BLYNK_ACTIVE == true
				BlynkConfiguration->stop();
			#endif
			PoolClockDisplays->setAllSegmentColors(OTA_UPDATE_COLOR);
			PoolClockDisplays->turnAllLEDsOff(); //instead of the loading animation show a progress bar
			PoolClockDisplays->setGlobalBrightness(50);
		})
		.onEnd([]()
		{
			Serial.println("\nOTA End");
		})
		.onProgress([](unsigned int progress, unsigned int total)
		{
			Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
			if(progressFirstStep == true)
			{
				PoolClockDisplays->displayProgress(total);
				progressFirstStep = false;
			}
			PoolClockDisplays->updateProgress(progress);
		})
		.onError([](ota_error_t error)
		{
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
			{
				Serial.println("Auth Failed");
			}
			else if (error == OTA_BEGIN_ERROR)
			{
				Serial.println("Begin Failed");
			}
			else if (error == OTA_CONNECT_ERROR)
			{
				Serial.println("Connect Failed");
			}
			else if (error == OTA_RECEIVE_ERROR)
			{
				Serial.println("Receive Failed");
			}
			else if (error == OTA_END_ERROR)
			{
				Serial.println("End Failed");
			}
			PoolClockDisplays->setAllSegmentColors(ERROR_COLOR);
		});

		ArduinoOTA.begin();

		Serial.println("OTA update functionality is ready");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
	}
#endif