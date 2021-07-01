#include <Arduino.h>

#include "log.h"
#include "const.h"

// network
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#endif

#include <TimerCall.h>

#include "_define_wifi.h"
#include <math.h>

#include "display.h"
#include "httpserver.h"

TimerCall timer = TimerCall();

int count = 0;

void mdns_announce() {
  MDNS.announce();
  mdnslog("mDNS Announced.");
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("");
  Serial.println("");
  sectionlog("Start setup.");

  setup_tft();
  draw_wifi_connecting_screen();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // SPIFFS
  SPIFFS.begin();
  mainlog("SPIFFS ready");

  // mDNS
  if (!MDNS.begin(MDNS_NAME)) {
    mdnslog(F("Error setting up MDNS responder!"));
  } else {
    MDNS.setInstanceName("ESP dumb-display Ver." + VER +" " + String(MDNS_NAME));
    MDNS.addService("http", "tcp", 80);
    mdnslog("mDNS responder started");

    mdns_announce();
  }

  // http
  setup_http_server();

  draw_ready_screen(WiFi.localIP().toString(), String(MDNS_NAME));

  timer.add(mdns_announce, "MDNS_ANNO", 30000);
  timer.start();

  sectionlog("End setup.");
  return;
}

void check_wifi_connection() {

  if (WiFi.status() != WL_CONNECTED) {
    wifilog(F("WiFi disconnected. restart WiFi"));
    draw_wifi_connecting_screen();
  } else {
    return;
  }

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();   
    delay(300);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(300);
  }
}

void loop()
{
  check_wifi_connection();

  MDNS.update();
  loop_http_server();
  timer.loop();
}
