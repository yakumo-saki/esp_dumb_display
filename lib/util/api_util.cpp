#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

#include "log.h"

// millisecond -> 00:00:00 形式の日付文字列を返す
String getTimeString(unsigned long ms) {
  unsigned long second = ms / 1000;
  unsigned long minute = second / 60;

  int sec = second % 60;
  int min = (second / 60) % 60;
  int hr =  minute / 60;

  char buf[15];
  snprintf (buf, sizeof buf,"%02d:%02d:%02d", hr, min, sec);
  return String(buf);
}

String jsonToString(DynamicJsonDocument& json) {
    String jsonStr;
    serializeJson(json, jsonStr);
    return jsonStr;
}

std::vector<String> stringSplit(String value, String delimiter) {
    std::vector<String> ret;

    String temp = "";
    for (unsigned int i = 0; i < value.length(); i++) {
        String v;
        if (i + 1 > value.length()) {
            v = value.substring(i);
        } else {
            v = value.substring(i, i + 1);
        }
        if (v == delimiter) {
            temp.trim();
            ret.push_back(temp);
            temp = "";
        } else {
            temp += v;
        }
    }

    temp.trim();
    ret.push_back(temp);

    return ret;
}
