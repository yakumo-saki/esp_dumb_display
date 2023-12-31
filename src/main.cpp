#include <Arduino.h>

#include "log.h"
#include "const.h"

// network
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#endif

#include <LittleFS.h>

// #include <TimerCall.h>

#include "_define_wifi.h"
#include <math.h>

#include "display.h"
#include "httpserver.hpp"

// TimerCall timer = TimerCall();

int count = 0;

#ifdef ESP8266
void mdns_announce() {
  MDNS.announce();
  mdnslog("mDNS Announced.");
}
#endif

#ifdef ESP32
void mdns_announce() {
  // MDNS.announce();
  mdnslog("mDNS Announced.");
}
#endif

void printMemFree() {
  char ram[15];
  char psram[15];
  dtostrf(ESP.getFreeHeap(), 0, 0, ram);
  dtostrf(ESP.getFreePsram(), 0, 0, psram);
  mainlog("Free main heap: " + String(ram) 
          + "bytes / Free PSRAM heap:" + String(psram) + "bytes");

}

void setup()
{

  Serial.begin(115200);
  delay(100);
  Serial.println("");
  Serial.println("");
  sectionlog("Start setup.");

  // PSRAM
  if (psramInit()) {
    mainlog("The PSRAM is correctly initialized");
  } else {
    mainlog("PSRAM does not work");
  }
  printMemFree();


  // SPIFFS
  while (!LittleFS.begin(true)) {
    mainlog("LittleFS not ready. retry...");
    delay(100);
  }
  mainlog("LittleFS ready");
  printMemFree();


  WiFi.setHostname(MDNS_NAME);
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);
  WiFi.disconnect();

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int count = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    count++;
    if (count == 10) {
      count = 0;
      mainlog("Still waiting for WiFi connection.");
      printMemFree();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  }
  mainlog("WiFi connected");
  printMemFree();

  mainlog("Start display");
  setup_tft();
  draw_wifi_connecting_screen();
  mainlog("Start display done.");
  printMemFree();

  // mDNS
  // if (!MDNS.begin(MDNS_NAME)) {
  //   mdnslog(F("Error setting up MDNS responder!"));
  // } else {
  //   MDNS.setInstanceName("ESP dumb-display Ver." + VER + " " + String(MDNS_NAME));
  //   MDNS.addService("http", "tcp", 80);
  //   mdnslog("mDNS responder started");

  //   mdns_announce();
  // }

  // http
  setup_http_server();

  draw_ready_screen(WiFi.localIP().toString(), String(MDNS_NAME));

  // timer.add(mdns_announce, "MDNS_ANNO", 180000);
  // timer.start();

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

#ifdef ESP8266
  MDNS.update();
#endif 

  loop_http_server();
  // timer.loop();
}
