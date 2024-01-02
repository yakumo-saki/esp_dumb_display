#pragma once

#include <Arduino.h>

class AppConfig {

  const unsigned int CONF_JSON_SIZE = 4000;

  protected:

  public:
    String configId;
    String ssid;
    String password;
    String mDNSName;
    int brightness;
    int rotation;

    /** コンストラクタ */
    AppConfig() {
      // 初期値を入れていく
      this->configId = "NULLCONFIG";
      this->ssid = "";
      this->password = "";
      this->brightness = 255;
      this->mDNSName = "ebxdumb" + String(random(300));
      this->rotation = 0;
    }

    AppConfig(AppConfig &other) = delete;
    void operator=(const AppConfig &) = delete;

    // 公開データメンバーと公開メンバー関数
};

void delete_flagfile();
void create_flagfile();
