#include <Arduino.h>
#include "log.h"

void printMemFree() {
  char ram[15];
  char psram[15];
  dtostrf(ESP.getFreeHeap(), 0, 0, ram);
  dtostrf(ESP.getFreePsram(), 0, 0, psram);
  statlog("Free main heap: " + String(ram)
          + "bytes / Free PSRAM heap:" + String(psram) + "bytes");

}
