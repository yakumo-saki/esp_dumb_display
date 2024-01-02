#include <Arduino.h>

void real_log(String msgString, String prefixString) {
  char log[130];
  char prefix[10];
  char msg[100];
  prefixString.toCharArray(prefix, sizeof prefix);
  msgString.toCharArray(msg, sizeof msg);
  snprintf(log, sizeof log, "%08lu %-10s: %s", millis(),  prefix, msg);
  Serial.println(log);
}

void mainlog(String msg) {
  real_log(msg, F("MAIN"));
}

void lpslog(String msg) {
  real_log(msg, F("LPS22HB"));
}

void mhzlog(String msg) {
  real_log(msg, F("MHZ19B"));
}

void tsllog(String msg) {
  real_log(msg, F("TSL2561"));
}

void bmelog(String msg) {
  real_log(msg, F("BME280"));
}

void amlog(String msg) {
  real_log(msg, F("AM2320"));
}

void bhlog(String msg) {
  real_log(msg, F("BH1750"));
}

void adtlog(String msg) {
  real_log(msg, F("ADT7410"));
}

void cfglog(String msg) {
  real_log(msg, F("CONFIG"));
}

void httplog(String msg) {
  real_log(msg, F("HTTP"));
}

void displog(String msg) {
  real_log(msg, F("DISPLAY"));
}

void mdnslog(String msg) {
  real_log(msg, F("mDNS"));
}

void wifilog(String msg) {
  real_log(msg, F("WiFi"));
}

void i2clog(String msg) {
  real_log(msg, F("I2C"));
}
void ssdlog(String msg) {
  real_log(msg, F("SSD1306"));
}
void stlog(String msg) {
  real_log(msg, F("ST7789"));
}

void statlog(String msg) {
  real_log(msg, F("STAT"));
}

void haltlog(String msg) {
  real_log(msg, F("HALT"));
}

void deltalog(String msg) {
  real_log(msg, F("DELTA"));
}

void apilog(String msg) {
  real_log(msg, F("API"));
}

void debuglog(String msg) {
  real_log(msg, F("DEBUG"));
}

void sectionlog(String msg) {
  Serial.println(F(""));
  real_log(msg, F("========"));
  Serial.println(F(""));
}

