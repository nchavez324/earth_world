#include "app.h"

#include "debug_axes.h"
#include "filename.h"
#include "globe.h"
#include "panda3d/asyncTaskManager.h"
#include "panda3d/boundingBox.h"
#include "panda3d/clockObject.h"
#include "panda3d/computeNode.h"
#include "panda3d/geomLines.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/graphicsPipe.h"
#include "panda3d/mouseAndKeyboard.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"
#include "panda3d/windowFramework.h"
#include "panda3d/windowProperties.h"
#include "quaternion.h"
#include "typedefs.h"

namespace earth_world {

bool const kEnableDebugAxes = false;
int const kGlobeVerticesPerEdge = 100;
PN_stdfloat const kAxesScale = 40.f;
PN_stdfloat const kGlobeScale = 20.f;
PN_stdfloat const kGlobeWaterSurfaceHeight = 0.95f;
PN_stdfloat const kBoatScale = 0.05f;
PN_stdfloat const kBoatSpeed = 0.3f;
PN_stdfloat const kCameraDistanceMin = 7.f;
PN_stdfloat const kCameraDistanceMax = 20.f;
PN_stdfloat const kCameraZoomSpeed = 5.f;
LColor const kClearColor(0, 0, 0, 1);

App::App(PT<WindowFramework> window)
    : framework_{window->get_panda_framework()},
      window_{window},
      clock_{ClockObject::get_global_clock()},
      globe_{},
      globe_view_{window, globe_, kGlobeVerticesPerEdge},
      input_{LVector3::zero()},
      camera_distance_{kCameraDistanceMin},
      boat_unit_sphere_position_{/* azimuthal= */ 0, /* polar= */ 0},
      boat_heading_{0.f} {
  window_->get_display_region_3d()->set_clear_color(kClearColor);

  if (kEnableDebugAxes) {
    NodePath axes = debug_axes::build();
    axes.reparent_to(window_->get_render());
    axes.set_scale(kAxesScale);
  }

  globe_view_.getPath().reparent_to(window_->get_render());
  globe_view_.getPath().set_scale(kGlobeScale);

  boat_path_ = window_->load_model(framework_->get_models(),
                                   filename::forModel("boat/S_Boat.bam"));
  boat_path_.reparent_to(window_->get_render());
  boat_path_.set_scale(kBoatScale);

  camera_path_ = window_->get_camera_group();
  camera_path_.set_pos((kGlobeScale + camera_distance_) * LVector3::right());
  camera_path_.set_quat(
      quaternion::fromLookAt(LVector3::left(), LVector3::up()));

  PT<GenericAsyncTask> update_task =
      new GenericAsyncTask("Update", &App::onUpdate, /* app= */ this);
  framework_->get_task_mgr().add(update_task);

  defineAxisKey("w", "s", "Up", "Down", &App::onInputUp, &App::onInputDown);
  defineAxisKey("d", "a", "Right", "Left", &App::onInputRight,
                &App::onInputLeft);
  defineAxisKey("arrow_up", "arrow_down", "Up", "Down", &App::onInputUp,
                &App::onInputDown);
  defineAxisKey("arrow_right", "arrow_left", "Right", "Left",
                &App::onInputRight, &App::onInputLeft);
  defineAxisKey("e", "q", "Zoom In", "Zoom Out", &App::onInputZoomIn,
                &App::onInputZoomOut);
}

App::~App() {
  globe_view_.getPath().remove_node();
  camera_path_.remove_node();
  boat_path_.remove_node();
}

int App::run() {
  framework_->main_loop();
  framework_->close_framework();
  return 0;
}

void App::defineAxisKey(
    const std::string &positive_key_name, const std::string &negative_key_name,
    const std::string &positive_name, const std::string &negative_name,
    EventHandler::EventCallbackFunction *positive_callback,
    EventHandler::EventCallbackFunction *negative_callback) {
  framework_->define_key(positive_key_name, positive_name + " pressed",
                         positive_callback,
                         /* app= */ this);
  framework_->define_key(positive_key_name + "-up", positive_name + " released",
                         negative_callback,
                         /* app= */ this);
  framework_->define_key(negative_key_name, negative_name + " pressed",
                         negative_callback,
                         /* app= */ this);
  framework_->define_key(negative_key_name + "-up", negative_name + " released",
                         positive_callback,
                         /* app= */ this);
}

void App::onInputChange(LVector3 input_delta) {
  input_ += input_delta;
  input_.set(std::min(1.f, std::max(-1.f, input_.get_x())),
             std::min(1.f, std::max(-1.f, input_.get_y())),
             std::min(1.f, std::max(-1.f, input_.get_z())));
}

AsyncTask::DoneStatus App::onUpdate(GenericAsyncTask *task) {
  if (window_.is_null() || clock_.is_null()) {
    return AsyncTask::DS_exit;
  }

  // 1. Determine the velocity, by using the input in the camera's basis.
  CPT<TransformState> camera_xform = camera_path_.get_net_transform();
  LVector3 camera_right = camera_xform->get_quat().get_right();
  LVector3 camera_up = camera_xform->get_quat().get_up();
  LVector2 normalized_input = input_.get_xy().normalized();
  LVector3 heading =
      (camera_right * input_.get_x()) + (camera_up * input_.get_y());
  heading.normalize();
  LVector3 position_delta = heading * kBoatSpeed * clock_->get_dt();

  // 2. Update the boat's position in cartesian space and snap onto sphere.
  LVector3 old_boat_unit_position = boat_unit_sphere_position_.toCartesian();
  LVector3 new_boat_unit_position =
      (old_boat_unit_position + position_delta).normalized();
  SpherePoint2 new_boat_unit_sphere_position =
      SpherePoint2::fromCartesian(new_boat_unit_position);
  LVector3 old_boat_position =
      kGlobeWaterSurfaceHeight * kGlobeScale * old_boat_unit_position;
  LVector3 new_boat_position =
      kGlobeWaterSurfaceHeight * kGlobeScale * new_boat_unit_position;

  // 3. See if intended destination would be land, and withhold update if so.
  if (globe_.isLandAtPoint(new_boat_unit_sphere_position)) {
    // Roll back the proposed update.
    new_boat_unit_position = old_boat_unit_position;
    new_boat_unit_sphere_position = boat_unit_sphere_position_;
    new_boat_position = old_boat_position;
  } else {
    boat_unit_sphere_position_ = new_boat_unit_sphere_position;
    boat_path_.set_pos(new_boat_position);
  }

  // 4. Update the visible portion of the globe.
  globe_.updateVisibility(window_->get_graphics_output(),
                          boat_unit_sphere_position_);

  // 5. Update the heading if the input was non zero.
  if (!IS_NEARLY_ZERO(input_.get_x()) || !IS_NEARLY_ZERO(input_.get_y())) {
    PN_stdfloat new_heading =
        atan2f(normalized_input.get_y(), normalized_input.get_x());
    if (new_heading < 0) {
      new_heading += 2 * MathNumbers::pi;
    }
    boat_heading_ = new_heading;
  }

  // 6. Transform the heading into a world rotation.
  LVector3 boat_right = LVector3::up().cross(new_boat_unit_position);
  LVector3 boat_up = new_boat_unit_position.cross(boat_right);
  LVector3 boat_heading =
      (cosf(boat_heading_) * boat_right) + (sinf(boat_heading_) * boat_up);
  boat_heading.normalize();
  LQuaternion new_boat_rotation =
      quaternion::fromLookAt(boat_heading, new_boat_unit_position);
  boat_path_.set_quat(new_boat_rotation);

  // 7. Update the camera distance.
  PN_stdfloat camera_distance_delta =
      input_.get_z() * clock_->get_dt() * kCameraZoomSpeed;
  camera_distance_ = std::max(
      kCameraDistanceMin,
      std::min(kCameraDistanceMax, camera_distance_ + camera_distance_delta));

  // 8. Float the camera above the boat and look at it.
  LVector3 new_camera_position =
      new_boat_position + (camera_distance_ * new_boat_unit_position);
  LQuaternion new_camera_rotation =
      quaternion::fromLookAt(-new_boat_unit_position, LVector3::up());
  camera_path_.set_pos(new_camera_position);
  camera_path_.set_quat(new_camera_rotation);

  return AsyncTask::DoneStatus::DS_cont;
}

}  // namespace earth_world
