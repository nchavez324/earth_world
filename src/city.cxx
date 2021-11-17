#include "city.h"

#include "quaternion.h"

namespace earth_world {

CityStaticData::CityStaticData(std::string name, std::string country_name,
                               SpherePoint2 location)
    : name_(name), country_name_(country_name), location_(location) {}

City::City(NodePath path, NodePath label_path,
           const CityStaticData &city_static_data, PN_stdfloat height)
    : path_(path),
      label_path_(label_path),
      name_(city_static_data.getName()),
      country_name_(city_static_data.getCountryName()) {
  SpherePoint2 unit_sphere_position = city_static_data.getLocation();
  location_ = SpherePoint3(unit_sphere_position, height);

  LVector3 unit_position = unit_sphere_position.toCartesian();
  LVector3 tangent = unit_position.cross(LVector3::up());
  rotation_ = quaternion::fromLookAt(tangent, unit_position);
}

}  // namespace earth_world
