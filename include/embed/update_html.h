#include <Arduino.h>

static const char UPDATE_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
<html>

<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width'>
    <meta name='format-detection' content='telephone=no' />
    <title>EnvBoyX updater</title>
    <link href="/static/style.css?abca" rel=stylesheet>
    <script src="/static/config.js?aaa"></script>
</head>

<body>
    Firmware Update <br>
    This is beta feature. proceed with care !<br>
    <fieldset>
        <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
            アップロードするファイル<br>
            (esp32_envboyx_[version].bin<br>
            <br>
            <input type='file' name='update'><br>
            <br>
            <br>
            <input type='submit' value='Update and Flash firmware'><br>
        </form>
    </fieldset>
</body>
</html>
)=====";
