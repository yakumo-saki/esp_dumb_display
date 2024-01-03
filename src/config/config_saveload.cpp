#include <Arduino.h>
#include <ArduinoJson.h>

#include "log.h"
#include "config/AppConfig.hpp"

namespace config {

  // Configファイルの初期ロード、Configのアップデート時に使用される
  void mergeJsonToConfig(DynamicJsonDocument doc, AppConfig *cfg) {
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
    if (doc.containsKey("mDNSName")) {
      cfg->mDNSName = doc["mDNSName"].as<String>();
    }
    if (doc.containsKey("rotation")) {
      cfg->rotation = doc["rotation"].as<int>();
    }

  }

  void _saveConfigToJson(AppConfig *cfg, DynamicJsonDocument *pdoc) {
    (*pdoc)["configId"] = cfg->configId;
    (*pdoc)["ssid"] = cfg->ssid;
    (*pdoc)["password"] = cfg->password;
    (*pdoc)["brightness"] = cfg->brightness;
    (*pdoc)["mDNSName"] = cfg->mDNSName;
    (*pdoc)["rotation"] = cfg->rotation;
  }

}
