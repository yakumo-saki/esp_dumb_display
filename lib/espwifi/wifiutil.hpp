#include <Arduino.h>

void make_sure_wifi_connected(String ssid, String password, String hostname,
                              std::function<void(void)> watchdog_feeder, std::function<void(String)> logger);
