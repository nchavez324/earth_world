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

std::vector<CityStaticData> kDefaultCities = {
    CityStaticData(
        "New York", "USA",
        SpherePoint2::fromLatitudeAndLongitude(40.712776f, -74.005974f)),
    CityStaticData(
        "Boston", "USA",
        SpherePoint2::fromLatitudeAndLongitude(42.359903f, -71.058447f)),
    CityStaticData(
        "Washington D.C.", "USA",
        SpherePoint2::fromLatitudeAndLongitude(38.907147f, -77.036453f)),
    CityStaticData(
        "Miami", "USA",
        SpherePoint2::fromLatitudeAndLongitude(25.761832f, -80.191629f)),
    CityStaticData(
        "Nassau", "Bahamas",
        SpherePoint2::fromLatitudeAndLongitude(25.044238f, -77.350199f)),
    CityStaticData(
        "Havana", "Cuba",
        SpherePoint2::fromLatitudeAndLongitude(23.113568f, -82.366425f)),
    CityStaticData(
        "Santo Domingo", "Dominican Republic",
        SpherePoint2::fromLatitudeAndLongitude(18.486018f, -69.931417f)),
    CityStaticData(
        "Kingston", "Jamaica",
        SpherePoint2::fromLatitudeAndLongitude(18.017859f, -76.809942f)),
    CityStaticData(
        "San Juan", "Puerto Rico",
        SpherePoint2::fromLatitudeAndLongitude(18.465600f, -66.105204f)),
    CityStaticData(
        "New Orleans", "USA",
        SpherePoint2::fromLatitudeAndLongitude(29.951046f, -90.071463f)),
    CityStaticData(
        "Houston", "USA",
        SpherePoint2::fromLatitudeAndLongitude(29.760426f, -95.369771f)),
    CityStaticData(
        "Veracruz", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(19.173910f, -96.134067f)),
    CityStaticData(
        "Cancun", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(21.161881f, -86.851574f)),
    CityStaticData(
        "Belize City", "Belize",
        SpherePoint2::fromLatitudeAndLongitude(17.504275f, -88.196351f)),
    CityStaticData(
        "San Pedro Sula", "Honduras",
        SpherePoint2::fromLatitudeAndLongitude(15.503914f, -88.013879f)),
    CityStaticData(
        "San Salvador", "El Salvador",
        SpherePoint2::fromLatitudeAndLongitude(13.692922f, -89.218090f)),
    CityStaticData(
        "Managua", "Nicaragua",
        SpherePoint2::fromLatitudeAndLongitude(12.114764f, -86.236012f)),
    CityStaticData(
        "San Jose", "Costa Rica",
        SpherePoint2::fromLatitudeAndLongitude(9.927598f, -84.090610f)),
    CityStaticData(
        "Panama City", "Panama",
        SpherePoint2::fromLatitudeAndLongitude(8.982239f, -79.519576f)),
    CityStaticData(
        "Acapulco", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(16.853047f, -99.823477f)),
    CityStaticData(
        "Puerto Vallarta", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(20.653413f, -105.225242f)),
    CityStaticData(
        "Cabo San Lucas", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(22.890660f, -109.916962f)),
    CityStaticData(
        "San Diego", "USA",
        SpherePoint2::fromLatitudeAndLongitude(32.715562f, -117.161681f)),
    CityStaticData(
        "Los Angeles", "USA",
        SpherePoint2::fromLatitudeAndLongitude(34.052197f, -118.243390f)),
    CityStaticData(
        "San Francisco", "USA",
        SpherePoint2::fromLatitudeAndLongitude(37.775018f, -122.419272f)),
    CityStaticData(
        "Portland", "USA",
        SpherePoint2::fromLatitudeAndLongitude(45.515148f, -122.678267f)),
    CityStaticData(
        "Seattle", "USA",
        SpherePoint2::fromLatitudeAndLongitude(47.606086f, -122.332885f)),
    CityStaticData(
        "Vancouver", "Canada",
        SpherePoint2::fromLatitudeAndLongitude(49.282561f, -123.120550f)),
    CityStaticData(
        "Anchorage", "USA",
        SpherePoint2::fromLatitudeAndLongitude(61.218058f, -149.899900f)),
    CityStaticData(
        "Buenaventura", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(3.882920f, -77.019631f)),
    CityStaticData(
        "Cartagena", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(10.393273f, -75.483010f)),
    CityStaticData(
        "Guayaquil", "Ecuador",
        SpherePoint2::fromLatitudeAndLongitude(-2.189239f, -79.889122f)),
    CityStaticData(
        "Lima", "Peru",
        SpherePoint2::fromLatitudeAndLongitude(-12.046434, -77.042566f)),
    CityStaticData(
        "Santiago", "Chile",
        SpherePoint2::fromLatitudeAndLongitude(-33.449439f, -70.668600f)),
    CityStaticData(
        "Buenos Aires", "Argentina",
        SpherePoint2::fromLatitudeAndLongitude(-34.603652f, -58.381354f)),
    CityStaticData(
        "Montevideo", "Uruguay",
        SpherePoint2::fromLatitudeAndLongitude(-34.901034f, -56.164223f)),
    CityStaticData(
        "Honolulu", "USA",
        SpherePoint2::fromLatitudeAndLongitude(21.309919f, -157.858154f)),
    CityStaticData(
        "Lisbon", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(38.685108f, -9.238115f)),
};

}  // namespace earth_world
