#include "app.h"

#include "debug_axes.h"
#include "filename.h"
#include "globe.h"
#include "panda3d/asyncTaskManager.h"
#include "panda3d/boundingBox.h"
#include "panda3d/clockObject.h"
#include "panda3d/computeNode.h"
#include "panda3d/geomLines.h"
#include "panda3d/collisionSphere.h"
#include "panda3d/collisionNode.h"
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

const bool kEnableDebugAxes = false;
const int kGlobeVerticesPerEdge = 100;
const PN_stdfloat kAxesScale = 40.f;
const PN_stdfloat kGlobeScale = 20.f;
const PN_stdfloat kBoatScale = 0.05f;
const PN_stdfloat kBoatSpeed = 0.07f;
const PN_stdfloat kCameraDistanceMin = 7.f;
const PN_stdfloat kCameraDistanceMax = 20.f;
const PN_stdfloat kCameraZoomSpeed = 5.f;
const std::string kTagCityId = "city_id";
const LColor kClearColor(0, 0, 0, 1);

App::App(PT<WindowFramework> window)
    : framework_{window->get_panda_framework()},
      window_{window},
      collision_handler_queue_{new CollisionHandlerQueue},
      globe_view_{window->get_graphics_output(), globe_, kGlobeVerticesPerEdge},
      minimap_view_{globe_},
      input_{0},
      last_window_size_{0},
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

  // Create the collection of cities, and place them.
  cities_.reserve(kDefaultCities.size());
  for (std::vector<CityStaticData>::size_type i = 0; i < kDefaultCities.size();
       i++) {
    const CityStaticData &city_static_data = kDefaultCities[i];
    PN_stdfloat height =
        globe_.getHeightAtPoint(city_static_data.getLocation());
    City city(city_static_data, i, height);
    cities_.push_back(std::move(city));
  }
  city_views_.reserve(cities_.size());
  for (std::vector<City>::size_type i = 0; i < cities_.size(); i++) {
    const City &city = cities_[i];
    CityView city_view(window_, city);
    city_view.getPath().reparent_to(globe_view_.getPath());
    city_view.getPath().set_tag(kTagCityId, std::to_string(i));
    city_views_.push_back(std::move(city_view));
  }

  boat_path_ = window_->load_model(framework_->get_models(),
                                   filename::forModel("boat/S_Boat.bam"));
  boat_path_.reparent_to(window_->get_render());
  boat_path_.set_scale(kBoatScale);

  PT<CollisionSphere> boat_collider = new CollisionSphere(0, 0, 0, 3);
  PT<CollisionNode> boat_collider_node = new CollisionNode("BoatCollider");
  boat_collider_node->add_solid(boat_collider);
  boat_collider_path_ = boat_path_.attach_new_node(boat_collider_node);
  collision_traverser_.add_collider(boat_collider_path_,
                                    collision_handler_queue_);

  camera_path_ = window_->get_camera_group();
  camera_path_.set_pos((kGlobeScale + camera_distance_) * LVector3::right());
  camera_path_.set_quat(
      quaternion::fromLookAt(LVector3::left(), LVector3::up()));

  minimap_view_.getPath().reparent_to(window->get_pixel_2d());

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
  framework_->get_task_mgr().remove_task_chain("Update");
  minimap_view_.getPath().remove_node();
  camera_path_.remove_node();
  collision_traverser_.remove_collider(boat_collider_path_);
  boat_path_.remove_node();
  globe_view_.getPath().remove_node();
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
  ClockObject *clock = ClockObject::get_global_clock();
  if (window_.is_null() || clock == nullptr) {
    return AsyncTask::DS_exit;
  }
  GraphicsWindow *graphics_window = window_->get_graphics_window();
  if (graphics_window == nullptr) {
    return AsyncTask::DS_exit;
  }
  GraphicsOutput *graphics_output = window_->get_graphics_output();
  if (graphics_output == nullptr) {
    return AsyncTask::DS_exit;
  }

  // 0. Reposition UI if the window size has changed.
  LVector2i new_window_size = graphics_window->get_size();
  if (new_window_size != last_window_size_) {
    last_window_size_ = new_window_size;
    minimap_view_.onWindowResize(new_window_size);
  }

  // 1. Determine the velocity, by using the input in the camera's basis.
  CPT<TransformState> camera_xform = camera_path_.get_net_transform();
  LVector3 camera_right = camera_xform->get_quat().get_right();
  LVector3 camera_up = camera_xform->get_quat().get_up();
  LVector2 normalized_input = input_.get_xy().normalized();
  LVector3 heading =
      (camera_right * input_.get_x()) + (camera_up * input_.get_y());
  heading.normalize();
  LVector3 position_delta = heading * kBoatSpeed * clock->get_dt();

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
  globe_.updateVisibility(graphics_output, boat_unit_sphere_position_);

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

  collision_traverser_.traverse(window_->get_render());
  if (collision_handler_queue_->get_num_entries() > 0) {
    CollisionEntry *entry = collision_handler_queue_->get_entry(0);
    bool into_city =
        entry->has_into() && entry->get_into_node_path().has_parent() &&
        entry->get_into_node_path().get_parent().has_tag(kTagCityId);
    if (into_city) {
      std::vector<City>::size_type city_id = std::stoul(
          entry->get_into_node_path().get_parent().get_tag(kTagCityId));
      if (city_id < cities_.size()) {
        City &city = cities_[city_id];
        // TODO: Allow interaction with city 
      }
    }
  }

  // 7. Update the camera distance.
  PN_stdfloat camera_distance_delta =
      input_.get_z() * clock->get_dt() * kCameraZoomSpeed;
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
