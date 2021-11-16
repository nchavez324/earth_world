#ifndef EARTH_WORLD_CITY_H
#define EARTH_WORLD_CITY_H

#include <string>

#include "sphere_point.h"

namespace earth_world {

/** Represents a city on the map. */
class City {
 public:
  City(std::string name, std::string country_name, SpherePoint2 location);

  std::string getName() const { return name_; }
  std::string getCountryName() const { return country_name_; }
  SpherePoint2 getLocation() const { return location_; }

 protected:
  std::string name_;
  std::string country_name_;
  SpherePoint2 location_;
};

}  // namespace earth_world

#endif  // EARTH_WORLD_CITY_H
