#include <Arduino.h>

#include "mimetype.hpp"
#include "httpCore.hpp"

#include "log.h"

#include "embed/style_css.h"
#include "embed/config_js.h"

namespace httpServer {
  void handleStatic_configjs() {
    server.send(200, MimeType::JAVASCRIPT, CONFIG_JS);
    httplog("handleStatic_configjs done");
  }

  void handleStatic_stylecss() {
    server.send(200, MimeType::CSS, STYLE_CSS);
    httplog("handleStatic_stylecss done");
  }

}
