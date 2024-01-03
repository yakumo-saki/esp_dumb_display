#include <Arduino.h>

namespace httpServer {
  // HTTP ヘッダ (HTTP 1.1 〜) text/htmlで送信する
  void sendHttpHeader();

  // HTTP ヘッダ (HTTP 1.1 〜)をコンテントタイプ(text/html..)を送信する
  void sendHttpHeader(String contentType);

  // HTMLヘッダ (<html>~<head/>まで)を送信する。
  // <body>〜</body></html> は呼び出し元で送信する必要がある
  void sendHtmlHeader();

  // HTTP REDIRECTヘッダを送信する。これを実行したあとは何も送れない
  void sendHttpRedirectHeader(String url);

  // CORS リクエストに応答する（http OPTIONS リクエスト)
  void http_handle_cors();
}
