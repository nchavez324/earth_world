#ifndef EARTH_WORLD_CITY_H
#define EARTH_WORLD_CITY_H

#include <string>

#include "panda3d/aa_luse.h"
#include "panda3d/nodePath.h"
#include "sphere_point.h"

namespace earth_world {

/** Represents a city on the map. */
class CityStaticData {
 public:
  CityStaticData(std::string name, std::string country_name,
                 SpherePoint2 location);

  std::string getName() const { return name_; }
  std::string getCountryName() const { return country_name_; }
  SpherePoint2 getLocation() const { return location_; }

 protected:
  std::string name_;
  std::string country_name_;
  SpherePoint2 location_;
};

/** Represents a city on the map. */
class City {
 public:
  City(NodePath path, NodePath label_path, const CityStaticData &cityStaticData, PN_stdfloat height);

  NodePath getPath() const { return path_; }
  NodePath getLabelPath() const { return label_path_; }
  std::string getName() const { return name_; }
  std::string getCountryName() const { return country_name_; }
  SpherePoint3 getLocation() const { return location_; }
  LQuaternion getRotation() const { return rotation_; }

 protected:
  NodePath path_;
  NodePath label_path_;
  std::string name_;
  std::string country_name_;
  SpherePoint3 location_;
  LQuaternion rotation_;
};

}  // namespace earth_world

#endif  // EARTH_WORLD_CITY_H
