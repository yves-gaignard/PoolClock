/**
 * \file WebSrvManager.h
 * \author Yves Gaignard
 * \brief Definition of Web Server Management with OTA (Over-The-Air) support
 */

#ifndef _WEB_SRV_MANAGER_H_
#define _WEB_SRV_MANAGER_H_

#include <ESPAsyncWebServer.h>

// Declare handler functions for the various URLs on the server
void WebSrvManager_notFound(AsyncWebServerRequest *request);
void WebSrvManager_sendWelcome(AsyncWebServerRequest *request);

// Web server setup and start
void WebSrvManager_setup(boolean isWebSerial);

// Web server in loop 
void WebSrvManager_loop();

// handler to treat "page not found"
void WebSrvManager_notFound(AsyncWebServerRequest *request);

// handler to treat "root URL"
void WebSrvManager_root(AsyncWebServerRequest *request);

#endif