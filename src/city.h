#ifndef EARTH_WORLD_CITY_H
#define EARTH_WORLD_CITY_H

#include <string>

#include "city_static_data.h"
#include "sphere_point.h"

namespace earth_world {

/** Models a city on the globe. */
class City {
 public:
  City(const CityStaticData& city_static_data, int id, PN_stdfloat height);
  City(const City&) = default;
  City(City&&) noexcept = default;
  City& operator=(const City&) = default;
  City& operator=(City&&) noexcept = default;
  ~City() = default;

  const std::string& getName() const;
  const std::string& getCountryName() const;
  int getId() const;
  const SpherePoint3& getLocation() const;
  const LQuaternion& getRotation() const;

 protected:
  CityStaticData city_static_data_;
  int id_;
  SpherePoint3 location_;
  LQuaternion rotation_;
};
}  // namespace earth_world

#endif  // EARTH_WORLD_CITY_H
