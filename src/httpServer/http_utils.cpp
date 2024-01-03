#include "global.hpp"
#include "mimetype.hpp"
#include "httpCore.hpp"

namespace httpServer {

  void sendHttpHeader(String contentType) {
    server.sendContent("HTTP/1.1 200 OK\r\n");
    server.sendContent("Content-Type: " + contentType + "\r\n");
    if (global::DEBUG_BUILD) {
      server.sendContent("Access-Control-Allow-Origin: *\r\n");
    }
    server.sendContent("\r\n");
  }

  void sendHttpRedirectHeader(String url) {
    server.sendContent("HTTP/1.0 301 Moved Permanently\r\n");
    server.sendContent("Location: " + url + "\r\n");
    server.sendContent("\r\n");
  }

  void sendHttpHeader() {
    sendHttpHeader(MimeType::HTML);
  }
}
