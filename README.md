# ESP dumb display

Display PNG image via HTTP

## Update image

`curl -XPOST -d "url=http://placehold.jp/320x240.png" http://espdumb.local/api/update`

## Backlight Control

`curl -XPOST -d "brightness=0" http://espdumb.local/api/brightness`
