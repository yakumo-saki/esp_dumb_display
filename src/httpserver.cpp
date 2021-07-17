#include <Arduino.h>

#include "log.h"
#include "const.h"

#include "httpserver.h"
#include "display.h"
#include "webfetch.h"

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

  setDisplayBrightness(brightness);

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
  String reason = load_png(url.c_str());

  String json = create_json(true, KEY, reason);
  server.send(200, "application/json", json);
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

  String reason = load_jpg(JPG_FILEPATH);

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

/**
 * 初期化(設定用Webサーバモード)
 */
void setup_http_server() {
  httplog(F("HTTP web server initializing"));
  server.on("/api/brightness", HTTP_POST, handle_brightness);
  server.on("/api/beep", HTTP_POST, handle_beep);
  server.on("/api/png", HTTP_POST, handle_update_png);
  server.on("/api/jpg", HTTP_POST, handle_update_jpg);
  server.on("/", HTTP_GET, handle_get);
  server.begin();
  httplog(F("HTTP web server initialized"));
}

void loop_http_server() {
  server.handleClient();
}