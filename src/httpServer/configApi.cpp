#include <Arduino.h>
#include <ArduinoJson.h>

#include "const.h"
#include "httpCore.hpp"

#include "global.hpp"
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

    // Validation
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

    // update config
    if (msgs.size() == 0) {
      DynamicJsonDocument cfgUpdate(4000);
      cfgUpdate["configId"] = global::SETTING_ID;

      for (int i = 0; i < server.args(); i++) {
        String key = server.argName(i);
        String value = server.arg(i);

        cfgUpdate[key] = value;
      }

      // wifi passwordが空なら更新しない
      if (cfgUpdate["password"] == "") {
        cfgUpdate.remove("password");
      }

      config::mergeJsonToConfig(cfgUpdate, config::getConfig());
      cfgUpdate.clear();
    }

    // ここ自体は大した容量を必要としないのでこれで十分。ESP8266ではメモリがカツカツなので無駄に増やさないこと
    DynamicJsonDocument json(1000);
    json["command"] = "CONFIG_SET";
    json["success"] = !failed;
    json["msgs"] = msgs;

    if (failed) {
      json["message"] = "Error detected. Check msgs. Settings not changed.";
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

  void handleConfigCommit() {
    apilog("handleConfigCommit Start");
    config::saveConfig();

    DynamicJsonDocument doc(1024);
    auto cfg = config::serializeConfig();
    doc["success"] = true;

    String jsonStr;
    serializeJson(doc, jsonStr);

    sendHttpHeader(MimeType::JSON);
    server.sendContent(jsonStr);

    apilog("handleConfigCommit end");
  }

}
