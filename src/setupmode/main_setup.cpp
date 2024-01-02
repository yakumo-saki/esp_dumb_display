#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "TimerCall.h"
#include "config/config.hpp"
#include "display.hpp"
#include "global.hpp"
#include "log.h"

#include "job/printMemFree.hpp"

#include "httpserver.hpp"

namespace setupMode {

  TimerCall timer;

  /**
   * 初期化
   */
  void setup() {
    sectionlog("Starting setup mode.");

    // configured
    // ファイルがなければconfigを読まずにここに来るので一度configを読む。
    if (config::readConfig()) {
      mainlog("Config load successful.");
    } else {
      // failedしているが、デフォルト値は入っているのでOK.
      mainlog("Config load failed.");
    }

    // initialize display
    display::setup();
    display::drawWifiConnectingScreen();
    // disp_setup_startup_screen(WiFi.softAPIP().toString(), WiFi.softAPSSID());

    timer.add(printMemFree, "MEMFREE", 60000);
    timer.start();

    mainlog(F("Setup complete. Setup mode ready."));
  }

  void loop() {
    timer.loop();
    httpServer::loop();
  }
}
