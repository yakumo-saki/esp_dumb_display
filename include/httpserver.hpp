#pragma once

#include <Arduino.h>

#include "httpCore.hpp"

namespace httpServer {
  /**
   * 初期化
   */
  void setupSetupMode();

  /**
   * 初期化
   */
  void setupNormalMode();

  /**
   * ループ
   */
  void loop();
}
