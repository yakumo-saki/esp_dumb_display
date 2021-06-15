#include <Arduino.h>

// network
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#include "_define_wifi.h"
#include <math.h>
#include "pngle.h"

#include "SPI.h"
#include "TFT_eSPI.h"

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

WiFiClient client;

int count = 0;

void cls()
{
  tft.fillScreen(TFT_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
}

double g_scale = 1.0;
void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  uint16_t color = (rgba[0] << 8 & 0xf800) | (rgba[1] << 3 & 0x07e0) | (rgba[2] >> 3 & 0x001f);

  if (rgba[3]) {
    x = ceil(x * g_scale);
    y = ceil(y * g_scale);
    w = ceil(w * g_scale);
    h = ceil(h * g_scale);
    tft.fillRect(x, y, w, h, color);
  }
}

void load_png(const char *url, double scale = 1.0)
{
  HTTPClient http;

  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.println("HTTP ERROR: " + String(httpCode));
    http.end();
    return;
  }

  Serial.println("HTTP OK: " + String(httpCode));
  Serial.println("Start decoding");
//   cls();

  WiFiClient *stream = http.getStreamPtr();

  pngle_t *pngle = pngle_new();
  pngle_set_draw_callback(pngle, pngle_on_draw);
  g_scale = scale; // XXX:

  uint8_t buf[2048];
  int remain = 0;
  while (http.connected()) {
    size_t size = stream->available();
    if (!size) { delay(1); continue; }

    if (size > sizeof(buf) - remain) {
      size = sizeof(buf) - remain;
    }

    int len = stream->readBytes(buf + remain, size);
    if (len > 0) {
      int fed = pngle_feed(pngle, buf, remain + len);
      if (fed < 0) {
        cls();
        tft.printf("ERROR: %s\n", pngle_error(pngle));
        break;
      }

      remain = remain + len - fed;
      if (remain > 0) memmove(buf, buf + fed, remain);
    } else {
      delay(1);
    }
  }

  pngle_destroy(pngle);

  http.end();
}
// ===================================================

void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);

  tft.printf("Welcome.\n");

  tft.printf("Connecting to wifi.\n");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  tft.printf("WiFi connected.\n");
}

void loop()
{
  Serial.println("Image update " + String(count));

  load_png("http://10.10.10.10:80/intra/dumbdisplay/image.png");

  count++;
  delay(15000);
}
