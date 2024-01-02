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

#include "config/config.hpp"
#include "job/printMemFree.hpp"

#include "setupmode/main_setup.hpp"
#include "normalmode/main_normal.hpp"

#include "display.hpp"
#include "httpserver.hpp"

bool normal_mode = false;


void setup() {

  Serial.begin(115200);
  delay(100);
  Serial.println("");
  Serial.println("");
  sectionlog("Start minimum setup.");

  // PSRAM
  if (psramInit()) {
    mainlog("The PSRAM is correctly initialized");
  } else {
    mainlog("PSRAM does not work");
  }
  printMemFree();

  // FS
  while (!LittleFS.begin(true)) {
    mainlog("LittleFS not ready. retry...");
    delay(100);
  }
  mainlog("LittleFS ready");
  printMemFree();

  sectionlog("End minimum setup.");

  // need migrate, then do migrate.
  sectionlog("Check flagfile and config");

  if (config::hasFlagFile() && config::needConfigMigrate()) {
    config::migrateConfig();
  }

  // Choise boot mode
  if (!config::hasFlagFile()) {
    // setup mode
    setupMode::setup();
  } else {
    // normal mode
    normal_mode = true;
    normalMode::setup();
  }

  return;
}

void loop() {
  if (normal_mode) {
    normalMode::loop();
  } else {
    setupMode::loop();
  }
}
