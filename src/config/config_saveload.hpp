#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/AppConfig.hpp"

namespace config {
  void _loadJsonToConfig(DynamicJsonDocument doc, AppConfig *cfg);
  void _saveConfigToJson(AppConfig *cfg, DynamicJsonDocument *doc);
}
