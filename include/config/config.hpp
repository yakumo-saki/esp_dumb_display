#include <ArduinoJson.h>
#include "config/AppConfig.hpp"

namespace config {

  extern const String MSG_OK;

  AppConfig* getConfig();

  // flag files
  void createFlagfile();
  void deleteFlagfile();
  bool hasFlagFile();

  // config
  bool readConfig();
  bool saveConfig();
  void mergeJsonToConfig(DynamicJsonDocument doc, AppConfig *cfg);

  bool needConfigMigrate();
  bool migrateConfig();
  String validate(String key,String value);

  DynamicJsonDocument serializeConfig();
}
