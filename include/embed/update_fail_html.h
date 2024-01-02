#include <Arduino.h>

static const char UPDATE_FAIL_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
<html>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width'>
    <meta name='format-detection' content='telephone=no' />
    <title>EnvBoyX update completed</title>
    <link href="/static/style.css?abca" rel=stylesheet>
    <script src="/static/config.js?aaa"></script>
</head>

<body>
    <h1>Firmware Update failed.</h1>
    <p>Please reboot now.</p> 
    <p>But EnvBoy might be damaged.</p>
</body>
</html>
)=====";
