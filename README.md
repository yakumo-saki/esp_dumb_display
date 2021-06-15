# ESP dumb display

Display PNG image via HTTP.  
Development target is ESP8266 and ILI9341 320x240 TFT(https://akizukidenshi.com/catalog/g/gM-16265/).

## Using 

This is PlatformIO Project.

* TFT_eSPI https://github.com/Bodmer/TFT_eSPI
* pngle https://github.com/kikuchan/pngle

# APIs
## Update image

`curl -XPOST -d "url=http://placehold.jp/320x240.png" http://espdumb.local/api/update`

## Backlight Control

`curl -XPOST -d "brightness=0" http://espdumb.local/api/brightness`
