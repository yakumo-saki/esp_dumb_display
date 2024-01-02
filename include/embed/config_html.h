#include <Arduino.h>

static const char CONFIG_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
<html>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width'>
    <meta name='format-detection' content='telephone=no' />
    <title>ESP Dumb Display setting</title>
    <link href="/static/style.css?abca" rel="stylesheet">
    <script src="/static/config.js?aaa"></script>
</head>

<body>
    <dialog id="waitDialog">
        <p>Now loading. Please wait.</p>
    </dialog>

    <h1>ESP Dumb Display Settings (settingID: <span replace="settingId">DISPCFG.v01</span>)</h1>

    <div class="setupModeOnly notice" hidden>
        <p>セットアップモードです。</p>
        <p>初回設定時はWiFi設定を入力して保存することで通常モードに移行します。</p>
        <p>誤ってセットアップモードに移行した場合は一度設定を保存して再起動すると通常モードに移行します。</p>
        <p>ST7789液晶（TTGO T-Displayなど)を使用している場合は、今のうちに有効に設定しておくことをおすすめします。（WiFi接続失敗時の表示のため）</p>
    </div>

    <a href="/update">ファームウェアアップデータ(ESP32 only)</a>

    <form method='post'>
        <br>
        <fieldset>
            <legend class='m_legend_network'>ネットワーク設定</legend>
            <strong class='m_wifi_info'>WiFi接続情報</strong><br>
            <span class='m_wifi_msg2'>※ 2.4GHz帯のみ対応しています。</span><br>
            <span class="header">SSID</span><input type='text' name='ssid' placeholder='WiFi SSID' value='ssid'><br>
            <span class="header">パスワード</span><input type='password' name='password' placeholder='※ 変更時のみ入力' value=''>
            <br>
            <br> <strong>ホスト名</strong><br>
            ※ mDNS名(a.k.a Rendezvous, avahi-daemon) 及び DHCPのホスト名として使用されます。<br>
            ※ 英大文字小文字、数字、アンダースコア(_)が使用できます。<br>
            <input type='text' name='mDNS' value='mdnsname'><br>
        </fieldset>
        <fieldset>
            <legend>動作モード設定</legend>
            <strong>動作モード</strong><br>
            <label>
                <input type='radio' name='opMode' value='always' id='opMode__always'>
                常時起動モード（測定値常時表示, HTTPサーバーあり）
            </label><br>

            <label>
                <input type='radio' name='opMode' value='mqtt' id='opMode__mqtt'>
                MQTTモード（間欠動作・MQTT送信後ディープスリープ・非推奨）
            </label><br>
        </fieldset>


        <fieldset>
            <legend>時刻関連設定</legend>
            <strong>NTPサーバー設定</strong><br>
            NTPサーバーを使用しない場合、時刻関係の機能は動作しません。<br>
            <label>
                <input type='radio' name='ntp' value='no' id='ntp__no'>
                使用しない
            </label><br>
            <label>
                <input type='radio' name='ntp' value='yes' id='ntp__yes'>
                使用する
            </label><br>
            <br>

            <strong>NTPサーバーのIPアドレス</strong><br>
            <input type='text' name='ntpAddress' placeholder='IP Address' id='ntpAddress'><br>
            <br>

            <strong>タイムゾーン</strong><br>
            <p><a href="https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h">有効なタイムゾーン一覧</a></p>
            <input type='text' name='ntpTimezone' placeholder='timezone'><br>
            <br>

            <strong>時刻によるディスプレイ自動消灯</strong><br>
            <label>
                <input type='radio' name='ntpAutoDimmer' value='no' id='ntpAutoDimmer__no'>
                使用しない
            </label><br>
            <label>
                <input type='radio' name='ntpAutoDimmer' value='yes' id='ntpAutoDimmer__yes'>
                使用する
            </label><br>
            <br>

            <strong>時刻によるディスプレイ自動消灯時刻</strong><br>
            指定した時刻の間、ディスプレイを消灯します。<br>
            <input type="time" name='ntpAutoDimmerStart' id='ntpAutoDimmerStart'>
            &nbsp;～&nbsp;
            <input type="time" name='ntpAutoDimmerEnd' id='ntpAutoDimmerEnd'>
            <br>

        </fieldset>


        <fieldset>
            <legend>表示デバイス共通設定</legend>
            <strong>画面反転</strong><br>

            <label>
                <input type='radio' name='displayFlip' value='no' id='displayFlip__no'>
                反転しない
            </label><br>

            <label>
                <input type='radio' name='displayFlip' value='yes' id='displayFlip__yes'>
                反転する
            </label><br>

            <strong>明るさ(0-255)</strong><br>
            <input type='number' name='displayBrightness' class="num" placeholder='brightness 0-255' value='255'><br>

            <strong>Wait for reconfigure画面</strong><br>
            ※ 起動時にセットアップモードに入るための待ち時間を追加します。<br>
            v46以降、セットアップモードに意図的に入る必要はほぼありません。<br>

            <label>
                <input type='radio' name='displayWaitForReconfigure' value='skip' id='displayWaitForReconfigure__skip'>
                表示しない（推奨）
            </label><br>
            <label>
                <input type='radio' name='displayWaitForReconfigure' value='on' id='displayWaitForReconfigure__on'>
                表示する
            </label><br>


            <strong>自動消灯する明るさ（暗いと判定するしきい値）</strong><br>
            <span>※ 光量センサーがない場合は自動的に無効になります。</span><br>
            <span>※ 自動消灯を使用しない場合は、999999を入力してください。</span><br>
            <input type='number' name='displayAutoDimLux' class="num" placeholder='Lux' value='999999'> Lux<br>

            <strong>自動消灯判定時間</strong><br>
            <span>※ しきい値未満の明るさがこの時間継続したら消灯します。</span><br>
            <input type='number' name='displayAutoDimWaitSec' class="num" placeholder='second to display sleep'
                value='10'> 秒<br>
        </fieldset>
        <fieldset>
            <legend>I2C OLED デバイス設定</legend>
            ※ 接続されていない場合無視されます。画面端にゴミが表示されている、または数ドット欠けている場合は変更してください。<br>

            <label>
                <input type='radio' name='oledType' value='SSD' id='oledType__SSD'>
                SSD1306
            </label><br>

            <label>
                <input type='radio' name='oledType' value='SH' id='oledType__SH'>
                SH1106
            </label><br>
        </fieldset>
        <fieldset>
            <legend>ST7789 デバイス設定</legend>
            <strong>ST7789 SPI液晶の有無</strong><br>
            ※ MQTTモードでは無効。<br> ※ SPIピンはここでは指定できません。<br>

            <label>
                <input type='radio' name='st7789' value='no' id='st7789__no'>
                使用しない
            </label><br>

            <label>
                <input type='radio' name='st7789' value='yes' id='st7789__yes'>
                使用する
            </label><br>

            <strong>ST7789 表示モード</strong><br>

            <label>
                <input type='radio' name='st7789Mode' value='st7789_normal' id='st7789Mode__st7789_normal'>
                横表示モード
            </label><br>


            <label>
                <input type='radio' name='st7789Mode' value='st7789_BIG' id='st7789Mode__st7789_BIG'>
                縦表示モード（デカ文字）
            </label><br>
        </fieldset>

        <fieldset>
            <legend>MH-Z19B デバイス設定</legend>
            <strong>MH-Z19B CO2センサー有無（金色のセンサー）</strong><br>

            <label>
                <input type='radio' name='mhz19b' value='no' id='mhz19b__no'>
                使用しない
            </label><br>

            <label>
                <input type='radio' name='mhz19b' value='yes_uart' id='mhz19b__yes_uart'>
                使用する（UARTモード）
            </label><br>

            <label>
                <input type='radio' name='mhz19b' value='yes_pwm' id='mhz19b__yes_pwm' disabled>
                使用する（PWMモード・非サポート）
            </label><br>
            <input type='hidden' name='mhz19bPwmPin' placeholder='14' class='hidden' value='99' readonly><br>

            <strong>MH-Z19BのUARTが接続されているGPIOピン番号</strong><br>
            ※ MH-Z19Bを使用しない場合は設定不要<br> ※ ESP8266では RX 14 TX 0 で固定<br> ※ ボードによって使用可能なピンが異なるので動作しない場合は他のピンを試してください。<br>
            RXピン&nbsp;
            <input type='number' class="num" name='mhz19bRxPin' placeholder='16' value='88'><br>
            TXピン&nbsp;
            <input type='number' class="num" name='mhz19bTxPin' placeholder='17' value='77'><br>

            <br> <strong>MH-Z19BのAuto Baseline Correction</strong><br>
            ※ MH-Z19Bを使用しない場合は無視されます。<br>

            <label>
                <input type='radio' name='mhz19bAbc' value='no' id='mhz19bAbc__no'>
                無効（推奨）
            </label><br>

            <label>
                <input type='radio' name='mhz19bAbc' value='yes' id='mhz19bAbc__yes'>
                有効
            </label><br>
        </fieldset>
        <fieldset>
            <legend>MQTTモード専用設定</legend>
            <strong>MQTTブローカーのIPアドレス</strong><br>
            ※ ホスト名も指定可能です。mDNS(bonjour, avahi)は使用出来ません。<br>
            <input type='text' name='mqttBroker' placeholder='MQTT Broker' value=''><br>
            <br> <strong>MQTT名</strong><br>
            ※ クライアント名とトピック名として使用<br><input type='text' name='mqttName' placeholder='MQTT Client Name' value=''><br>
        </fieldset>

        <fieldset>
            <legend>センサー値の補正</legend>
            <p>お使いのセンサーの値を補正することができます。マイナス値も入力可能です。</p>
            <span class="header">温度</span><input type='number' class="num" name='adjustTemperature' step="0.1"> ℃<br>
            <span class="header">湿度</span><input type='number' class="num" name='adjustHumidity' step="0.1"> ％<br>
            <span class="header">気圧</span><input type='number' class="num" name='adjustPressure' step="0.1"> hPa<br>
            <span class="header">照度</span><input type='number' class="num" name='adjustLux'> Lux<br>
            <span class="header">CO2</span><input type='number' class="num" name='adjustCo2'> PPM<br>
        </fieldset>

        <fieldset>
            <legend>アラート設定</legend>
            <fieldset>
                <legend>気温</legend>
                <span class='m_caution1'>注意１</span>:&nbsp;
                <input type='number' name='tempAlerts.caut1.L' class='num' value='0'>
                以上
                <input type='number' name='tempAlerts.caut1.H' class='num' value='0'>
                未満<br>
                <span class='m_caution1'>注意２</span>:&nbsp;
                <input type='number' name='tempAlerts.caut2.L' class='num' value='1'>
                以上
                <input type='number' name='tempAlerts.caut2.H' class='num' value='1'>
                未満<br>
                <span class='m_caution1'>警報１</span>:&nbsp;
                <input type='number' name='tempAlerts.warn1.L' class='num' value='-991'>
                以上
                <input type='number' name='tempAlerts.warn1.H' class='num' value='100'>
                未満<br>
                <span class='m_caution1'>警報２</span>:&nbsp;
                <input type='number' name='tempAlerts.warn2.L' class='num' value='3340'>
                以上
                <input type='number' name='tempAlerts.warn2.H' class='num' value='9339'>
                未満<br>
            </fieldset>
            <fieldset>
                <legend>湿度</legend>
                <span class='m_caution1'>注意１</span>:&nbsp;
                <input type='number' name='humiAlerts.caut1.L' class='num' value='21110'>
                以上
                <input type='number' name='humiAlerts.caut1.H' class='num' value='33535'>
                未満<br>
                <span class='m_caution1'>注意２</span>:&nbsp;
                <input type='number' name='humiAlerts.caut2.L' class='num' value='65555'>
                以上
                <input type='number' name='humiAlerts.caut2.H' class='num' value='75555'>
                未満<br>
                <span class='m_caution1'>警報１</span>:&nbsp;
                <input type='number' name='humiAlerts.warn1.L' class='num' value='1102'>
                以上
                <input type='number' name='humiAlerts.warn1.H' class='num' value='3320'>
                未満<br>
                <span class='m_caution1'>警報２</span>:&nbsp;
                <input type='number' name='humiAlerts.warn2.L' class='num' value='432475'>
                以上
                <input type='number' name='humiAlerts.warn2.H' class='num' value='142300'>
                未満<br>
            </fieldset>
            <fieldset>
                <legend>照度</legend>
                <span class='m_caution1'>注意１</span>:&nbsp;
                <input type='number' name='luxAlerts.caut1.L' class='num' value='-1234'>
                以上
                <input type='number' name='luxAlerts.caut1.H' class='num' value='-1444'>
                未満<br>
                <span class='m_caution1'>注意２</span>:&nbsp;
                <input type='number' name='luxAlerts.caut2.L' class='num' value='-155'>
                以上
                <input type='number' name='luxAlerts.caut2.H' class='num' value='-166'>
                未満<br>
                <span class='m_caution1'>警報１</span>:&nbsp;
                <input type='number' name='luxAlerts.warn1.L' class='num' value='550'>
                以上
                <input type='number' name='luxAlerts.warn1.H' class='num' value='661'>
                未満<br>
                <span class='m_caution1'>警報２</span>:&nbsp;
                <input type='number' name='luxAlerts.warn2.L' class='num' value='2222'>
                以上
                <input type='number' name='luxAlerts.warn2.H' class='num' value='4444'>
                未満<br>
            </fieldset>
            <fieldset>
                <legend>気圧</legend>
                <span class='m_caution1'>注意１</span>:&nbsp;
                <input type='number' name='presAlerts.caut1.L' class='num' value='995'>
                以上
                <input type='number' name='presAlerts.caut1.H' class='num' value='1000'>
                未満<br>
                <span class='m_caution1'>注意２</span>:&nbsp;
                <input type='number' name='presAlerts.caut2.L' class='num' value='3000'>
                以上
                <input type='number' name='presAlerts.caut2.H' class='num' value='3000'>
                未満<br>
                <span class='m_caution1'>警報１</span>:&nbsp;
                <input type='number' name='presAlerts.warn1.L' class='num' value='0'>
                以上
                <input type='number' name='presAlerts.warn1.H' class='num' value='995'>
                未満<br>
                <span class='m_caution1'>警報２</span>:&nbsp;
                <input type='number' name='presAlerts.warn2.L' class='num' value='1025'>
                以上
                <input type='number' name='presAlerts.warn2.H' class='num' value='3000'>
                未満<br>
            </fieldset>
            <fieldset>
                <legend>CO2濃度</legend>
                <span class='m_caution1'>注意１</span>:&nbsp;
                <input type='number' name='co2Alerts.caut1.L' class='num' value='200'>
                以上
                <input type='number' name='co2Alerts.caut1.H' class='num' value='300'>
                未満<br>
                <span class='m_caution1'>注意２</span>:&nbsp;
                <input type='number' name='co2Alerts.caut2.L' class='num' value='800'>
                以上
                <input type='number' name='co2Alerts.caut2.H' class='num' value='1000'>
                未満<br>
                <span class='m_caution1'>警報１</span>:&nbsp;
                <input type='number' name='co2Alerts.warn1.L' class='num' value='0'>
                以上
                <input type='number' name='co2Alerts.warn1.H' class='num' value='200'>
                未満<br>
                <span class='m_caution1'>警報２</span>:&nbsp;
                <input type='number' name='co2Alerts.warn2.L' class='num' value='1000'>
                以上
                <input type='number' name='co2Alerts.warn2.H' class='num' value='9999'>
                未満<br>
            </fieldset>
        </fieldset>
        <br> <input type='button' id="submit" value='設定する'><br>
    </form>
    <br>
    <hr>ESP Dumb Display <span replace="productVer">Ver.00.0</span>, Copyright 2018-2023 Yakumo Saki / ziomatrix.org.
</body>

</html>

)=====";
