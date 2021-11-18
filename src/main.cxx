#include <string>

#include "app.h"
#include "filename.h"
#include "panda3d/load_prc_file.h"
#include "panda3d/pStatClient.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/windowFramework.h"
#include "typedefs.h"

std::string const kWindowTitle("Earth World");
LVector2i const kWindowSizeInitial(800, 600);

int run_app(PT<WindowFramework> window);

int run_app(PT<WindowFramework> window) {
  earth_world::App app(window);
  return app.run();
}

int main(int argc, char *argv[]) {
  load_prc_file(earth_world::filename::kConfigFilename);

  if (PStatClient::is_connected()) {
    PStatClient::disconnect();
  }
  if (!PStatClient::connect()) {
    std::cout << "Could not connect to PStat server." << std::endl;
  }

  PandaFramework framework;
  framework.open_framework(argc, argv);

  WindowProperties window_properties;
  window_properties.set_title(kWindowTitle);
  window_properties.set_size(kWindowSizeInitial);
  window_properties.set_fixed_size(false);
  int flags = GraphicsPipe::BF_require_window;
  PT<WindowFramework> window = framework.open_window(window_properties, flags);
  window->enable_keyboard();

  int exit_code = run_app(window);
  return exit_code;
}
