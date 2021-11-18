#include "city_static_data.h"

namespace earth_world {

CityStaticData::CityStaticData(std::string name, std::string country_name,
                               SpherePoint2 location)
    : name_{name}, country_name_{country_name}, location_{location} {}

const std::string& CityStaticData::getName() const { return name_; }

const std::string& CityStaticData::getCountryName() const {
  return country_name_;
}

const SpherePoint2& CityStaticData::getLocation() const { return location_; }

}  // namespace earth_world
