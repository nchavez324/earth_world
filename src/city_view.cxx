#include "city_view.h"

#include "city.h"
#include "filename.h"
#include "panda3d/aa_luse.h"
#include "panda3d/collisionNode.h"
#include "panda3d/collisionSphere.h"
#include "panda3d/fontPool.h"
#include "panda3d/nodePath.h"
#include "panda3d/textFont.h"
#include "panda3d/textNode.h"
#include "quaternion.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {

const PN_stdfloat kCityScale = 0.005f;
const PN_stdfloat kCityLabelScale = 0.007f;
const PN_stdfloat kCityLabelOffset = 0.01f / MathNumbers::pi;

CityView::CityView(PT<WindowFramework> window, City &city)
    : city_{city}, path_{city.getName() + "_CityRoot"} {
  const SpherePoint3 &sphere_position = city.getLocation();
  LVector3 cartesian_position = sphere_position.toCartesian();
  LVector3 tangent = cartesian_position.cross(LVector3::up());
  LQuaternion rotation = quaternion::fromLookAt(tangent, cartesian_position);

  path_.set_pos(cartesian_position);

  model_path_ = window->load_model(window->get_panda_framework()->get_models(),
                                   filename::forModel("city/S_City.bam"));
  model_path_.reparent_to(path_);
  model_path_.set_pos(LVector3::zero());
  model_path_.set_quat(rotation);
  model_path_.set_scale(kCityScale);

  PT<CollisionSphere> collider = new CollisionSphere(0, 0, 0, 1.5f * kCityScale);
  PT<CollisionNode> collider_node =
      new CollisionNode("CityCollider");
  collider_node->add_solid(collider);
  collider_path_ = path_.attach_new_node(collider_node);
  collider_path_.show();

  PT<TextFont> font = FontPool::load_font("cmr12.egg");
  PT<TextNode> city_label = new TextNode(city.getName() + "_label");
  city_label->set_text(city.getName());
  city_label->set_font(font);

  label_path_ = path_.attach_new_node(city_label);

  // Offset the label a bit up and right of the city.
  SpherePoint3 label_offset_global =
      SpherePoint3(sphere_position.get_azimuthal() + kCityLabelOffset,
                   sphere_position.get_polar() + kCityLabelOffset,
                   sphere_position.get_radial());
  LVector3 label_offset =
      label_offset_global.toCartesian() - cartesian_position;
  label_path_.set_pos(label_offset);

  label_path_.set_quat(
      quaternion::fromLookAt(-sphere_position.toCartesian(), LVector3::up()));
  label_path_.set_scale(kCityLabelScale);
  label_path_.set_depth_test(false);
}

CityView::CityView(CityView &&other) noexcept
    : city_{other.city_},
      path_{other.path_},
      model_path_{other.model_path_},
      label_path_{other.label_path_},
      collider_path_{other.collider_path_} {
  other.path_.clear();
  other.model_path_.clear();
  other.label_path_.clear();
  other.collider_path_.clear();
}

CityView &CityView::operator=(CityView &&other) noexcept {
  path_.remove_node();

  city_ = other.city_;
  path_ = other.path_;
  model_path_ = other.model_path_;
  label_path_ = other.label_path_;
  collider_path_ = other.collider_path_;

  other.path_.clear();
  other.model_path_.clear();
  other.label_path_.clear();
  other.collider_path_.clear();

  return *this;
}

CityView::~CityView() { path_.remove_node(); }

NodePath CityView::getPath() const { return path_; }

NodePath CityView::getColliderPath() const { return collider_path_; }

}  // namespace earth_world
