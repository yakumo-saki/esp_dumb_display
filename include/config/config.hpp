#include "config/AppConfig.hpp"

namespace config {

  AppConfig* getConfig();

  // flag files
  void createFlagfile();
  void deleteFlagfile();
  bool hasFlagFile();

  // config
  bool readConfig();
  bool saveConfig();
  bool needConfigMigrate();
  bool migrateConfig();
}
