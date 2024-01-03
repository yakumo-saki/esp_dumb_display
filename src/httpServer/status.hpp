#include <Arduino.h>
#include <ArduinoJson.h>

#include "log.h"
#include "global.hpp"
#include "config/config.hpp"
#include "http_utils.h"
#include "api_util.h"

namespace httpServer {
  void handleStatusGet();
}
