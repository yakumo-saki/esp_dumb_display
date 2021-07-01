#include <Arduino.h>

void setup_tft();

String load_png(const char *url, double scale = 1.0);
String load_jpg(String path, double scale = 1.0);

void draw_ready_screen(String ip, String mdns);
void draw_wifi_connecting_screen();