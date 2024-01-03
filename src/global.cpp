#include <Arduino.h>

// --------------------------------------------------------------------
// プロダクト名定数
// --------------------------------------------------------------------

namespace global {

  // ebx
  extern const String productShort = "DDISP";

  // Enable debug function
  extern const bool DEBUG_BUILD = false;

  // Dumb-Display
  // note: Also referenced by create_release.sh
  extern const String product = "Dumb-Display";
  extern const String ver = "0";      // Major
  extern const String minorVer = "92";  // Minor
  extern const String verLong = " Ver." + ver + "." + minorVer;

  // Setting version (version number when settings changed)
  extern const String SETTING_ID = "DDISPCFG.v01";

  // Dumb-Display Ver.53.0
  extern const String productLong = product + " " + (DEBUG_BUILD ? " Dev." + ver + "." + minorVer : verLong);

  String operationMode = "INVALID";

}
