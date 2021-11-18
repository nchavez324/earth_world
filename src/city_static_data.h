#ifndef EARTH_WORLD_CITY_STATIC_DATA_H
#define EARTH_WORLD_CITY_STATIC_DATA_H

#include <string>

#include "sphere_point.h"

namespace earth_world {

/** Represents the data known for a city at compile time. */
class CityStaticData {
 public:
  CityStaticData(std::string name, std::string country_name,
                 SpherePoint2 location);
  CityStaticData(const CityStaticData&) = default;
  CityStaticData(CityStaticData&&) noexcept = default;
  CityStaticData& operator=(const CityStaticData&) = default;
  CityStaticData& operator=(CityStaticData&&) noexcept = default;
  ~CityStaticData() = default;

  const std::string& getName() const;
  const std::string& getCountryName() const;
  const SpherePoint2& getLocation() const;

 protected:
  std::string name_;
  std::string country_name_;
  SpherePoint2 location_;
};

/** A list of default cities on earth to display. */
const std::vector<CityStaticData> kDefaultCities = {
    CityStaticData(
        "New York", "USA",
        SpherePoint2::fromLatitudeAndLongitude(40.712776f, -74.005974)),
    CityStaticData(
        "Bogota", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(4.710989f, -74.072090f)),
    CityStaticData(
        "Honolulu", "USA",
        SpherePoint2::fromLatitudeAndLongitude(21.309919f, -157.858154f)),
    CityStaticData(
        "Lisbon", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(38.685108f, -9.238115f)),
};

}  // namespace earth_world

#endif  // EARTH_WORLD_CITY_STATIC_DATA_H
