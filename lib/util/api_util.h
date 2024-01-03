#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>

bool parseBooleanString(const String value);
std::vector<String> stringSplit(String value, String delimiter);
String jsonToString(DynamicJsonDocument& json);
String http_normal_ping_json();

// millisecond -> 00:00:00 形式の日付文字列を返す
String getTimeString(unsigned long ms);
