#ifdef ESP32

#include <Arduino.h>

#include "log.h"
#include "const.h"

#include "SPI.h"
#include <HTTPClient.h>
#include "global.hpp"
#include <vector>

#include "display32/JpegFunc.h"

#include <Arduino_GFX_Library.h>

// ESP32_4827S043
// default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define GFX_BL DF_GFX_BL 
#define TFT_BL 2

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
    0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
    0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
    1 /* pclk_active_neg  */, 9000000 /* prefer_speed */
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    480 /* width */, 272 /* height */, rgbpanel
);


void cls()
{
  gfx->fillScreen(BLACK);
}

double g_scale = 1.0;

int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

/**
 * load JPG file using .
 * @return error message or RET_OK
 */
String load_jpg(String path, double scale = 1.0) {
  displog("JPG decode start " + path);

  uint32_t t = millis();

  uint16_t w = 0, h = 0;
  char path_char[path.length() + 1];
  path.toCharArray(path_char, path.length() + 1);

  jpegDraw(path_char, jpegDrawCallback, true /* useBigEndian */,
           0 /* x */, 0 /* y */,
           gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);

  displog("JPG decode done");
  t = millis() - t;
  displog("JPG decode & draw took " + String(t) + "ms");
  return RET_OK;
}


void draw_ready_screen(String ip, String mdns) {
  cls();   

  gfx->setCursor(0, 0);
  gfx->setTextColor(WHITE);

  gfx->printf("WiFi connection established.\n");
  gfx->printf("Waiting for your order.\n");
  gfx->printf(ip.c_str());
  gfx->printf("\n");
  gfx->printf(mdns.c_str());
  gfx->printf("\n");
}

void draw_wifi_connecting_screen() {
  cls();

  gfx->setCursor(0, 0);
  gfx->setTextColor(WHITE);

  gfx->printf("Welcome.\n");
  gfx->printf("Connecting to wifi.\n");

}

void setDisplayBrightness(int brightness) {
#ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
#endif
}

void setup_tft() {
  displog("Arduino_GFX init");
  gfx->begin();
  gfx->fillScreen(BLACK);

#ifdef TFT_BL
  displog("Backlight control is stub.");
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
}

#endif
