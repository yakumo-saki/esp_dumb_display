#include <Arduino.h>
#include <ArduinoJson.h>

#include "log.h"
#include "global.hpp"
#include "config/config.hpp"

#include "httpCore.hpp"

#include "mimetype.hpp"
#include "http_utils.h"
#include "api_util.h"

namespace httpServer {

  String status_json() {
    auto cfg = config::getConfig();

    unsigned long ms = millis();
    String timeString = getTimeString(ms);

    DynamicJsonDocument doc(2000);
    doc["product"] = global::product;
    doc["datetime"] = ""; // NTP not impl
    doc["uptime"] = timeString;
    doc["uptimeMills"] = ms;
    doc["majorVer"] = global::ver;
    doc["minorVer"] = global::minorVer;
    doc["settingId"] = cfg->configId;
    doc["name"] = cfg->mDNSName;
    doc["build"] = global::DEBUG_BUILD ? "DEBUG" : "Release";

    doc["mode"] = global::operationMode;

    String json;
    serializeJson(doc, json);
    doc.clear();

    httplog(json);
    return json;
  }

  void handleStatusGet() {
    sendHttpHeader(MimeType::JSON);

    String jsonStr = status_json();

    server.sendContent(jsonStr);
  }
}
