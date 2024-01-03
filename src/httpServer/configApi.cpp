#include <Arduino.h>
#include <ArduinoJson.h>

#include "const.h"
#include "httpCore.hpp"

#include "config/config.hpp"

#include "log.h"
#include "mimetype.hpp"
#include "embed/config_html.h"
#include "http_utils.h"
#include "api_util.h"

namespace httpServer {
  extern HTTPWEBSERVER server;

  String build_json(bool success, String method, String reason) {
    String json = "{ ";
    json = json + " \"success\": " + (success ? "true" : "false") + ",";
    json = json + " \"reason\": \"" + reason + "\"";
    json = json + " }";

    return json;
  }

  // セットアップ画面のHTMLを送信
  void handleSetupGet() {
    server.send(200, MimeType::HTML, CONFIG_HTML);
  }

  void _get_config() {

    sendHttpHeader(MimeType::JSON);

    String keys = server.arg("key");

    std::vector<String> keyArray = stringSplit(keys, ",");

    DynamicJsonDocument json = config::serializeConfig();
    json["command"] = "CONFIG_GET";
    json["success"] = true;

    String jsonStr;
    serializeJson(json, jsonStr);
    server.sendContent(jsonStr);
  }

  void _commit_config() {

    DynamicJsonDocument json(100);

    json["command"] = "CONFIG_COMMIT";
    json["success"] = true;
    config::saveConfig();

    sendHttpHeader(MimeType::JSON);

    String jsonStr;
    serializeJson(json, jsonStr);
    server.sendContent(jsonStr);
  }

  void _backup_config() {

    sendHttpHeader(MimeType::TEXT);

    // String ret = http_api_backup_config();

    server.sendContent("NOT IMPL");
  }

  // APIで飛んできた値で設定変更（バリデーション＆反映）
  String updateConfig() {

    // DynamicJsonDocument.createNestedArray() スべきだがそうするとここですべての
    // JSONの容量を食ってしまい、メモリ不足で落ちる(ESP8266)
    DynamicJsonDocument msgs(2000);

    bool failed = false;

    // dump
    for (int i = 0; i < server.args(); i++) {
      String key = server.argName(i);
      String value = server.arg(i);
      apilog("key=" + key + " value=" + value);
    }

    for (int i = 0; i < server.args(); i++) {
      String key = server.argName(i);
      String value = server.arg(i);

      if (key == "plain") {
        continue;  // ESP8266実装では、POST BODY が plain として渡されるので無視
      }

      String msg = config::validate(key, value);
      if (msg != config::MSG_OK) {
        msgs.add(msg);
      }
    }

    // ここ自体は大した容量を必要としないのでこれで十分。ESP8266ではメモリがカツカツなので無駄に増やさないこと
    DynamicJsonDocument json(1000);
    json["command"] = "CONFIG_SET";
    json["success"] = !failed;
    json["msgs"] = msgs;

    if (failed) {
      json["message"] = "Some error detected. Check msgs.";
    } else {
      json["message"] = "Don't forget calling config/commit. API";
    }
    json.shrinkToFit();

    String jsonStr;
    serializeJson(json, jsonStr);

    return jsonStr;
  }

  void handleConfigSet() {

    sendHttpHeader(MimeType::JSON);

    String jsonStr = updateConfig();

    server.sendContent(jsonStr);
  }

  void handleConfigGet() {
    apilog("handleConfigGet start");

    sendHttpHeader(MimeType::JSON);

    DynamicJsonDocument doc(1024);
    auto cfg = config::serializeConfig();
    cfg.remove("password");

    doc["success"] = true;
    doc["config"] = cfg;

    String jsonStr;
    serializeJson(doc, jsonStr);

    apilog(jsonStr);
    server.sendContent(jsonStr);
    apilog("handleConfigGet end");
  }

}
