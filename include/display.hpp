#include <Arduino.h>

namespace display {

  void setup();

  String loadPng(const char *url, double scale = 1.0);
  String loadJpg(String path, double scale = 1.0);

  void setDisplayBrightness(int brightness);
  void drawReadyScreen(String ip, String mdns);
  void drawWifiConnectingScreen();
  void drawWifiAPScreen(String ip, String ssid);

}
