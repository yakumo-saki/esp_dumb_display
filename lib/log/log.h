#include <Arduino.h>

void mainlog(String msg);
void lpslog(String msg);
void mhzlog(String msg);
void tsllog(String msg);
void bmelog(String msg);
void amlog(String msg);
void bhlog(String msg);
void adtlog(String msg);
void cfglog(String msg);
void httplog(String msg);
void displog(String msg);
void mdnslog(String msg);
void wifilog(String msg);
void i2clog(String msg);
void stlog(String msg);
void ssdlog(String msg);
void statlog(String msg);
void deltalog(String msg);
void apilog(String msg);
void haltlog(String msg);
void debuglog(String msg);
void sectionlog(String msg);

// I2C エラーコードをメッセージに変換
String get_i2c_error_message(byte code);