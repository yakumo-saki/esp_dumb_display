#include <Arduino.h>

static const char UPDATE_UNSUPPORT_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
<html>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width'>
    <meta name='format-detection' content='telephone=no' />
    <title>EnvBoyX update</title>
    <link href="/static/style.css?abca" rel=stylesheet>
    <script src="/static/config.js?aaa"></script>
</head>

<body>
    <h1>Firmware Update is not available</h1>
    <p>Your board is not support web updater</p> 
</body>
</html>
)=====";
