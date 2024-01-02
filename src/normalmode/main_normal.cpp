#include <Arduino.h>

#include "log.h"
#include "const.h"

// network
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#endif

#include <LittleFS.h>
#include <TimerCall.h>

#include "job/printMemFree.hpp"

#include "config/config.hpp"
#include "display.hpp"
#include "httpserver.hpp"

namespace normalMode {

  TimerCall timer = TimerCall();

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

  void setup()
  {
    sectionlog("Starting normal mode.");

    if (config::readConfig()) {
      mainlog("Config load successful.");
    } else {
      // failedしているが、デフォルト値は入っているのでOK.
      mainlog("Config load failed.");
    }

    // PSRAM
    if (psramInit()) {
      mainlog("The PSRAM is correctly initialized");
    } else {
      mainlog("PSRAM does not work");
    }
    printMemFree();

    auto cfg = config::getConfig();

    WiFi.setHostname(cfg->mDNSName.c_str());
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
    WiFi.disconnect();

    WiFi.begin(cfg->ssid, cfg->password);

    int count = 1;
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      count++;
      if (count == 10) {
        count = 0;
        mainlog("Still waiting for WiFi connection.");
        printMemFree();
        WiFi.begin(cfg->ssid, cfg->password);
      }
    }
    mainlog("WiFi connected");
    printMemFree();

    mainlog("Start display");
    display::setup();
    display::drawWifiConnectingScreen();
    mainlog("Start display done.");
    printMemFree();

    // mDNS
    if (!MDNS.begin(cfg->mDNSName)) {
      mdnslog(F("Error setting up MDNS responder!"));
    } else {
      MDNS.setInstanceName("ESP dumb-display Ver." + VER + " " + cfg->mDNSName);
      MDNS.addService("http", "tcp", 80);
      mdnslog("mDNS responder started");

      mdns_announce();
    }

    // http
    httpServer::setupNormalMode();

    display::drawReadyScreen(WiFi.localIP().toString(), cfg->mDNSName);

    timer.add(printMemFree, "MEMFREE", 60000);
    timer.add(mdns_announce, "MDNS_ANNO", 300000);
    timer.start();

    mainlog(F("Setup complete. Normal mode ready."));
    sectionlog("End setup.");
    return;
  }

  void check_wifi_connection() {
    auto cfg = config::getConfig();

    if (WiFi.status() != WL_CONNECTED) {
      wifilog(F("WiFi disconnected. restart WiFi"));
      display::drawWifiConnectingScreen();
    } else {
      return;
    }

    while (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      delay(300);
      WiFi.begin(cfg->ssid, cfg->password);
      delay(300);
    }
  }

  void loop()
  {
    check_wifi_connection();

  #ifdef ESP8266
    MDNS.update();
  #endif

    httpServer::loop();
    timer.loop();
  }

}
