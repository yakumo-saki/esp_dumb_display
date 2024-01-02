#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "config/AppConfig.hpp"
#include "log.h"

#include "config_saveload.hpp"

namespace config {

  static AppConfig* _instance = nullptr;

  const bool DEBUG_BUILD = false;

  const String FLAG_FILEPATH = "/boot_ok.txt";
  const String CONFIG_FILEPATH = "/config.json";
  const int CONF_JSON_SIZE = 4100; // allocate over 4000 bytes uses PSRAM

  AppConfig* getConfig() {
    if (_instance == nullptr) {
      _instance = new AppConfig();
    }

    return _instance;
  }

  void createFlagfile() {
    File f = LittleFS.open(FLAG_FILEPATH, FILE_WRITE);
    f.println("ok");
    f.close();
    cfglog(F("Flag file created."));

  }

  void deleteFlagfile() {
    if (LittleFS.remove(FLAG_FILEPATH)) {
      cfglog(F("Flag file deleted."));
    } else {
      cfglog(F("Flag file delete requested, but already gone. Ignore request."));
    }
  }

  bool hasFlagFile() {
    return LittleFS.exists(FLAG_FILEPATH);
  }

  bool needConfigMigrate() {
    // TODO: STUB
    cfglog("needConfigUpgrade is not implemented");
    return false;
  }

  bool migrateConfig() {
    // TODO: STUB
    cfglog("migrateConfig is not implemented");
    return false;
  }

  void _dump_file(File f) {
    debuglog("*****  CONFIG RAW DATA DUMP (DEBUG BUILD ONLY) *****");
    while(f.available()){
      Serial.write(f.read());
    }
    Serial.println("");
    f.seek(0);
    debuglog("*****  CONFIG RAW DATA DUMP END                *****");
  }

  bool readConfig() {
    DynamicJsonDocument doc(CONF_JSON_SIZE);

    if (!LittleFS.exists(CONFIG_FILEPATH)) {
      cfglog("Config read failed. No config json file.");
      return false;
    }

    File f = LittleFS.open(CONFIG_FILEPATH, FILE_READ);

    if (DEBUG_BUILD) {
      _dump_file(f);
    }

    auto result = deserializeJson(doc, f);
    if (result == DeserializationError::NoMemory) {
      cfglog("Config read failed. No memory. Config not changed");
      return false;
    } else if (result != DeserializationError::Ok) {
      cfglog("Config read failed. Config not changed");
      return false;
    }

    _loadJsonToConfig(doc, getConfig());
    cfglog("Config read success.");
    return true;
  }

  bool saveConfig() {

    DynamicJsonDocument doc(CONF_JSON_SIZE);
    _saveConfigToJson(getConfig(), &doc);
    doc.garbageCollect();

    if (DEBUG_BUILD) {
      serializeJsonPretty(doc, Serial);
    }

    File f = LittleFS.open(CONFIG_FILEPATH, FILE_WRITE);
    size_t size = serializeJson(doc, f);
    f.close();

    cfglog("Config file wrote " + String(size) + " bytes.");

    return true;
  }

  String serializeConfig() {

    DynamicJsonDocument doc(CONF_JSON_SIZE);
    _saveConfigToJson(getConfig(), &doc);
    doc.garbageCollect();

    if (DEBUG_BUILD) {
      serializeJsonPretty(doc, Serial);
    }

    return "NOT IMPL";
  }

}
