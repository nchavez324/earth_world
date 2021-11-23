#include "city_view.h"

#include "city.h"
#include "filename.h"
#include "panda3d/aa_luse.h"
#include "panda3d/collisionNode.h"
#include "panda3d/collisionSphere.h"
#include "panda3d/depthTestAttrib.h"
#include "panda3d/fontPool.h"
#include "panda3d/geom.h"
#include "panda3d/geomNode.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/geomVertexData.h"
#include "panda3d/geomVertexFormat.h"
#include "panda3d/geomVertexWriter.h"
#include "panda3d/nodePath.h"
#include "panda3d/textFont.h"
#include "panda3d/textNode.h"
#include "panda3d/texturePool.h"
#include "quaternion.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {

const PN_stdfloat kCityIconScale = 0.004f;
const PN_stdfloat kCityLabelScale = 0.007f;
const PN_stdfloat kColliderScale = kCityIconScale * 1.5f;
const PN_stdfloat kCityLabelOffset = 0.01f / MathNumbers::pi;
const LColor kCityLabelTextColor(1, 1, 1, 1);
const LColor kCityLabelShadowColor(0, 0, 0, 1);
const LVector2 kCityLabelShadowOffset(0.05f, 0.05f);

CityView::CityView(PT<WindowFramework> window, const City &city)
    : city_id_{city.getId()}, path_{city.getName() + "_CityRoot"} {
  const SpherePoint3 &sphere_position = city.getLocation();
  LVector3 cartesian_position = sphere_position.toCartesian();
  LVector3 tangent = cartesian_position.cross(LVector3::up());
  LQuaternion rotation =
      quaternion::fromLookAt(-sphere_position.toCartesian(), LVector3::up());

  path_.set_pos(cartesian_position);

  icon_path_ = buildIconNode();
  icon_path_.reparent_to(path_);
  icon_path_.set_scale(kCityIconScale);
  icon_path_.set_pos(LVector3::zero());
  icon_path_.set_quat(rotation);
  icon_path_.set_depth_write(false);
  icon_path_.set_depth_test(false);
  icon_path_.set_bin("fixed", 0);

  PT<CollisionSphere> collider = new CollisionSphere(0, 0, 0, kColliderScale);
  PT<CollisionNode> collider_node = new CollisionNode("CityCollider");
  collider_node->add_solid(collider);
  NodePath collider_path = path_.attach_new_node(collider_node);

  PT<TextFont> font = FontPool::load_font("cmr12.egg");
  PT<TextNode> city_label = new TextNode(city.getName() + "_label");
  city_label->set_text(city.getName());
  city_label->set_font(font);
  city_label->set_text_color(kCityLabelTextColor);
  city_label->set_shadow(kCityLabelShadowOffset);
  city_label->set_shadow_color(kCityLabelShadowColor);

  label_path_ = path_.attach_new_node(city_label);

  // Offset the label a bit up and right of the city.
  SpherePoint3 label_offset_global =
      SpherePoint3(sphere_position.get_azimuthal() + kCityLabelOffset,
                   sphere_position.get_polar() + kCityLabelOffset,
                   sphere_position.get_radial());
  LVector3 label_offset =
      label_offset_global.toCartesian() - cartesian_position;
  label_path_.set_pos(label_offset);

  label_path_.set_quat(rotation);
  label_path_.set_scale(kCityLabelScale);
  label_path_.set_depth_write(false);
  label_path_.set_depth_test(false);
  label_path_.set_bin("fixed", 1);
}

CityView::CityView(CityView &&other) noexcept
    : city_id_{other.city_id_},
      path_{other.path_},
      icon_path_{other.icon_path_},
      label_path_{other.label_path_} {
  other.path_.clear();
  other.icon_path_.clear();
  other.label_path_.clear();
}

CityView &CityView::operator=(CityView &&other) noexcept {
  path_.remove_node();

  city_id_ = other.city_id_;
  path_ = other.path_;
  icon_path_ = other.icon_path_;
  label_path_ = other.label_path_;

  other.path_.clear();
  other.icon_path_.clear();
  other.label_path_.clear();

  return *this;
}

CityView::~CityView() { path_.remove_node(); }

NodePath CityView::getPath() const { return path_; }

int CityView::getCityId() const { return city_id_; }

NodePath CityView::buildIconNode() {
  PT<GeomTriangles> triangles = new GeomTriangles(Geom::UH_static);
  PT<GeomVertexData> vertex_data = new GeomVertexData(
      "CityIcon", GeomVertexFormat::get_v3t2(), Geom::UH_static);
  vertex_data->set_num_rows(4);
  GeomVertexWriter vertices(vertex_data, "vertex");
  GeomVertexWriter uvs(vertex_data, "texcoord");
  vertices.add_data3(-0.5f, 0, -0.5f);
  vertices.add_data3(+0.5f, 0, -0.5f);
  vertices.add_data3(-0.5f, 0, +0.5f);
  vertices.add_data3(+0.5f, 0, +0.5f);
  uvs.add_data2(0, 0);
  uvs.add_data2(1, 0);
  uvs.add_data2(0, 1);
  uvs.add_data2(1, 1);
  triangles->add_vertices(0, 1, 2);
  triangles->add_vertices(2, 1, 3);
  triangles->close_primitive();
  PT<Geom> geom = new Geom(vertex_data);
  geom->add_primitive(triangles);
  PT<GeomNode> geom_node = new GeomNode("CityIcon");
  geom_node->add_geom(geom);

  NodePath path = NodePath(geom_node);

  PT<Texture> icon_texture =
      TexturePool::load_texture(filename::forTexture("city_icon.png"));
  path.set_texture(icon_texture);
  path.set_transparency(TransparencyAttrib::M_alpha);

  return path;
}

}  // namespace earth_world
