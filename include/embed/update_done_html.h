#include <Arduino.h>

static const char UPDATE_DONE_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
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
    <h1>Firmware Update completed.</h1>
    <p>EnvBoy is rebooting... please wait</p>
    <p>NOTE: If not rebooting, reboot manually.</p> 
</body>
</html>
)=====";
