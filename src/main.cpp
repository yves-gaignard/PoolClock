/**
 * \file main.cpp
 * \author Yves Gaignard
 * \brief Glue logic to bring together all the modules and some additional stuff
 */

#include <Arduino.h>
#include <string>

#define TAG "PoolClock_main"

// Project name, version and author
// ---------------------------------
struct ProjectStructure {
  std::string Name;
  std::string Version;
  std::string Author;
};
const ProjectStructure Project {"Pool Clock", "1.0.0", "Yves Gaignard"};

#include "Configuration.h"
#include "LogManager.h"
#include "DisplayManager.h"
#include "ClockState.h"

#if RUN_WITHOUT_WIFI == false
	#include "WiFi.h"
#endif
#if ENABLE_OTA_UPLOAD == true
	//#include <ArduinoOTA.h>
	#include "WebSrvManager.h"
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

#if LCD_SCREEN == true
	#include "LCDManager.h"
	#include "LCDScreens.h"
	#include "Utilities.h"
	void LCDScreen_Init(const ProjectStructure Project);
	LCDManager lcd(LCD_ID, LCD_COLS, LCD_LINES);
	LCDScreens screens;
#endif

DisplayManager* PoolClockDisplays = DisplayManager::getInstance();
TimeManager* timeM = TimeManager::getInstance();
ClockState* states = ClockState::getInstance();

#if IS_BLYNK_ACTIVE == true
	BlynkConfig* BlynkConfiguration = BlynkConfig::getInstance();
#endif

#if PUSH_BUTTONS == true
	#include "PoolClockCmd.h"
	PoolClockCmd* PCCmd = PoolClockCmd::getInstance();
#endif

#if ENABLE_OTA_UPLOAD == true
	//void setupOTA();
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

	// Set appropriate log level. The defaul DEFAULT_LOG_LEVEL is defined in Configuration.h file
	Log.setTag("*"                   , DEFAULT_LOG_LEVEL);
	Log.setTag("BuzzerManager"       , DEFAULT_LOG_LEVEL);
	Log.setTag("ClockState"          , DEFAULT_LOG_LEVEL);
	Log.setTag("DisplayManager"      , DEFAULT_LOG_LEVEL);
	Log.setTag("LCDManager"          , DEFAULT_LOG_LEVEL);
	Log.setTag("LCDScreens"          , DEFAULT_LOG_LEVEL);
	Log.setTag("LogManager"          , DEFAULT_LOG_LEVEL);
	Log.setTag("PoolClockCmd"        , LOG_DEBUG);
	Log.setTag("PoolClock_main"      , LOG_DEBUG);
	Log.setTag("Sensor_AM232X"       , LOG_DEBUG);
	Log.setTag("Sensor_DS18B20"      , DEFAULT_LOG_LEVEL);
	Log.setTag("Sensor_HCSR501"      , LOG_DEBUG);
	Log.setTag("SevenSegment"        , DEFAULT_LOG_LEVEL);
	Log.setTag("TimeManager"         , DEFAULT_LOG_LEVEL);
	Log.setTag("WebSrvManager"       , LOG_DEBUG);

    //LOG_(TAG, "send %d %s \nResponse: %s", http_rc, Response_type, Response);

	#if LCD_SCREEN == true
		LOG_I(TAG, "LCDScreen initialization...");
	    LCDScreen_Init(Project);
	#endif

    LOG_I(TAG, "Init Segment...");
	PoolClockDisplays->InitSegments(0, WIFI_CONNECTING_COLOR, 50);

    LOG_I(TAG, "setHourSegmentColors...");
	PoolClockDisplays->setHourSegmentColors(HOUR_COLOR);
    LOG_I(TAG, "setMinuteSegmentColors...");
	PoolClockDisplays->setMinuteSegmentColors(MINUTE_COLOR);
    LOG_I(TAG, "T...");
	PoolClockDisplays->setTemp1SegmentColors(TEMP1_COLOR);
    LOG_I(TAG, "setTemp2SegmentColors...");
	PoolClockDisplays->setTemp2SegmentColors(TEMP2_COLOR);
    LOG_I(TAG, "setInternalLEDColor...");
	PoolClockDisplays->setInternalLEDColor(INTERNAL_COLOR);
    LOG_I(TAG, "setDotLEDColor...");
	PoolClockDisplays->setDotLEDColor(SEPARATION_DOT_COLOR);

	#if RUN_WITHOUT_WIFI == false
    	LOG_I(TAG, "wifi setup...");
		wifiSetup();
	#endif
	#if ENABLE_OTA_UPLOAD == true
    	LOG_I(TAG, "OTA setup...");
		//setupOTA();
		WebSrvManager_setup(IS_WEB_SERIAL_ACTIVATED);
	#endif

	#if IS_BLYNK_ACTIVE == true
    	LOG_I(TAG, "Blynk configuration setup...");
		BlynkConfiguration->setup();
	#endif

    LOG_I(TAG, "Fetching time from NTP server...");
	if(timeM->init() == false)
	{
	    LOG_E(TAG, "TimeManager failed to synchronize for the first time with the NTP server. Retrying in %d seconds", TIME_SYNC_INTERVAL);
	}
	String sCurrentTime = timeM->getCurrentTimeString(TimeManager::HourMinSecFormat);
    LOG_I(TAG, "Current time : %s", sCurrentTime);
	timeM->setTimerTickCallback(TimerTick);
	timeM->setTimerDoneCallback(TimerDone);
	timeM->setAlarmCallback(AlarmTriggered);

	#if AIR_TEMP_SENSOR == true
	    LOG_I(TAG, "initialize air temperature sensor...");
		if (! am232x->init(I2C_SDA_PIN, I2C_SCL_PIN, AIR_TEMP_READ_FREQUENCY) ) 
		{
		    LOG_E(TAG, "Air temperature sensor Initialization FAILED !!");
		}
	#endif

	#if WATER_TEMP_SENSOR == true
  		DS18B20Sensors->init(dallasWaterTemp);

  		int DS18B20SensorsNumber = DS18B20Sensors->getDeviceCount();
	    LOG_I(TAG, "%d DS18B20 sensors found", DS18B20SensorsNumber);

		for (int i = 0; i< DS18B20SensorsNumber; i++){
			std::string deviceAddrStr = DS18B20Sensors->getDeviceAddress(i);
		    LOG_I(TAG, "DS18B20 sensor address [%d] : %s", i, deviceAddrStr.c_str() );
			if (deviceAddrStr == waterThermometerAddress) {
				DS18B20Sensors->addDevice(waterThermometerName, waterThermometerAddress);
			} else {
			    LOG_E(TAG, "Unknown DS18B20 temperature sensor found. Its address is: %s", deviceAddrStr.c_str() );
			}
		} 
	#endif

	#if PIR_SENSOR == true
		LOG_I(TAG, "PIR Motion Sensor Initialization ...");
  		if (PIRSensor->init(PIR_SENSOR_PIN, PIR_SENSOR_DELAY))   // consider a delay of 5 minutes when the PIR sensor detects a motion
		{
		    LOG_I(TAG, "PIR Motion Sensor Initialization OK");
		}
		else
		{
		    LOG_E(TAG, "PIR Motion Sensor Initialization FAILED");
		}
	#endif

	#if PUSH_BUTTONS == true
		LOG_I(TAG, "Push button initialization...");
		PCCmd->setup();
	#endif

	LOG_I(TAG, "Displaying startup animation...");
	startupAnimation();
	LOG_I(TAG, "Setup done...");
}

void loop()
{
	LOG_V(TAG, "Begin of Main Loop...");
	#if ENABLE_OTA_UPLOAD == true
	    //DBG Serial.println("ArduinoOTA.handle()...");
		//ArduinoOTA.handle();
	#endif
	LOG_V(TAG, "states->handleStates()...");
	states->handleStates(); //updates display states, switches between modes etc.

	// Test code:
	// if((millis()-last)>= 1500)
	// {
	// 	PoolClockDisplays->test();
	// 	last = millis();
	// }
	LOG_V(TAG, "PoolClockDisplays->handle()...");
    PoolClockDisplays->handle();
	LOG_V(TAG, "timeM->handle()...");
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
				LOG_I(TAG, "Motion is detected");
				previousIsDetected = isDetected;
			}
		}
		else
		{
			if (previousIsDetected != isDetected)
			{
				LOG_I(TAG, "No Motion detected");
				previousIsDetected = isDetected;
			}
		}
	#endif

	#if LCD_SCREEN == true
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
		LOG_E(TAG, "WiFi lost connection. Reason: %d", info.wifi_sta_disconnected.reason);
		LOG_E(TAG, "Trying to Reconnect");
		WiFi.disconnect();
		WiFi.reconnect();
	}

	void wifiSetup()
	{
		#if USE_ESPTOUCH_SMART_CONFIG == true
			WiFi.reconnect(); //try to reconnect
			LOG_I(TAG, "Trying to reconnect to previous wifi network");
		#else

		  #if USE_STATIC_IP_CONFIG == true
		    LOG_I(TAG, "WIFI config ...");
		    // Configures static IP address
		    IPAddress local_IP(MY_IP_ADDRESS);
		    IPAddress gateway(MY_IP_GATEWAY_ADDRESS);
		    IPAddress subnet(MY_IP_SUBNET_ADDRESS);
		    IPAddress primaryDNS(PRIMARY_DNS);
		    IPAddress secondaryDNS(SECONDARY_DNS);
  		    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
		      LOG_E(TAG, "STA Failed to configure");
  		    }
		  #endif
		  
		  LOG_I(TAG, "WIFI Mode ...");
		  WiFi.mode(WIFI_STA);
		  LOG_I(TAG, "WIFI begin ...");
		  #if WIFI_WITHOUT_SCANNING_PHASE == true
			WiFi.begin(WIFI_SSID, WIFI_PW, 6 );   // specify the WiFi channel number (6) when calling WiFi.begin(). This skips the WiFi scanning phase and saves about 4 seconds when connecting to the WiFi.
		  #else
			WiFi.begin(WIFI_SSID, WIFI_PW);
		  #endif
		#endif
		if(WiFi.status() == WL_CONNECTED)
		{
		  LOG_I(TAG, "WIFI Connection successful");
		  LOG_I(TAG, "Local IP =%S",WiFi.localIP().toString());			
		}
		else
		  LOG_E(TAG, "WIFI Connection failed");
		

		LOG_I(TAG, "setAllSegmentColors ...");
		PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTING_COLOR);
		LOG_I(TAG, "showLoadingAnimation ...");
		PoolClockDisplays->showLoadingAnimation();
		for (int i = 0; i < NUM_RETRIES; i++)
		{
			LOG_I(TAG, "Wifi connection retry number = %d", i);
			#if USE_ESPTOUCH_SMART_CONFIG == true
				if(WiFi.begin() == WL_CONNECTED)
			#else
				if(WiFi.status() == WL_CONNECTED)
			#endif
			{
				LOG_I(TAG, "Reconnect successful");
	            LOG_I(TAG, "Local IP =%S",WiFi.localIP().toString());			
        		LOG_I(TAG, "setAllSegmentColors ...");
				PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTION_SUCCESSFUL_COLOR);
				break;
			}
				else {
					LOG_E(TAG, "WIFI Connection failed");
				}
			//delay(500);
			PoolClockDisplays->delay(500);
			LOG_I(TAG, "WIFI Connection after Delay");
		}

		if(WiFi.status() != WL_CONNECTED)
		{
			#if USE_ESPTOUCH_SMART_CONFIG == true
				LOG_E(TAG, "Reconnect failed. starting smart config");
				WiFi.mode(WIFI_AP_STA);
				// start SmartConfig
				WiFi.beginSmartConfig();

				// Wait for SmartConfig packet from mobile
				LOG_I(TAG, "Waiting for SmartConfig.");
				PoolClockDisplays->setAllSegmentColors(WIFI_SMART_CONFIG_COLOR);
				while (!WiFi.smartConfigDone())
				{
					LOG_I(TAG, ".");
					PoolClockDisplays->delay(500);
				}
				PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTING_COLOR);
				LOG_I(TAG, "");
				LOG_I(TAG, "SmartConfig done.");

				// Wait for WiFi to connect to AP
				LOG_I(TAG, "Waiting for WiFi");
				while (WiFi.status() != WL_CONNECTED)
				{
					LOG_I(TAG, ".");
					PoolClockDisplays->setAllSegmentColors(WIFI_CONNECTION_SUCCESSFUL_COLOR);
					PoolClockDisplays->delay(500);
				}
				LOG_I(TAG, "WiFi Connected.");
				LOG_I(TAG, "IP Address: ");
				LOG_I(TAG, WiFi.localIP());
			#else
				LOG_I(TAG, "WIFI connection failed");
				PoolClockDisplays->setAllSegmentColors(ERROR_COLOR);
			#endif
			if(WiFi.status() != WL_CONNECTED)
			{
				LOG_E(TAG, "WIFI connection failed. Aborting execution.");
				abort();
			}
		}
		WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
		PoolClockDisplays->stopLoadingAnimation();
		LOG_I(TAG, "Waiting for loading animation to finish...");
		PoolClockDisplays->waitForLoadingAnimationFinish();
		PoolClockDisplays->turnAllSegmentsOff();
	}
#endif

#if ENABLE_OTA_UPLOAD == true
	/*
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
	*/
#endif


#if LCD_SCREEN == true
	// =================================================================================================
	//                              SCREEN DEFINITIONS AND RENDERING
	// =================================================================================================

	void LCDScreen_Init(const ProjectStructure Project) 
	{
		std::vector<std::string> screen;
		/*  
		|--------------------|
		|         1         2|
		|12345678901234567890|
		|--------------------|
		|     SCREEN 0       |
		|--------------------|
		|Pool Clock          |
		|Version: 1.0.0      |
		|                    | 
		|Yves Gaignard       |
		|--------------------|
		*/ 
		screen.clear();
		screen.push_back(Project.Name);
		screen.push_back("Version: " + Project.Version);
		screen.push_back("");
		screen.push_back(Project.Author);

		screens.addScreen("Screen Init");
		screens.setCurrentScreen(0);
		screens.setInactivityTimeOutReset();

		lcd.init();
		lcd.clear();
		lcd.display();
		lcd.backlight();
		lcd.printScreen(screen);
	}

	void LCDScreen_Clock_Mode(TimeManager* currentTime, float temperature1, float humidity1, float temperature2, float humidity2)
	{
		std::vector<std::string> screen;
		std::string DisplayLine;
		/*  
		|--------------------|
		|         1         2|
		|12345678901234567890|
		|--------------------|
		|     SCREEN 1       |
		|--------------------|
		|Mode : CLOCK        |
		|Time : 20:22:44     | 
		|Air  : T=12.7° H=67%| 
		|Water: T=29.6°      |
		|--------------------|
		*/  
		char degreeAsciiChar[2];
		sprintf(degreeAsciiChar, "%c", 176);

		screen.clear();
		screen.push_back("Mode : CLOCK");

        std::string currentTimeStr  = currentTime->getCurrentTimeString(TimeManager::HourMinSecFormat).c_str();
		screen.push_back("Time : "+currentTimeStr);

		std::string InAirTemp  = fct_ftoa(temperature1, "%3.1f");
		std::string InAirHum   = fct_itoa((int) (humidity1 +0.5 - (humidity1<0)));
		std::string WaterTemp  = fct_ftoa(temperature2, "%3.1f");
		screen.push_back("Air  : T="+InAirTemp+degreeAsciiChar+" H=:"+InAirHum+"%");
		screen.push_back("Water: T="+WaterTemp+degreeAsciiChar);

		screens.addOrReplaceScreen("Screen Clock Mode");
		screens.setCurrentScreen(1);

		lcd.clear();
		lcd.display();
		lcd.backlight();
		lcd.printScreen(screen);
	}

	void LCDScreen_Timer_Mode(TimeManager* currentTimer, bool isTimerStarted)
	{
		std::vector<std::string> screen;
		std::string DisplayLine;
		/*  
		|--------------------|
		|         1         2|
		|12345678901234567890|
		|--------------------|
		|      SCREEN 2      |         alt+175 »   alt+186 ║   alt+242 ≥
		|--------------------|
		|Mode : TIMER        |
		|Timer: HH:MM:SS     |
		|    >  START        | or 	|    ║  PAUSE        |       
		| Mode  CANCEL       | or 	|    ■  STOP         |
		|--------------------|
		*/  

		screen.clear();
		screen.push_back("Mode : TIMER");

        std::string currentTimerStr  = currentTimer->getCurrentTimeString(TimeManager::HourMinSecFormat).c_str();
		screen.push_back("Timer: "+currentTimerStr);

		if (!isTimerStarted)
		{
			screen.push_back("    >  START");	
			screen.push_back(" Mode  CANCEL");	
		}
		else
		{
			screen.push_back("    ║  PAUSE");	
			screen.push_back("    ■  STOP");	
		}

		screens.addOrReplaceScreen("Screen Timer Mode");
		screens.setCurrentScreen(2);

		lcd.clear();
		lcd.display();
		lcd.backlight();
		lcd.printScreen(screen);
	}

	/**
	 * \brief The LCDScreen_Set_Timer function allows to display into the LCD, the set timer capability
	 *        The blink digit represent where the blinking cursor is. 
	 */
	void LCDScreen_Set_Timer(TimeManager* currentTimer, LCDScreen_TimeDigit digitCursor)   
	{
		std::vector<std::string> screen;
		/*
		|--------------------|
		|         1         2|
		|12345678901234567890|
		|--------------------|
		|     SCREEN 3       |
		|--------------------|
		|Mode: SET TIMER     |
		|Duration: HH:MM:SS  |
		|+/- : Add/Decrease  |
		|>  Next   Mode: OK  |
		|--------------------|
		*/
		screen.clear();
		screen.push_back("Mode: SET TIMER");

        std::string currentTimerStr  = currentTimer->getCurrentTimeString(TimeManager::HourMinSecFormat).c_str();
		screen.push_back("Duration: "+currentTimerStr);

		screen.push_back("+/- : Add/Decrease");	
		screen.push_back(">  Next   Mode: OK");	

		screens.addOrReplaceScreen("Screen Set Timer");
		screens.setCurrentScreen(3);

		lcd.clear();
		lcd.display();
		lcd.backlight();
		lcd.printScreen(screen);
	}
#endif