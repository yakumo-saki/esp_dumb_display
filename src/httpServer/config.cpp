#include "const.h"
#include "httpCore.hpp"

namespace httpServer {
  extern HTTPWEBSERVER server;

  String build_json(bool success, String method, String reason) {
    String json = "{ ";
    json = json + " \"success\": " + (success ? "true" : "false") + ",";
    json = json + " \"reason\": \"" + reason + "\"";
    json = json + " }";

    return json;
  }

  void handle_config_get() {
    String json = build_json(true, "CONFIG_GET", "This is ESP DUMB DISPLAY ver" + VER);
    server.send(200, "application/json", json);
  }
  void handle_config_set() {
    String json = build_json(true, "SET_CONFIG", "This is ESP DUMB DISPLAY ver" + VER);
    server.send(200, "application/json", json);
  }
  void handle_setup_get() {
    String json = build_json(true, "SETUP_GET", "This is ESP DUMB DISPLAY ver" + VER);
    server.send(200, "application/json", json);
  }
  void handle_setup_post() {
    String json = build_json(true, "SETUP_POST", "This is ESP DUMB DISPLAY ver" + VER);
    server.send(200, "application/json", json);
  }

}
