#pragma once

#ifdef ESP8266
#include <ESP8266WebServer.h>
typedef ESP8266WebServer HTTPWEBSERVER;
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
typedef WebServer HTTPWEBSERVER;
#endif

namespace httpServer {

  extern HTTPWEBSERVER server;
} // namespace httpCore
