#include <Arduino.h>

#include "log.h"
#include "const.h"

#include "configApi.hpp"
#include "display.hpp"
#include "webfetch.hpp"

#include "httpCore.hpp"
#include "httpserver.hpp"

#include "static.hpp"
#include "status.hpp"


namespace httpServer {

  HTTPWEBSERVER server(80);

  String create_json(bool success, String method, String reason) {
    String json = "{ ";
    json = json + " \"success\": " + (success ? "true" : "false") + ",";
    json = json + " \"reason\": \"" + reason + "\"";
    json = json + " }";

    return json;
  }

  void handle_brightness() {
    const String KEY = "brightness";

    bool argExist = server.hasArg(KEY);
    if (argExist == false) {
      String json = create_json(false, KEY, "No brightness");
      server.send(400, "application/json", json);
      return;
    }

    String brightnessStr = server.arg(KEY);

    debuglog("server arg brightness=" + brightnessStr);
    int brightness = brightnessStr.toInt();

    display::setDisplayBrightness(brightness);

    String json = create_json(true, KEY, "brightness = " + brightnessStr);
    server.send(200, "application/json", json);
  }

  void handle_update_png() {
    const String KEY = "url";
    bool argExist = server.hasArg(KEY);
    if (argExist == false) {
      String json = create_json(false, KEY, "No URL");
      server.send(400, "application/json", json);
      return;
    }

    String url = server.arg(KEY);
    // String reason = loadPng(url.c_str());

    //String json = create_json(true, KEY, reason);
    String json = create_json(false, KEY, "Not implemented");
    server.send(500, "application/json", json);
  }

  void handle_update_jpg() {
    const String KEY = "url";
    bool argExist = server.hasArg(KEY);
    if (argExist == false) {
      String json = create_json(false, KEY, "No URL");
      server.send(400, "application/json", json);
      return;
    }

    String url = server.arg(KEY);

    bool got = getFile(url, JPG_FILEPATH);
    if (got == false) {
      String json = create_json(false, KEY, "URL fetch failed");
      server.send(400, "application/json", json);
      httplog("File download failed. exit.");
      return;
    } else {
      httplog("File download complete. continue");
    }

    String reason = display::loadJpg(JPG_FILEPATH);

    String json = create_json(true, KEY, reason);
    server.send(200, "application/json", json);
  }

  void handle_get() {
    String json = create_json(true, "GET_ROOT", "This is ESP DUMB DISPLAY ver" + VER);
    server.send(200, "application/json", json);
  }

  void handle_beep_pc98() {
    tone(BEEP_PIN, 2000, 100);
    delay(100);
    tone(BEEP_PIN, 1000, 100);
    delay(100);
  }

  void handle_beep_warning() {
    int longms = 200;
    tone(BEEP_PIN, 2000, longms);
    delay(longms);
    delay(50);
    tone(BEEP_PIN, 2000, longms);
    delay(longms);
    delay(50);
    tone(BEEP_PIN, 2000, longms);
    delay(longms);
    delay(50);
  }

  void handle_beep() {
    const String KEY = "type";
    bool argExist = server.hasArg(KEY);
    if (argExist == false) {
      String json = create_json(false, KEY, "no type specified");
      server.send(400, "application/json", json);
      return;
    }

    String type = server.arg(KEY);

    httplog("Start beep");
    if (type == "pc98") {
      handle_beep_pc98();
    } else {
      handle_beep_warning();
    }
    digitalWrite(BEEP_PIN, 0);
    httplog("End   beep");

    String json = create_json(true, "BEEP", "beep finished " + type);
    server.send(200, "application/json", json);
  }

  void setupWebConfig() {
    server.on("/api/v1/config", HTTP_GET, handleConfigGet);
    server.on("/api/v1/config", HTTP_POST, handleConfigSet);

    server.on("/static/config.js", HTTP_GET, handleStatic_configjs);
    server.on("/static/style.css", HTTP_GET, handleStatic_stylecss);
    server.on("/", HTTP_GET, handleSetupGet);
  }

  /**
   * 初期化 (SETUP MODE)
   */
  void setupSetupMode() {
    httplog(F("HTTP web server (setup mode) initializing"));
    server.on("/api/status", HTTP_GET, handleStatusGet);

    // config web
    setupWebConfig();

    server.begin();
    httplog(F("HTTP web server (setup mode) initialized"));
  }

  /**
   * 初期化
   */
  void setupNormalMode() {
    httplog(F("HTTP web server initializing"));
    server.on("/api/brightness", HTTP_POST, handle_brightness);
    server.on("/api/beep", HTTP_POST, handle_beep);
    server.on("/api/png", HTTP_POST, handle_update_png);
    server.on("/api/jpg", HTTP_POST, handle_update_jpg);

    server.on("/api/status", HTTP_GET, handleStatusGet);

     // config web
    setupWebConfig();

    server.enableCORS(true);

    server.begin();
    httplog(F("HTTP web server initialized"));
  }

  void loop() {
    server.handleClient();
  }
}
