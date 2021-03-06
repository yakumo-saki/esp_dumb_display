#include <Arduino.h>

#include "log.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#endif

extern WiFiClient client;

// https://raw.githubusercontent.com/Bodmer/TJpg_Decoder/master/examples/Web_Jpg/Web_Fetch.h

// Fetch a file from the URL given and save it in SPIFFS
// Return 1 if a web fetch was needed or 0 if file already exists
bool getFile(String url, String filename) {

  wifilog("Downloading "  + filename + " from " + url);

  // Check WiFi connection
  if ((WiFi.status() != WL_CONNECTED)) {
    wifilog("WiFi status != WL_CONNECTED. fail");
    return false;
  }

  wifilog("[HTTP] begin...");

#ifdef ESP8266
  HTTPClient http;

  if (false) {
    // std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    // client -> setInsecure();
    // http.begin(*client, url);
  } else {
    http.begin(client, url);
  }
#else
  HTTPClient http;
  // Configure server and url
  http.begin(url);
#endif

  // Start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode < 0) {
    // connection failed
    wifilog("HTTP GET... failed, error: " + String(httpCode) + " " + http.errorToString(httpCode));
    http.end();
    return false;
  }

  // HTTP header has been send and Server response header has been handled
  wifilog("HTTP GET... code: " + String(httpCode));

  // File found at server
  if (httpCode != HTTP_CODE_OK) {
    http.end();
    wifilog("HTTP Get failed.");
    return false;
  }

  fs::File f = SPIFFS.open(filename, "w+");
  if (!f) {
    wifilog("file open failed");
    return false;
  }

  // Get length of document (is -1 when Server sends no Content-Length header)
  int total = http.getSize();
  int len = total;

  // Create buffer for read
  uint8_t buff[128] = { 0 };

  // Get tcp stream
  WiFiClient * stream = http.getStreamPtr();

  // Read all data from server
  while (http.connected() && (len > 0 || len == -1)) {
    // Get available data size
    size_t size = stream->available();

    if (size) {
      // Read up to 128 bytes
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

      // Write it to file
      f.write(buff, c);

      // Calculate remaining bytes
      if (len > 0) {
        len -= c;
      }
    }
    yield();
  }

  wifilog("HTTP connection closed or file end.");

  f.close();
  http.end();

  return true; // File was fetched from web
}
