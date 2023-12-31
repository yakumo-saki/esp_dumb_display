#include <Arduino.h>
#include <vector>
#include <numeric>

// WiFi
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

std::vector<long> rssiArray;

String wl_status_t_to_string(wl_status_t wl_stat) {
    if (wl_stat == WL_NO_SHIELD) {
        return F("WL_NO_SHIELD");
    } else if (wl_stat == WL_IDLE_STATUS) {
        return F("WL_IDLE_STATUS");
    } else if (wl_stat == WL_NO_SSID_AVAIL) {
        return F("WL_NO_SSID_AVAIL");
    } else if (wl_stat == WL_SCAN_COMPLETED) {
        return F("WL_SCAN_COMPLETED");
    } else if (wl_stat == WL_CONNECTED) {
        return F("WL_CONNECTED");
    } else if (wl_stat == WL_CONNECT_FAILED) {
        return F("WL_CONNECT_FAILED");
    } else if (wl_stat == WL_CONNECTION_LOST) {
        return F("WL_CONNECTION_LOST");
    } else if (wl_stat == WL_DISCONNECTED) {
        return F("WL_DISCONNECTED");
    }

    return "UNKNOWN_STATUS";

}

void try_wifilog(std::function<void(String)> logger, String msg) {
  if (logger != NULL) {
    logger(msg);
  }
}

void try_watchdog_feed(std::function<void(void)> watchdogFeeder) {
  if (watchdogFeeder != NULL) {
    watchdogFeeder();
  }
}

/**
 * WiFi接続する
 */
void make_sure_wifi_connected(String ssid, String password, String hostname,
                              std::function<void(void)> watchdog_feeder, std::function<void(String)> logger) {
  
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  auto wifilog = [&](String msg) { try_wifilog(logger, msg); };
  auto wifilogc = [&](const char msg[]) { try_wifilog(logger, String(msg)); };

  try_watchdog_feed(watchdog_feeder);

  WiFi.setHostname(hostname.c_str());
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.enableAP(false);

  delay(100);
  try_watchdog_feed(watchdog_feeder);

  int retryCount = 0;
  WiFi.begin(ssid.c_str(), password.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {

  try_watchdog_feed(watchdog_feeder);
    delay(300);
    retryCount++;

    if (retryCount % 10 == 0) {
      wifilog("WiFI.status() = " + wl_status_t_to_string(WiFi.status()));
      wifilog(F("Still waiting for wifi connection"));
    }
    if (retryCount % 30 == 0) {
      wifilog(F("Restarting WiFi"));
      wifilog(F("WiFi disconnect."));
      WiFi.disconnect();   
      delay(100);
      WiFi.setHostname(hostname.c_str());
      WiFi.begin(ssid.c_str(), password.c_str());
      wifilog(F("RETRY connecting WiFi from start"));
    }
  }

  wifilog(F("WiFi (re) connected."));
  wifilog("IP address: " + WiFi.localIP().toString());
}


/**
 * WiFi AP モードを起動する
 */
void start_wifi_access_point(String ssidSuffix, std::function<void(String)> logger) {
  byte mac[6];
  WiFi.macAddress(mac);

  auto wifilog = [&](String msg) { try_wifilog(logger, msg); };
  auto wifilogc = [&](const char msg[]) { try_wifilog(logger, String(msg)); };

  // SSID は macaddress をSUFFIXする。前半はespressifのIDなので後半3つだけ
  String ssid = "_SETUP_" + ssidSuffix;
  for (int i = 3; i < 6; i++) {
    ssid += String(mac[i], HEX);
  }
  
  wifilog("SSID: " + ssid);

  WiFi.softAP(ssid.c_str());
  wifilog("WiFi AP Started. SSID=" + ssid);
}

/**
 * WiFiのIPアドレスを得る
 */
String get_wifi_ip_addr() {
  return WiFi.localIP().toString();
}

int8_t get_wifi_rssi() {

  if (WiFi.status() == WL_CONNECTED) {
    rssiArray.push_back(WiFi.RSSI());
  } else {
    rssiArray.push_back(0);
  }

  if (rssiArray.size() > 5) {
    rssiArray.erase(rssiArray.begin());
  }

  int sum = std::accumulate(rssiArray.begin(), rssiArray.end(), 0);
  int avg = sum / rssiArray.size();

  return avg;
}
