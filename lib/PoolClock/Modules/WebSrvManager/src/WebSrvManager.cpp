/**
 * \file WebSrvManager.h
 * \author Yves Gaignard
 * \brief Implementation of Web Server Management with OTA (Over-The-Air) support
 */

#define TAG "WebSrvManager"

#define ARDUINOJSON_USE_DOUBLE 1  // Required to force ArduinoJSON to treat float as double

// Standard library definitions
#include <Arduino.h>
#include <ElegantOTA.h>
#include <LittleFS.h>
//#include <ArduinoJson.h>          // JSON library

#include "Configuration.h"
#include "LogManager.h"
#include "Utilities.h"
#include "WebSrvManager.h"
#include "WebSerialLite.h"         // Library to reroute Serial on webserver

#define FileSys LittleFS

AsyncWebServer OTAServer(OTA_UPDATE_PORT);

// forward declaration
void WebSrvManager_recvMsg(uint8_t *data, size_t len);

//void WebSrvManager_getMeasures(AsyncWebServerRequest *request);

unsigned long ota_progress_millis = 0;

void onOTAStart() {
// Log when OTA has started
Serial.println("OTA update started!");
// <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
// Log every 1 second
if (millis() - ota_progress_millis > 1000) {
  ota_progress_millis = millis();
  Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
}
}

void onOTAEnd(bool success) {
// Log when OTA has finished
if (success) {
  Serial.println("OTA update finished successfully!");
} else {
  Serial.println("There was an error during OTA update!");
}
// <Add your own code here>
}


// Web server setup and start
void WebSrvManager_setup(boolean isWebSerial) {

  LOG_I(TAG, "Start Web Server Manager setup....");

  //----------------------------------------------------FileSys
  if(!FileSys.begin()) {
      LOG_E(TAG, "Cannot open LittleFS....");
    return;
  }

  LOG_D(TAG, "List of Files:");
  File root = FileSys.open("/");
  File file = root.openNextFile();

  while(file)
  {
    LOG_D(TAG, "File: %s", file.name());
    file.close();
    file = root.openNextFile();
  }

  
  OTAServer.on("/", HTTP_GET, WebSrvManager_root);
  //OTAServer.on("/getMeasures", HTTP_GET, WebSrvManager_getMeasures);
  OTAServer.onNotFound(WebSrvManager_notFound);
  OTAServer.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(FileSys, "/w3.css", "text/css");
  });

  OTAServer.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(FileSys, "/script.js", "text/javascript");
  });

  OTAServer.serveStatic("/", FileSys, "/");
  
  if (isWebSerial) {
    LOG_I(TAG, "Start Web Serial ....");
    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&OTAServer);
    // Attach Message Callback
    WebSerial.onMessage(WebSrvManager_recvMsg);

    Log.setWebSerialOn();
  }
  
  ElegantOTA.begin(&OTAServer);
  //ElegantOTA.begin(&OTAServer);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  OTAServer.begin();
  LOG_I(TAG, "HTTP server started");
}

// handler to treat "page not found"
void WebSrvManager_notFound(AsyncWebServerRequest *request) {
  int http_rc = 404;
  char Response_type[]="text/html";
  String Response;
  Response+="<!DOCTYPE html>";
  Response+="<html>";
  Response+="<head><title>Not Found</title></head>";
  Response+="<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>";
  Response+="</html>";
  request->send(http_rc, Response_type, Response);
  LOG_V(TAG, "send %d %s \nResponse: %s", http_rc, Response_type, Response);
}

// handler to treat "root URL"
void WebSrvManager_root(AsyncWebServerRequest *request) {
  request->send(FileSys, "/index.html", "text/html");
}

/*
// handler to treat "GET Temperature"
void WebSrvManager_getMeasures(AsyncWebServerRequest *request) {
  const int capacity = JSON_OBJECT_SIZE(48);
  StaticJsonDocument<capacity> root;
  LOG_V(TAG, "Start API Get measuressend");
  
  root["Vers"]   = pm_measures.PMVersion;                    // firmware revision
  root["Reboot"] = pm_measures.RebootNumber;
  root["RTime"]  = pm_measures.LastRebootTimestamp;
  root["RSTime"] = pm_measures.LastDayResetTimestamp;
  
  root["DFUpt"]  = pm_measures.DayFiltrationUptime;          // Filtration Duration since the begin of the day
  root["DFTrgt"] = pm_measures.DayFiltrationTarget;          // Maximum Filtration duration for the whole day
  root["PFSta"]  = pm_measures.PeriodFiltrationStartTime;    // Next period start time of the filtration
  root["PFEnd"]  = pm_measures.PeriodFiltrationEndTime;      // Next period end time of the filtration
  root["PDFUpt"] = pm_measures.PreviousDayFiltrationUptime;  // Filtration Duration of the previous day
  root["PDFTrgt"]= pm_measures.PreviousDayFiltrationTarget;  // Target Filtration duration of the previous day
  root["pHUTL"]  = pm_measures.pHPumpUpTimeLimit / 60;       // Time in seconds max per day for pH injection
  root["OrpUTL"] = pm_measures.OrpPumpUpTimeLimit / 60;      // Time in seconds max per day for Chlorine injection

  root["Auto"]   = pm_measures.AutoMode;                     // Mode Automatic (true) or Manual (false)
  root["Winter"] = pm_measures.WinterMode;                   // Winter Mode if true  
  root["pHROO"]  = pm_measures.pH_RegulationOnOff;   
  root["OrpROO"] = pm_measures.Orp_RegulationOnOff;   
  root["FPmpS"]  = pm_measures.FilterPumpState;   
  root["pHPmpS"] = pm_measures.pHMinusPumpState;   
  root["OrpPmpS"]= pm_measures.ChlorinePumpState;   

  root["IATemp"] = pm_measures.InAirTemp;   
  root["WaTemp"] = pm_measures.WaterTemp;   
  root["OATemp"] = pm_measures.OutAirTemp;   
  root["WaTLT"]  = pm_measures.WaterTempLowThreshold;   

  root["pH"]     = pm_measures.pHValue;   
  root["Orp"]    = pm_measures.OrpValue;   
  root["PSI"]    = pm_measures.Pressure;   
  root["pHFill"] = pm_measures.pHMinusTankFill;   
  root["ChFill"] = pm_measures.ChlorineTankFill;   

  root["pHWS"]  = pm_measures.pHPIDWindowSize / 1000 / 60;        //pH PID window size (/!\ mins)
  root["ChlWS"] = pm_measures.OrpPIDWindowSize / 1000 / 60;       //Orp PID window size (/!\ mins)
  root["pHSP"]  = pm_measures.pH_SetPoint * 100;                  //pH setpoint (/!\ x100)
  root["OrpSP"] = pm_measures.Orp_SetPoint;                       //Orp setpoint
  //root["WSP"]   = pm_measures.WaterTemp_SetPoint * 100;           //Water temperature setpoint (/!\ x100)
  root["WLT"]   = pm_measures.WaterTempLowThreshold * 100;        //Water temperature low threshold to activate anti-freeze mode (/!\ x100)
  root["PSIHT"] = pm_measures.PressureHighThreshold * 100;            //Water pressure high threshold to trigger error (/!\ x100)
  root["PSIMT"] = pm_measures.PressureMedThreshold * 100;             //Water pressure medium threshold (unused yet) (/!\ x100)

  root["pHC0"]  = pm_measures.pHCalibCoeffs0;            //pH sensor calibration coefficient C0
  root["pHC1"]  = pm_measures.pHCalibCoeffs1;            //pH sensor calibration coefficient C1
  root["OrpC0"] = pm_measures.OrpCalibCoeffs0;           //Orp sensor calibration coefficient C0
  root["OrpC1"] = pm_measures.OrpCalibCoeffs1;           //Orp sensor calibration coefficient C1
  root["PSIC0"] = pm_measures.PSICalibCoeffs0;           //Pressure sensor calibration coefficient C0
  root["PSIC1"] = pm_measures.PSICalibCoeffs1;           //Pressure sensor calibration coefficient C1

  root["pHKp"]  = pm_measures.pH_Kp;    //pH PID coeffcicient Kp
  root["pHKi"]  = pm_measures.pH_Ki;    //pH PID coeffcicient Ki
  root["pHKd"]  = pm_measures.pH_Kd;    //pH PID coeffcicient Kd

  root["OrpKp"] = pm_measures.Orp_Kp;    //Orp PID coeffcicient Kp
  root["OrpKi"] = pm_measures.Orp_Ki;    //Orp PID coeffcicient Ki
  root["OrpKd"] = pm_measures.Orp_Kd;    //Orp PID coeffcicient Kd

  root["Dpid"]   = pm_measures.DelayPIDs;     //Delay from FSta for the water regulation/PIDs to start (mins) 
  //root["PubP"]   = pm_measures.PublishPeriod/1000; // Settings publish period in sec

  root["pHTV"]  = pm_measures.pHMinusTankVolume;           //Acid tank nominal volume (Liters)
  root["ChlTV"] = pm_measures.ChlorineTankVolume;          //Chl tank nominal volume (Liters)
  root["pHFR"]  = pm_measures.pHMinusFlowRate;            //Acid pump flow rate (L/hour)
  root["OrpFR"] = pm_measures.ChlorineFlowRate;           //Chl pump flow rate (L/hour)

  String response;
  serializeJson(root, response);  
  request->send(200, "application/json", response.c_str());
  LOG_V(TAG, "End API Get measuressend");
}
*/

/**
 * @brief Message callback of WebSerial 
 * 
 * @param data : string to display
 * @param len  : length of the string
 */
void WebSrvManager_recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  std::string d (data, data+len);
  boolean help = false;

  std::vector<std::string> words;
  words.clear();
  ExtractWordsFromString(d, words);
  if (words.size() > 0 ) {
    //LOG_D(TAG, "words.size: %d", words.size());
    //for (int i=0; i<words.size(); i++) {
    //  LOG_D(TAG, "words[%d]: %s", i, words[i].c_str());
    //}
    if (string_iequals(words[0], (std::string)"log")) { 
      //LOG_D(TAG, "words[0] = %s", words[0].c_str());
      if (words.size() == 3 ) { 
        Log.setTag(words[2].c_str(), words[1].c_str());
        WebSerial.printf ("Command done: %s\n", d.c_str()); 
      }
      else { 
        WebSerial.printf ("Unknown log command: %s\n", d.c_str()); 
        help =true;
      }
    } 
    else {
      WebSerial.printf ("Unknown command: %s", d.c_str());
      help =true;
    }
  }
  else {
    help=true;
  }

  if (help) {
    WebSerial.println("Command help:");
    WebSerial.println("- log LEVEL TAG     # LEVEL = ERROR, WARNING, INFO, DEBUG or VERBOSE    # TAG = name of the class");
  }
}