#include <Arduino.h>

#ifdef ESP32
#include <WebServer.h>
extern WebServer server;
typedef WebServer HTTPWEBSERVER;
#endif

#ifdef ESP8266
#include <ESP8266WebServer.h>
typedef ESP8266WebServer HTTPWEBSERVER;
#endif

/**
 * 初期化
 */
void setup_http_server();

/**
 * ループ
 */
void loop_http_server();
