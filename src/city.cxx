#include "city.h"

#include "quaternion.h"

namespace earth_world {

City::City(const CityStaticData& city_static_data, int id, PN_stdfloat height)
    : city_static_data_{city_static_data}, id_{id}, is_discovered_{false} {
  SpherePoint2 unit_sphere_position = city_static_data.getLocation();
  location_ = SpherePoint3(unit_sphere_position, height);

  LVector3 unit_position = unit_sphere_position.toCartesian();
  LVector3 tangent = unit_position.cross(LVector3::up());
  rotation_ = quaternion::fromLookAt(tangent, unit_position);
}

const std::string& City::getName() const { return city_static_data_.getName(); }

const std::string& City::getCountryName() const {
  return city_static_data_.getCountryName();
}

int City::getId() const { return id_; }

bool City::getIsDiscovered() const { return is_discovered_; }

const SpherePoint3& City::getLocation() const { return location_; }

const LQuaternion& City::getRotation() const { return rotation_; }

void City::setIsDiscovered(bool is_discovered) {
  is_discovered_ = is_discovered;
}

}  // namespace earth_world
