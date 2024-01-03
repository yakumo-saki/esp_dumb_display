#ifdef ESP32

#include <Arduino.h>

#include "log.h"
#include "const.h"

#include "SPI.h"
#include <HTTPClient.h>
#include "global.hpp"
#include <vector>

#include "display/esp32/JpegFunc.h"

#include <U8g2lib.h>
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

namespace display {

  double g_scale = 1.0;

  void cls() {
    gfx->fillScreen(BLACK);
  }

  void _writeText(int x, int y, String text) {
    gfx->setCursor(x, y);
    gfx->printf(text.c_str());
  }

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
  String loadJpg(String path, double scale = 1.0) {
    displog("JPG decode start " + path);
    gfx->startWrite();
    cls();

    uint32_t t = millis();

    uint16_t w = 0, h = 0;

    jpegDraw(path.c_str(), jpegDrawCallback, true /* useBigEndian */,
            0 /* x */, 0 /* y */,
            gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);

    displog("JPG decode done");
    t = millis() - t;
    displog("JPG decode & draw took " + String(t) + "ms");

    gfx->endWrite();

    return RET_OK;
  }


  void drawReadyScreen(String ip, String mdns) {
    gfx->startWrite();
    cls();

    gfx->drawRect(0, 0, gfx->width() - 3, gfx->height() - 5, RGB565_WHITE);
    gfx->drawRect(1, 1, gfx->width() - 4, gfx->height() - 6, RGB565_WHITE);
    gfx->drawRect(2, 2, gfx->width() - 5, gfx->height() - 7, RGB565_WHITE);

    gfx->setFont(u8g2_font_UnnamedDOSFontIV_tr);
    gfx->setTextColor(WHITE);

    _writeText(30, 30, "WiFi connection established.");
    _writeText(30, 50, "Waiting for your order.");
    _writeText(30, 80, ip.c_str());
    _writeText(30, 100, mdns.c_str());

    gfx->endWrite();

  }

  void drawWifiAPScreen(String ip, String ssid) {
    displog("drawWifiAPScreen start.");
    gfx->startWrite();
    cls();

    gfx->drawRect(8, 8, gfx->width() - 18, gfx->height() - 20, RGB565_PURPLE);
    gfx->drawRect(6, 6, gfx->width() - 14, gfx->height() - 16, RGB565_GREEN);
    gfx->drawRect(4, 4, gfx->width() - 10, gfx->height() - 12, RGB565_BLUE);
    gfx->drawRect(2, 2, gfx->width() - 6, gfx->height() - 8, RGB565_RED);
    gfx->drawRect(0, 0, gfx->width() - 2, gfx->height() - 4, RGB565_WHITE);

    // gfx->setFont(u8g2_font_unifont_h_utf8);
    gfx->setFont(u8g2_font_UnnamedDOSFontIV_tr);
    gfx->setTextColor(WHITE);

    _writeText(30,30, "Welcome to setup.");
    _writeText(30,60, "WiFi info:");
    _writeText(30,80, "SSID: " + ssid);
    _writeText(30,110, "Access via web browser:");
    _writeText(30,130, "http://" + ip);

    gfx->endWrite();

    displog("drawWifiAPScreen done.");
  }

  void drawWifiConnectingScreen() {
    displog("drawWifiConnectingScreen start.");
    gfx->startWrite();
    cls();

    gfx->drawRect(8, 8, gfx->width() - 18, gfx->height() - 20, RGB565_PURPLE);
    gfx->drawRect(6, 6, gfx->width() - 14, gfx->height() - 16, RGB565_GREEN);
    gfx->drawRect(4, 4, gfx->width() - 10, gfx->height() - 12, RGB565_BLUE);
    gfx->drawRect(2, 2, gfx->width() - 6, gfx->height() - 8, RGB565_RED);
    gfx->drawRect(0, 0, gfx->width() - 2, gfx->height() - 4, RGB565_WHITE);

    // gfx->setFont(u8g2_font_unifont_h_utf8);
    gfx->setFont(u8g2_font_UnnamedDOSFontIV_tr);
    gfx->setTextColor(WHITE);
    gfx->setCursor(30, 0);

    _writeText(30,30, "Welcome.");
    _writeText(30,50, "Connecting to wifi.");
    _writeText(30,70, "Please wait.");

    gfx->endWrite();

    displog("drawWifiConnectingScreen done.");
  }

  void setDisplayBrightness(int brightness) {
  #ifdef TFT_BL
      pinMode(TFT_BL, OUTPUT);
      digitalWrite(TFT_BL, HIGH);
  #endif
  }

  void setup() {
    displog("Arduino_GFX init");
    gfx->begin();
    gfx->setFont();
    gfx->fillScreen(BLACK);
    gfx->setTextColor(WHITE);

  #ifdef TFT_BL
    displog("Backlight control is stub.");
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  #endif

    displog("Arduino_GFX init done.");
  }
}

#endif
