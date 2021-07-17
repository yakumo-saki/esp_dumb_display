#include <Arduino.h>

#include "log.h"
#include "const.h"

#include "pngle.h"

#include "SPI.h"
#include "TFT_eSPI.h"

#include <TJpg_Decoder.h>

// network
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

WiFiClient client;

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

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

/**
 * load PNG file using pngle.
 * In ESP8266 , this causes out of memory
 */
String load_png(const char *url, double scale = 1.0)
{
  HTTPClient http;

  displog("Start loading " + String(url));

  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    displog("HTTP ERROR: " + String(httpCode));
    http.end();
    return "HTTP ERROR";
  }

  displog("HTTP OK: " + String(httpCode));
  displog("Start decoding");
//   cls();

  WiFiClient *stream = http.getStreamPtr();

  pngle_t *pngle = pngle_new();
  pngle_set_draw_callback(pngle, pngle_on_draw);
  g_scale = scale; // XXX:

  uint8_t buf[2048];
  int remain = 0;
  while (http.connected()) {
    size_t size = stream->available();
    if (!size) { 
      delay(1);
      continue; 
    }

    if (size > sizeof(buf) - remain) {
      size = sizeof(buf) - remain;
    }

    int len = stream->readBytes(buf + remain, size);
    if (len > 0) {
      int fed = pngle_feed(pngle, buf, remain + len);
      if (fed < 0) {
        cls();
        tft.printf("ERROR: %s\n", pngle_error(pngle));
        displog("ERROR:" + String(pngle_error(pngle)));
        return "ERROR:" + String(pngle_error(pngle));
      }

      remain = remain + len - fed;
      if (remain > 0) memmove(buf, buf + fed, remain);
    } else {
      delay(1);
    }
  }

  displog("Decoding done");
  pngle_destroy(pngle);

  http.end();

  displog("Success");
  return RET_OK;
}

// from https://platformio.org/lib/show/6906/TJpg_Decoder
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

/**
 * load JPG file using pngle.
 * @return error message or RET_OK
 */
String load_jpg(String path, double scale = 1.0) {
  displog("JPG decode start " + path);

  uint16_t w = 0, h = 0;
  TJpgDec.getFsJpgSize(&w, &h, path); // Note name preceded with "/"
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  uint32_t t = millis();

  TJpgDec.drawFsJpg(0, 0, path);
  displog("JPG decode done");
  t = millis() - t;
  displog("JPG decode & draw took " + String(t) + "ms");
  return RET_OK;
}


void draw_ready_screen(String ip, String mdns) {
  cls();   

  tft.printf("WiFi connection established.\n");
  tft.printf("Waiting for your order.\n");
  tft.printf(ip.c_str());
  tft.printf("\n");
  
  tft.printf(mdns.c_str());
  tft.printf("\n");
}

void draw_wifi_connecting_screen() {
  cls();
  tft.printf("Welcome.\n");
  tft.printf("Connecting to wifi.\n");

}

void setDisplayBrightness(int brightness) {
  // ILI9341 Backlight is controlled by PWM.
  analogWrite(TFT_BL, brightness);
}

void setup_tft() {
  tft.begin();
  tft.setRotation(1);

  // TJpgDec
  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  cls();
}
