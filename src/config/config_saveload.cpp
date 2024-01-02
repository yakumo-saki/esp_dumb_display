#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/AppConfig.hpp"

namespace config {

  void _loadJsonToConfig(DynamicJsonDocument doc, AppConfig *cfg) {
    if (doc.containsKey("configId")) {
      cfg->configId = doc["configId"].as<String>();
    }
    if (doc.containsKey("ssid")) {
      cfg->ssid = doc["ssid"].as<String>();
    }
    if (doc.containsKey("password")) {
      cfg->password = doc["password"].as<String>();
    }
    if (doc.containsKey("brightness")) {
      cfg->brightness = doc["brightness"].as<int>();
    }
    if (doc.containsKey("rotation")) {
      cfg->rotation = doc["rotation"].as<int>();
    }

  }

  void _saveConfigToJson(AppConfig *cfg, DynamicJsonDocument *doc) {


  }

}
