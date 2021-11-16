#ifndef APP_H
#define APP_H

#include "globe.h"
#include "panda3d/asyncTask.h"
#include "panda3d/clockObject.h"
#include "panda3d/event.h"
#include "panda3d/genericAsyncTask.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/graphicsOutput.h"
#include "panda3d/graphicsStateGuardian.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/windowFramework.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {
/** The entry point of the simulation. */
class App {
 public:
  /**
   * Creates the app, loading all necessary resources.
   * @param argc The number of arguments contained in the argument list.
   * @param argv An array of arguments, as C strings.
   */
  explicit App(int argc, char *argv[]);

  /**
   * Starts the app's event loop, until terminated by the user.
   * @return The app's exit status.
   */
  int run();

 protected:
  PandaFramework framework_;
  PT<ClockObject> clock_;
  PT<WindowFramework> window_;
  PT<GraphicsEngine> graphics_engine_;
  PT<GraphicsStateGuardian> graphics_state_guardian_;

  PT<Globe> globe_;
  /**
   * The user's input, where the X axis is horizontal motion, the Y axis is
   * vertical motion, the Z axis is zoom level.
   */
  LVector3 input_;

  NodePath camera_path_;
  PN_stdfloat camera_distance_;

  NodePath boat_path_;
  SpherePoint2 boat_unit_sphere_position_;
  PN_stdfloat boat_heading_;

  /**
   * Registers event callbacks for the given keys, treating them as an axis.
   * @param positive_key_code The key code for the positive button.
   * @param negative_key_code The key code for the negative button.
   * @param positive_key_name The name for the positive action.
   * @param negative_key_name The name for the negative action.
   * @param positive_callback The function to call for the positive button.
   * @param negative_callback The function to call for the negative button.
   */
  void defineAxisKey(const std::string &positive_key_code,
                     const std::string &negative_key_code,
                     const std::string &positive_key_name,
                     const std::string &negative_key_name,
                     EventHandler::EventCallbackFunction *positive_callback,
                     EventHandler::EventCallbackFunction *negative_callback);

  /**
   * Handles a change to the input.
   * @param input_delta The change to the input, where the X axis is horizontal
   *     motion, the Y axis is vertical motion, the Z axis is zoom level.
   */
  void onInputChange(LVector3 input_delta);

  /**
   * Handles updating for a single frame.
   * @param task The task for this update loop.
   */
  AsyncTask::DoneStatus onUpdate(GenericAsyncTask *task);

  /** Static event callback function shims. */

  static AsyncTask::DoneStatus onUpdate(GenericAsyncTask *task, void *app) {
    return (static_cast<App *>(app))->onUpdate(task);
  }
  static void onInputUp(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(0, +1, 0));
  }
  static void onInputDown(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(0, -1, 0));
  }
  static void onInputLeft(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(-1, 0, 0));
  }
  static void onInputRight(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(+1, 0, 0));
  }
  static void onInputZoomOut(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(0, 0, +1));
  }
  static void onInputZoomIn(const Event *event, void *app) {
    (static_cast<App *>(app))->onInputChange(LVector3(0, 0, -1));
  }
};
}  // namespace earth_world

#endif  // APP_H
